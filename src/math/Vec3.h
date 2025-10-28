/**
 * @file Vec3.h
 */

#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>

/**
 * @struct Vec3
 * @brief Represents a 3D vector.
 * @note Use Vec3D as a typedef for Vec3<double>.
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
template <typename T = double>
struct Vec3 {
    // require numeric types only.
    // https://stackoverflow.com/a/26207551
    static_assert(std::is_arithmetic<T>::value, "Vec3 requires an arithmetic generic type");

    // attributes
   public:
    T x;
    T y;
    T z;

    // constructors
    Vec3() : x(0), y(0), z(0) {}
    Vec3(T e) : x(e), y(e), z(e) {}
    Vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}

    // methods

    /**
     * @brief Deep copy of a Vec3.
     */
    // the type conversion to double is to avoid issues when T is an integer type
    inline constexpr Vec3 clone() const { return Vec3<T>(x, y, z); }

    /**
     * @brief Vec3 dot product.
     * @details Returns the scalar product of this vector with another vector.
     */
    // the type conversion to double is to avoid issues when T is an integer type
    inline constexpr double dot(const Vec3 &other) const {
        return double(x) * other.x + double(y) * other.y + double(z) * other.z;
    }

    /**
     * @brief Vec3 cross product.
     */
    // the type conversion to double is to avoid issues when T is an integer type
    inline constexpr Vec3 cross(const Vec3 &other) const {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    /**
     * @brief Retrieve the length of the vector.
     * @see https://de.wikipedia.org/wiki/Euklidische_Norm
     */
    // TODO differentiate by 1-norm, 2-norm, and the other norm that exists
    inline constexpr double length() const { return std::sqrt(dot(*this)); }

    /**
     * @brief Convert Vec3 to an array type. Useful for printing.
     */
    inline constexpr std::array<T, 3> asArray() const { return {x, y, z}; }

    // operator overloads
    // allows to override symbols like the plus and minus and use it with vectors
    // https://en.cppreference.com/w/cpp/language/operators.html
    // https://www.geeksforgeeks.org/cpp/how-to-overload-the-plus-operator-in-cpp/

    // arithmetic and logical operators

    /**
     * @brief Unary plus operator overload for Vec3.
     */
    inline constexpr Vec3 operator+() const { return *this; }

    /**
     * @brief Unary minus operator overload for Vec3.
     */
    inline constexpr Vec3 operator-() const { return Vec3(-x, -y, -z); }

    /**
     * @brief Binary addition operator overload for Vec3.
     */
    inline constexpr Vec3 operator+(const Vec3 &other) const { return Vec3(x + other.x, y + other.y, z + other.z); }

    /**
     * @brief Binary subtraction operator overload for Vec3.
     */
    inline constexpr Vec3 operator-(const Vec3 &other) const { return Vec3(x - other.x, y - other.y, z - other.z); }

    /**
     * @brief Binary multiplication operator overload for Vec3 with a scalar.
     */
    inline constexpr Vec3 operator*(const T &scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }

    /**
     * @brief Binary multiplication operator overload for a scalar with a Vec3.
     * @note This is a friend function as the scalar is on the left.
     */
    inline constexpr friend Vec3 operator*(const T &scalar, const Vec3 &other) {
        return Vec3(other.x * scalar, other.y * scalar, other.z * scalar);
    }

    /**
     * @brief Binary division operator overload for Vec3 with a scalar.
     */
    inline constexpr Vec3 operator/(const T &scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

    /**
     * @brief Vec3 equivalence.
     */
    inline constexpr bool operator==(const Vec3 &other) const {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    /**
     * @brief Vec3 inequivalence.
     */
    inline constexpr bool operator!=(const Vec3 &other) const {
        return (x != other.x) || (y != other.y) || (z != other.z);
    }

    // assignment operators

    /**
     * @brief Assignment operator overload for Vec3.
     */
    inline constexpr Vec3 &operator=(const Vec3 &other) {
        // guard self assignment
        if (this == &other) return *this;

        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    /**
     * @brief Binary addition operator overload for Vec3.
     */
    inline constexpr Vec3 &operator+=(const Vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    /**
     * @brief Binary subtraction operator overload for Vec3.
     */
    inline constexpr Vec3 &operator-=(const Vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    /**
     * @brief Binary multiplication operator overload for Vec3 with a scalar.
     */
    inline constexpr Vec3 &operator*=(const T &scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    /**
     * @brief Binary division operator overload for Vec3 with a scalar.
     */
    inline constexpr Vec3 &operator/=(const T &scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
};

/**
 * @struct Vec3D
 * @brief Represents a 3D vector of type double.
 */
// https://www.geeksforgeeks.org/cpp/typedef-in-cpp/
typedef Vec3<double> Vec3D;
