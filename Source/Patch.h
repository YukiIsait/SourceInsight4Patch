#pragma once
#include <Windows.h>
#include <wininet.h>

class Patch {
private:
    Patch() = delete;

private:
    static decltype(::CryptVerifySignatureW)* cryptVerifySignatureW;
    static decltype(::InternetOpenW)* internetOpenW;
    static BOOL WINAPI CryptVerifySignatureW(HCRYPTHASH hHash, const BYTE* pbSignature, DWORD dwSigLen, HCRYPTKEY hPubKey, LPCWSTR szDescription, DWORD dwFlags);
    static HINTERNET WINAPI InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);

public:
    static void Install() noexcept;
    static void Uninstall() noexcept;
};
