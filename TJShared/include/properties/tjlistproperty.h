#ifndef _TJLISTPROPERTY_H
#define _TJLISTPROPERTY_H

namespace tj {
	namespace shared {
		template<typename T> class GenericListProperty: public Property {
			public:
				struct Item {
					Item(): _separator(false) {
					}

					std::wstring _name;
					T _value;
					bool _separator;

					bool operator <(const Item& b) const {
						return _name < b._name;
					}
				};

				class PropertyWnd: public ChildWnd {
					public:
						PropertyWnd(GenericListProperty<T>& prop): ChildWnd(L"", false, true), _prop(prop), _arrowIcon(Icons::GetIconPath(Icons::IconDownArrow)) {
							SetWantMouseLeave(true);
						}

						virtual ~PropertyWnd() {
						}

						virtual void Paint(Gdiplus::Graphics& g, ref<Theme> theme) {
							// Draw background
							Area rc = GetClientArea();
							Gdiplus::SolidBrush back(theme->GetBackgroundColor());
							g.FillRectangle(&back, rc);
							if(IsMouseOver()) {
								theme->DrawToolbarBackground(g, 0.0f, 0.0f, float(rc.GetWidth()), float(rc.GetHeight()));
							}
							
							// Draw icon to the right
							Area iconArea(rc.GetRight()-rc.GetHeight(), rc.GetTop(), 16, 16);
							g.DrawImage(_arrowIcon, iconArea);

							Gdiplus::SolidBrush borderBrush(theme->GetActiveStartColor());
							Area borderArea = rc;
							borderArea.Narrow(0,0,1,1);
							Gdiplus::Pen borderPen(&borderBrush, 1.0f);
							g.DrawRectangle(&borderPen, borderArea);

							// Text parameters
							Gdiplus::SolidBrush tbr(theme->GetTextColor());
							Area text = rc;
							text.Narrow(2,2,0,0);
							Gdiplus::StringFormat sf;
							sf.SetTrimming(Gdiplus::StringTrimmingEllipsisPath);

							// Fetch text
							T value = *(_prop._value);
							std::wstring displayText = L"";
							std::vector<Item>::const_iterator it = _prop._options.begin();
							while(it!=_prop._options.end()) {
								const Item& item = *it;
								if(item._value==value) {
									displayText = item._name;
									break;
								}
								++it;
							}

							// Draw text
							g.DrawString(displayText.c_str(), (int)displayText.length(), theme->GetGUIFont(), text, &sf, &tbr);
						}

						virtual void OnMouse(MouseEvent ev, Pixels x, Pixels y) {
							if(ev==MouseEventMove) {
								SetWantMouseLeave(true);
								Repaint();
							}
							else if(ev==MouseEventLeave) {
								Repaint();
							}
							else if(ev==MouseEventLDown) {
								OnContextMenu(x,y);
							}
						}

						virtual void OnContextMenu(Pixels x, Pixels y) {
							ContextMenu cm;

							T val = *(_prop._value);

							std::vector<Item>::const_iterator it = _prop._options.begin();
							int i = 0;
							while(it!=_prop._options.end()) {
								const Item& item = *it;
								if(item._separator) {
									cm.AddSeparator(item._name);
								}
								else {
									cm.AddItem(item._name, i, false, item._value==val ? MenuItem::RadioChecked : MenuItem::NotChecked);
								}
								++it;
								++i;
							}

							Area rc = GetClientArea();
							int result = cm.DoContextMenu(this, rc.GetLeft(), rc.GetBottom());
							if(result>=0) {
								const Item& item = _prop._options.at(result);
								_prop.Set(item._value);
							}

							Update();
						}

						virtual void Update() {
							Repaint();
						}

						virtual void OnSize(const Area& ns) {
							Layout();
							Repaint();
						}

						virtual void Layout() {
						}

					protected:
						GenericListProperty<T>& _prop;
						Icon _arrowIcon;
				};

				GenericListProperty(std::wstring name, T* value, T* also, T def): Property(name) {
					_value = value;
					_alsoSet = also;
					_default = def;
				}

				virtual ~GenericListProperty() {
				}

				void AddOption(const std::wstring& name, T value) {
					Item it;
					it._name = name;
					it._value = value;
					_options.push_back(it);
				}

				void AddSeparator(const std::wstring& title = L"") {
					Item it;
					it._separator = true;
					it._name = title;
					_options.push_back(it);
				}
			
				virtual HWND Create(HWND parent) {
					if(!_pw) {
						_pw = GC::Hold(new PropertyWnd(*this));
					}
					::SetParent(_pw->GetWindow(), parent);
					return _pw->GetWindow();
				}

				virtual void Changed() {
					_pw->Update();
				}

				virtual void Update() {
					_pw->Update();
				}

				virtual std::wstring GetValue() {
					assert(_value!=0);
					return Stringify(*_value);
				}

				virtual HWND GetWindow() {
					return _pw->GetWindow();
				}

			protected:
				void Set(const T& value) {
					if(_value!=0) {
						*_value = value;
					}

					if(_alsoSet!=0) {
						*_alsoSet = value;
					}
				}

				T* _value;
				T _default;
				T* _alsoSet;
				ref<PropertyWnd> _pw;
				std::vector<Item> _options;
		};
	}
}

#endif
