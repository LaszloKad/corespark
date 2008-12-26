#ifndef _TJTAB_H
#define _TJTAB_H

namespace tj {
	namespace shared {
		class EXPORTED TabWnd: public ChildWnd {
			friend class FloatingPane;

			class TabPane {
				friend class tj::shared::TabWnd;

				public:
					TabPane(strong<Pane> p);
					~TabPane();
					Pixels GetWidth() const;
					void Layout(strong<Theme> theme);
					void Close();
					bool IsDestroyed() const;
					ref<Pane> GetPane();

				protected:
					ref<Pane> _pane;
					Pixels _width;
					Animation _appearAnimation;
			};

			public:
				TabWnd(ref<WindowManager> root, const std::wstring& id = L"");
				virtual ~TabWnd();
				
				ref<Pane> AddPane(ref<Pane> pane, bool select = false);
				ref<Pane> GetPane(int index);
				void RemovePane(ref<Wnd> wnd);
				void SelectPane(unsigned int index);
				void SelectPane(ref<Wnd> wnd);
				virtual void Layout();
				virtual void Update();
				virtual void Clear();
				virtual void Paint(graphics::Graphics& g, ref<Theme> theme);
				void SetChildStyle(bool c);
				
				void Rename(ref<Wnd> pane, std::wstring name);
				ref<Wnd> GetCurrentPane();
				ref<Pane> GetPaneAt(Pixels x);
				void Detach(ref<Pane> p);
				void Attach(ref<Pane> p);
				bool RevealWindow(ref<Wnd> w);
				void SetDetachAttachAllowed(bool allow);
				void SelectPane(ref<Pane> pane);
				virtual void Add(ref<Wnd> child); // do not use; use AddPane instead
				const std::wstring& GetID() const;
				Placement GetPlacement() const;
				virtual std::wstring GetTabTitle() const;
				virtual ref<Icon> GetTabIcon() const;
			
			protected:
				virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
				virtual void OnSize(const Area& ns);
				virtual void OnKey(Key k, wchar_t t, bool down, bool isAccelerator);
				virtual void OnFocus(bool focus);
				virtual void OnMouse(MouseEvent ev, Pixels x, Pixels y);
				virtual void OnTimer(unsigned int id);
				virtual void ClosePane(ref<Pane> pane);
				std::vector<TabPane>::iterator GetPaneIteratorAt(Pixels x, Pixels& xOnPane);

				void SetDraggingPane(ref<Pane> pane);
				void DoAddMenu(Pixels x, Pixels y);
				void FixScrollerOffset();
				Pixels GetTotalTabWidth();
				void DropPaneAt(Pixels x, ref<Pane> dragging);
				
				std::vector<TabPane> _panes;
				ref<Pane> _current;
				ref<Pane> _dragging;
				weak<WindowManager> _root;
				Pixels _headerHeight;
				Pixels _offset;
				int _dragStartX, _dragStartY;
				Icon _closeIcon;
				Icon _addIcon;
				bool _detachAttachAllowed;
				bool _childStyle;
				bool _in;
				std::wstring _id;
				Animation _entryAnimation;
				Animation _tabAppearAnimation;

				const static int TearOffLimit = 15;
				enum {defaultHeaderHeight = 24};
				const static int KIconWidth = 16;
				const static int KRealIconWidth = 14;
				const static Pixels KScrollerHeight = 3;
		};
	}
}

#endif
