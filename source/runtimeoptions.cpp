#include "runtime.hpp"
#include "sprite.hpp"
#include <log.hpp>
#include <render.hpp>
#include <runtime/blocks/blockUtils.hpp>
#include <unzip.hpp>

// TODO: support hat blocks
// TODO: support window changing size when setting project size

SCRATCH_BLOCK(runtimeoptions, getEnabled) {
  Value val;
  if (!Scratch::getInput(block, "thing", thread, sprite, val))
    return BlockResult::REPEAT;

  const std::string valStr = val.asString();

  if (valStr == "remove fencing") {
    *outValue = Value(!Scratch::fencing);
  } else if (valStr == "remove misc limits") {
    *outValue = Value(!Scratch::miscellaneousLimits);
  } else if (valStr == "interpolation") {
    // unsupported by SE!
    *outValue = Value(false);
  } else if (valStr == "turbo mode") {
    *outValue = Value(Scratch::turbo);
  } else if (valStr == "high quality pen") {
    *outValue = Value(Scratch::hqpen);
  }

  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, setEnabled) {
  Value val;
  if (!Scratch::getInput(block, "thing", thread, sprite, val))
    return BlockResult::REPEAT;
  Value enabledVal;
  if (!Scratch::getInput(block, "enabled", thread, sprite, enabledVal))
    return BlockResult::REPEAT;

  const std::string valStr = val.asString();

  if (valStr == "remove fencing") {
    Scratch::fencing = !enabledVal.asBoolean();
  } else if (valStr == "remove misc limits") {
    Scratch::miscellaneousLimits = !enabledVal.asBoolean();
  } else if (valStr == "interpolation") {
    // unsupported by SE!
  } else if (valStr == "turbo mode") {
    Scratch::turbo = enabledVal.asBoolean();
  } else if (valStr == "high quality pen") {
    Scratch::hqpen = enabledVal.asBoolean();
  }

  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, setFramerate) {
  Value val;
  if (!Scratch::getInput(block, "fps", thread, sprite, val))
    return BlockResult::REPEAT;

  Scratch::FPS = static_cast<int>(val.asDouble());
  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, getFramerate) {
  *outValue = Value(Scratch::FPS);
  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, setCloneLimit) {
  Value val;
  if (!Scratch::getInput(block, "limit", thread, sprite, val))
    return BlockResult::REPEAT;

  if (val.asString() == "Infinity") {
    Scratch::maxClones = std::numeric_limits<int>::max();
  } else {
    Scratch::maxClones = 300;
  }
  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, getCloneLimit) {
  if (Scratch::maxClones == std::numeric_limits<int>::max()) {
    *outValue = Value(std::string("Infinity"));
  } else {
    *outValue = Value(Scratch::maxClones);
  }
  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, setDimensions) {
  Value widthVal;
  Value heightVal;
  if (!Scratch::getInput(block, "width", thread, sprite, widthVal))
    return BlockResult::REPEAT;
  if (!Scratch::getInput(block, "height", thread, sprite, heightVal))
    return BlockResult::REPEAT;

  Scratch::projectWidth = static_cast<int>(widthVal.asDouble());
  Scratch::projectHeight = static_cast<int>(heightVal.asDouble());

#ifdef RENDERER_CITRO2D
  if (Scratch::projectWidth == 400 && Scratch::projectHeight == 480)
    Render::renderMode = Render::BOTH_SCREENS;
  else if (Scratch::projectWidth == 320 && Scratch::projectHeight == 240)
    Render::renderMode = Render::BOTTOM_SCREEN_ONLY;
  else {
    auto bottomScreen = Unzip::getSetting("bottomScreen");
    if (!bottomScreen.is_null() && bottomScreen.get<bool>())
      Render::renderMode = Render::BOTTOM_SCREEN_ONLY;
    else
      Render::renderMode = Render::TOP_SCREEN_ONLY;
  }
#elif defined(RENDERER_GL2D)
  auto bottomScreen = Unzip::getSetting("bottomScreen");
  if (!bottomScreen.is_null() && bottomScreen.get<bool>())
    Render::renderMode = Render::BOTTOM_SCREEN_ONLY;
  else
    Render::renderMode = Render::TOP_SCREEN_ONLY;
#else
  Render::renderMode = Render::TOP_SCREEN_ONLY;
#endif

  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, getDimension) {
  Value val;
  if (!Scratch::getInput(block, "dimension", thread, sprite, val))
    return BlockResult::REPEAT;

  if (val.asString() == "width") {
    *outValue = Value(Scratch::projectWidth);
  } else {
    *outValue = Value(Scratch::projectHeight);
  }

  return BlockResult::CONTINUE;
}

SCRATCH_BLOCK(runtimeoptions, setUsername) {
  Value val;
  if (!Scratch::getInput(block, "username", thread, sprite, val))
    return BlockResult::REPEAT;

  Scratch::useCustomUsername = true;
  Scratch::customUsername = val.asString();

  return BlockResult::CONTINUE;
}

SCRATCH_SHADOW_BLOCK(runtimeoptions_menu_thing, thing);
SCRATCH_SHADOW_BLOCK(runtimeoptions_menu_enabled, enabled);
SCRATCH_SHADOW_BLOCK(runtimeoptions_menu_clones, clones);
SCRATCH_SHADOW_BLOCK(runtimeoptions_menu_dimension, dimension);
