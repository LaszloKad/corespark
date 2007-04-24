#include "../include/internal/tjscript.h"
using namespace tj::shared;
using namespace tj::script;

namespace tj {
	namespace script {
		class ScriptMapIterator: public Scriptable {
			public:
				ScriptMapIterator(ref<ScriptMap> x, bool iterateKey) {
					_array = x;
					_it = _array->_array.begin();
					_key = iterateKey;
				}

				virtual ~ScriptMapIterator() {
				}

				virtual ref<Scriptable> Execute(Command c, ref<ParameterList> p) {
					ThreadLock lock(&(_array->_lock));
					if(c==L"toString") {
						return GC::Hold(new ScriptString(L"[ScriptMapIterator]"));
					}
					else if(c==L"next") {
						if(_it!=_array->_array.end()) {
							if(_key) {
								std::wstring key = (*_it).first;
								++_it;
								return GC::Hold(new ScriptString(key));
							}
							else {
								ref<Scriptable> val = (*_it).second;
								++_it;
								return val;
							}
						}
						return ScriptConstants::Null; // no more elements
					}
					return 0;
				};

				ref<ScriptMap> _array;
				std::map< std::wstring, ref<Scriptable> >::iterator _it;
				bool _key;
		};
	}
}

ScriptMapType::~ScriptMapType() {
}

ref<Scriptable> ScriptMapType::Construct(ref<ParameterList> p) {
	ref<ScriptMap> ar = GC::Hold(new ScriptMap());
	if(p) {
		ar->Set(p);
	}

	return ar;
}

ScriptMap::ScriptMap() {
}

ScriptMap::~ScriptMap() {
}

std::map<std::wstring, tj::shared::ref<Scriptable> >::iterator ScriptMap::GetBegin() {
	return _array.begin();
}

std::map<std::wstring, tj::shared::ref<Scriptable> >::iterator ScriptMap::GetEnd() {
	return _array.end();
}

void ScriptMap::Initialize() {
	Bind(L"size", &Size);
	Bind(L"set", &Set);
	Bind(L"get", &Get);
	Bind(L"toString", &ToString);
	Bind(L"keys", &Keys);
	Bind(L"values", &Values);
}

ref<Scriptable> ScriptMap::Size(ref<ParameterList> p) {
	ThreadLock lock(&_lock);
	return GC::Hold(new ScriptInt((int)_array.size()));
}

ref<Scriptable> ScriptMap::Set(ref<ParameterList> p) {
	ThreadLock lock(&_lock);

	ParameterList::iterator it = p->begin();
	while(it!=p->end()) {
		std::pair< const std::wstring, ref<Scriptable> > data = *it;
		_array[data.first] = data.second;
		++it;
	}
	return ScriptConstants::Null;
}

ref<Scriptable> ScriptMap::Get(ref<ParameterList> p) {
	ThreadLock lock(&_lock);

	RequiredParameter<std::wstring> key(p, L"key", L"");
	if(_array.find(key.Get())!=_array.end()) {
		return _array[key.Get()];
	}
	
	return ScriptConstants::Null;
}

ref<Scriptable> ScriptMap::ToString(ref<ParameterList> p) {
	ThreadLock lock(&_lock);

	std::wostringstream wos;
	wos << L"[";
	std::map<std::wstring, ref<Scriptable> >::iterator it = _array.begin();
	while(it!=_array.end()) {
		wos << it->first << L"=" << ScriptContext::GetValue<std::wstring>(it->second, L"?") << L" ";
		++it;
	}
	wos << L"]";

	return GC::Hold(new ScriptString(wos.str()));
}

ref<Scriptable> ScriptMap::Keys(ref<ParameterList> p) {
	ThreadLock lock(&_lock);
	return GC::Hold(new ScriptMapIterator(This<ScriptMap>(), true));
}

ref<Scriptable> ScriptMap::Values(ref<ParameterList> p) {
	ThreadLock lock(&_lock);
	return GC::Hold(new ScriptMapIterator(This<ScriptMap>(), false));
}