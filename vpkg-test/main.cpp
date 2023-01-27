#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/spdlog.h>

int main() {
    #ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    SPDLOG_INFO("Hello world !");
    SPDLOG_DEBUG("Should show up in debug mode");
    SPDLOG_INFO(std::string("VCPKG_ROOT = ") + std::getenv("VCPKG_ROOT"));
}