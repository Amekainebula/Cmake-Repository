#include <iostream>
#include <vector>
#include "sort.h"

void Maopao(std::vector<int> &a)
{
    for (int i = 0; i < a.size() - 1; ++i)
    {
        for (int j = i + 1; j < a.size(); ++j)
        {
            if (a[i] > a[j])
                std::swap(a[i], a[j]);
        }
    }
}