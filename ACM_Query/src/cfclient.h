#pragma once
#include <string>
#include <optional>

struct CFUserData {
    std::string handle;
    int rating = 0;
    int maxRating = 0;
    std::string rank;
    int solvedCount = 0;
    std::string error;
};

// Query Codeforces user: returns rating + solved problems count
CFUserData queryCodeforces(const std::string& handle);
