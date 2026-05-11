#include <runtime/blocks/blockUtils.hpp>
#include <downloader.hpp>
#include <filesystem.hpp>
#include <os.hpp>

SCRATCH_BLOCK(fetch, get) {
#if defined(ENABLE_DOWNLOAD)
  BlockState *state = thread->getState(block);

  if (state->completedSteps == 0) {
    Value urlVal;
    if (!Scratch::getInput(block, "URL", thread, sprite, urlVal))
      return BlockResult::REPEAT;

    std::string url = urlVal.asString();
    state->name = url;

    std::string tempDir = OS::getScratchFolderLocation() + "cache/";
    std::size_t h = std::hash<std::string>{}(state->name);
    std::string tempFile = tempDir + "fetch_temp_" + std::to_string(h) + ".txt";

    state->completedSteps = 1;

    if (!DownloadManager::init())
      return BlockResult::CONTINUE;

    if (FileSystem::fileExists(tempFile) &&
        !DownloadManager::isDownloading(state->name)) {
      std::ifstream file(tempFile);
      std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
      file.close();

      *outValue = Value(content);
      thread->eraseState(block);
      return BlockResult::CONTINUE;
    }

    DownloadManager::addDownload(state->name, tempFile);
    return BlockResult::REPEAT;
  }

  std::string tempDir = OS::getScratchFolderLocation() + "cache/";
  std::size_t h = std::hash<std::string>{}(state->name);
  std::string tempFile = tempDir + "fetch_temp_" + std::to_string(h) + ".txt";

  if (DownloadManager::isDownloading(state->name))
    return BlockResult::REPEAT;

  std::ifstream file(tempFile);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  *outValue = Value(content);

  thread->eraseState(block);
#else
  *outValue = Value("");
#endif
  return BlockResult::CONTINUE;
}