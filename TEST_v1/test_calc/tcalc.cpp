#include <iostream>
#include "calc.h"

int main()
{
    std::cout << "Calc Test" << std::endl;
#ifdef DEBUG
    std::cout << "Debug!\n";
#endif
    int a, b;
    std::cin >> a >> b;
    std::cout << "a + b = " << add(a, b) << std::endl;
    std::cout << "a - b = " << subtract(a, b) << std::endl;
    std::cout << "a * b = " << multiply(a, b) << std::endl;
    std::cout << "a / b = " << divide(a, b) << std::endl;
    return 0;
}
