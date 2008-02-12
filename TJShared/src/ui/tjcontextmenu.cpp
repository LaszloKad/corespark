#include "../../include/tjshared.h"
using namespace tj::shared;
using namespace Gdiplus;

/// TODO: 
// - Alle calls naar ContextMenu::DoContextMenu omschrijven (gebruikt nu Pixels ipv gerotzooi)
// - Tekenen en afhandelen mouse clicks/mouse overs in popup wnd
// - Mooie layout

const Pixels ContextMenu::KItemHeight = 19;
const Pixels ContextMenu::KMinContextMenuWidth = 150;

ContextMenu::ContextMenu() {
}

ContextMenu::~ContextMenu() {
}

/* If x=-1 and y=-1, we use the current cursor pos */
int ContextMenu::DoContextMenu(ref<Wnd> wnd, Pixels x, Pixels y) {
	// If there are no items, always fail
	if(_items.empty()) {
		return -1;
	}
	
	if(wnd) {
		if(x<0 || y<0) {
			ref<Theme> theme = ThemeManager::GetTheme();
			float df = theme->GetDPIScaleFactor();
			POINT px;
			if(GetCursorPos(&px)) {
				ScreenToClient(wnd->GetWindow(), &px);
				if(x<0) {
					x = Pixels(float(px.x) / df);
				}
				if(y<0) {
					y = Pixels(float(px.y) / df);
				}
			}
		}

		// Create popup
		EnableWindow(wnd->GetWindow(), FALSE);

		ref<ContextPopupWnd> cpw = GC::Hold(new ContextPopupWnd(this, wnd));

		// Calculate size of text
		ref<Theme> theme = ThemeManager::GetTheme();
		Area measured = theme->MeasureText(_longestString, theme->GetGUIFontBold());
		Pixels width = max(KMinContextMenuWidth, measured.GetWidth());

		// Showtime
		cpw->SetSize(width, Pixels(int(_items.size())*KItemHeight));
		cpw->PopupAt(x,y,wnd);
		int result =  cpw->DoModal();
		EnableWindow(wnd->GetWindow(), TRUE);
		return result;
	}

	Throw(L"Context menu must have an owner window", ExceptionTypeSevere);
}

int ContextMenu::DoContextMenu(ref<Wnd> wnd) {
	if(wnd) {	
		Area  rc = wnd->GetClientArea();
		return DoContextMenu(wnd, -1, -1);
	}

	Throw(L"Context menu must have an owner window", ExceptionTypeSevere);
}

/** if command == -1, then the item will be grayed out/disabled **/
void ContextMenu::AddItem(const std::wstring& name, int command, bool hilite, bool radiocheck) {
	AddItem(name,command,hilite, radiocheck?ContextItem::Checked: ContextItem::NotChecked);
}

void ContextMenu::AddItem(const std::wstring& name, int command, bool hilite, ContextItem::CheckType checked) {
	if(name.length() > _longestString.length()) {
		_longestString = name;
	}
	ContextItem ci(name,command,hilite,checked);
	_items.push_back(ci);
}

void ContextMenu::AddSeparator() {
	_items.push_back(ContextItem());
}

/** ContextPopupWnd **/
ContextPopupWnd::ContextPopupWnd(ContextMenu* cm, ref<Wnd> parent): PopupWnd(parent), _cm(cm), _result(-1), _mouseOver(-1), _mouseDown(-1), _checkedIcon(L"icons/shared/check.png"), _radioCheckedIcon(L"icons/shared/radiocheck.png") {
	SetWantMouseLeave(true);
}

ContextPopupWnd::~ContextPopupWnd() {
}

int ContextPopupWnd::DoModal() {
	_result = -1;
	ModalLoop::Result res = _loop.Enter();
	if(res==ModalLoop::ResultSucceeded) {
		return _result;
	}

	return -1;
}

void ContextPopupWnd::EndModal(int r) {
	_result = r;
	_loop.End(ModalLoop::ResultSucceeded);
}

void ContextPopupWnd::OnMouse(MouseEvent ev, Pixels x, Pixels y) {
	if(ev==MouseEventMove || ev==MouseEventLDown) {
		int idx = y/ContextMenu::KItemHeight;
		if(idx>=0 && idx<int(_cm->_items.size())) {
			_mouseOver = idx;
			if(ev==MouseEventLDown) {
				_mouseDown = idx;
			}
			else {
				_mouseDown = -1;
			}
		}
		else {
			_mouseOver = -1;
			_mouseDown = -1;
		}
		Repaint();
	}
	else if(ev==MouseEventLUp) {
		int idx = y/ContextMenu::KItemHeight;
		if(idx>=0 && idx<int(_cm->_items.size())) {
			const ContextItem& ci = _cm->_items.at(idx);
			if(!ci.IsDisabled() || ci.IsSeparator()) {
				int command = _cm->_items.at(idx)._command;
				EndModal(command);
			}
		}
	}
	else if(ev==MouseEventLeave) {
		_mouseOver = -1;
		Repaint();
	}
	PopupWnd::OnMouse(ev,x,y);
}

