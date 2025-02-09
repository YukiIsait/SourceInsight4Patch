#include <Windows.h>
#include <wininet.h>
#include <sstream>
#include "Detours.h"
#include "Patch.h"

decltype(::CryptVerifySignatureW)* Patch::cryptVerifySignatureW = ::CryptVerifySignatureW;
decltype(::InternetOpenW)* Patch::internetOpenW = ::InternetOpenW;

BOOL Patch::CryptVerifySignatureW(HCRYPTHASH hHash, const BYTE* pbSignature, DWORD dwSigLen, HCRYPTKEY hPubKey, LPCWSTR szDescription, DWORD dwFlags) {
    // 跳过签名验证
    return TRUE;
}

HINTERNET Patch::InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags) {
    // 阻止检查授权，防止序列号被加入在线黑名单
    // 检查授权的 UA 是 Source Insight，检查更新的 UA 是 SourceInsight/4.0
    if (std::wcscmp(lpszAgent, L"Source Insight") == 0) {
        SetLastError(ERROR_ACCESS_DENIED);
        return NULL;
    }
    return internetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
}

void Patch::Install() noexcept {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&reinterpret_cast<PVOID&>(cryptVerifySignatureW), Patch::CryptVerifySignatureW);
    DetourAttach(&reinterpret_cast<PVOID&>(internetOpenW), Patch::InternetOpenW);
    LONG code = DetourTransactionCommit();
    if (code != NO_ERROR) {
        MessageBoxW(nullptr, (std::wstringstream() << L"Failed to hook, error code: " << code).str().data(), L"Hook Failed", MB_ICONSTOP);
    }
}

void Patch::Uninstall() noexcept {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&reinterpret_cast<PVOID&>(cryptVerifySignatureW), Patch::CryptVerifySignatureW);
    DetourDetach(&reinterpret_cast<PVOID&>(internetOpenW), Patch::InternetOpenW);
    LONG code = DetourTransactionCommit();
    if (code != NO_ERROR) {
        MessageBoxW(nullptr, (std::wstringstream() << L"Failed to unhook, error code: " << code).str().data(), L"Unhook Failed", MB_ICONSTOP);
    }
}
