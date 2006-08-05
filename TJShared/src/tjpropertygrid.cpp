#include "../include/tjshared.h"
using namespace tj::shared;
using namespace Gdiplus;

/* PropertyGridWnd implementation */
PropertyGridWnd::PropertyGridWnd(HWND parent): ChildWnd(TL(properties), parent) {
	ClearThemeCache();
	_nameWidth = 100;
	SetStyle(WS_CLIPCHILDREN);
	Layout();
	_editBackground = 0;
	_editFont = 0;
	_isDraggingSplitter = false;
	Show(true);
	
}

PropertyGridWnd::~PropertyGridWnd() {
	if(_editBackground!=0) DeleteObject(_editBackground);
	_editBackground = 0;

	if(_editFont!=0) DeleteObject(_editFont);
	_editFont = 0;
}

void PropertyGridWnd::ClearThemeCache() {
	if(_editBackground!=0) DeleteObject(_editBackground);
	Color back = ThemeManager::GetTheme()->GetPropertyBackgroundColor();
	_editBackground = CreateSolidBrush(RGB(back.GetRed(),back.GetGreen(),back.GetBlue()));

	if(_editFont!=0) DeleteObject(_editFont);
	_editFont = CreateFont(-10, 0, 0, 0, 400, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, TL(ui_font));
}

void PropertyGridWnd::EnterHotkeyMode() {
	if(_properties.size()<1) {
		// no properties -> no fun
		return;
	}

	ChildWnd::EnterHotkeyMode();
}

void PropertyGridWnd::LeaveHotkeyMode(wchar_t key) {
	if(IsInHotkeyMode()) {
		_inHotkeyMode = false;

		if(key!=L'\0'&& key >= L'0' && key <= L'9') {
			unsigned int index = key - L'0';
			try {
				ref<Property> prop = _properties.at(index);
				SetFocus(prop->GetWindow());
			}
			catch(...) {
			}
		}
	}
	Update();
}

wchar_t PropertyGridWnd::GetPreferredHotkey() {
	return L'P';
}

void PropertyGridWnd::Paint(Graphics& g) {
	RECT r;
	GetClientRect(_wnd, &r);
	ref<Theme> theme = ThemeManager::GetTheme();

	SolidBrush br(theme->GetPropertyBackgroundColor());
	g.FillRectangle(&br,-1,-1,r.right-r.left+1,r.bottom-r.top+1);

	int cH = 0;
	int hI = 0;
	std::vector< ref<Property> >::iterator it = _properties.begin();

	// TODO: cache theme colors over here
	float stringLeft = IsInHotkeyMode()?(6.0f+16.0f):5.0f;
	while(it!=_properties.end()) {
		ref<Property> p = *it;
		if(!p) continue;

		p->Update();
		bool bold = p.IsCastableTo<PropertySeparator>();

		if(GetFocus()==p->GetWindow() || bold) {
			LinearGradientBrush gbr(Gdiplus::Point(0, cH), Gdiplus::Point(0, cH+p->GetHeight()+10), theme->GetActiveStartColor(), theme->GetActiveEndColor());
			g.FillRectangle(&gbr, Rect(1, cH+1, r.right-r.left-2, p->GetHeight()+6-2));
		}

		if(bold) {
			SolidBrush dbr(theme->GetDisabledOverlayColor());
			g.FillRectangle(&dbr, Rect(1, cH+1, r.right-r.left-2, p->GetHeight()+6-2));
		}

		std::wstring ws = p->GetName();
		SolidBrush tb(theme->GetTextColor());
		
		g.DrawString(ws.c_str(), (int)ws.length(),bold?theme->GetGUIFontBold():theme->GetGUIFont(), PointF(stringLeft, float(cH+5)), &tb);
		
		if(IsInHotkeyMode()&&hI<=9) {
			std::wostringstream wos;
			wos << hI;
			std::wstring id = wos.str();
			DrawHotkey(&g, id.c_str(),8+3, cH+(p->GetHeight()/2)+3+1);
		}
		
		cH += p->GetHeight() + 6;

		it++;
		hI++;
	}
}

