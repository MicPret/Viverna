#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>

#if defined(VERNA_WINDOWS)
#include <windows.h>
#elif defined(VERNA_LINUX)
#include <limits.h>
#include <unistd.h>
#include <algorithm>
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
    int bytes;
#if defined(VERNA_WINDOWS)
    bytes = static_cast<int>(
        GetModuleFileName(nullptr, bf.data(), static_cast<DWORD>(len)));
#elif defined(VERNA_LINUX)
    bytes =
        std::min(static_cast<int>(readlink("/proc/self/exe", bf.data(), len)),
                 static_cast<int>(len - 1));
#else
#error Platform not supported!
#endif
    std::string process_path(bf.data(), bytes);
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

std::vector<char> LoadRawAsset(const std::filesystem::path& path) {
    auto fullpath = assets_folder_path / path;
    std::ifstream file(fullpath, std::ios::binary);
    if (!file.is_open()) {
        VERNA_LOGE("LoadRawAsset failed: can't find " + fullpath.string());
        return {};
    }
    file.seekg(0, file.end);
    auto size = file.tellg();
    file.seekg(0, file.beg);
    std::vector<char> output(size);
    file.read(output.data(), size);
    if (file.fail()) {
        VERNA_LOGE("LoadRawAsset failed: read failure for "
                   + fullpath.string());
        output.clear();
    }
    return output;
}

bool AssetExists(const std::filesystem::path& path) {
    std::error_code err_code;
    return std::filesystem::is_regular_file(assets_folder_path / path,
                                            err_code);
}

std::vector<std::filesystem::path> GetAssetsInDirectory(
    const std::filesystem::path& path) {
    std::vector<std::filesystem::path> result;
    auto in_path = assets_folder_path / path;
    if (!std::filesystem::exists(in_path)) {
        VERNA_LOGW("Failed to open directory " + in_path.string());
        return result;
    }
    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(in_path))
        if (!entry.is_directory())
            result.push_back(entry.path());

    return result;
}
}  // namespace verna
