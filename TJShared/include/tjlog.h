#ifndef _TJLOG_H
#define _TJLOG_H

namespace tj {
	namespace shared {
		class LogThread;

		/** The event logger logs events that matter to the application and to the user. The 'Log' class
		can be used for stuff like debug logging. In TJShow for example, the event log could be transferred
		from client to server, whilst debug logs will probably never be transferred. **/
		class EXPORTED EventLogger {
			public:
				virtual ~EventLogger();
				virtual void AddEvent(const String& message, ExceptionType e, bool read = false) = 0;
		};

		class EXPORTED Log: public virtual Object {
			public:
				static void Write(const String& source, const String& message);
				static strong<EventLogger> GetEventLogger();
				static void SetEventLogger(strong<EventLogger> se);

			protected:
				static ref<EventLogger> _eventLogger;
		};
	}
}

#endif
