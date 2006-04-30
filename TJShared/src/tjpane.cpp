#include "../include/tjshared.h"

Pane::Pane(std::wstring title, ref<Wnd> window, bool detached) {
	_title = title;
	_wnd = window;
	_detached = detached;
}

ref<Wnd> Pane::GetWindow() {
	return _wnd;
}

FloatingPane::FloatingPane(RootWnd* rw, ref<Pane> p, TabWnd* source): Wnd(L"FloatingPane", 0, TJ_DEFAULT_CLASS_NAME, false) {
	assert(p);
	assert(p->_wnd);
	_pane = p;
	_root = rw;
	_source = source;
	SetStyleEx(WS_EX_PALETTEWINDOW);
	SetStyle(WS_OVERLAPPEDWINDOW|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU|WS_CAPTION);

	// adapt the window to this pane
	ref<Wnd> paneWnd = _pane->GetWindow();
	RECT sz;
	GetWindowRect(paneWnd->GetWindow(), &sz);
	SetWindowPos(_wnd, 0, sz.left+25, sz.top+25, sz.right-sz.left, sz.bottom-sz.top, SWP_NOZORDER);
	SetParent(paneWnd->GetWindow(), _wnd);

	// set window title
	int n = GetWindowTextLength(paneWnd->GetWindow());
	wchar_t* buf = new wchar_t[n+2];
	GetWindowText(paneWnd->GetWindow(), buf, n+1);
	SetWindowText(_wnd, buf);
	delete[] buf;

	Layout();
	paneWnd->Show(true);
	Show(true);
}

FloatingPane::~FloatingPane() {
}

LRESULT FloatingPane::Message(UINT msg, WPARAM wp, LPARAM lp) {
	if(msg==WM_SIZE) {
		Layout();
	}
	else if(msg==WM_CLOSE) {
		ShowWindow(_wnd, SW_HIDE);
		if(_source) {
			_source->Attach(_pane);
			_root->RemoveFloatingPane(_pane);
			return 0;
		}
	}
	else if(msg==WM_GETMINMAXINFO) {
		return _pane->GetWindow()->Message(msg,wp,lp);
	}
	return Wnd::Message(msg,wp,lp);
}

void FloatingPane::Layout() {
	//HWND child = GetWindow(_wnd, GW_CHILD);
	HWND child = _pane->GetWindow()->GetWindow();
	if(child!=0) {
		RECT r;
		GetClientRect(_wnd, &r);
		SetWindowPos(child, 0, 0,0, r.right-r.left, r.bottom-r.top, SWP_NOZORDER);
	}
}


void FloatingPane::Paint(Gdiplus::Graphics& g) {
}

wchar_t FloatingPane::GetPreferredHotkey() {
	return L'P';
}