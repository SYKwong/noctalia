#include "render/animation/animation_manager.h"
#include "render/animation/motion_service.h"
#include "tests/test_check.h"

#include <cmath>
#include <iostream>

namespace {

  bool check(bool cond, const char* msg) {
    if (!cond) {
      std::cerr << "FAIL: " << msg << '\n';
    }
    return cond;
  }

  void resetMotion() {
    auto& motion = MotionService::instance();
    motion.setSpeed(1.0F);
    motion.setEnabled(true);
    motion.setLauncherEnabled(true);
    motion.setPanelsEnabled(true);
    motion.setBarEnabled(true);
    motion.setWidgetsEnabled(true);
    motion.setDockEnabled(true);
  }

} // namespace

int main() {
  bool ok = true;
  resetMotion();

  {
    AnimationManager manager;
    MotionService::instance().setEnabled(false);

    float value = 0.0F;
    bool completed = false;
    const auto id = manager.animate(
        0.0F, 10.0F, 200.0F, Easing::Linear, [&value](float v) { value = v; }, [&completed]() { completed = true; }
    );

    ok &= check(id != 0, "reduced-motion animation with completion should remain cancellable");
    TEST_NEAR(value, 10.0F, 0.0001F);
    ok &= check(!completed, "reduced-motion completion ran synchronously");

    manager.tick(0.0F);

    ok &= check(completed, "reduced-motion completion did not run on tick");
    ok &= check(!manager.hasActive(), "reduced-motion animation stayed active after completion");
  }

  resetMotion();

  {
    AnimationManager manager;

    float value = 0.0F;
    bool completed = false;
    const auto id = manager.animate(
        0.0F, 10.0F, 1000.0F, Easing::Linear, [&value](float v) { value = v; }, [&completed]() { completed = true; }
    );

    ok &= check(id != 0, "normal animation did not start");

    MotionService::instance().setEnabled(false);

    TEST_NEAR(value, 10.0F, 0.0001F);
    ok &= check(!completed, "active animation completion ran synchronously when motion was disabled");
    ok &= check(manager.hasActive(), "active animation did not remain pending for async completion");

    manager.tick(0.0F);

    ok &= check(completed, "active animation completion did not run after reduced-motion snap");
  }

  resetMotion();

  {
    AnimationManager manager;
    MotionService::instance().setEnabled(false);

    float value = -1.0F;
    bool completed = false;
    const auto id = manager.animateTimer(
        1.0F, 0.0F, 1000.0F, Easing::Linear, [&value](float v) { value = v; }, [&completed]() { completed = true; }
    );

    ok &= check(id != 0, "timer animation did not start while motion was disabled");
    TEST_NEAR(value, -1.0F, 0.0001F);

    manager.tick(0.0F);

    ok &= check(!completed, "timer animation completed early while motion was disabled");
    ok &= check(manager.hasActive(), "timer animation was removed early while motion was disabled");
    manager.cancel(id);
  }

  resetMotion();

  {
    AnimationManager manager;
    auto& motion = MotionService::instance();
    motion.setLauncherEnabled(false);

    float launcherVal = 0.0F;
    bool launcherCompleted = false;
    const auto launcherId = manager.animate(
        0.0F, 10.0F, 500.0F, Easing::Linear, [&launcherVal](float v) { launcherVal = v; },
        [&launcherCompleted]() { launcherCompleted = true; }, nullptr, AnimationKind::Launcher
    );

    float panelsVal = 0.0F;
    bool panelsCompleted = false;
    const auto panelsId = manager.animate(
        0.0F, 20.0F, 500.0F, Easing::Linear, [&panelsVal](float v) { panelsVal = v; },
        [&panelsCompleted]() { panelsCompleted = true; }, nullptr, AnimationKind::Panels
    );

    ok &= check(launcherId != 0, "disabled-kind launcher animation was not created");
    TEST_NEAR(launcherVal, 10.0F, 0.0001F);
    ok &= check(!launcherCompleted, "disabled-kind launcher completion ran synchronously");

    ok &= check(panelsId != 0, "enabled-kind panels animation was not created");
    TEST_NEAR(panelsVal, 0.0F, 0.0001F);
    ok &= check(!panelsCompleted, "enabled-kind panels completion ran unexpectedly early");

    manager.tick(0.0F);
    ok &= check(launcherCompleted, "disabled-kind launcher completion did not run on tick");
    ok &= check(!panelsCompleted, "enabled-kind panels completed on tick 0");

    manager.cancel(panelsId);
  }

  resetMotion();

  {
    AnimationManager manager;
    auto& motion = MotionService::instance();

    float launcherVal = 0.0F;
    bool launcherCompleted = false;
    const auto launcherId = manager.animate(
        0.0F, 10.0F, 1000.0F, Easing::Linear, [&launcherVal](float v) { launcherVal = v; },
        [&launcherCompleted]() { launcherCompleted = true; }, nullptr, AnimationKind::Launcher
    );

    float panelsVal = 0.0F;
    bool panelsCompleted = false;
    const auto panelsId = manager.animate(
        0.0F, 20.0F, 1000.0F, Easing::Linear, [&panelsVal](float v) { panelsVal = v; },
        [&panelsCompleted]() { panelsCompleted = true; }, nullptr, AnimationKind::Panels
    );

    motion.setLauncherEnabled(false);

    TEST_NEAR(launcherVal, 10.0F, 0.0001F);
    TEST_NEAR(panelsVal, 0.0F, 0.0001F);

    manager.tick(0.0F);
    ok &= check(launcherCompleted, "dynamically disabled launcher did not complete");
    ok &= check(!panelsCompleted, "panels completed unexpectedly when launcher was disabled");

    manager.cancel(panelsId);
  }

  resetMotion();
  return ok ? 0 : 1;
}
