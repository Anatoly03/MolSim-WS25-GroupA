/**
 * @file Vec3.cpp
 */
#include <type_traits>

namespace math {
    /**
     * @struct Vec3
     * @brief Represents a 3D vector.
     * 
     * @example
     * 
     * ```c++
     * Vec3 a, b, c;
     * 
     * a = Vec3(1.0);
     * b = Vec3(-0.5);
     * c = a + b;
     * 
     * assert(c.x == 0.5);
     * ```
     */
    template<typename T = double>
    struct Vec3 {
        // require numeric types only.
        // https://stackoverflow.com/a/26207551
        static_assert(std::is_arithmetic<T>::value, "Vec3 requires an arithmetic generic type");

        // attributes
        T x;
        T y;
        T z;
        
        // constructors
        Vec3() : x(0), y(0), z(0) {}
        Vec3(T e) : x(e), y(e), z(e) {}
        Vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}

        // operator overloads
        // allows to override symbols like the plus and minus and use it with vectors
        // https://en.cppreference.com/w/cpp/language/operators.html
        // https://www.geeksforgeeks.org/cpp/how-to-overload-the-plus-operator-in-cpp/

        /**
         * @brief Unary negation operator overload for Vec3.
         */
        Vec3 operator - () const {
            return Vec3(-this->x, -this->y, -this->z);
        }

        /**
         * @brief Binary addition operator overload for Vec3.
         */
        Vec3 operator + (const Vec3 &other) const {
            return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
        }

        /**
         * @brief Binary subtraction operator overload for Vec3.
         */
        Vec3 operator - (const Vec3 &other) const {
            return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
        }
    };
}
