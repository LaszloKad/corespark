#include "../include/tjshared.h"
#include <windowsx.h>
using namespace Gdiplus;
using namespace tj::shared;

#define ISVKKEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000))

SliderWnd::SliderWnd(HWND parent, const wchar_t* title): ChildWnd(title, parent) {
	_value = 0.0f;
	_hotkey = L'\0';
	_hasFocus = false;
	_listener = 0;
	_displayValue = 0.0f;
	_flash = false;
	_oldValue = 0.0f;
	_mark = -1.0f;
	_showValue = true;
	_snapHalf = false;
}

void SliderWnd::SetShowValue(bool t) {
	_showValue = t;
}

void SliderWnd::SetSnapToHalf(bool s) {
	_snapHalf = s;
}

float SliderWnd::GetValue() const {
	return _value;
}

void SliderWnd::SetColor(int idx) {
	if(_color!=idx) {
		_color = idx;
		Repaint();
	}
}

void SliderWnd::Update() {
	Repaint();
}

void SliderWnd::Fill(LayoutFlags flags, tj::shared::Rectangle& r) {
	if(flags==LayoutLeft) {
		Move(r.GetLeft(), r.GetTop(), KMinimumWidth, r.GetHeight());
		r.Narrow(KMinimumWidth,0,0,0);
	}
	else if(flags==LayoutRight) {
		Move(r.GetLeft()+r.GetWidth()-KMinimumWidth, r.GetTop(), KMinimumWidth, r.GetHeight());
		r.Narrow(0,0,KMinimumWidth,0);
	}
	else {
		ChildWnd::Fill(flags,r);
	}
}

void SliderWnd::SetDisplayValue(float v, bool notify) {
	if(_displayValue!=v) {
		_displayValue = v;

		if(notify) {
			Repaint();
			if(_listener!=0) {
				_listener->Notify(this, NotificationChanged);
			}
		}
	}
}

void SliderWnd::SetMarkValue(float v) {
	_mark = v;
	//Repaint();
}

void SliderWnd::SetValue(float f, bool notify) {
	if(_value!=f) {
		if(f<0.0f) f = 0.0f;
		if(f>1.0f) f = 1.0f;
		_value = f;
		
		if(notify) {
			Repaint();
			if(_listener!=0) {
				_listener->Notify(this, NotificationChanged);
			}
		}
	}
}

SliderWnd::~SliderWnd() {
}

void SliderWnd::Paint(Graphics& g) {
	if(_listener!=0) {
		_listener->Notify(this, NotificationUpdate);
	}
	
	ref<Theme> theme = ThemeManager::GetTheme();
	RECT rect;
	GetClientRect(_wnd, &rect);

	Gdiplus::Color colorStart = theme->GetSliderColorStart(_color);
	Gdiplus::Color colorEnd = theme->GetSliderColorEnd(_color);

	// background
	SolidBrush backBrush(theme->GetBackgroundColor());
	g.FillRectangle(&backBrush,Rect(0,0,rect.right-rect.left, rect.bottom-rect.top));

	// middle rectangle, 6 pixels wide
	rect.top += 5;
	rect.bottom -= 30;
	if(!_showValue) rect.bottom += 15;
	LinearGradientBrush br(PointF(0.0f, float(rect.top-10)), PointF(0.0f, float(rect.bottom-rect.top+15)), colorStart, colorEnd);
	Pen pn(&br, 1.0f);
	const static int squareWidth = 6;
	int x = (rect.right-rect.left)/2 - (squareWidth/2);
	g.DrawRectangle(&pn, RectF(float(x), float(rect.top), float(squareWidth), float(rect.bottom-rect.top)));

	if(_displayValue>0.0f) {
		float dvh = (1.0f-_displayValue) * (rect.bottom-rect.top);
		g.FillRectangle(&br,RectF(float(x+2), float(float(rect.top)+dvh), float(squareWidth-3), float(rect.bottom-rect.top)-dvh));
	}

	// markers
	int mx = (rect.right-rect.left)/2 + (squareWidth/2);
	for(float my=0.0f;my<=1.0f;my+=0.1f) {
		float mty = float(int(rect.bottom) - int(my*int(rect.bottom-rect.top)));
		g.DrawLine(&pn, (REAL)mx, mty, (REAL)mx+2,mty);
	}

	if(_mark != _value && _mark <= 1.0f && _mark >= 0.0f) {
		mx = (rect.right-rect.left)/2 - (squareWidth/2);
		Pen mpn(theme->GetCommandMarkerColor(), 3.0f);
		float mty = float(int(rect.bottom) - int(_mark*int(rect.bottom-rect.top)));
		g.DrawLine(&mpn, (REAL)mx, mty, (REAL)mx+squareWidth+1,mty);
	}

	// larger markers at 0.0, 0.5, 1.0
	float mty = float(int(rect.bottom) - int(0.5f*int(rect.bottom-rect.top)));
	g.DrawLine(&pn, (REAL)mx, mty, (REAL)mx+4,mty);
	mty = float( int(rect.bottom) - int(1.0f*int(rect.bottom-rect.top)));
	g.DrawLine(&pn, (REAL)mx, mty, (REAL)mx+4,mty);
	mty = float(int(rect.bottom) - int(0.0f*int(rect.bottom-rect.top)));
	g.DrawLine(&pn, (REAL)mx, mty, (REAL)mx+4,mty);

	// dragger
	const static int draggerWidth = 22;
	x = (rect.right-rect.left)/2 - draggerWidth/2;
	int y = rect.bottom - int(_value*int(rect.bottom-rect.top));
	SolidBrush border(colorEnd);
	g.FillRectangle(&border, RectF(float(x), float(y), float(draggerWidth), 6.0f));
	g.FillRectangle(&backBrush, RectF(float(x+1), float(y+1), float(draggerWidth-2), 4.0f));
	
	if(_hasFocus||_flash) {
		LinearGradientBrush lbr(PointF(float(x+1), float(y)), PointF(float(x+1), float(y+6)), colorStart, colorEnd );
		g.FillRectangle(&lbr, RectF(float(x+1), float(y+1), float(draggerWidth-2), 4.0f));
	}

	if(_showValue) {
		std::wostringstream os;
		os << int(_value*100) << L'%';
		std::wstring msg = os.str();
		StringFormat sf;
		sf.SetAlignment(StringAlignmentCenter);
		SolidBrush tbr(theme->GetTextColor());
		g.DrawString(msg.c_str(), (INT)msg.length(), theme->GetGUIFont(), RectF(0.0f, float(rect.bottom+15), float(rect.right), 16.0f), &sf, &tbr);
	}
}

