#include "../include/tjshared.h"
#include <windowsx.h>
using namespace tj::shared;
using namespace Gdiplus;

/* Path */
Path::Path() {
}

Path::~Path() {
}

void Path::Add(std::wstring text, std::wstring icon, ref<Inspectable> is) {
	_crumbs.push_back(GC::Hold(new BasicCrumb(text,icon,is)));
}

ref<Crumb> Path::GetHead() {
	std::vector<ref<Crumb> >::reverse_iterator rit = _crumbs.rbegin();
	if(rit!=_crumbs.rend()) {
		return *rit;
	}
	return 0;
}

void Path::Add(ref<Crumb> r) {
	_crumbs.push_back(r);
}

/* Crumb */
Crumb::Crumb(std::wstring text, std::wstring icon) {
	_text = text;
	std::wstring icfn = ResourceManager::Instance()->Get(icon);
	_icon = Bitmap::FromFile(icfn.c_str(), TRUE);
}

Crumb::~Crumb() {
	delete _icon;
}

ref< std::vector< ref<Crumb> > > Crumb::GetChildren() {
	return 0;
}

ref<Inspectable> Crumb::GetSubject() {
	return 0;
}

std::wstring Crumb::GetTextTrimmed() const {
	if(_text.length()>KMaxTextLength) {
		return _text.substr(0, KMaxTextLength-4)+L"...";
	}
	return _text;
}

std::wstring Crumb::GetText() const {
	return _text;
}

void Crumb::SetText(const std::wstring& x) {
	_text = x;
}

Gdiplus::Bitmap* Crumb::GetIcon() {
	return _icon;
}

/* BasicCrumb */
BasicCrumb::BasicCrumb(std::wstring text, std::wstring icon, ref<Inspectable> subject): Crumb(text,icon) {
	_subject = subject;
}

BasicCrumb::~BasicCrumb() {
}

ref<Inspectable> BasicCrumb::GetSubject() {
	return _subject;
}

ref< std::vector< ref<Crumb> > > BasicCrumb::GetChildren() {
	return 0;
}

/* PathWnd*/
PathWnd::PathWnd(HWND parent, PropertyGridWnd* pg): ChildWnd(L"", parent, true, true) {
	_propertyGrid = pg;
	std::wstring spfn = ResourceManager::Instance()->Get(L"icons/path_separator.png");
	_separator = Bitmap::FromFile(spfn.c_str(), TRUE);
	SetWantMouseLeave(true);
}

PathWnd::~PathWnd() {
	delete _separator;
}

void PathWnd::Update() {
	Repaint();
}

ref<Crumb> PathWnd::GetCrumbAt(int x, int* left) {
	if(_path) {
		ref<Theme> theme = ThemeManager::GetTheme();
		HDC dc = GetDC(_wnd);
		{
			Graphics g(dc);
			std::vector< ref<Crumb> >::iterator it = _path->_crumbs.begin();
			int rx = 1;
			while(it!=_path->_crumbs.end()) {
				ref<Crumb> crumb = *it;
				RectF textrc;
				g.MeasureString(crumb->GetTextTrimmed().c_str(), int(crumb->GetTextTrimmed().length()), theme->GetGUIFont(), PointF(0.0f,0.0f), &textrc);
				int totalWidth = int(textrc.Width)+KMarginLeft+KMarginRight+KIconWidth;
				rx += totalWidth;

				if(x<rx) {
					if(left!=0) {
						*left = rx-totalWidth;
					}
					return crumb;
				}
				it++;
			}
		}
		ReleaseDC(_wnd, dc);
	}
	
	return 0;
}