LRESULT PropertyGridWnd::Message(UINT msg, WPARAM wParam, LPARAM lParam) {
	if(msg==WM_CLOSE) {
		ShowWindow(_wnd,SW_HIDE);
		return 1;
	}
	else if(msg==WM_COMMAND) {
		if(HIWORD(wParam)==EN_UPDATE||HIWORD(wParam)==BN_CLICKED||HIWORD(wParam)==CBN_SELCHANGE) {
			HWND he = (HWND)lParam;

			std::vector< ref<Property> >::iterator it = _properties.begin();
			while(it!=_properties.end()) {
				ref<Property> prw = (*it);
				assert(prw!=0);

				if(prw->GetWindow()==he) {
					prw->Changed();
					break;
				}
				it++;
			}
		}
		else if(HIWORD(wParam)==EN_SETFOCUS) {
			Layout();
		}
		
		if(HIWORD(wParam)==EN_CHANGE /*||HIWORD(wParam)==EN_UPDATE */ || HIWORD(wParam)==BN_CLICKED||HIWORD(wParam)==CBN_SELCHANGE) {
			Wnd* root = GetRootWindow();
			if(root!=0) {
				root->Update();
			}
			
		}
	}
	else if(msg==WM_CTLCOLOREDIT) {
		wchar_t className[100];
		GetClassName((HWND)lParam, className, 100-1);
		if(_wcsicmp(className, L"COMBOBOX")!=0) {
			SetBkMode((HDC)wParam, TRANSPARENT);
			Color text = ThemeManager::GetTheme()->GetTextColor();
			SetTextColor((HDC)wParam, RGB(text.GetRed(),text.GetGreen(),text.GetBlue()));
			return (LRESULT)(HBRUSH)_editBackground;
		}
	}
	else if(msg==WM_CTLCOLORBTN) {
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)(HBRUSH)_editBackground;
	}
	else if(msg==WM_CTLCOLORSTATIC) {
		SetBkMode((HDC)wParam, TRANSPARENT);
		Color text = ThemeManager::GetTheme()->GetTextColor();
		//Color text(255,0,0);
		SetTextColor((HDC)wParam, RGB(text.GetRed(),text.GetGreen(),text.GetBlue()));
		return (LRESULT)(HBRUSH)_editBackground;
	}
	else if(msg==WM_SIZE) {
		Layout();
		return 0;
	}
	else if(msg==WM_ACTIVATE) {
		Layout();
		return 0;
	}
	else if(msg==WM_LBUTTONUP) {
		_isDraggingSplitter = false;
		Layout();
		ReleaseCapture();
		return 0;
	}
	else if(msg==WM_LBUTTONDOWN) {
		int x = GET_X_LPARAM(lParam);
		if(x>(_nameWidth-5) && x<(_nameWidth+5)) {
			_isDraggingSplitter = true;
			SetCapture(_wnd);
		}
	}
	else if(msg==WM_MOUSEMOVE) {
		int x = GET_X_LPARAM(lParam);

		if(_isDraggingSplitter) {
			_nameWidth = x;
			Layout();
		}

		if(x>(_nameWidth-5) && x<(_nameWidth+5)) {
			SetCursor(LoadCursor(0, IDC_SIZEWE));
		}
		else {
			SetCursor(LoadCursor(0, IDC_ARROW));
		}
	}

	return ChildWnd::Message(msg,wParam, lParam);
}

void PropertyGridWnd::Layout() {
	RECT rect;
	GetClientRect(_wnd, &rect);

	int cH = 0;
	std::vector< ref<Property> >::iterator it = _properties.begin();
	while(it!=_properties.end()) {
		ref<Property> pr = *it;
		assert(pr!=0);
		HWND vw = pr->GetWindow();
		int h = pr->GetHeight();
		SendMessage(vw, WM_SETFONT, (WPARAM)(HFONT)_editFont, FALSE);
		SetWindowPos(vw, 0,  _nameWidth, cH+3,  rect.right-rect.left-_nameWidth-3, h, SWP_NOZORDER);
		ShowWindow(vw, SW_SHOW);
		cH += h + 6;
		it++;
	}
	Repaint();
}

void PropertyGridWnd::Update() {
	std::vector< ref<Property> >::iterator it = _properties.begin();
	while(it!=_properties.end()) {
		ref<Property> pr = (*it);
		pr->Update();
		it++;
	}
	Repaint();
}

void PropertyGridWnd::Clear() {
	_properties.clear();
	Layout();
}

void PropertyGridWnd::Inspect(Inspectable* isp) {
	_properties.clear();

	if(isp==0) {	
		ClearThemeCache();
		Layout();
		return;
	}

	ref< std::vector< ref<Property> > > props = isp->GetProperties();
	if(!props) {
		ClearThemeCache();
		Layout();
		return;
	}

	_properties.clear();
	std::vector< ref<Property> >::iterator it = props->begin();
	HWND first = 0;
	while(it!=props->end()) {
		ref<Property> pr = *it;
		if(pr!=0) {
			HWND f = pr->Create(_wnd);
			if(first==0) {
				first = f;
			}
			// make sure we're not having any GWL_USERDATA stuff attached to our HWND, as the hotkey-system
			// might choke on it
			//SetWindowLong(pr->GetWindow(), GWL_USERDATA,0);
			_properties.push_back(pr);
		}
		it++;
	}

	SetFocus(first);
	ClearThemeCache();
	Layout();
}

void PropertyGridWnd::Inspect(ref<Inspectable> isp) {
	if(!isp) {
		_properties.clear();
		return;
	}

	Inspectable* is = isp.GetPointer();
	if(is==0) {
		Throw(L"GetPointer==0!", ExceptionTypeError);
	}
	Inspect(is);
}