#ifndef _TJAPPCORE_H
#define _TJAPPCORE_H

namespace tj {
	namespace shared {	
		class EXPORTED RunnableApplication {
			public:
				virtual ~RunnableApplication();
				virtual void Message(MSG& msg) = 0;
				virtual void AddCommandHistory(ref<Action> action);

				static const int KUndoMemory = 10;
				std::deque< ref<Action> > _undo;
		};

		class GraphicsInit;

		class EXPORTED Core {
			public:
				static strong<Core> Instance();
				void Run(RunnableApplication* app, ref<Arguments> args);
				RunnableApplication* GetApplicationPointer();
				Core();
				virtual ~Core();
				void AddAction(ref<Runnable> action, bool wait=false);
				void Quit();

			protected:
				static ref<Core> _instance;
				RunnableApplication* _app;
				CriticalSection _actionLock;
				std::vector< ref<Runnable> > _actions;
				Event _actionEvent;
				Event _actionsProcessedEvent;
				GraphicsInit* _init;

				void ProcessActions();
		};

		class EXPORTED ModalLoop {
			public:
				ModalLoop();
				virtual ~ModalLoop();
				
				enum Result {
					ResultUnknown = 0,
					ResultSucceeded = 1,
					ResultCancelled = 2,
				};
				
				virtual Result Enter(HWND m, bool isDialog);
				virtual void End(Result r = ResultUnknown);

			protected:
				volatile bool _running;
				volatile Result _result;
		};

		class EXPORTED Alert {
			public:
				enum AlertType {
					TypeNone = 0,
					TypeInformation,
					TypeWarning,
					TypeError,
					TypeQuestion,
				};

				enum Result {
					ResultYes = 1,
					ResultNo = 0,
					ResultCancel = -1,
				};

				static void Show(const std::wstring& title, const std::wstring& text, AlertType t);
				static bool ShowOKCancel(const std::wstring& title, const std::wstring& text, AlertType t);
				static bool ShowYesNo(const std::wstring& title, const std::wstring& text, AlertType t, bool modal = false);
				static Result ShowYesNoCancel(const std::wstring& title, const std::wstring& text, AlertType t, bool modal = false);
		};
	}
}

#endif