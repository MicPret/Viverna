#ifndef VERNA_ASSETS_HPP
#define VERNA_ASSETS_HPP

#include <viverna/core/VivernaState.hpp>

#include <filesystem>
#include <vector>

namespace verna {
void InitializeAssets(VivernaState& state);
void TerminateAssets(VivernaState& state);
/**
 * @brief Loads an asset as raw data
 *
 * @param path Filepath of the asset, excluding "asset/" (e.g.
 * "shaders/unlit.vert")
 * @return Raw data
 */
std::vector<char> LoadRawAsset(const std::filesystem::path& path);
}  // namespace verna

#endif