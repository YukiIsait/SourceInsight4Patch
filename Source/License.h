#pragma once

class License {
private:
    License() = delete;

public:
    static void EnsureAvailability() noexcept;
};
