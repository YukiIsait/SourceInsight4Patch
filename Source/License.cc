#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <format>
#include <array>
#include <span>
#include "LicenseSerial.h"
#include "License.h"

void License::EnsureAvailability() noexcept {
    try {
        // 获取 %ProgramData%\Source Insight\4.0\si4.lic 文件路径，如果父目录不存在则创建
        std::array<wchar_t, MAX_PATH> commonAppDataPath;
        if (SHGetFolderPathW(nullptr, CSIDL_COMMON_APPDATA, nullptr, 0, commonAppDataPath.data()) != S_OK) {
            throw std::runtime_error("Failed to get CSIDL_COMMON_APPDATA path, error code: " + std::to_string(GetLastError()));
        }
        std::filesystem::path licenseFilePath = std::filesystem::path(commonAppDataPath.data()) / L"Source Insight" / L"4.0" / L"si4.lic";
        std::filesystem::create_directories(licenseFilePath.parent_path());

        // 如果 License 文件不存在或不包含 SourceInsight4Patch 字符串，则生成一个新的
        std::wfstream licenseFile;
        licenseFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        if (std::filesystem::exists(licenseFilePath)) {
            licenseFile.open(licenseFilePath, std::ios::in);
            std::span<const wchar_t> magic(L"SourceInsight4Patch", 19);
            std::istreambuf_iterator<wchar_t> end;
            if (std::search(std::istreambuf_iterator<wchar_t>(licenseFile), end, magic.begin(), magic.end()) != end) {
                licenseFile.close();
                return;
            }
            licenseFile.close();
        }
        licenseFile.open(licenseFilePath, std::ios::out | std::ios::trunc);
        LicenseSerial licenseSerial;
        std::chrono::time_point now = std::chrono::system_clock::now();
        licenseFile << std::format(L"<SourceInsightLicense>\n" \
                                   L"    <LicenseProperties ActId=\"\"\n" \
                                   L"                       HWID=\"\"\n" \
                                   L"                       Serial=\"{:s}\"\n" \
                                   L"                       LicensedUser=\"SourceInsight4Patch\"\n" \
                                   L"                       Organization=\"Fox\"\n" \
                                   L"                       Email=\"\"\n" \
                                   L"                       Type=\"Standard\"\n" \
                                   L"                       Version=\"4\"\n" \
                                   L"                       MinorVersion=\"0\"\n" \
                                   L"                       Date=\"{:%Y-%m-%d}\" />\n" \
                                   L"    <Signature Value=\"SourceInsight4Patch\" />\n" \
                                   L"</SourceInsightLicense>", licenseSerial.Generate(), now);
        licenseFile.close();
    } catch (const std::exception& exception) {
        MessageBoxW(nullptr, (std::wstringstream() << L"Failed to generate license file: " << exception.what()).str().data(), L"License Generation Failed", MB_ICONSTOP);
    }
}
