#ifndef _TJPROPERTY_H
#define _TJPROPERTY_H

class EXPORTED Property: public virtual Object {
	public:
		Property(std::wstring name) {
			_name = name;
		}

		virtual ~Property() {
		}

		virtual std::wstring GetName() {
			return _name;
		}

		virtual std::wstring GetValue() = 0;

		virtual HWND GetWindow() = 0;
		virtual HWND Create(HWND parent) = 0;
		
		// Called when the value in the edit window has changed (EN_CHANGED)
		virtual void Changed() = 0;
		
		// Called when a repaint is about to begin and the value in the window needs to be updated
		virtual void Update() = 0;

		virtual int GetHeight() {
			return 17;
		}

	protected:
		std::wstring _name;
};

class Inspectable {
	public:
		virtual ~Inspectable() {}
		virtual ref< std::vector< ref<Property> > > GetProperties()=0;
};

template<typename T> class GenericProperty: public Property {
	public:
		GenericProperty(std::wstring name, T* value, T* alsoSet, T defaultValue): Property(name) {
			if(value==0) Throw(L"Property value pointer cannot be null", ExceptionTypeWarning);
			_value = value;
			_alsoSet = alsoSet;
			assert(value!=0);
			_wnd = 0;
			_default = defaultValue;
		}

		virtual HWND Create(HWND parent) {
			if(_wnd!=0) return _wnd;
			HINSTANCE hinst;
			hinst = GetModuleHandle(NULL);
			_wnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, TJ_PROPERTY_EDIT_CLASS_NAME, Stringify(*_value).c_str(), ES_AUTOHSCROLL|WS_CHILD, 0, 0, 100, 100, parent, (HMENU)0, hinst, 0);
			if(_wnd==0) {
				int x = GetLastError();
				MessageBox(0L, Stringify(x).c_str(), L"", MB_OK|MB_ICONERROR);
				Throw(L"Property window creation failed ", ExceptionTypeError);
			}
			return _wnd;
		}

		virtual ~GenericProperty() {
			DestroyWindow(_wnd);
			_wnd = 0;
			_value = 0;
		}

		virtual HWND GetWindow() {
			return _wnd;
		}

		virtual void Changed() {
			int l = GetWindowTextLength(_wnd);
			wchar_t* cp = new wchar_t[l+2];
			GetWindowText(_wnd, cp, l+1);
			
			std::wstring cs(cp);
			cs += L".0";
			delete[] cp;

			T value = _default;
			std::wistringstream os(cs);
			os >> value;

			if(*_value!=value) {
				(*_value) = value;
				if(_alsoSet!=0) {
					(*_alsoSet) = value;
				}
			}
		}

		virtual void Update() {
			try {
				if(_value!=0) {
					std::wostringstream os;
					os << T(*_value);
					std::wstring v = os.str();
					SetWindowText(_wnd, v.c_str());
				}
			}
			catch(std::exception e) {
				// ignore
			}
		}

		virtual std::wstring GetValue() {
			assert(_value!=0);
			return Stringify(T(*_value));
		}

		virtual LRESULT Message(UINT msg, WPARAM wp, LPARAM lp) {
			return 0;
		}

	protected:
		T* _value;
		T _default;
		T* _alsoSet;
		HWND _wnd;
};

template<> HWND GenericProperty<bool>::Create(HWND parent) {
	if(_wnd!=0) return _wnd;
	_wnd = ::CreateWindowEx(0, (const wchar_t*)L"BUTTON", Stringify(*_value).c_str(), WS_TABSTOP|BS_AUTOCHECKBOX|BS_PUSHLIKE|WS_CHILD, 0, 0, 100, 100, parent, (HMENU)0, GetModuleHandle(NULL), 0);
	if(_wnd!=0) {
		SetWindowLong(_wnd, GWL_USERDATA, (LONG)(long long)this);
	}
	else {
		Throw(L"Property window creation failed", ExceptionTypeError);
	}
	return _wnd;
}

// for numeric edit stuff, spinner boxes and lots of other candy
template<> HWND GenericProperty<unsigned int>::Create(HWND parent) {
	if(_wnd!=0) return _wnd;
	_wnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, TJ_PROPERTY_EDIT_NUMERIC_CLASS_NAME, Stringify(*_value).c_str(), ES_AUTOHSCROLL|WS_CHILD, 0, 0, 100, 100, parent, (HMENU)0, GetModuleHandle(NULL), 0);
	if(_wnd==0) {
		Throw(L"Property window creation failed", ExceptionTypeError);
	}
	return _wnd;
}

template<> HWND GenericProperty<int>::Create(HWND parent) {
	if(_wnd!=0) return _wnd;
	_wnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, TJ_PROPERTY_EDIT_NUMERIC_CLASS_NAME, Stringify(*_value).c_str(), ES_AUTOHSCROLL|WS_CHILD, 0, 0, 100, 100, parent, (HMENU)0, GetModuleHandle(NULL), 0);
	if(_wnd==0) {
		Throw(L"Property window creation failed", ExceptionTypeError);
	}
	return _wnd;
}

template<> void GenericProperty<std::wstring>::Changed() {
	int l = GetWindowTextLength(_wnd);
	wchar_t* cp = new wchar_t[l+2];
	GetWindowText(_wnd, cp, l+1);
	
	std::wstring value(cp);
	delete[] cp;

	if(*_value!=value) {
		(*_value) = value;
		if(_alsoSet!=0) {
			(*_alsoSet) = value;
		}
	}
}

template<> void GenericProperty<bool>::Changed() {
	LRESULT st = SendMessage(_wnd, BM_GETCHECK, 0, 0);
	bool value = _default;
	if(st==BST_CHECKED) {
		value = true;
	}
	else if(st==BST_UNCHECKED) {
		value = false;
	}

	(*_value) = value;
	if(_alsoSet!=0) {
		(*_alsoSet) = value;
	}

	if(value) {
		SetWindowText(_wnd, Language::Get(L"yes"));
	}
	else {
		SetWindowText(_wnd, Language::Get(L"no"));
	}
}

template<> void GenericProperty<bool>::Update() {
	SendMessage(_wnd, BM_SETCHECK, (*_value)?BST_CHECKED:BST_UNCHECKED,0L);

	if((*_value)) {
		SetWindowText(_wnd, Language::Get(L"yes"));
	}
	else {
		SetWindowText(_wnd, Language::Get(L"no"));
	}
}

#endif