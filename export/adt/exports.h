/*! exports.h */

#ifndef _ADT_VERSION_H_
#define _ADT_VERSION_H_

#if defined(_WIN32) && !defined(__GCC__)
#  ifdef BUILDING_ADT
#    define ADT_API __declspec(dllexport)
#  else
#    define ADT_API __declspec(dllimport)
#  endif
#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#else
#  ifdef BUILDING_ADT
#    define ADT_API __attribute__ ((visibility ("default")))
#  else
#    define ADT_API 
#  endif
#endif

#if defined(__cplusplus)
#  define ADT_EXTERN_C extern "C"
#  define ADT_C_API ADT_EXTERN_C ADT_API
#else
#  define ADT_EXTERN_C
#  define ADT_C_API ADT_API
#endif

#endif/*_ADT_VERSION_H_*/
