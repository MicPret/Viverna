#ifndef VERNA_EDITOR_EDITOR_GUI
#define VERNA_EDITOR_EDITOR_GUI

#include <viverna/ecs/World.hpp>
#include <viverna/graphics/DirectionLight.hpp>

#include <vector>

namespace editor {

void InitGUI();
void TermGUI();
void BeginGUI();
void EndGUI();

// Must call EndWindow (no matter its return value)!
bool BeginWindow(const char* name);
void EndWindow();

bool BeginTabs();
void EndTabs();
void EntityTab(verna::World& world,
               std::vector<verna::Entity>& entities,
               int& selected_id);
void AssetsTab(verna::World& world, std::vector<verna::Entity>& entities);
void LightingTab(verna::DirectionLight& dirlight);
void CameraTab(float& camera_speed);

}  // namespace editor

#endif
