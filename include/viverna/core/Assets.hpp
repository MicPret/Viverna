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

/**
 * @brief Checks whether an asset exists (path should not include "asset/")
 *
 * @param path Path to an asset (e.g. "shaders/unlit.vert")
 * @return true if specified path leads to a valid asset directory, false
 * otherwise
 */
bool AssetExists(const std::filesystem::path& path);

/**
 * @brief Retrieves all assets inside the specified directory (recursively)
 *
 * @param path Base folder
 * @param fullpath Assets directories are returned as fullpaths
 * @return Every asset inside path
 */
std::vector<std::filesystem::path> GetAssetsInDirectory(
    const std::filesystem::path& path,
    bool fullpath = false);
}  // namespace verna

#endif