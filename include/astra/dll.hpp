#pragma once

#define NOMINMAX

#if defined(_WIN32) && !defined(ASTRA_STATICLIB)
#ifdef ASTRA_BUILD
#define ASTRA_API __declspec(dllexport)
#else
#define ASTRA_API __declspec(dllimport)
#endif
#else
#define ASTRA_API
#endif