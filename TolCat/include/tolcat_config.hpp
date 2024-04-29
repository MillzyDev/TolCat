#pragma once

#ifdef TOLCAT_EXPORT
#define TOLCAT_API __declspec(dllexport)
#else
#define TOLCAT_API
#endif