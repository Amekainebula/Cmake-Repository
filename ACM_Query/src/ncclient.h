#pragma once
#include <string>

struct NCUserData {
    std::string handle;
    int rating = 0;
    int maxRating = 0;
    int solvedCount = 0;
    std::string rank;
    std::string error;
};

NCUserData queryNowCoder(const std::string& handle);
