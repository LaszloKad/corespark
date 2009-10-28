#ifndef _TJSCRIPTRANGE_H
#define _TJSCRIPTRANGE_H

#pragma warning(push)
#pragma warning(disable: 4251 4275)

namespace tj {
	namespace script {

		class ScriptRangeType: public ScriptType {
			public:
				virtual tj::shared::ref<Scriptable> Construct(tj::shared::ref<ParameterList> p);
				virtual ~ScriptRangeType();
		};

		// ScriptRange is [a,b]
		class SCRIPT_EXPORTED ScriptRange: public ScriptObject<ScriptRange> {
			friend class ScriptArrayIterator;

			public:	
				static void Initialize();
				ScriptRange(int a, int b);
				virtual ~ScriptRange();


				virtual tj::shared::ref<Scriptable> ToString(tj::shared::ref<ParameterList> p);
				virtual tj::shared::ref<Scriptable> Next(tj::shared::ref<ParameterList> p);
				virtual tj::shared::ref<Scriptable> IsInside(tj::shared::ref<ParameterList> p);
				virtual tj::shared::ref<Scriptable> Size(tj::shared::ref<ParameterList> p);
				virtual tj::shared::ref<Scriptable> From(tj::shared::ref<ParameterList> p);
				virtual tj::shared::ref<Scriptable> To(tj::shared::ref<ParameterList> p);

				int _a, _b;
		};
	}
}

#pragma warning(pop)

#endif