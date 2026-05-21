#pragma once

#ifdef _WIN32
#ifdef ASTRA_BUILD
#define ASTRA_API __declspec(dllexport)
#else
#define ASTRA_API __declspec(dllimport)
#endif
#else
#define ASTRA_API
#endif