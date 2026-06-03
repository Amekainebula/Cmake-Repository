#include "atclient.h"
#include <windows.h>
#include <winhttp.h>
#include "nlohmann/json.hpp"

#pragma comment(lib, "winhttp.lib")

static std::string WinHttpGet(const std::wstring& server, const std::wstring& path) {
    std::string body;
    HINTERNET hSession = WinHttpOpen(L"ACM_Query/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";

    HINTERNET hConnect = WinHttpConnect(hSession, server.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
        nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    DWORD secFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                     SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                     SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &secFlags, sizeof(secFlags));

    DWORD timeout = 10000;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    BOOL ok = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (ok) ok = WinHttpReceiveResponse(hRequest, nullptr);
    if (ok) {
        DWORD size = 0;
        char buf[4096];
        while (WinHttpQueryDataAvailable(hRequest, &size) && size > 0) {
            DWORD read = 0;
            DWORD toRead = (size < sizeof(buf)) ? size : (DWORD)sizeof(buf);
            if (WinHttpReadData(hRequest, buf, toRead, &read))
                body.append(buf, read);
            else break;
        }
    }
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return body;
}

static std::string atcoderRank(int rating) {
    if (rating >= 2800) return "red";
    if (rating >= 2400) return "orange";
    if (rating >= 2000) return "yellow";
    if (rating >= 1600) return "blue";
    if (rating >= 1200) return "cyan";
    if (rating >=  800) return "green";
    if (rating >=  400) return "brown";
    if (rating >=    1) return "gray";
    return "unrated";
}

ACUserData queryAtCoder(const std::string& handle) {
    ACUserData data;
    data.handle = handle;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, handle.c_str(), -1, nullptr, 0);
    std::wstring whandle(wlen - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, handle.c_str(), -1, &whandle[0], wlen);

    // --- Fetch rating history ---
    std::wstring path = L"/users/" + whandle + L"/history/json";
    std::string body = WinHttpGet(L"atcoder.jp", path);

    if (body.empty()) {
        data.error = "User not found or network error";
        return data;
    }

    try {
        auto json = nlohmann::json::parse(body);
        if (!json.is_array() || json.empty()) {
            data.error = "No contest history found";
            return data;
        }
        int maxR = 0, curR = 0;
        for (auto& entry : json) {
            int nr = entry.value("NewRating", 0);
            if (nr > maxR) maxR = nr;
            curR = nr;
        }
        data.rating = curR;
        data.maxRating = maxR;
        data.rank = atcoderRank(curR);
    } catch (const std::exception& e) {
        data.error = std::string("Parse error: ") + e.what();
        return data;
    }

    // --- Fetch solved count from kenkoooo API ---
    std::string abody = WinHttpGet(L"kenkoooo.com",
        L"/atcoder/atcoder-api/v3/user_info?user=" + whandle);
    if (!abody.empty()) {
        try {
            auto aj = nlohmann::json::parse(abody);
            data.solvedCount = aj.value("accepted_count", 0);
        } catch (...) {
            // solved count is optional, don't fail
        }
    }

    return data;
}
