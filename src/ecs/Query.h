
#pragma once

#include <typeinfo>

#include "Entity.h"
#include "World.h"

/**
 * @brief Query structure to retrieve entities with specific components.
 * @tparam Components Component types that an entity must have to be included in the query.
 */
template <typename... Components>
struct Query {
   private:
    const World& world;

   public:
    Query() = delete;

    /**
     * @brief Default constructor taking a world reference.
     */
    Query(const World& world) : world(world) {}

    /**
     * @return Entity Id of the first matching entity.
     */
    Entity begin() {
        return 0;
    }

    /**
     * @return Entity Id of the last matching entity.
     */
    Entity end() {
        return 0;
    }

    /**
     * @return Entity Id of the first matching entity.
     */
    Entity begin() const {
        return 0;
    }

    /**
     * @return Entity Id of the last matching entity.
     */
    Entity end() const {
        return 0;
    }
};
