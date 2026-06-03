#pragma once
#include <string>

struct QOJUserData {
    std::string handle;
    int solvedCount = 0;
    std::string error;
};

QOJUserData queryQOJ(const std::string& handle);
