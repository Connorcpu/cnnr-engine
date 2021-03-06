#pragma once

#if defined(_MSC_VER) || defined(MSVC)

#ifndef MSVC
#define MSVC
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <atlbase.h>
#include <Shellapi.h>
#include <comdef.h>

template <typename T>
using ComPtr = ATL::CComPtr<T>;

#pragma warning(3 : 4996)

#ifdef NDEBUG
#define unreachable() (__assume(0))
#else
#include <cassert>
#define unreachable() ((assert(false && "Unreachable statement reached")), (__assume(0)))
#endif

#else /* ifdef MSVC */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>

#ifdef MACOS
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#ifdef NDEBUG
#define unreachable() (__builtin_unreachable())
#else
#include <cassert>
#define unreachable() ((assert(false && "Unreachable statement reached")), (__builtin_unreachable()))
#endif

#endif /* ifdef MSVC */
