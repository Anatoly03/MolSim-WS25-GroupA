/**
 * @file Vec3.h
 */

#pragma once

#include "Vec3.h"

/**
 * @struct Vec3Iter
 * @brief Represents a 3D vector iterator.
 * @note Use Vec3D as a typedef for Vec3<double>.
 *
 * @example
 *
 * ```c++
 * for (const v : Vec3Iter(1, 2, 3)) {
 *     // 0 0 0
 *     // 0 0 1
 *     // 0 0 2
 *     // 0 1 0
 *     // 0 1 1
 *     // 0 1 2
 * }
 * ```
 */
// TODO document
struct Vec3Iter {
    // attributes
   public:
    Vec3I current = Vec3I(0);
    Vec3I total;

    // constructors
    Vec3Iter() : total(0) {}
    Vec3Iter(int e) : total(e) {}
    Vec3Iter(int x, int y, int z) : total(x, y, z) {}

   private:
    Vec3Iter(Vec3I current, Vec3I total) : current(current), total(total) {}

   public:
    Vec3Iter begin() {
        return Vec3Iter(0, total);
    }

    Vec3Iter end() {
        return Vec3Iter(total, total);
    }

    Vec3Iter begin() const {
        return Vec3Iter(0, total);
    }

    Vec3Iter end() const {
        return Vec3Iter(total, total);
    }

    bool hasNext() {
        return current.x < total.x && current.y < total.y && current.z < total.z;
    }

    Vec3I next() {
        ++(*this);
        return current;
    }

    //
    // ITERATOR
    // https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
    //

    // dereference
    const Vec3I& operator*() const { return current; }
    Vec3I& operator*() { return current; }

    // increment
    Vec3Iter& operator++() {
        current.z++;

        if (current.z >= total.z) {
            current.z = 0;
            current.y ++;
        }

        if (current.y >= total.y) {
            current.y = 0;
            current.x ++;
        }

        return *this;
    }

    Vec3Iter operator++(int) { Vec3Iter tmp = *this; ++(*this); return tmp; }

    // comparison
    bool operator==(const Vec3Iter& o) const { return current == o.current; }
    bool operator!=(const Vec3Iter& o) const { return !(*this == o); }

};
