#include "../include/tjshared.h"

ContextMenu::ContextMenu() {
	_index = 0;
	_menu = CreatePopupMenu();
}

ContextMenu::~ContextMenu() {
	DestroyMenu(_menu);
}

int ContextMenu::DoContextMenu(HWND window, int x, int y) {
	return TrackPopupMenu(_menu, TPM_RETURNCMD|TPM_TOPALIGN|TPM_VERPOSANIMATION, x,y, 0, window, 0);
}

void ContextMenu::AddItem(std::wstring name, int command, bool hilite) {
	MENUITEMINFO mif;
	memset(&mif, 0, sizeof(MENUITEMINFO));

	mif.cbSize = sizeof(MENUITEMINFO);
	mif.fMask = MIIM_ID|MIIM_STRING|MIIM_STATE;

	mif.wID = command;
	mif.fType = MFT_STRING;
	mif.fState = MFS_ENABLED | (hilite?MFS_DEFAULT:0);
	mif.dwTypeData = (LPWSTR)name.c_str();
	mif.cch = (UINT)name.length();
	InsertMenuItem(_menu, _index, FALSE, &mif);
	_index++;
}