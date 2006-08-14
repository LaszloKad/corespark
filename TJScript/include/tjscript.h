#ifndef _TJSCRIPT_H
#define _TJSCRIPT_H

#ifdef TJSCRIPT_EXPORTS 
	#define SCRIPT_EXPORTED __declspec(dllexport)
#else
	#define SCRIPT_EXPORTED
#endif

#pragma warning(push)
#pragma warning(disable: 4251) // protected member needs dll interface
#pragma warning(disable: 4275) // base class needs dll interface

#include <map>
#include <string>
#include <stack>
#include <vector>
#include <deque>

#include "../../TJShared/include/tjshared.h"
#include "tjscriptexception.h"
#include "tjscriptable.h"
#include "tjscriptvalue.h"
#include "tjscriptparameterlist.h"
#include "tjscriptvm.h"
#include "tjcompiledscript.h"
#include "tjscripttype.h"
#include "tjscriptthread.h"
#include "tjscriptcontext.h"
#include "tjscriptparameter.h"
#include "tjscriptscope.h"
#include "types/tjstatictypes.h"

#pragma warning(pop)

#endif