void PathWnd::Paint(Gdiplus::Graphics& g) {
	tj::shared::Rectangle rc = GetClientRectangle();
	ref<Theme> theme = ThemeManager::GetTheme();
	
	SolidBrush zwart(theme->GetBackgroundColor());
	g.FillRectangle(&zwart, rc);

	LinearGradientBrush br(PointF(0.0f, 0.0f), PointF(0.0f, float(rc.GetHeight())), theme->GetToolbarColorStart(), theme->GetToolbarColorEnd());
	SolidBrush dbr(theme->GetDisabledOverlayColor());
	g.FillRectangle(&br, rc);
	g.FillRectangle(&dbr, rc);

	LinearGradientBrush glas(PointF(0.0f,0.0f), PointF(0.0f,float(rc.GetHeight())/2.0f), theme->GetGlassColorStart(), theme->GetGlassColorEnd());
	g.FillRectangle(&glas, RectF(0.0f, 0.0f, float(rc.GetWidth()), float(rc.GetHeight())/2.0f));

	Pen pn(theme->GetActiveEndColor(), 1.0f);
	g.DrawLine(&pn, PointF(0.0f, float(rc.GetHeight()-1.0f)), PointF(float(rc.GetWidth()), float(rc.GetHeight()-1.0f)));
	
	// knopjes!
	if(_path) {
		SolidBrush tbr(theme->GetActiveEndColor());
		SolidBrush atbr(theme->GetTextColor());

		std::vector< ref<Crumb> >::iterator it = _path->_crumbs.begin();
		int rx = 1;
		while(it!=_path->_crumbs.end()) {
			ref<Crumb> crumb = *it;
			std::wstring text = crumb->GetTextTrimmed();
			/*

			| margin_l | icon-size | text-size | margin_r

			*/
			RectF textrc;
			g.MeasureString(text.c_str(), int(text.length()), theme->GetGUIFont(), PointF(0.0f,0.0f), &textrc);
			RectF rtext(float(rx+KMarginLeft+KIconWidth), 4.0f, float(textrc.Width+1), float(15.0f));

			StringFormat sf;
			sf.SetAlignment(StringAlignmentFar);

			// active item draws differently
			if(it+1 == _path->_crumbs.end()) {
				LinearGradientBrush lbr(PointF(0.0f, 0.0f), PointF(0.0f, float(rc.GetHeight())), theme->GetActiveStartColor(), theme->GetActiveEndColor());
				g.FillRectangle(&lbr,RectF(float(rx), 1.0f, float(textrc.Width+KMarginLeft+KMarginRight+KIconWidth), float(rc.GetHeight()-3)));
				
				if(_over!=crumb) {
					SolidBrush dbr(theme->GetDisabledOverlayColor());
					g.FillRectangle(&dbr,RectF(float(rx), 1.0f, float(textrc.Width+KMarginLeft+KMarginRight+KIconWidth), float(rc.GetHeight()-3)));
				}
				
				g.DrawString(text.c_str(), int(text.length()), theme->GetGUIFont(), rtext, &sf, &atbr);
			}
			else { 
				if(_over==crumb) {
					LinearGradientBrush lbr(PointF(0.0f, 0.0f), PointF(0.0f, float(rc.GetHeight())), theme->GetActiveStartColor(), theme->GetActiveEndColor());
					g.FillRectangle(&lbr,RectF(float(rx), 1.0f, float(textrc.Width+KMarginLeft+KMarginRight+KIconWidth), float(rc.GetHeight()-3)));
					g.DrawString(text.c_str(), int(text.length()), theme->GetGUIFont(), rtext, &sf, &atbr);
				}
				else {
					g.DrawString(text.c_str(), int(text.length()), theme->GetGUIFont(), rtext, &sf, &tbr);
				}

				// draw separator after
				if(_separator!=0) {
					g.DrawImage(_separator, RectF(float(rx)+textrc.Width+KMarginLeft+KIconWidth-1,(rc.GetHeight()-KIconWidth)/2.0f, float(KIconWidth), float(KIconWidth)));
				}
			}

			// draw icon
			if(crumb->_icon!=0) {
				g.DrawImage(crumb->_icon, RectF(float(rx+KMarginLeft/2), (rc.GetHeight()-KIconWidth)/2.0f, float(KIconWidth), float(KIconWidth)));
			}

			rx += int(textrc.Width)+KMarginLeft+KMarginRight+KIconWidth;
			it++;
		}
	}
}

wchar_t PathWnd::GetPreferredHotkey() {
	return L'P';
}

LRESULT PathWnd::Message(UINT msg, WPARAM wp, LPARAM lp) {
	if(msg==WM_LBUTTONDOWN) {
		int x = GET_X_LPARAM(lp);
		int left = 0;
		ref<Crumb> cr = GetCrumbAt(x,&left);
		if(cr) {
			DoCrumbMenu(cr, left);
		}
		return 0;
	}
	else if(msg==WM_MOUSEMOVE) {
		int x = GET_X_LPARAM(lp);
		_over = GetCrumbAt(x);
		Repaint();
	}
	else if(msg==WM_MOUSELEAVE) {
		_over = 0;
		Repaint();
	}
	return ChildWnd::Message(msg,wp,lp);
}

void PathWnd::SetPath(ref<Path> p) {
	_path = p;
	Update();
}

void PathWnd::DoCrumbMenu(ref<Crumb> crumb, int x) {
	assert(_path);

	tj::shared::Rectangle rc = GetClientRectangle();

	ContextMenu cm;
	cm.AddItem(crumb->GetTextTrimmed(), 0, true, false);

	ref< std::vector< ref<Crumb> > > cr = crumb->GetChildren();
	if(cr && cr->size()>0) {
		cm.AddSeparator();
		std::vector< ref<Crumb> >::iterator it = cr->begin();
		int n = 2;
		while(it!=cr->end()) {
			ref<Crumb> child = *it;
			if(child) {
				cm.AddItem(child->GetText(), n, false, false);				
			}
			it++;
			n++;
		}
	}

	int command = cm.DoContextMenu(_wnd, x, rc.GetHeight()-1, true);
	ref<Crumb> next = 0;
	if(command==1) {
		next = crumb;
	}
	else if(command>1) {
		next = cr->at(command-2);
	}

	if(next && next!=_path->GetHead()) {
		ref<Path> np = GC::Hold(new Path());
		std::vector< ref<Crumb> >::iterator it = _path->_crumbs.begin();
		while(it!=_path->_crumbs.end()) {
			ref<Crumb> cr = *it;
			np->Add(cr);
			if(cr==crumb) {
				break;
			}
			it++;
		}

		if(next!=crumb) {
			np->Add(next);
		}

		if(_propertyGrid!=0 && next->GetSubject()) {
			_propertyGrid->Inspect(next->GetSubject(),np);
		}
	}
}