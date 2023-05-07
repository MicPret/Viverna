#ifndef VERNA_EDITOR_EDITOR_GUI
#define VERNA_EDITOR_EDITOR_GUI

#include <viverna/ecs/World.hpp>

#include <vector>

namespace editor {

using NewEntityFunc = verna::Entity (*)(verna::World& world);

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
               int& selected_id,
               NewEntityFunc on_new_entity);
void LightingTab();
void CameraTab(float& camera_speed);

}  // namespace editor

#endif
