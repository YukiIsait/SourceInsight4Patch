#pragma once
#include <array>
#include <string>
#include <random>
#include <cstdint>

class LicenseSerial {
private:
    static std::array<uint8_t, 256> substitutionTable;
    static std::array<uint8_t, 26> alphabetTable;

private:
    std::random_device randomDevice;
    std::mt19937 randomEngine;
    wchar_t GenerateRandomLetterOrDigit() noexcept;
    wchar_t GenerateRandomRGDF() noexcept;

public:
    LicenseSerial() noexcept;
    std::wstring Generate() noexcept;
};
