#ifndef TOLCAT_TOLCAT_CONFIG_H_
#define TOLCAT_TOLCAT_CONFIG_H_

#ifdef TOLCAT_EXPORT

#ifdef _WIN32
#define TOLCAT_API __declspec(dllexport)
#else // _WIN32
#define TOLCAT_API __attribute__((visibility("default")))
#endif // _WIN32

#else // TOLCAT_EXPORT

#ifdef _WIN32
#define TOLCAT_API __declspec(dllimport)
#else // _WIN32
#define TOLCAT_API
#endif // _WIN32

#endif // TOLCAT_EXPORT

#endif // TOLCAT_TOLCAT_CONFIG_H_