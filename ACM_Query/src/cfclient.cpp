#include "cfclient.h"
#include <windows.h>
#include <winhttp.h>
#include <set>
#include "nlohmann/json.hpp"

#pragma comment(lib, "winhttp.lib")

static std::string WinHttpGet(const std::wstring& server, const std::wstring& path) {
    std::string body;

    HINTERNET hSession = WinHttpOpen(L"ACM_Query/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) return "";

    HINTERNET hConnect = WinHttpConnect(hSession, server.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
        nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    DWORD secFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                     SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
                     SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &secFlags, sizeof(secFlags));

    DWORD timeout = 10000;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    BOOL ok = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

    if (ok) ok = WinHttpReceiveResponse(hRequest, nullptr);

    if (ok) {
        DWORD size = 0;
        char buf[4096];
        while (WinHttpQueryDataAvailable(hRequest, &size) && size > 0) {
            DWORD read = 0;
            DWORD toRead = (size < sizeof(buf)) ? size : (DWORD)sizeof(buf);
            if (WinHttpReadData(hRequest, buf, toRead, &read)) {
                body.append(buf, read);
            } else break;
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return body;
}

CFUserData queryCodeforces(const std::string& handle) {
    CFUserData data;
    data.handle = handle;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, handle.c_str(), -1, nullptr, 0);
    std::wstring whandle(wlen - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, handle.c_str(), -1, &whandle[0], wlen);

    std::wstring path = L"/api/user.info?handles=" + whandle;
    std::string body = WinHttpGet(L"codeforces.com", path);

    if (body.empty()) {
        data.error = "Network error or timeout";
        return data;
    }

    try {
        auto json = nlohmann::json::parse(body);
        if (json["status"] != "OK") {
            data.error = json.value("comment", "Unknown API error");
            return data;
        }
        auto& user = json["result"][0];
        data.rating    = user.value("rating", 0);
        data.maxRating = user.value("maxRating", 0);
        data.rank      = user.value("rank", "unrated");
    } catch (const std::exception& e) {
        data.error = std::string("JSON parse error: ") + e.what();
        return data;
    }

    path = L"/api/user.status?handle=" + whandle + L"&from=1&count=10000";
    body = WinHttpGet(L"codeforces.com", path);

    if (body.empty()) {
        data.error = "Failed to fetch submissions (network error)";
        return data;
    }

    try {
        auto json = nlohmann::json::parse(body);
        if (json["status"] != "OK") {
            data.error = json.value("comment", "Unknown API error");
            return data;
        }
        std::set<std::string> solved;
        for (auto& sub : json["result"]) {
            if (sub.value("verdict", "") == "OK") {
                auto& prob = sub["problem"];
                std::string key = std::to_string(prob.value("contestId", 0))
                    + "/" + prob.value("index", "");
                solved.insert(key);
            }
        }
        data.solvedCount = (int)solved.size();
    } catch (const std::exception& e) {
        data.error = std::string("JSON parse error (submissions): ") + e.what();
    }

    return data;
}
