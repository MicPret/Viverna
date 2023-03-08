#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>

#include <android_native_app_glue.h>

namespace verna {

static AAssetManager* asset_manager = nullptr;

static void Error(VivernaState& state,
                  [[maybe_unused]] std::string_view message) {
    state.SetFlag(VivernaState::ERROR_FLAG, true);
    VERNA_LOGE(message);
}

void InitializeAssets(VivernaState& state) {
    if (state.GetFlag(VivernaState::ASSETS_INITIALIZED_FLAG))
        return;
    if (state.native_app == nullptr) {
        Error(state, "InitializeAssets failed: state.native_app == nullptr!");
        return;
    }
    android_app* app = static_cast<android_app*>(state.native_app);
    if (app->activity == nullptr) {
        Error(state, "InitializeAssets failed: app->activity == nullptr!");
        return;
    }
    if (app->activity->assetManager == nullptr) {
        Error(
            state,
            "InitializeAssets failed: app->activity->assetManager == nullptr!");
        return;
    }
    asset_manager = app->activity->assetManager;
    state.SetFlag(VivernaState::ASSETS_INITIALIZED_FLAG, true);
    VERNA_LOGI("Assets initialized!");
}

void TerminateAssets(VivernaState& state) {
    if (!state.GetFlag(VivernaState::ASSETS_INITIALIZED_FLAG))
        return;
    asset_manager = nullptr;
    state.SetFlag(VivernaState::ASSETS_INITIALIZED_FLAG, false);
    VERNA_LOGI("Assets terminated!");
}

std::vector<char> LoadRawAsset(const std::filesystem::path& path) {
    VERNA_LOGE_IF(asset_manager == nullptr,
                  "LoadRawAsset failed: Call InitializeAssets!");
    AAsset* asset =
        AAssetManager_open(asset_manager, path.c_str(), AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        VERNA_LOGE("LoadRawAsset failed: failed to load " + path.string());
        return {};
    }
    off_t size = AAsset_getLength(asset);
    std::vector<char> buffer(size);
    int read_size = AAsset_read(asset, buffer.data(), size);
    AAsset_close(asset);
    if (read_size != size) {
        VERNA_LOGE("LoadRawAsset failed: incoherent asset size!");
        buffer.clear();
    }
    return buffer;
}

bool AssetExists(const std::filesystem::path& path) {
    VERNA_LOGE_IF(asset_manager == nullptr,
                  "AssetExists failed: Call InitializeAssets!");
    AAsset* asset =
        AAssetManager_open(asset_manager, path.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == nullptr) {
        return false;
    }
    AAsset_close(asset);
    return true;
}
}  // namespace verna