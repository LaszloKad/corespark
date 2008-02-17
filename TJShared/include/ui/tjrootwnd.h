#ifndef _TJROOTWND_H
#define _TJROOTWND_H

namespace tj {
	namespace shared {
		class NotificationWnd;

		class EXPORTED WindowManager: public virtual Object {
			public:
				virtual ~WindowManager();

				// Floating panes
				virtual ref<FloatingPane> AddFloatingPane(ref<Pane> pane) = 0;
				virtual void RemoveFloatingPane(ref<Pane> pn) = 0;

				// Tab windows (pane holders)
				virtual void AddTabWindow(ref<TabWnd> tw) = 0;
				virtual void RemoveTabWindow(ref<TabWnd> tw) = 0;
				virtual void RemoveTabWindow(TabWnd* tw) = 0;

				// Orphan panes
				virtual bool IsOrphanPane(ref<Wnd> wnd) = 0;
				virtual void AddOrphanPane(ref<Pane> pane) = 0;
				virtual std::vector< ref<Pane> >* GetOrphanPanes() = 0;
				virtual void RemoveOrphanPane(ref<Pane> pane) = 0;

				// Other
				virtual void RevealWindow(ref<Wnd> wnd, ref<TabWnd> addTo=0) = 0;
				virtual ref<TabWnd> FindTabWindowAt(int x, int y) = 0;
				virtual ref<TabWnd> GetTabWindowById(const std::wstring& id) = 0;
				virtual void SetDragTarget(ref<TabWnd> tw) = 0;
				virtual ref<TabWnd> GetDragTarget() = 0;
				virtual void RemoveWindow(ref<Wnd> w) = 0;
				virtual void RenameWindow(ref<Wnd> w, std::wstring name) = 0;
				virtual void AddPane(ref<Pane> p, bool select = false) = 0; // add by preferred placement

		};

		class EXPORTED RootWnd: public TopWnd, public WindowManager {
			friend class NotificationWnd; 

			public:
				RootWnd(std::wstring title, const wchar_t* className=TJ_DEFAULT_CLASS_NAME, bool useDoubleBuffering=true);
				virtual ~RootWnd();
				virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
				virtual void Update();

				// WindowManager implementation
				virtual ref<FloatingPane> AddFloatingPane(ref<Pane> pane);
				virtual void RemoveFloatingPane(ref<Pane> pn);

				virtual void AddTabWindow(ref<TabWnd> tw);
				virtual void RemoveTabWindow(ref<TabWnd> tw);
				virtual void RemoveTabWindow(TabWnd* tw);

				virtual void RevealWindow(ref<Wnd> wnd, ref<TabWnd> addTo=0);
				virtual ref<TabWnd> FindTabWindowAt(int x, int y);
				virtual ref<TabWnd> GetTabWindowById(const std::wstring& id);
				virtual void SetDragTarget(ref<TabWnd> tw);
				virtual ref<TabWnd> GetDragTarget();
				virtual bool IsOrphanPane(ref<Wnd> wnd);

				virtual void AddOrphanPane(ref<Pane> pane);
				virtual std::vector< ref<Pane> >* GetOrphanPanes();
				virtual void RemoveOrphanPane(ref<Pane> pane);
				virtual void RemoveWindow(ref<Wnd> w);
				virtual void RenameWindow(ref<Wnd> w, std::wstring name);
				virtual void AddPane(ref<Pane> p, bool select = false); // add by preferred placement

				/* Notification API */
				virtual void AddNotification(const std::wstring& message, std::wstring icon, int time=-1);
				void RemoveNotification(NotificationWnd* nw);

				void FullRepaint(); // use after switching theme

			protected:
				std::vector< ref<FloatingPane> > _floatingPanes;
				std::vector < ref<TabWnd> > _tabWindows;
				std::vector< ref<Pane> > _orphans;
				std::vector< ref<NotificationWnd> > _notifications;
				ref<TabWnd> _dragTarget;
		};

		class EXPORTED AddNotificationRunnable: public Runnable {
			public:
				AddNotificationRunnable(RootWnd* root, const std::wstring& text, std::wstring icon, int time);
				virtual ~AddNotificationRunnable();
				virtual void Run();

			protected:
				RootWnd* _root;
				std::wstring _text;
				std::wstring _icon;
				int _time;
		};
	}
}

#endif
