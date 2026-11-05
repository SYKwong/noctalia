#include "render/animation/motion_service.h"

#include "render/animation/animation_manager.h"

#include <algorithm>
#include <vector>

MotionService& MotionService::instance() {
  static MotionService service;
  return service;
}

void MotionService::registerManager(AnimationManager* manager) {
  if (manager == nullptr) {
    return;
  }
  m_managers.insert(manager);
}

void MotionService::unregisterManager(AnimationManager* manager) {
  if (manager == nullptr) {
    return;
  }
  m_managers.erase(manager);
}

void MotionService::reduceMotionForKind(AnimationKind kind) {
  const std::vector<AnimationManager*> managers(m_managers.begin(), m_managers.end());
  for (auto* manager : managers) {
    if (manager != nullptr && m_managers.contains(manager)) {
      manager->reduceMotion(kind);
    }
  }
}

void MotionService::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  if (!m_enabled) {
    reduceMotionForKind(AnimationKind::General);
  }
}

void MotionService::setSpeed(float speed) { m_speed = std::clamp(speed, 0.05F, 4.0F); }

void MotionService::setLauncherEnabled(bool enabled) {
  if (m_launcher == enabled) {
    return;
  }
  m_launcher = enabled;
  if (!m_launcher) {
    reduceMotionForKind(AnimationKind::Launcher);
  }
}

void MotionService::setPanelsEnabled(bool enabled) {
  if (m_panels == enabled) {
    return;
  }
  m_panels = enabled;
  if (!m_panels) {
    reduceMotionForKind(AnimationKind::Panels);
  }
}

void MotionService::setBarEnabled(bool enabled) {
  if (m_bar == enabled) {
    return;
  }
  m_bar = enabled;
  if (!m_bar) {
    reduceMotionForKind(AnimationKind::Bar);
  }
}

void MotionService::setWidgetsEnabled(bool enabled) {
  if (m_widgets == enabled) {
    return;
  }
  m_widgets = enabled;
  if (!m_widgets) {
    reduceMotionForKind(AnimationKind::Widgets);
  }
}

void MotionService::setDockEnabled(bool enabled) {
  if (m_dock == enabled) {
    return;
  }
  m_dock = enabled;
  if (!m_dock) {
    reduceMotionForKind(AnimationKind::Dock);
  }
}

bool MotionService::enabled(AnimationKind kind) const noexcept {
  if (!m_enabled) {
    return false;
  }
  switch (kind) {
  case AnimationKind::Launcher:
    return m_launcher;
  case AnimationKind::Panels:
    return m_panels;
  case AnimationKind::Bar:
    return m_bar;
  case AnimationKind::Widgets:
    return m_widgets;
  case AnimationKind::Dock:
    return m_dock;
  case AnimationKind::General:
  default:
    return true;
  }
}
