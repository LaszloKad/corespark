#ifndef _TJDB_H
#define _TJDB_H

#include "../../TJShared/include/tjshared.h"
using tj::shared::ref;

#ifdef TJDB_EXPORTS
	#ifdef TJ_OS_WIN
		#define DB_EXPORTED __declspec(dllexport)
	#else
		#define DB_EXPORTED
	#endif
#else
	#ifdef TJ_OS_WIN
		#define DB_EXPORTED __declspec(dllimport)
	#else
		#define DB_EXPORTED
	#endif
#endif

#pragma warning(push)
#pragma warning(disable: 4251 4275)

#include "tjdbdatabase.h"

#pragma warning(pop)

#endif