#ifndef TOLCAT_TOLCAT_CONFIG_H_
#define TOLCAT_TOLCAT_CONFIG_H_

#ifdef TOLCAT_EXPORT
#define TOLCAT_API __declspec(dllexport)
#else // TOLCAT_EXPORT
#define TOLCAT_API __declspec(dllimport)
#endif // TOLCAT_EXPORT

#endif // TOLCAT_TOLCAT_CONFIG_H_