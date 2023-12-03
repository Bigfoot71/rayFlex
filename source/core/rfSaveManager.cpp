#include "core/rfSaveManager.hpp"
#include <cstring>

using namespace rf;

template <typename _Ts>
core::SaveManager::SaveManager(const _Ts& _origin, int version, const std::string& directory)
: version(version), directory(directory)
{
    origin = operator new(sizeof(_Ts));
    std::memcpy(origin, &_origin, sizeof(_Ts));

    data = operator new(sizeof(_Ts));
    std::memcpy(data, &_origin, sizeof(_Ts));
}

core::SaveManager::~SaveManager()
{
    operator delete(origin);
    operator delete(data);
}

template <typename _Ts>
int core::SaveManager::Load(const std::string& filePath)
{
    // Open file in reading mode
    std::ifstream file(directory + filePath, std::ios::binary);
    if (!file.is_open()) return FILE_NOT_FOUND;

    // Read version from file
    int fileVersion;
    file.read(reinterpret_cast<char*>(&fileVersion), sizeof(int));

    // Check if the version read matches the expected version
    if (fileVersion != version)
    {
        RetCode ret = INCOMPATIBLE_VERSION;

        if (onIncompatibleVersion && onIncompatibleVersion(file, fileVersion))
        {
            ret = SUCCESS;
        }

        file.close();
        return ret;
    }

    // Read '_Ts' data from file
    file.read(reinterpret_cast<char*>(&data), sizeof(_Ts));

    // Check if reading was successful
    if (file.fail())
    {
        file.close();
        data = origin;
        return LOAD_FAILURE;
    }

    // Close file
    file.close();

    return SUCCESS;
}

template <typename _Ts>
int core::SaveManager::Write(const std::string& filePath)
{
    // Open file in write mode
    std::ofstream file(directory + filePath, std::ios::binary);
    if (!file.is_open()) return FILE_NOT_FOUND;

    // Write version first to file then data
    file.write(reinterpret_cast<char*>(&version), sizeof(int));
    file.write(reinterpret_cast<char*>(&data), sizeof(_Ts));

    // Check if the write was successful
    if (file.fail()) return WRITE_FAILURE;

    // Close file
    file.close();

    return SUCCESS;
}
