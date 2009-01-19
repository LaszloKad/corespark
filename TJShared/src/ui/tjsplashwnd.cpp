#include "../../include/ui/tjui.h" 
using namespace tj::shared::graphics;
using namespace tj::shared;

SplashWnd::SplashWnd(std::wstring path, Pixels w, Pixels h) {
	SetStyleEx(WS_EX_TOPMOST);
	SetStyle(WS_POPUP);
	UnsetStyle(WS_CAPTION);

	_image = Bitmap::FromFile(path.c_str(), TRUE);
	_progress = GC::Hold(new ProgressWnd());
	_progress->SetIndeterminate(true);
	Add(_progress);
	
	RECT rc;
	GetWindowRect(GetWindow(), &rc);
    HMONITOR mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
    GetMonitorInfo(mon, &mi);

	RECT nrc;
	nrc.left   = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
    nrc.top   = mi.rcWork.top  + (mi.rcWork.bottom - mi.rcWork.top  - h) / 2;
	SetWindowPos(GetWindow(), HWND_TOPMOST, nrc.left, nrc.top, 0, 0, SWP_NOSIZE);
	SetSize(w,h);
	Layout();

	// Make window transparent
	SetOpacity(0.9f);
}

SplashWnd::~SplashWnd() {
	delete _image;
}

void SplashWnd::Paint(graphics::Graphics& g, strong<Theme> theme) {
	Area rc = GetClientArea();
	
	g.DrawImage(_image, (RectF)rc);
}

void SplashWnd::Layout() {
	Area rc = GetClientArea();
	if(_progress) {
		_progress->Move(7, rc.GetHeight()-34, rc.GetWidth()-14, 18);
	}
}

/* Splash thread */
SplashThread::SplashThread(std::wstring path, int w, int h) {
	_path = path;
	_w = w;
	_h = h;
}

SplashThread::~SplashThread() {
	Hide();
	WaitForCompletion();
}

void SplashThread::Hide() {
	if(_wnd) {
		PostMessage(_wnd->GetWindow(), WM_CLOSE, 0, 0L);
	}
	_closeEvent.Signal();
}

void SplashThread::Run() {
	SetName("SplashThread");
	MSG msg;

	// init GDI+
	ULONG_PTR ptr;
	GdiplusStartupInput input;
	GdiplusStartup(&ptr, &input, 0);

	_wnd = GC::Hold(new SplashWnd(_path, _w, _h));
	_wnd->Show(true);

	HANDLE handles[1] = { _closeEvent._event };

	while(true) {
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		int result = MsgWaitForMultipleObjects(1, handles, FALSE, 1000, QS_ALLINPUT|QS_ALLPOSTMESSAGE); 

		// The result tells us the type of event we have.
		if(result==(WAIT_OBJECT_0 + 1)) {
			// New messages have arrived
			continue;
		} 
		else if(result==WAIT_ABANDONED) {
			continue;
		}
		else if(result==WAIT_OBJECT_0) {
			break; // show is over
		}
	}

	GdiplusShutdown(ptr);
}
