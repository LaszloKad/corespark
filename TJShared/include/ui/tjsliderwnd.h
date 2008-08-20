#ifndef _TJSLIDER_H
#define _TJSLIDER_H

namespace tj {
	namespace shared {
		class EXPORTED SliderWnd: public ChildWnd {
			public:
				SliderWnd(const wchar_t* title);
				virtual ~SliderWnd();
				virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp);
				float GetValue() const;
				void SetValue(float f, bool notify=true);
				void SetDisplayValue(float f, bool notify);
				float GetDisplayValue() const;
				void SetMarkValue(float f);
				float GetMarkValue() const;
				virtual void Update();
				virtual void Paint(graphics::Graphics& g, ref<Theme> theme);
				virtual void SetColor(int idx);
				void SetShowValue(bool show);
				void SetSnapToHalf(bool snap);
				virtual void Fill(LayoutFlags flags, Area& rect, bool direct = true);

				struct NotificationUpdate {
				};

				struct NotificationChanged {
				};

				Listenable<NotificationUpdate> EventUpdate;
				Listenable<NotificationChanged> EventChanged;

			protected:
				virtual void OnMouse(MouseEvent ev, Pixels x, Pixels y);
				virtual void OnFocus(bool focus);

				float _value;
				float _displayValue;
				bool _flash;
				float _oldValue;
				int _color;
				float _mark;
				bool _showValue;
				bool _snapHalf;
				Pixels _startX, _startY;
				bool _preciseDrag;

				const static int KMinimumWidth = 30;
		};
	}
}

#endif
