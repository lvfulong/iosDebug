#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <utils/Config.h>

namespace laya
{

class IFileSystem
{
  public:
    enum Status
    {
        OK = 0,
        NOT_EXISTS = 1,
        OPEN_FAILED = 2,
        READ_FAILED = 3,
        NOT_INITIALIZED = 4,
        TOO_LARGE = 5,
        OBTAIN_SIZE_FAILED = 6
    };

    virtual ~IFileSystem() = default;

    virtual bool isFileExistInAsset(const std::string &filename) = 0;
    virtual bool isDirectoryExistInAsset(const std::string &dirname) = 0;
    virtual size_t getFileSizeInAsset(const std::string &filename) = 0;
    virtual Status readFileInAsset(const std::string &filename, std::vector<uint8_t> &outData) = 0;
    virtual std::vector<std::string> listFilesInAssetDir(const std::string &dirPath) = 0;
};

CONCH_EXPORT IFileSystem *getIFileSystem();
CONCH_EXPORT void setIFileSystem(IFileSystem *fileSystem);

} // namespace laya
