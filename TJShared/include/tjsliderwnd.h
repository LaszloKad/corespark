#ifndef _TJSLIDER_H
#define _TJSLIDER_H

#pragma warning(push)
#pragma warning(disable: 4251)

class EXPORTED SliderWnd: public ChildWnd {
	public:
		SliderWnd(HWND parent, const wchar_t* title);
		virtual ~SliderWnd();
		virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
		void SetListener(Listener* listener);
		float GetValue() const;
		void SetValue(float f, bool notify=true);
		virtual void Update();
		void SetDisplayValue(float f, bool notify);
		void SetMarkValue(float f);
		virtual void Paint(Gdiplus::Graphics& g);
		virtual void SetColor(int idx);
		void SetShowValue(bool show);
		void SetSnapToHalf(bool snap);
		virtual void Fill(LayoutFlags flags, tj::shared::Rectangle& rect);
	protected:
		Listener* _listener;
		float _value;
		float _displayValue;
		bool _hasFocus;
		bool _flash;
		float _oldValue;
		int _color;
		float _mark;
		bool _showValue;
		bool _snapHalf;

		const static int KMinimumWidth = 30;
		const static int KDraggerWidth;
};

#pragma warning(pop)

#endif