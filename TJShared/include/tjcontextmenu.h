#ifndef _TJCONTEXTMENU_H
#define _TJCONTEXTMENU_H

class EXPORTED ContextMenu {
	public:
		ContextMenu();
		virtual ~ContextMenu();
		int DoContextMenu(HWND wnd, int x, int y, bool correct=false);
		void AddItem(std::wstring name, int command, bool hilite=false, bool radiocheck=false);
		void AddSeparator();

	protected:
		HMENU _menu;
		int _index;
};

#endif