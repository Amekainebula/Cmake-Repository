#ifndef _SORT_H
#define _SORT_H

#include <vector>

#ifdef sort_EXPORTS
#define SORT_API __declspec(dllexport)
#else
#define SORT_API __declspec(dllimport)
#endif

SORT_API void Maopao(std::vector<int> &a);
SORT_API void Qsort(std::vector<int> &a);
#endif
