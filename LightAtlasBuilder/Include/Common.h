#pragma once

#ifndef null
#define null 0
#endif

//#define PI 3.14159265358979323846f

#define SafeDeleteAndNull(ptr) if (ptr != null) { delete ptr; ptr = null; }
#define SafeDeleteArrayAndNull(ptr) if (ptr != null) { delete [] ptr; ptr = null; }