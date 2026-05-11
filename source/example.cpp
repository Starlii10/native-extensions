#include <log.hpp>
#include <runtime/blocks/blockUtils.hpp>

SCRATCH_BLOCK(example, example) {
  Log::log("example!");
  return BlockResult::CONTINUE;
}
