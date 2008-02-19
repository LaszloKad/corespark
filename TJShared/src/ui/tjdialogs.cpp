#include "../../include/tjshared.h"
using namespace tj::shared;
using namespace Gdiplus;

DialogWnd::DialogWnd(const std::wstring& title, const std::wstring& question): TopWnd(title.c_str()), _question(question) {
	SetStyle(WS_POPUPWINDOW|WS_THICKFRAME);
	_grid = GC::Hold(new PropertyGridWnd(false));
	_ok = GC::Hold(new ButtonWnd(Icons::GetIconPath(Icons::IconOK).c_str(), TL(ok)));
	Add(_grid, true);
	Add(_ok, true);

	SetSize(640,480);
	Layout();
}

DialogWnd::~DialogWnd() {
}

void DialogWnd::OnCreated() {
	_ok->SetListener(this);
}

void DialogWnd::Notify(Wnd* source, Notification evt) {
	if(evt==NotificationClick) {
		if(source==_ok.GetPointer()) {
			EndModal(true);
		}
	}
}

void DialogWnd::Layout() {
	static const Pixels KOKButtonWidth = 90;
	static const Pixels KOKButtonHeight = 20;

	Area rc = GetClientArea();
	Pixels margin = (KHeaderHeight-KOKButtonHeight)/2;
	_ok->Move(rc.GetRight()-KOKButtonWidth-margin, rc.GetBottom()-KHeaderHeight+margin, KOKButtonWidth, KOKButtonHeight);

	rc.Narrow(0,KHeaderHeight,0,KHeaderHeight);
	_grid->Fill(LayoutFill, rc);	
}

bool DialogWnd::DoModal(HWND parent) {
	// Get root window of parent
	HWND root = GetAncestor(parent, GA_ROOT);
	EnableWindow(root,FALSE);

	/* Center this window */
	ref<Theme> theme = ThemeManager::GetTheme();
	float df = theme->GetDPIScaleFactor();
	Area rc = GetClientArea();
	int w = int(rc.GetWidth()*df);
	int h = int(rc.GetHeight()*df);

	// Get root rectangle
	RECT rootRect;
	GetWindowRect(root, &rootRect);
	SetWindowPos(GetWindow(), 0L, rootRect.left + ((rootRect.right-rootRect.left - w)/2), rootRect.top + ((rootRect.bottom-rootRect.top - h)/2), 0,0, SWP_NOSIZE|SWP_NOZORDER);
	Show(true);

	ModalLoop::Result result = _loop.Enter();

	EnableWindow(root, TRUE);
	SetForegroundWindow(root);
	return result == ModalLoop::ResultSucceeded;
}

LRESULT DialogWnd::Message(UINT msg, WPARAM wp, LPARAM lp) {
	if(msg==WM_CLOSE) {
		EndModal(false);
		return 0;
	}
	return TopWnd::Message(msg,wp,lp);
}

void DialogWnd::EndModal(bool result) {
	_loop.End(result?ModalLoop::ResultSucceeded:ModalLoop::ResultCancelled);
}

void DialogWnd::Paint(Gdiplus::Graphics& g, ref<Theme> theme) {
	Area header = GetClientArea();
	header.SetHeight(KHeaderHeight);

	Area buttons = GetClientArea();
	buttons.Narrow(0,buttons.GetHeight()-KHeaderHeight, 0, 0);

	// Header
	SolidBrush disabled(theme->GetDisabledOverlayColor());
	LinearGradientBrush headerBrush(PointF(0.0f, 0.0f), PointF(0.0f, float(KHeaderHeight)), theme->GetActiveStartColor(), theme->GetActiveEndColor());
	g.FillRectangle(&headerBrush, header);
	g.FillRectangle(&disabled, header);
	SolidBrush tbr(theme->GetTextColor());
	StringFormat sf;
	sf.SetAlignment(StringAlignmentNear);
	g.DrawString(_question.c_str(), (int)_question.length(), theme->GetGUIFont(), PointF(3.0f, 4.0f), &sf, &tbr);

	theme->DrawToolbarBackground(g, buttons);
}

void DialogWnd::OnSize(const Area& ns) {
	Layout();
	TopWnd::OnSize(ns);
	Repaint();
}

ref<PropertyGridWnd> DialogWnd::GetPropertyGrid() {
	return _grid;
}

std::wstring Dialog::AskForSaveFile(HWND owner, std::wstring title, const wchar_t* filter, std::wstring defExt) {
	wchar_t* fname = new wchar_t[MAX_PATH];
	memset(fname,0,sizeof(wchar_t)*MAX_PATH);
	OPENFILENAME fn;
	memset(&fn, 0, sizeof(OPENFILENAME));
	fn.lStructSize = sizeof(OPENFILENAME); 
	fn.hwndOwner = owner;
	fn.hInstance = GetModuleHandle(NULL);
	fn.lpstrFilter = filter;
	fn.lpstrFile = fname;
	fn.nMaxFile = 1023; 
	fn.lpstrTitle = title.c_str();
	fn.lpstrDefExt = defExt.c_str();

	BOOL result = GetSaveFileName(&fn);

	std::wstring filename(fname);
	delete[] fname;
	return result?filename:L"";
}

std::wstring Dialog::AskForOpenFile(HWND owner, std::wstring title, const wchar_t* filter, std::wstring defExt) {
	wchar_t* fname = new wchar_t[MAX_PATH];
	memset(fname,0,sizeof(wchar_t)*MAX_PATH);
	OPENFILENAME fn;
	memset(&fn, 0, sizeof(OPENFILENAME));
	fn.lStructSize = sizeof(OPENFILENAME); 
	fn.hwndOwner = owner;
	fn.hInstance = GetModuleHandle(NULL);
	fn.lpstrFilter = filter;
	fn.lpstrFile = fname;
	fn.nMaxFile = 1023; 
	fn.lpstrTitle = title.c_str();
	fn.lpstrDefExt = defExt.c_str();

	BOOL result = GetOpenFileName(&fn);

	std::wstring filename(fname);
	delete[] fname;
	return result?filename:L"";
}