#ifndef VERNA_MODEL_HPP
#define VERNA_MODEL_HPP

#include "Mesh.hpp"
#include "Material.hpp"

#include <cstdint>
#include <vector>

namespace verna {

using ModelId = uint64_t;
using MeshId = uint64_t;

class Model {
   public:
    Model();
    const std::vector<Mesh>& Meshes() const;
    const std::vector<Material>& Materials() const;
    MeshId AddMesh(const Mesh& mesh);
    MeshId AddMesh(Mesh&& mesh);
    const Material& GetMaterial(MeshId mesh_id) const;
    void SetMaterial(MeshId mesh_id, const Material& material);
    const Mesh& GetMesh(MeshId mesh_id) const;

   private:
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    ModelId id;
    bool Contains(MeshId mesh_id) const;
};
}  // namespace verna

#endif