#ifndef RAYFLEX_CORE_SAVE_MANAGER_HPP
#define RAYFLEX_CORE_SAVE_MANAGER_HPP

#include <raylib-cpp.hpp>
#include <functional>
#include <fstream>
#include <string>

namespace rf { namespace core {

    /**
     * @brief A class for managing save data, providing simplified handling for various data structures and version compatibility.
     */
    class SaveManager
    {
      private:
        enum RetCode {
            SUCCESS = 0,
            LOAD_FAILURE = 1,
            WRITE_FAILURE = 2,
            FILE_NOT_FOUND = 3,
            INCOMPATIBLE_VERSION = 4,
        };

        int version = -1;
        std::string directory;
        void* origin = nullptr, *data = nullptr;

        using IncompatibleVersionCallback = std::function<bool(const std::ifstream& file, const int version)>;
        IncompatibleVersionCallback onIncompatibleVersion;

      public:
        /**
         * @brief Constructor for the SaveManager class.
         * @tparam _Ts The type of the save data.
         * @param origin The original data to be saved.
         * @param version The version number of the save data format.
         * @param directory The directory path for saving and loading files.
         */
        template <typename _Ts>
        SaveManager(const _Ts& origin, int version = 1, const std::string& directory = "");

        /**
         * @brief Destructor for the SaveManager class.
         * Cleans up allocated memory and resources.
         */
        ~SaveManager();

        /**
         * @brief Sets the directory path for saving and loading files.
         * @param directory The directory path to set.
         */
        inline void SetPath(const std::string& directory)
        {
            this->directory = directory;
        }

        /**
         * @brief Sets a callback function to handle incompatible version scenarios during loading.
         * @param callback The callback function to set.
         */
        inline void SetOnIncompatibleVersion(const IncompatibleVersionCallback& callback)
        {
            onIncompatibleVersion = callback;
        }

        /**
         * @brief Gets a pointer to the save data of the specified type.
         * @tparam _Ts The type of the save data.
         * @return A pointer to the save data.
         */
        template <typename _Ts>
        inline _Ts* GetData()
        {
            return &data;
        }

        /**
         * @brief Loads save data from a file.
         * @tparam _Ts The type of the save data.
         * @param filePath The path to the file to load.
         * @return An error code indicating the result of the operation.
         */
        template <typename _Ts>
        int Load(const std::string& filePath);

        /**
         * @brief Writes save data to a file.
         * @tparam _Ts The type of the save data.
         * @param filePath The path to the file to write.
         * @return An error code indicating the result of the operation.
         */
        template <typename _Ts>
        int Write(const std::string& filePath);
    };

}}

#endif //RAYFLEX_CORE_SAVE_MANAGER_HPP
