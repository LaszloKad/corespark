#include "../include/tjshared.h"

ContextMenu::ContextMenu() {
	_index = 0;
	_menu = CreatePopupMenu();
}

ContextMenu::~ContextMenu() {
	DestroyMenu(_menu);
}

int ContextMenu::DoContextMenu(HWND window, int x, int y, bool correct) {
	if(correct) {
		RECT wr;
		GetWindowRect(window, &wr);
		x += wr.left;
		y += wr.top;
	}
	return TrackPopupMenu(_menu, TPM_RETURNCMD|TPM_TOPALIGN|TPM_VERPOSANIMATION, x,y, 0, window, 0);
}

void ContextMenu::AddItem(std::wstring name, int command, bool hilite, bool radiocheck) {
	MENUITEMINFO mif;
	memset(&mif, 0, sizeof(MENUITEMINFO));

	mif.cbSize = sizeof(MENUITEMINFO);
	mif.fMask = MIIM_ID|MIIM_STRING|MIIM_STATE|MIIM_FTYPE;

	mif.wID = command;
	mif.fType = MFT_STRING | (radiocheck?MFT_RADIOCHECK:0);
	mif.fState = MFS_ENABLED | (hilite?MFS_DEFAULT:0)| (radiocheck?MFS_CHECKED:0);
	mif.dwTypeData = (LPWSTR)name.c_str();
	mif.cch = (UINT)name.length();
	InsertMenuItem(_menu, _index, TRUE, &mif);
	_index++;
}

void ContextMenu::AddSeparator() {
	MENUITEMINFO mif;
	memset(&mif, 0, sizeof(MENUITEMINFO));

	mif.cbSize = sizeof(MENUITEMINFO);
	mif.fMask = MIIM_FTYPE;
	mif.fType = MFT_SEPARATOR;
	InsertMenuItem(_menu, _index, TRUE, &mif);
	_index++;
}