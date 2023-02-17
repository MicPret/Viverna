#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>

#if defined(VERNA_WINDOWS)
#include <windows.h>
#elif defined(VERNA_LINUX)
#include <limits.h>
#include <unistd.h>
#endif

#include <array>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace verna {

static std::filesystem::path assets_folder_path;

void InitializeAssets(VivernaState& state) {
    if (state.GetFlag(VivernaState::ASSETS_INITIALIZED_FLAG))
        return;
    constexpr size_t len = 256;
    constexpr auto separator = std::filesystem::path::preferred_separator;
    std::array<char, len> bf;
    std::string process_path;
    int bytes;
#if defined(VERNA_WINDOWS)
    bytes = GetModuleFileName(nullptr, bf.data(), len);
    process_path = std::string(bf.data(), bytes);
#elif defined(VERNA_LINUX)
    // TODO test!!!
    bytes = MIN(readlink("/proc/self/exe", bf.data(), len), len - 1);
    process_path = std::string(bf.data(), bytes);
#else
#error Platform not supported!
#endif
    process_path = process_path.substr(0, process_path.rfind(separator));
    constexpr std::string_view assets_folder_name = "assets";
    assets_folder_path =
        process_path / std::filesystem::path(assets_folder_name);

    state.SetFlag(VivernaState::ASSETS_INITIALIZED_FLAG, true);
    VERNA_LOGI("Assets initialized!");
}

void TerminateAssets(VivernaState& state) {
    if (!state.GetFlag(VivernaState::ASSETS_INITIALIZED_FLAG))
        return;
    assets_folder_path = std::filesystem::path();
    state.SetFlag(VivernaState::ASSETS_INITIALIZED_FLAG, false);
    VERNA_LOGI("Assets terminated!");
}
}  // namespace verna