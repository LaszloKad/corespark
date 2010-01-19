#ifndef _TJSCRIPTSCOPE_H
#define _TJSCRIPTSCOPE_H

#pragma warning(push)
#pragma warning(disable: 4251)

namespace tj {
	namespace script {
		class SCRIPT_EXPORTED ScriptScope: public Scriptable {
			public:
				ScriptScope();
				virtual ~ScriptScope();
				tj::shared::ref<Scriptable> GetPrevious();
				void SetPrevious(tj::shared::ref<Scriptable> r);
				
				virtual tj::shared::ref<Scriptable> Execute(Command command, tj::shared::ref<ParameterList> params);
				virtual bool Set(Field field, tj::shared::ref<Scriptable> var);
				tj::shared::ref<Scriptable> Get(const std::wstring& key);
				bool Exists(const std::wstring& key);

				std::map< std::wstring, tj::shared::ref<Scriptable> > _vars;
				
			protected:
				tj::shared::ref<Scriptable> _previous;
		};

	}
}

#pragma warning(pop)
#endif