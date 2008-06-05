#include "../../include/tjshared.h"
using namespace tj::shared::graphics;
using namespace tj::shared;

ButtonWnd::ButtonWnd(const wchar_t* image, const wchar_t* text): ChildWnd(L"", false, true), _disabled(false) {
	SetWantMouseLeave(true);
	std::wstring fn = ResourceManager::Instance()->Get(image);
	_image =  Bitmap::FromFile(fn.c_str(),TRUE);
	_down = false;

	if(text!=0) {
		_text = text;
	}
}

ButtonWnd::~ButtonWnd() {
	delete _image;
}

void ButtonWnd::Fill(LayoutFlags f, Area& r, bool direct) {
	ref<Theme> theme = ThemeManager::GetTheme();
	Pixels h = theme->GetMeasureInPixels(Theme::MeasureToolbarHeight);
	if(f==LayoutTop) {
		if(direct) Move(r.GetLeft(), r.GetTop(), r.GetWidth(), h);
		r.Narrow(0,h,0,0);
	}
	else if(f==LayoutBottom) {
		if(direct) Move(r.GetLeft(), (r.GetTop()+r.GetHeight())-h, r.GetWidth(), h);
		r.Narrow(0,0,0,h);
	}
	else {
		ChildWnd::Fill(f, r, direct);
	}
}

void ButtonWnd::SetDisabled(bool d) {
	_disabled = d;
	Repaint();
}

bool ButtonWnd::IsDisabled() const {
	return _disabled;
}


void ButtonWnd::SetText(const wchar_t* t) {
	_text = t;
	Repaint();
}

void ButtonWnd::OnKey(Key k, wchar_t t, bool down, bool isAccelerator) {
	if(!isAccelerator) {
		if(k==KeyCharacter && t==VK_SPACE) {
			if(down) {
				_down = true;
			}
			else {
				if(!_disabled) {
					ref<Listener> listener = _listener;
					if(listener) {
						listener->Notify(this, NotificationClick);
					}
				}
				_down = false;
			}
			Repaint();
		}
	}
}

void ButtonWnd::OnFocus(bool f) {
	Repaint();
}

void ButtonWnd::SetListener(ref<Listener> lr) {
	_listener = lr;
}

void ButtonWnd::Paint(Graphics& g, ref<Theme> theme) {
	Area rc = GetClientArea();

	// TODO: make changeable at runtime
	static const Pixels KImgHeight = 16;
	static const Pixels KImgWidth = 16;

	// Fill background
	LinearGradientBrush backGradient(PointF(0.0f, 0.0f), PointF(0.0f, (float)rc.GetHeight()), theme->GetActiveStartColor(), theme->GetActiveEndColor());
	LinearGradientBrush shadowGradient(PointF(0.0f, (float)rc.GetHeight()/2), PointF(0.0f, 0.0f), Color(0,0,0,0), Color(50,0,0,0));
	LinearGradientBrush highlightGradient(PointF(0.0f, (float)rc.GetHeight()/2), PointF(0.0f, 0.0f), Color(0,255,255,255), Color(50,255,255,255));
	SolidBrush disabledBrush(theme->GetDisabledOverlayColor());

	g.FillRectangle(&backGradient, rc);
	Area shadowRC = rc;
	shadowRC.Narrow(0,0,0,rc.GetHeight()/2);

	if(_down) {
		g.FillRectangle(&shadowGradient, shadowRC);
	}
	else {
		g.FillRectangle(&disabledBrush, rc);
		g.FillRectangle(&highlightGradient, shadowRC);
	}
	
	// Draw icon
	Pixels margin = (rc.GetHeight()-KImgHeight)/2;
	
	if(_image!=0) {
		g.DrawImage(_image, Area(rc.GetLeft()+margin, rc.GetTop()+margin, KImgWidth, KImgHeight));
	}
	
	// Draw border
	SolidBrush border(theme->GetActiveEndColor());
	rc.Narrow(0,0,1,1);
	Pen borderPen(&border,1.0f);
	g.DrawRectangle(&borderPen, rc);

	// Draw text
	Font* fnt = theme->GetGUIFontBold();
	SolidBrush textBrush(theme->GetTextColor());
	StringFormat sf;
	sf.SetLineAlignment(StringAlignmentNear);
	sf.SetLineAlignment(StringAlignmentCenter);
	sf.SetTrimming(StringTrimmingEllipsisPath);
	sf.SetFormatFlags(StringFormatFlagsLineLimit);

	g.DrawString(_text.c_str(), (unsigned int)_text.length(), fnt,RectF(float(rc.GetLeft()+margin+KImgWidth+margin), (float)rc.GetTop(), (float)rc.GetRight(), (float)rc.GetBottom()), &sf, &textBrush);

	if(_disabled) {
		g.FillRectangle(&disabledBrush, rc);
	}
}

