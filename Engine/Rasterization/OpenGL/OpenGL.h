#pragma once

#ifdef WIN32
#include <GL/glew.h>
#endif

#ifdef ANDROID
#include <GLES2/gl2.h>
#endif

#ifdef EMSCRIPTEN
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif