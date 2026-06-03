#ifndef _CALC_H
#define _CALC_H

#ifdef calc_EXPORTS
#define CALC_API __declspec(dllexport)
#else
#define CALC_API __declspec(dllimport)
#endif

// 加法
CALC_API int add(int a, int b);
// 减法
CALC_API int subtract(int a, int b);
// 乘法
CALC_API int multiply(int a, int b);
// 除法
CALC_API double divide(int a, int b);
#endif
