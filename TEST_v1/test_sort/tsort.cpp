#include <iostream>
#include <vector>
#include "sort.h"

int main()
{
    std::vector<int> a = {4, 3, 2, 1};
    std::cout << "First: ";
    for (auto &x : a)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    Maopao(a);
    std::cout << "Maopao: ";
    for (auto &x : a)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    a = {4, 3, 2, 1};

    Qsort(a);
    std::cout << "Qsort: ";
    for (auto &x : a)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    return 0;
}