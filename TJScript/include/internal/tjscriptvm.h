#ifndef _TJSCRIPTVM_H
#define _TJSCRIPTVM_H

namespace tj {
	namespace script {
		class CompiledScript;
		class Op;
		class ScriptScope;
		class ScriptContext;

		class StackFrame {
			public:
				inline StackFrame(tj::shared::ref<Scriptlet> s, unsigned int pc) {
					_scriptlet = s;
					_pc = pc;
					_stackSize = 0;
				}

				virtual ~StackFrame() {
				}
				
				unsigned int _pc;
				tj::shared::ref<Scriptlet> _scriptlet;
				unsigned int _stackSize;

			/*private:
				StackFrame(StackFrame& o) {
				}*/
		};

		class VM: public virtual tj::shared::Object {
			public:
				VM(int stackLimit=512);
				virtual ~VM();
				void Execute(tj::shared::ref<ScriptContext> c, tj::shared::ref<CompiledScript> script, tj::shared::ref<ScriptScope> global);
				void SetDebug(bool d);				

				// to be called by ops or vm during execution
				void Call(tj::shared::ref<Scriptlet> s, tj::shared::ref<ScriptParameterList> sc=0);
				void Call(int n);
				void Return(bool takeLastValue);
				void Break();

				// Inlines
				inline tj::shared::ref<ScriptScope> GetCurrentScope() {
					return _scope;
				}
				
				inline tj::shared::ref<CompiledScript> GetScript() {
					return _script;
				}

				inline tj::shared::ref<ScriptContext> GetContext() {
					return _context;
				}

				inline tj::shared::ref<StackFrame> GetStackFrame() {
					return *(_call.rbegin());
				}

				inline ScriptStack& GetStack() {
					return _stack;
				}
				
			protected:
				ScriptStack _stack;
				tj::shared::ref<ScriptScope> _scope;
				tj::shared::ref<CompiledScript> _script;
				tj::shared::weak<ScriptContext> _context;
				std::deque<tj::shared::ref<StackFrame> > _call;
				bool _debug;
		};
	}
}

#endif