
#pragma once

#include "DiscGenerator.h"

/**
 * @brief Particle generator abstraction.
 */
class BallGenerator : public DiscGenerator {
    int dimensions() const override {
        return 3;
    };
};
