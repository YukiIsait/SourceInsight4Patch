#include <Windows.h>
#include "ProxyLibrary.h"
#include "License.h"
#include "Patch.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD reasonForCall, LPVOID reserved) {
    switch (reasonForCall) {
        case DLL_PROCESS_ATTACH:
            ProxyLibrary_Load();
            if (GetModuleHandleW(L"sourceinsight4.exe") != NULL) {
                Patch::Install();
                License::EnsureAvailability();
            }
            break;
        case DLL_PROCESS_DETACH:
            if (GetModuleHandleW(L"sourceinsight4.exe") != NULL) {
                Patch::Uninstall();
            }
            ProxyLibrary_Unload();
            break;
    }
    return TRUE;
}