LRESULT SliderWnd::Message(UINT msg, WPARAM wp, LPARAM lp) {
	if(msg==WM_CONTEXTMENU) {
		msg = WM_RBUTTONDOWN;
	}

	if(msg==WM_MOUSEMOVE || msg==WM_LBUTTONDOWN || msg==WM_RBUTTONDOWN) {
		if(ISVKKEYDOWN(VK_LBUTTON) || ISVKKEYDOWN(VK_RBUTTON)) {
			int y = GET_Y_LPARAM(lp);
			RECT rc;
			GetClientRect(_wnd, &rc);
			rc.top += 5;
			rc.bottom -= 30;
			if(!_showValue) rc.bottom += 15;

			y -= rc.top;
			float val = float(y)/float(rc.bottom-rc.top);

			if(msg==WM_RBUTTONDOWN) {
				if(!_flash) {
					_oldValue = _value;
					_flash = true;
				}
			}
			
			if(_snapHalf && val<0.51f && val>0.49f) {
				SetValue(0.5f);
			}
			else {
				SetValue(1.0f - val);
			}
		}
	}
	else if(msg==WM_KEYUP || msg==WM_RBUTTONUP) {
		if(_flash) {
			SetValue(_oldValue);
		}
		_flash = false;
	}
	else if(msg==WM_KEYDOWN) {
		if(ISVKKEYDOWN(VK_CONTROL)) {
			if(!_flash) {
				_flash = true;	
				_oldValue = _value;
			}
		}
		
		if(wp==VK_DOWN) {
			SetValue(_value - (1.0f/255.0f));
		}
		else if(wp==VK_UP) {
			SetValue(_value+(1.0f/255.0f));
		}
		else if(wp==VK_NEXT) {
			SetValue(0.0f);
		}
		else if(wp==VK_PRIOR) {
			SetValue(1.0f);
		}
		else if(wp>=L'0'&& wp <= L'9' || wp == VK_OEM_3) {
			float v = (wp - L'0')*0.1f;
			if(wp==L'0') {
				v = 1.0f;
			}
			else if(wp==VK_OEM_3) {
				v = 0.0f;
			}

			SetValue(v);
		}
		else {
			HWND first = ::GetWindow(_wnd, GW_HWNDFIRST);
			HWND last = ::GetWindow(_wnd, GW_HWNDLAST);

			if(wp==VK_LEFT) {
				if(_wnd==last) {
					SetFocus(first);
				}
				else {
					HWND next = ::GetWindow(_wnd, GW_HWNDNEXT);
					SetFocus(next);
				}
			}
			else if(wp==VK_RIGHT) {
				if(_wnd==first) {
					SetFocus(last);
				}
				else {
					HWND next = ::GetWindow(_wnd, GW_HWNDPREV);
					SetFocus(next);
				}
			}
		}
	}
	else if(msg==WM_MOUSEWHEEL) {
		int delta = GET_WHEEL_DELTA_WPARAM(wp);
		if(delta<0) {
			SetValue(max(0.0f, _value - 0.05f));
			Repaint();
		}
		else {
			SetValue(min(1.0f,_value+0.05f));
			Repaint();
		}
	}
	else if(msg==WM_SETFOCUS) {
		_hasFocus = true;
		Repaint();
	}
	else if(msg==WM_KILLFOCUS) {
		_hasFocus = false;
		Repaint();
	}

	return ChildWnd::Message(msg, wp, lp);
}

wchar_t SliderWnd::GetPreferredHotkey() {
	return _hotkey;
}

void SliderWnd::SetHotkey(wchar_t hotkey) {
	_hotkey = hotkey;
}

void SliderWnd::SetListener(Listener* listener) {
	_listener = listener;
}