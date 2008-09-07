#ifndef _TJNOTIFICATIONWND_H
#define _TJNOTIFICATIONWND_H

namespace tj {
	namespace shared {
		class EXPORTED NotificationWnd: public Wnd {
			public:
				NotificationWnd(const std::wstring& text, const std::wstring& icon, int time, int h, RootWnd* parent);
				virtual ~NotificationWnd();
				virtual void Show(bool t);
				virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
				virtual void Paint(graphics::Graphics& g, ref<Theme> theme);
				int GetIndex();

				enum {
					NotificationTimeoutForever = 0,
					NotificationTimeoutDefault = -1,
				};
				
			protected:
				std::wstring _text;
				graphics::Bitmap* _icon;
				RootWnd* _root;
				int _index;

				const static int DefaultNotificationHeight;
				const static int DefaultNotificationMargin;	
				const static int DefaultNotificationTimeout;
		};
	}
}

#endif