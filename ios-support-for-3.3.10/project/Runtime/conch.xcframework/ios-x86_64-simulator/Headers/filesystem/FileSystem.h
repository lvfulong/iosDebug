#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utils/Config.h>
#include <interfaces/IFileSystem.h>
#if defined(OS_OHOS)
#include <rawfile/raw_file.h>
#endif

namespace laya
{

class CONCH_EXPORT FileSystem : public IFileSystem
{
  public:
#if defined(OS_ANDROID) || defined(OS_OHOS)
    FileSystem(void *assetManager);
    void *getAssetManager();
#else
    FileSystem(const std::string &assetRootPath);
#endif
    ~FileSystem();
#if defined(OS_OHOS)
    Status getRawFileDescriptor(const std::string &filename, RawFileDescriptor64 &descriptor);
#endif
    bool isFileExistInAsset(const std::string &filename) override;
    bool isDirectoryExistInAsset(const std::string &dirname) override;
    size_t getFileSizeInAsset(const std::string &filename) override;
    Status readFileInAsset(const std::string &filename, std::vector<uint8_t> &outData) override;
    std::vector<std::string> listFilesInAssetDir(const std::string &dirPath) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace laya