void ContextPopupWnd::OnActivate(bool a) {
	PopupWnd::OnActivate(a);

	if(!a) {
		_loop.End(ModalLoop::ResultCancelled);
	}
}

void ContextPopupWnd::OnKey(Key k, wchar_t ch, bool down) {
	switch(k) {
		case KeyDown:
			if(down) {
				++_mouseOver;
				if(_mouseOver > int(_cm->_items.size())-1) {
					_mouseOver = 0;
				}
				_mouseDown = -1;
			}
			break;

		case KeyUp:
			if(down) {
				--_mouseOver;
				if(_mouseOver < 0) {
					_mouseOver = int(_cm->_items.size())-1;
				}
				_mouseDown = -1;
			}
			break;
		case KeyCharacter:
			if(ch==L'\n') {
				if(down) {
					_mouseDown = _mouseOver;
				}
				else {
					if(_mouseDown >= 0 && _mouseDown < int(_cm->_items.size())) {
						EndModal(_cm->_items.at(_mouseDown)._command);
					}
				}
			}
			break;
	}
}

void ContextPopupWnd::Paint(Gdiplus::Graphics& g, ref<Theme> theme) {
	Area rc = GetClientArea();
	SolidBrush back(theme->GetBackgroundColor());
	g.FillRectangle(&back, rc);

	SolidBrush text(theme->GetTextColor());
	SolidBrush disabled(theme->GetDisabledOverlayColor());
	SolidBrush colorSeparator(theme->GetActiveEndColor());
	Pen separator(&colorSeparator, 1.0f);
	LinearGradientBrush selected(PointF(0.0f, 0.0f), PointF(0.0f, REAL(ContextMenu::KItemHeight)), theme->GetTimeSelectionColorStart(), theme->GetTimeSelectionColorEnd());
	LinearGradientBrush down(PointF(0.0f, 0.0f), PointF(0.0f, REAL(ContextMenu::KItemHeight)), theme->GetTimeSelectionColorEnd(), theme->GetTimeSelectionColorStart());
	
	StringFormat sf;
	sf.SetAlignment(StringAlignmentNear);
	sf.SetLineAlignment(StringAlignmentCenter);
	sf.SetTrimming(StringTrimmingEllipsisPath);
	REAL tabs[] = {50, 50, 50};
	sf.SetTabStops(0, 3, tabs);

	Pixels y = 0;
	int n = 0;
	std::vector<ContextItem>::const_iterator it = _cm->_items.begin();
	while(it!=_cm->_items.end()) {
		const ContextItem& item = *it;
		Area current = rc;
		current.SetY(y);
		current.SetHeight(ContextMenu::KItemHeight);

		if(item.IsSeparator()) {
			current.Narrow(ContextMenu::KItemHeight,0,ContextMenu::KItemHeight,0);
			Pixels y = current.GetTop() + (current.GetBottom()-current.GetTop())/2;
			g.DrawLine(&separator, current.GetLeft(), y, current.GetRight(), y);
		}
		else {
			if(n==_mouseDown) {
				Area selection = current;
				selection.Narrow(1,0,1,0);
				g.FillRectangle(&down, selection);
			}
			else if(n==_mouseOver) {
				Area selection = current;
				selection.Narrow(1,0,1,0);
				g.FillRectangle(&selected, selection);
			}

			Area currentText = current;
			currentText.Narrow(ContextMenu::KItemHeight,2,2,2);
			g.DrawString(item._title.c_str(), (int)item._title.length(), item._hilite?theme->GetGUIFontBold():theme->GetGUIFont(), currentText, &sf, &text);

			if(item._checked!=ContextItem::NotChecked) {
				Area iconArea = current;
				iconArea.Narrow(3,1,1,1);
				iconArea.SetWidth(16);
				iconArea.SetHeight(16);
				g.DrawImage((item._checked == ContextItem::Checked)?_checkedIcon:_radioCheckedIcon, iconArea);
			}

			if(item.IsDisabled()) {
				g.FillRectangle(&disabled, current);
			}
		}

		y += ContextMenu::KItemHeight;
		++it;
		++n;
	}

	// Draw border
	rc.Narrow(0,0,1,1);
	SolidBrush border(theme->GetActiveStartColor());
	Pen pn(&border, 1.0f);
	g.DrawRectangle(&pn, rc);
}

/** ContextItem */
ContextItem::ContextItem(): _separator(true), _hilite(false), _checked(NotChecked), _command(0) {
}

ContextItem::ContextItem(const std::wstring& title, int command, bool highlight, CheckType checked): _title(title), _command(command), _hilite(highlight), _checked(checked), _separator(false) {
}

ContextItem::~ContextItem() {
}

bool ContextItem::IsDisabled() const {
	return _command == -1;
}

bool ContextItem::IsSeparator() const {
	return _separator;
}