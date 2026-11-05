#pragma once

#include "render/animation/animation.h"

#include <cstdint>
#include <unordered_set>

class AnimationManager;

class MotionService {
public:
  static MotionService& instance();

  MotionService(const MotionService&) = delete;
  MotionService& operator=(const MotionService&) = delete;

  void registerManager(AnimationManager* manager);
  void unregisterManager(AnimationManager* manager);

  void setEnabled(bool enabled);
  void setSpeed(float speed);
  void setLauncherEnabled(bool enabled);
  void setPanelsEnabled(bool enabled);
  void setBarEnabled(bool enabled);
  void setWidgetsEnabled(bool enabled);
  void setDockEnabled(bool enabled);

  bool enabled(AnimationKind kind = AnimationKind::General) const noexcept;
  float speed() const noexcept { return m_speed; }

private:
  MotionService() = default;

  void reduceMotionForKind(AnimationKind kind);

  bool m_enabled = true;
  float m_speed = 1.0F;
  bool m_launcher = true;
  bool m_panels = true;
  bool m_bar = true;
  bool m_widgets = true;
  bool m_dock = true;
  std::unordered_set<AnimationManager*> m_managers;
};
