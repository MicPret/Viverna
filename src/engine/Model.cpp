#include <viverna/graphics/Model.hpp>
#include <viverna/core/Debug.hpp>

#include <utility>

namespace verna {

static constexpr auto AsIndex(MeshId mesh_id) {
    return mesh_id & 0xFFFFFFFF;
}
static constexpr MeshId AsMeshId(ModelId model_id, size_t index) {
    return (model_id << 32) | index;
}
static constexpr ModelId GetModelId(MeshId mesh_id) {
    return mesh_id & 0xFFFFFFFF00000000;
}

Model::Model() {
    static uint64_t next_id = 1;
    id = next_id++;
    meshes.reserve(1);
}

const std::vector<Mesh>& Model::Meshes() const {
    return meshes;
}

const std::vector<Material>& Model::Materials() const {
    return materials;
}

MeshId Model::AddMesh(const Mesh& mesh) {
    MeshId output = AsMeshId(id, meshes.size());
    meshes.push_back(mesh);
    return output;
}

MeshId Model::AddMesh(Mesh&& mesh) {
    MeshId output = AsMeshId(id, meshes.size());
    meshes.push_back(std::move(mesh));
    return output;
}

const Material& Model::GetMaterial(MeshId mesh_id) const {
    auto index = AsIndex(mesh_id);
    VERNA_LOGE_IF(!Contains(mesh_id),
                  "GetMaterial failed: can't access material "
                      + std::to_string(mesh_id)
                      + " (index: " + std::to_string(index) + ')');
    return materials[index];
}

void Model::SetMaterial(MeshId mesh_id, const Material& material) {
    auto index = AsIndex(mesh_id);
    VERNA_LOGE_IF(!Contains(mesh_id),
                  "SetMaterial failed: can't access material "
                      + std::to_string(mesh_id)
                      + " (index: " + std::to_string(index) + ')');
    materials[index] = material;
}

const Mesh& Model::GetMesh(MeshId mesh_id) const {
    auto index = AsIndex(mesh_id);
    VERNA_LOGE_IF(!Contains(mesh_id),
                  "GetMesh failed: can't access mesh " + std::to_string(mesh_id)
                      + " (index: " + std::to_string(index) + ')');
    return meshes[index];
}

bool Model::Contains(MeshId mesh_id) const {
    auto model_id = GetModelId(mesh_id);
    auto index = AsIndex(mesh_id);
    return (model_id == id) && (index < meshes.size());
}

}  // namespace verna