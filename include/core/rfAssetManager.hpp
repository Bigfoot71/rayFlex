#ifndef RAYFLEX_CORE_ASSET_MANAGER_HPP
#define RAYFLEX_CORE_ASSET_MANAGER_HPP

#include <unordered_map>
#include <raylib.h>
#include <typeinfo>
#include <memory>
#include <string>

namespace rf { namespace core {

    /**
     * @brief The Asset class represents a movable container for any type of data.
     */
    class Asset
    {
      private:
        std::unique_ptr<void, void(*)(void*)> value_{nullptr, [](void*){}};
        const std::type_info& type; ///< Type information of the stored data.

      public:
        /**
         * @brief Default constructor for an empty Asset.
         */
        Asset() : type(typeid(void)) {}

        /**
         * @brief Constructor that takes a value of any type and moves it into the Asset.
         * @tparam T The type of the value to be stored.
         * @param value The value to be stored.
         */
        template <typename T>
        Asset(T&& value) : value_(new typename std::decay<T>::type(std::move(value)),
            [](void* ptr) { delete static_cast<typename std::decay<T>::type*>(ptr); })
        , type(typeid(T)) { }

        /**
         * @brief Static function to create an Asset with a given type and constructor arguments.
         * @tparam T The type of the value to be stored.
         * @tparam Args Variadic template for constructor arguments.
         * @param args The constructor arguments.
         * @return An Asset containing the created value.
         */
        template <typename T, typename... Args>
        static Asset Make(Args&&... args)
        {
            return Asset(T(args...));
        }

        /**
         * @brief Gets a pointer to the stored data of type T.
         * @tparam T The type of the data to retrieve.
         * @return A pointer to the stored data.
         * @throws std::bad_cast if the stored type does not match the requested type.
         */
        template <typename T> T* Get()
        {
            if (type == typeid(T)) return static_cast<T*>(value_.get());
            else throw std::bad_cast();
        }

        /**
         * @brief Gets a pointer to the stored data of type T.
         * @tparam T The type of the data to retrieve.
         * @return A pointer to the stored data.
         * @throws std::bad_cast if the stored type does not match the requested type.
         */
        template <typename T> const T* Get() const
        {
            if (type == typeid(T)) return static_cast<T*>(value_.get());
            else throw std::bad_cast();
        }

        /**
         * @brief Gets the type information of the stored data.
         * @return The type information.
         */
        const std::type_info& Type() const
        {
            return type;
        }
    };

    /**
     * @brief The AssetManager class manages assets/resources of various types using a map.
     */
    class AssetManager
    {
      private:
        std::unordered_map<std::string, Asset> map; ///< Map of assets with their associated names.

      public:
        /**
         * @brief Adds a new asset to the manager with a given name and constructor arguments.
         * @tparam _Ta The type of the asset to add.
         * @tparam Args Variadic template for constructor arguments.
         * @param name The name to associate with the asset.
         * @param args The constructor arguments.
         * @return A pair containing an iterator to the inserted asset and a boolean indicating success.
         */
        template<typename _Ta, typename... Args>
        auto Add(const std::string& name, Args... args)
        {
            return map.emplace(name, Asset::Make<_Ta>(args...));
        }

        /**
         * @brief Adds a new asset to the manager with a given name and a pre-constructed asset.
         * @tparam _Ta The type of the asset to add.
         * @param name The name to associate with the asset.
         * @param asset The pre-constructed asset.
         * @return A pair containing an iterator to the inserted asset and a boolean indicating success.
         */
        template<typename _Ta>
        auto Add(const std::string& name, _Ta&& asset)
        {
            return map.emplace(name, Asset(asset));
        }

        /**
         * @brief Adds a new asset to the manager or replaces an existing one with the given name and constructor arguments.
         * @tparam _Ta The type of the asset to add.
         * @tparam Args Variadic template for constructor arguments.
         * @param name The name to associate with the asset.
         * @param args The constructor arguments.
         * @return A pair containing an iterator to the inserted/replaced asset and a boolean indicating success.
         */
        template<typename _Ta, typename... Args>
        auto AddOrReplace(const std::string& name, Args... args)
        {
            return map.insert_or_assign(name, Asset::Make<_Ta>(args...));
        }

        /**
         * @brief Adds a new asset to the manager or replaces an existing one with the given name and a pre-constructed asset.
         * @tparam _Ta The type of the asset to add.
         * @param name The name to associate with the asset.
         * @param asset The pre-constructed asset.
         * @return A pair containing an iterator to the inserted/replaced asset and a boolean indicating success.
         */
        template<typename _Ta>
        auto AddOrReplace(const std::string& name, _Ta&& asset)
        {
            return map.insert_or_assign(name, Asset(asset));
        }

        /**
         * @brief Removes an asset from the manager with the given name.
         * @param name The name of the asset to remove.
         */
        void Remove(const std::string& name)
        {
            auto it = map.find(name);
            if (it != map.end()) map.erase(it);
        }

        /**
         * @brief Reserves space for a specified number of assets in the map.
         * @param size The number of assets to reserve space for.
         */
        void Reserve(std::size_t size)
        {
            map.reserve(size);
        }

        /**
         * @brief Gets the number of assets in the manager.
         * @return The number of assets.
         */
        std::size_t GetSize() const
        {
            return map.size();
        }

        /**
         * @brief Gets the type information of the asset with the given name.
         * @param name The name of the asset.
         * @return A pointer to the type information or nullptr if the asset does not exist.
         */
        const std::type_info* GetType(const std::string& name) const
        {
            auto it = map.find(name);
            if (it != map.end()) return &it->second.Type();
            return nullptr;
        }

        /**
         * @brief Gets a pointer to the stored data of the asset with the given name and type.
         * @tparam _Ta The type of the data to retrieve.
         * @param name The name of the asset.
         * @return A pointer to the stored data or nullptr if the asset does not exist or has a different type.
         */
        template<typename _Ta>
        _Ta* Get(const std::string& name)
        {
            auto it = map.find(name);
            if (it != map.end()) return it->second.Get<_Ta>();
            return nullptr;
        }

        /**
         * @brief Gets a pointer to the stored data of the asset with the given name and type.
         * @tparam _Ta The type of the data to retrieve.
         * @param name The name of the asset.
         * @return A pointer to the stored data or nullptr if the asset does not exist or has a different type.
         */
        template<typename _Ta>
        const _Ta* Get(const std::string& name) const
        {
            auto it = map.find(name);
            if (it != map.end()) return it->second.Get<_Ta>();
            return nullptr;
        }

        /**
         * @brief Gets a pointer to the asset with the given name.
         * @param name The name of the asset.
         * @return A pointer to the asset or nullptr if the asset does not exist.
         */
        Asset* Get(const std::string& name)
        {
            auto it = map.find(name);
            if (it != map.end()) return &it->second;
            return nullptr;
        }

        /**
         * @brief Gets a pointer to the asset with the given name.
         * @param name The name of the asset.
         * @return A pointer to the asset or nullptr if the asset does not exist.
         */
        const Asset* Get(const std::string& name) const
        {
            auto it = map.find(name);
            if (it != map.end()) return &it->second;
            return nullptr;
        }

        /**
         * @brief Gets a reference to the asset with the given name.
         * If the asset does not exist, a new empty asset is created and returned.
         * @param name The name of the asset.
         * @return A reference to the asset.
         */
        Asset& operator[](const std::string& name)
        {
            auto it = map.find(name);
            if (it != map.end()) return it->second;
            return map.emplace(name, Asset()).first->second;
        }
    };

}}

#endif //RAYFLEX_CORE_ASSET_MANAGER_HPP
