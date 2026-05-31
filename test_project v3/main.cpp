#include <iostream>
#include "head.h"

int main()
{
    std::cout << "Hello CMake in VS Code!" << std::endl;
    int a, b;
    std::cin >> a >> b;
    std::cout << "a + b = " << add(a, b) << std::endl;
    std::cout << "a - b = " << subtract(a, b) << std::endl;
    std::cout << "a * b = " << multiply(a, b) << std::endl;
    std::cout << "a / b = " << divide(a, b) << std::endl;
    return 0;
}
