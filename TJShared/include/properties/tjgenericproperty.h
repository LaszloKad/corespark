#ifndef _TJGENERICPROPERTY_H
#define _TJGENERICPROPERTY_H

namespace tj {
	namespace shared {
		template<typename T> class GenericProperty: public Property, public Listener<EditWnd::NotificationTextChanged> {
			public:
				GenericProperty(const std::wstring& name, T* value, T* alsoSet, T defaultValue): Property(name), _value(value), _alsoSet(alsoSet), _defaultValue(defaultValue) {
					if(value==0) Throw(L"Property value pointer cannot be null", ExceptionTypeWarning);
				}

				virtual ~GenericProperty() {
					_value = 0;
					_alsoSet = 0;
				}

				virtual void Notify(ref<Object> source, const EditWnd::NotificationTextChanged& ev) {
					if(_wnd && _value!=0) {
						T value = StringTo<T>(_wnd->GetText(), _defaultValue);
						*_value = value;
						if(_alsoSet!=0) {
							*_alsoSet = value;
						}
					}
				}

				virtual ref<Wnd> GetWindow() {
					if(!_wnd) {
						ref<EditWnd> ew = GC::Hold(new EditWnd());
						ew->EventTextChanged.AddListener(this);
						ew->SetBorder(true);
						ew->SetCue(Stringify(_defaultValue));
						_wnd = ew;
						Update();
					}
					return _wnd;
				}

				virtual void Update() {
					if(_wnd) {
						_wnd->SetText(Stringify(*_value));
					}
				}

			protected:
				T* _value;
				T* _alsoSet;
				T _defaultValue;
				ref<Wnd> _wnd;
		};

		template<> ref<Wnd> EXPORTED GenericProperty<bool>::GetWindow();
		template<> void EXPORTED GenericProperty<bool>::Update();
	}
}

#endif