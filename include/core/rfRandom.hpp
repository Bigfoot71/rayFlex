#ifndef RAYFLEX_CORE_RANDOM_HPP
#define RAYFLEX_CORE_RANDOM_HPP

#include <Vector2.hpp>
#include <Color.hpp>
#include <random>
#include <chrono>

namespace rf { namespace core {

    /**
     * @brief The RandomGenerator class provides functionality for generating random values.
     */
    class RandomGenerator
    {
      private:
        std::mt19937 generator;     ///< Mersenne Twister 19937 random number generator.
        unsigned long seed;         ///< Seed used for the random number generator.

      public:
        /**
         * @brief Constructs a RandomGenerator with an optional seed.
         * @param seed The seed for the random number generator. If not provided, it is generated from the current time.
         */
        RandomGenerator(unsigned long seed = 0)
        {
            SetSeed(seed);
        }

        /**
         * @brief Generates a random value using a discrete distribution.
         * @tparam T The type of the random value to be generated.
         * @param distribution The discrete distribution to use for generating the random value.
         * @return A random value generated based on the provided discrete distribution.
         */
        template<typename T>
        T operator()(std::discrete_distribution<T>& distribution)
        {
            return distribution(generator);
        }

        /**
         * @brief Sets the seed for the random number generator.
         * @param seed The seed to set. If not provided or set to 0, it is generated from the current time.
         */
        void SetSeed(unsigned long seed = 0)
        {
            if (seed == 0)
            {
                seed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            }
            generator.seed(seed);
            this->seed = seed;
        }

        /**
         * @brief Gets the current seed used by the random number generator.
         * @return The current seed.
         */
        unsigned long GetSeed() const
        {
            return seed;
        }

        /**
         * @brief Generates a random integral value within the specified range.
         * @tparam T The type of the value to generate (integral).
         * @param min The minimum value of the range.
         * @param max The maximum value of the range.
         * @return A random integral value within the specified range.
         */
        template <typename T>
        T Random(T min, T max, std::enable_if_t<std::is_integral<T>::value>* = nullptr)
        {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(generator);
        }

        /**
         * @brief Generates a random floating-point value within the specified range.
         * @tparam T The type of the value to generate (floating-point).
         * @param min The minimum value of the range.
         * @param max The maximum value of the range.
         * @return A random floating-point value within the specified range.
         */
        template <typename T>
        T Random(T min, T max, std::enable_if_t<std::is_floating_point<T>::value>* = nullptr)
        {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator);
        }

        /**
         * @brief Generates a random Vector2 within the specified range.
         * @param min The minimum values of the range for x and y.
         * @param max The maximum values of the range for x and y.
         * @return A random Vector2 within the specified range.
         */
        Vector2 RandomVec2(const Vector2& min, const Vector2& max)
        {
            std::uniform_real_distribution<float> distX(min.x, max.x);
            std::uniform_real_distribution<float> distY(min.y, max.y);
            return { distX(generator), distY(generator) };
        }

        /**
         * @brief Generates a random Vector3 within the specified range.
         * @param min The minimum values of the range for x, y, and z.
         * @param max The maximum values of the range for x, y, and z.
         * @return A random Vector3 within the specified range.
         */
        Vector3 RandomVec3(const Vector3& min, const Vector3& max)
        {
            std::uniform_real_distribution<float> distX(min.x, max.x);
            std::uniform_real_distribution<float> distY(min.y, max.y);
            std::uniform_real_distribution<float> distZ(min.z, max.z);
            return { distX(generator), distY(generator), distZ(generator) };
        }

        /**
         * @brief Generates a random Color with the specified saturation and value.
         * @param sat The saturation value for the generated color.
         * @param val The value (brightness) value for the generated color.
         * @return A random Color with the specified saturation and value.
         */
        Color RandomColor(float sat = 1.0, float val = 1.0)
        {
            std::uniform_real_distribution<float> distribution(0, 360);
            return raylib::Color::FromHSV(distribution(generator), sat, val);
        }

        /**
         * @brief Generates a random character within the specified range.
         * @param min The minimum character value.
         * @param max The maximum character value.
         * @return A random character within the specified range.
         */
        char RandomChar(char min, char max)
        {
            std::uniform_int_distribution<int> distribution(min, max);
            return static_cast<char>(distribution(generator));
        }

        /**
         * @brief Generates a random character from a given set of characters.
         * @param characters The set of characters to choose from.
         * @return A random character from the specified set.
         */
        char RandomChar(const std::string& characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
        {
            std::uniform_int_distribution<int> distribution(0, characters.size() - 1);
            return characters[distribution(generator)];
        }

        /**
         * @brief Generates a random string of the specified length using a given set of characters.
         * @param length The length of the random string to generate.
         * @param characters The set of characters to choose from.
         * @return A random string of the specified length.
         */
        std::string RandomString(int length, const std::string& characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
        {
            std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

            std::string randomString;
            randomString.reserve(length);

            for (int i = 0; i < length; i++)
            {
                randomString += characters[distribution(generator)];
            }

            return randomString;
        }
    };

}}

#endif //RAYFLEX_CORE_RANDOM_HPP
