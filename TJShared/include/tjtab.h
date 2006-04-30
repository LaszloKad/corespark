#ifndef _TJTAB_H
#define _TJTAB_H

#pragma warning(push)
#pragma warning(disable: 4251)

class EXPORTED TabWnd: public ChildWnd {
	public:
		TabWnd(HWND parent, RootWnd* root);
		virtual ~TabWnd();
		virtual wchar_t GetPreferredHotkey();
		void SetHotkey(wchar_t hotkey);
		virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
		ref<Pane> AddPane(std::wstring name, ref<Wnd> wnd);
		ref<Pane> AddPane(ref<Pane> pane);
		ref<Pane> GetPane(int index);
		void RemovePane(ref<Wnd> wnd);
		void SelectPane(unsigned int index);
		virtual void Layout();
		virtual void Update();
		virtual void LeaveHotkeyMode(wchar_t key);
		virtual void EnterHotkeyMode();
		virtual bool IsInHotkeyMode();
		virtual void Clear();
		virtual void Paint(Gdiplus::Graphics& g);
		void Rename(ref<Wnd> pane, std::wstring name);
		ref<Wnd> GetCurrentPane();
		ref<Pane> GetPaneAt(int x);
		void Detach(ref<Pane> p);
		void Attach(ref<Pane> p);
		bool RevealWindow(ref<Wnd> w);
	
	protected:
		void SetDraggingPane(ref<Pane> pane);
		void DoContextMenu(int x, int y);

		wchar_t _hotkey;
		std::vector< ref<Pane> > _panes;
		ref<Pane> _current;
		ref<Pane> _dragging;
		RootWnd* _root;
		int _headerHeight;
		int _dragStartX, _dragStartY;

		const static int TearOffLimit = 15;
		enum {defaultHeaderHeight = 21};
};

#pragma warning(pop)

#endif