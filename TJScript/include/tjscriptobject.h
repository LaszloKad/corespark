#ifndef _TJSCRIPTOBJECT_H
#define _TJSCRIPTOBJECT_H

namespace tj {
	namespace script {
		template<typename T> class ScriptObjectMemberIterator;

		/** This object provides late-bound methods and reflection. It implements Scriptable **/
		template<typename T> class ScriptObject: public virtual tj::shared::Object, public Scriptable {
			friend class ScriptObjectMemberIterator<T>;

			protected:
				typedef tj::shared::ref<Scriptable> (T::*Member)(tj::shared::ref<ParameterList>);
				typedef tj::shared::ref<Scriptable> (ScriptObject::*BaseMember)(tj::shared::ref<ParameterList>);
				typedef std::map<CommandType, BaseMember> MemberMap;

				ScriptObject();
				static void Bind(Command c, Member p);

				static typename MemberMap _members;
				static volatile bool _initialized;

			public:
				// Scriptable
				virtual tj::shared::ref<Scriptable> Execute(Command c, tj::shared::ref<ParameterList> p);
				virtual ~ScriptObject();
		};

		template<typename T> class ScriptObjectMemberIterator: public Scriptable {
			public:
				ScriptObjectMemberIterator() {
					_it = T::_members.begin();
				}

				virtual ~ScriptObjectMemberIterator() {
				}

				virtual tj::shared::ref<Scriptable> Execute(Command c, tj::shared::ref<ParameterList> p) {
					if(c==L"next") {
						if(_it!=T::_members.end()) {
							Command memberName = _it->first;
							++_it;
							return tj::shared::GC::Hold(new ScriptString(memberName));
						}
						return ScriptConstants::Null;
					}
					else if(c==L"toString") {
						return tj::shared::GC::Hold(new ScriptString(L"[ScriptObjectMemberIterator]"));
					}
					return 0;
				}

			protected:
				typename ScriptObject<T>::MemberMap::const_iterator _it;
		};

		/* Implementation */
		template<typename T> ScriptObject<T>::ScriptObject() {
			if(!_initialized) {
				_initialized = true;
				T::Initialize();
			}
		}

		template<typename T> ScriptObject<T>::~ScriptObject() {
		}

		template<typename T> void ScriptObject<T>::Bind(Command c, Member p) {
			ScriptObject<T>::_members[c] = (BaseMember)p;
		}

		template<typename T> tj::shared::ref<Scriptable> ScriptObject<T>::Execute(Command c, tj::shared::ref<ParameterList> p) {
			MemberMap::const_iterator it = _members.find(c);
			if(it!=_members.end()) {
				BaseMember m = it->second;
				return (this->*m)(p);
			}
			else if(c==L"members") {
				return tj::shared::GC::Hold(new ScriptObjectMemberIterator<T>());
			}
			else if(c==L"class") {
				return tj::shared::GC::Hold(new ScriptString(Wcs(typeid(T).name())));
			}
			else {
				return 0;
			}
		}

		template<typename T> typename ScriptObject<T>::MemberMap ScriptObject<T>::_members = typename ScriptObject<T>::MemberMap();
		template<typename T> volatile bool ScriptObject<T>::_initialized = false;
	}
}

#endif