void ButtonWnd::OnMouse(MouseEvent ev, Pixels x, Pixels y) {
	if(ev==MouseEventLDown) {
		Focus();
		_down = true;
		Repaint();
	}
	else if(ev==MouseEventLUp) {
		if(!_disabled) {
			ref<Listener> listener = _listener;
			if(listener) {
				listener->Notify(this, NotificationClick);
			}
		}

		_down = false;
		Repaint();
	}
	else if(ev==MouseEventMove) {
		if(IsKeyDown(KeyMouseLeft)) {
			_down = true;
		}
		Repaint();
	}
	else if(ev==MouseEventLeave) {
		_down = false;
		Repaint();
	}
	ChildWnd::OnMouse(ev,x,y);
}

LRESULT ButtonWnd::Message(UINT msg, WPARAM wp, LPARAM lp) {
	return ChildWnd::Message(msg,wp,lp);
}

/* StateButtonWnd */
StateButtonWnd::StateButtonWnd( const wchar_t* imageOn, const wchar_t* imageOff, const wchar_t* imageOther):
ButtonWnd(imageOn) {
	std::wstring fn = ResourceManager::Instance()->Get(imageOff);
	_offImage =  Bitmap::FromFile(fn.c_str(),TRUE);
	fn = ResourceManager::Instance()->Get(imageOn);
	_otherImage =  Bitmap::FromFile(fn.c_str(),TRUE);
	_on = Off;
}

StateButtonWnd::~StateButtonWnd() {
	delete _offImage;
	delete _otherImage;
}

void StateButtonWnd::Paint(Graphics& g, ref<Theme> theme) {
	Area rc = GetClientArea();

	SolidBrush backBr(IsMouseOver()?theme->GetActiveEndColor():theme->GetTimeBackgroundColor());
	g.FillRectangle(&backBr, rc);
	
	// Choose and draw icon
	Bitmap* img = 0;
	switch(_on) {
		case On:
			img = _image;
			break;
		case Off:
		default:			
			img = _offImage;
			break;
		case Other:
			img = _otherImage;
	}
	
	if(img!=0) {
		// TODO make changeable at runtime
		static const Pixels KImgHeight = 16;
		static const Pixels KImgWidth = 16;

		Pixels margin = (rc.GetHeight()-KImgHeight)/2;
		g.DrawImage(img, Area(margin,margin,KImgHeight, KImgWidth));
	}
}

LRESULT StateButtonWnd::Message(UINT msg, WPARAM wp, LPARAM lp) {
	if(msg==WM_MOUSEMOVE) {
		TRACKMOUSEEVENT evt;
		evt.cbSize = sizeof(evt);
		evt.dwFlags = TME_LEAVE;
		evt.dwHoverTime = 0;
		evt.hwndTrack = GetWindow();
		TrackMouseEvent(&evt);
		Repaint();
	}
	else if(msg==WM_MOUSELEAVE) {
		Repaint();
	}

	return ButtonWnd::Message(msg,wp,lp);
}

void StateButtonWnd::SetOn(ButtonState o) {
	_on = o;
	Repaint();
}
