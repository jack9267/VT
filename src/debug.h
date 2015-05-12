#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef _DEBUG
#define DBG(x) x
#ifdef _MSC_VER
#include <crtdbg.h>
#define MSCDBG(x) x
#else
#define MSCDBG(x)
#endif
#else
#define DBG(x)
#define MSCDBG(x)
#endif

int dbg_printf(const char *fmt, ...);

#endif // _DEBUG_H_
