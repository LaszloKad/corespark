#ifndef _TJTAB_H
#define _TJTAB_H

#pragma warning(push)
#pragma warning(disable: 4251)

class EXPORTED TabWnd: public ChildWnd {
	public:
		TabWnd(HWND parent);
		virtual ~TabWnd();
		virtual wchar_t GetPreferredHotkey();
		void SetHotkey(wchar_t hotkey);
		virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
		void AddPane(std::wstring name, ref<Wnd> wnd);
		void SelectPane(unsigned int index);
		virtual void Layout();
		virtual void Update();
		virtual void LeaveHotkeyMode(wchar_t key);
	
	protected:
		struct Pane {
			Pane(std::wstring, ref<Wnd>, bool);
			void SetDetached(bool d, TabWnd* tab);

			std::wstring _title;
			ref<Wnd> _wnd;
			bool _detached;
		};

		wchar_t _hotkey;
		void DoContextMenu(int x, int y);
		std::vector<Pane> _panes;
		ref<Wnd> _current;
		int _headerHeight;
		Gdiplus::Bitmap* _buffer;

		enum {defaultHeaderHeight = 21};
};

#pragma warning(pop)

#endif