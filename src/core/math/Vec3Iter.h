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
    Vec3I last;

    // constructors
    Vec3Iter() : last(0) {}
    Vec3Iter(int e) : last(e) {}
    Vec3Iter(int x, int y, int z) : last(x, y, z) {}

   private:
    Vec3Iter(Vec3I current, Vec3I last) : current(current), last(last) {}

   public:
    Vec3Iter begin() {
        return Vec3Iter(Vec3I(0), last);
    }

    Vec3Iter end() {
        // End sentinel: when x reaches last.x we are done. Use (last.x,0,0)
        return Vec3Iter(Vec3I(last.x, 0, 0), last);
    }

    Vec3Iter begin() const {
        return Vec3Iter(Vec3I(0), last);
    }

    Vec3Iter end() const {
        return Vec3Iter(Vec3I(last.x, 0, 0), last);
    }

    bool hasNext() const { return *this != end(); }

    // Return the current coordinate and advance the iterator (post-increment semantics)
    Vec3I next() {
        Vec3I cur = current;
        ++(*this);
        return cur;
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

        if (current.z >= last.z) {
            current.z = 0;
            current.y ++;
        }

        if (current.y >= last.y) {
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
