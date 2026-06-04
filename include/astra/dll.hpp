#pragma once

#ifdef _WIN32
#define NOMINMAX
#ifdef ASTRA_BUILD
#define ASTRA_API __declspec(dllexport)
#else
#define ASTRA_API __declspec(dllimport)
#endif
#else
#define ASTRA_API
#endif