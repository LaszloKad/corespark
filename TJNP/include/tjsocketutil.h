#ifndef _TJ_NP_SOCKET_UTIL_H
#define _TJ_NP_SOCKET_UTIL_H

#include "internal/tjnp.h"

struct sockaddr_in;
struct sockaddr_in6;

#pragma warning(push)
#pragma warning(disable: 4251 4275)

namespace tj {
	namespace np {
		class NP_EXPORTED NetworkInitializer {
			public:
				NetworkInitializer();
				~NetworkInitializer();
				void Initialize();

			protected:
				#ifdef TJ_OS_WIN
					void* _data;
				#endif
		};

		#ifdef TJ_OS_WIN
			typedef SOCKET NativeSocket;
		#else
			typedef int NativeSocket;
		#endif

		class NP_EXPORTED SocketListener: public virtual tj::shared::Object {
			public:
				virtual ~SocketListener();
				virtual void OnReceive(NativeSocket ns) = 0;
		};
		
		class NP_EXPORTED SocketListenerThread: public tj::shared::Thread {
			#ifdef TJ_OS_WIN
				friend LRESULT CALLBACK SocketListenerWindowProc(HWND, UINT, WPARAM, LPARAM);
			#endif
						
			public:
				SocketListenerThread(NativeSocket sock, tj::shared::ref<SocketListener> sl);
				virtual ~SocketListenerThread();
				virtual void Run();
				virtual void Stop();
			
			protected:
				virtual void OnReceive();
				
				tj::shared::weak<SocketListener> _listener;
				NativeSocket _sock;
				
			#ifdef TJ_OS_POSIX
				NativeSocket _controlSocket[2];
			#endif
						
			#ifdef TJ_OS_WIN
				HWND _window;
			#endif
		};
	}
}

#pragma warning(pop)

#endif