#pragma once

#ifndef null
#define null 0
#endif

#define SafeDeleteAndNull(ptr) if (ptr != null) { delete ptr; ptr = null; }
#define SafeDeleteArrayAndNull(ptr) if (ptr != null) { delete [] ptr; ptr = null; }