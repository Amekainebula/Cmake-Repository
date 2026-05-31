#include <iostream>
#include "head.h"

int main()
{
    std::cout << "Hello CMake in VS Code!" << std::endl;
    int a, b;
    std::cin >> a >> b;
    std::cout << add(a, b) << std::endl;
    std::cout << subtract(a, b) << std::endl;
    std::cout << multiply(a, b) << std::endl;
    std::cout << divide(a, b) << std::endl;
    return 0;
}
