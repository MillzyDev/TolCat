#ifndef TOLCAT_TOLCAT_CONFIG_H_
#define TOLCAT_TOLCAT_CONFIG_H_

#ifdef TOLCAT_EXPORT
//#define TOLCAT_API __declspec(dllexport)
#define TOLCAT_API __attribute__((visibility("default")))
#else // TOLCAT_EXPORT
//#define TOLCAT_API __declspec(dllimport)
#define TOLCAT_API
#endif // TOLCAT_EXPORT

#endif // TOLCAT_TOLCAT_CONFIG_H_