#ifndef _TJIMAGEBUTTON_H
#define _TJIMAGEBUTTON_H

namespace tj {
	namespace shared {
		class EXPORTED ButtonWnd: public ChildWnd {
			public:
				ButtonWnd(ref<Listener> listener, const wchar_t* image, const wchar_t* text=0);
				virtual ~ButtonWnd();
				virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
				virtual void Paint(Gdiplus::Graphics& g);
				virtual void SetListener(ref<Listener> lf);
				
			protected:
				std::wstring _text;
				Gdiplus::Bitmap* _image;
				ref<Listener> _listener;
				bool _down;
		};

		class EXPORTED StateButtonWnd: public ButtonWnd {
			public:
				enum ButtonState {On, Off, Other};
				StateButtonWnd(ref<Listener> listener, const wchar_t* imageOn, const wchar_t* imageOff, const wchar_t* imageOther);
				virtual ~StateButtonWnd();
				virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
				void SetOn(ButtonState o);
				virtual void Paint(Gdiplus::Graphics& g);
				
			protected:
				Gdiplus::Bitmap* _offImage;
				Gdiplus::Bitmap* _otherImage;
				ButtonState _on;
		};
	}
}

#endif