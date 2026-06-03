#include "qojclient.h"
#include <windows.h>
#include <winhttp.h>
#include <regex>

#pragma comment(lib, "winhttp.lib")

static std::string WinHttpGet(const std::wstring& server, const std::wstring& path) {
    std::string body;
    HINTERNET hSession = WinHttpOpen(L"ACM_Query/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";

    HINTERNET hConnect = WinHttpConnect(hSession, server.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    DWORD secFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &secFlags, sizeof(secFlags));
    DWORD timeout = 15000;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    LPCWSTR headers = L"Accept: text/html,application/xhtml+xml\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n";
    BOOL ok = WinHttpSendRequest(hRequest, headers, (DWORD)wcslen(headers), WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (ok) ok = WinHttpReceiveResponse(hRequest, nullptr);
    if (ok) {
        DWORD size = 0; char buf[8192];
        while (WinHttpQueryDataAvailable(hRequest, &size) && size > 0) {
            DWORD read = 0;
            DWORD toRead = (size < sizeof(buf)) ? size : (DWORD)sizeof(buf);
            if (WinHttpReadData(hRequest, buf, toRead, &read)) body.append(buf, read);
            else break;
        }
    }
    WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    return body;
}

QOJUserData queryQOJ(const std::string& handle) {
    QOJUserData data;
    data.handle = handle;

    int wlen = MultiByteToWideChar(CP_UTF8, 0, handle.c_str(), -1, nullptr, 0);
    std::wstring whandle(wlen - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, handle.c_str(), -1, &whandle[0], wlen);

    std::wstring path = L"/user/profile/" + whandle;
    std::string body = WinHttpGet(L"qoj.ac", path);

    if (body.empty()) {
        data.error = "Network error or user not found (check handle)";
        return data;
    }

    // Try multiple regex patterns for "Accepted problems"
    std::vector<std::regex> patterns = {
        std::regex(R"(Accepted problems[：:]\s*(\d+)\s*problems?)"),
        std::regex(R"(Accepted problems[：:]\s*(\d+))"),
        std::regex(R"(accepted[：:]\s*(\d+))", std::regex::icase),
    };

    for (auto& re : patterns) {
        std::smatch match;
        if (std::regex_search(body, match, re)) {
            data.solvedCount = std::stoi(match[1].str());
            return data;
        }
    }

    data.error = "Could not find Accepted problems count on profile page";
    return data;
}