#ifndef _TJCONTEXTMENU_H
#define _TJCONTEXTMENU_H

namespace tj {
	namespace shared {
		class MenuItem;

		class EXPORTED Menu: public virtual Object {
			public:
				virtual ~Menu();
				virtual void AddSeparator(const std::wstring& text = L"") = 0;
				virtual void AddItem(strong<MenuItem> ci) = 0;
				virtual unsigned int GetItemCount() const = 0;
				virtual ref<MenuItem> GetItemByIndex(unsigned int idx) = 0;
				virtual Pixels GetLargestWidth(strong<Theme> theme, graphics::Font* fnt) const = 0;
		};

		/** The icon for a MenuItem needs to be 16x16 */
		class EXPORTED MenuItem: public virtual Object {
			friend class ContextMenu;
			friend class ContextPopupWnd;

			public:
				enum CheckType {
					NotChecked = 0,
					Checked = 1,
					RadioChecked = 2,
				};

				MenuItem(); // separator
				MenuItem(const std::wstring& title, int command, bool highlight = false, CheckType checked = NotChecked, const std::wstring& icon = L"", const std::wstring& hotkey = L"");
				MenuItem(const std::wstring& title, int command, bool highlight, CheckType checked, ref<Icon> icon);
				
				virtual ~MenuItem();
				virtual bool IsSeparator() const;
				virtual bool IsDisabled() const;
				virtual bool HasIcon() const;
				virtual void SetIcon(const std::wstring& icon);
				virtual void SetIcon(ref<Icon> icon);
				virtual void SetHotkey(const std::wstring& hk);
				virtual const std::wstring& GetHotkey() const;
				virtual bool HasHotkey() const;
				virtual bool IsLink() const;
				virtual void SetLink(bool l);
				virtual ref<Icon> GetIcon();
				virtual const std::wstring& GetTitle() const;

				virtual void SetTitle(const std::wstring& title);
				virtual void SetSeparator(bool s);

				virtual unsigned char GetIndent() const;
				virtual void SetIndent(unsigned char level);

				virtual int GetCommandCode() const;
				virtual void SetCommandCode(int c);
				virtual ref<Menu> GetSubMenu(); // returns null when this item doesn't have a submenu

			protected:
				std::wstring _title;
				std::wstring _hotkey;
				int _command;
				CheckType _checked;
				ref<Icon> _icon;
				bool _separator;
				bool _hilite;
				bool _link;
				unsigned char _indent;
		};

		class EXPORTED BasicMenu: public Menu {
			public:
				BasicMenu();
				virtual ~BasicMenu();
				virtual void AddSeparator(const std::wstring& text = L"");
				virtual void AddItem(strong<MenuItem> ci);
				virtual unsigned int GetItemCount() const;
				virtual ref<MenuItem> GetItemByIndex(unsigned int idx);
				virtual Pixels GetLargestWidth(strong<Theme> theme, graphics::Font* fnt) const;

			protected:
				std::vector< strong<MenuItem> > _items;
				std::wstring _longestString;
		};

		class EXPORTED SubMenuItem: public MenuItem, public BasicMenu {
			public:
				SubMenuItem();
				SubMenuItem(const std::wstring& title, bool highlight = false, CheckType checked = NotChecked, const std::wstring& icon = L"");
				SubMenuItem(const std::wstring& title, bool highlight = false, CheckType checked = NotChecked, ref<Icon> icon = null);
				virtual ~SubMenuItem();
				virtual ref<Menu> GetSubMenu();
		};

		class EXPORTED ContextPopupWnd: public PopupWnd {
			public:
				ContextPopupWnd(strong<Menu> menu, HWND parent);
				virtual ~ContextPopupWnd();
				virtual int DoModal(strong<Wnd> parent, Pixels x, Pixels y); // returns -1 when no command
				virtual void Paint(graphics::Graphics& g, ref<Theme> theme);
				virtual void OnMouse(MouseEvent ev, Pixels x, Pixels y);
				virtual void OnKey(Key k, wchar_t ch, bool down, bool isAccelerator);

			protected:
				virtual void EndModal(int r);
				virtual void OnActivate(bool a);
				virtual void OnTimer(unsigned int id);
				int GetItemAt(Pixels y);
				strong<Menu> GetCurrentMenu();
				virtual void EnterSubMenu(strong<Menu> menu);
				virtual void LeaveSubMenu();
				virtual void OnSelectItem(strong<MenuItem> ci);
				virtual void DrawMenuItems(graphics::Graphics& g, strong<Theme> theme, strong<Menu> cm, const Area& rc);
				virtual void UpdateSize();

				// TODO: move some of these to Theme GetMeasure
				const static unsigned int KMaxItems;

			private:
				const static int KMouseOverNothing = -1;
				const static int KMouseOverBackButton = -2;

				strong<Menu> _firstMenu;
				std::deque< strong<Menu> > _menu;
				Animation _openAnimation;
				Animation _closeAnimation;
				int _result;
				ModalLoop _loop;
				Icon _checkedIcon, _radioCheckedIcon, _subIcon;
				int _mouseOver;
				int _mouseDown;
		};

		/** ContextMenu is a wrapper class that can be created on the stack. It handles the menu creation and 
		popup stuff for context menus **/
		class EXPORTED ContextMenu {
			friend class ContextPopupWnd;

			public:
				ContextMenu();
				~ContextMenu();
				int DoContextMenu(ref<Wnd> wnd, Pixels x, Pixels y);
				int DoContextMenu(ref<Wnd> wnd);
				void AddItem(const std::wstring& name, int command, bool hilite = false, bool checked = false);
				void AddItem(const std::wstring& name, int command, bool hilite, MenuItem::CheckType checked);
				void AddItem(ref<MenuItem> ci);
				void AddSeparator(const std::wstring& text = L"");
				strong<Menu> GetMenu();

			protected:
				strong<Menu> _menu;
		};
	}
}

#endif
