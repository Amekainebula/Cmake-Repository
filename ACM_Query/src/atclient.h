#pragma once
#include <string>

struct ACUserData {
    std::string handle;
    int rating = 0;
    int maxRating = 0;
    std::string rank;
    int solvedCount = 0;
    std::string error;
};

ACUserData queryAtCoder(const std::string& handle);
