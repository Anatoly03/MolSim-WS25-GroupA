
#pragma once

#include <typeinfo>
#include "Entity.h"

struct World {
   private:
    Entity next_id = 1;

   public:
    /**
     * @brief Creates a new entity and returns its identifier.
     * @return The unique identifier of the newly created entity.
     */
    Entity createEntity() { return next_id++; }

    /**
     * @brief Destroys an entity given its identifier.
     * @param entity The unique identifier of the entity to destroy.
     */
    void destroyEntity(Entity entity) {
        // TODO implement destructor
    }

    /**
     * @brief Amount of entities in the world.
     * @return The number of entities in the world.
     */
    unsigned int entityCount() {
        // TODO implement entity count
        return 0;
    }

    /**
     * @brief Adds a component to an entity.
     * @details Adds a component of the specified type to the given
     * entity, overriding the previous component of the same type. If
     * overridden, returns the previous component, otherwise returns nullptr.
     */
    template <typename Component>
    Component *addComponent(Entity entity, Component component) {
        size_t type_hash = typeid(Component).hash_code();

        // TODO implement add component
    }

    /**
     * @brief Gets a component from an entity.
     * @details Uses generic template to get component of the specified type.
     */
    template <typename Component>
    Component *getComponent(Entity entity) {
        size_t type_hash = typeid(Component).hash_code();

        // TODO implement get component
    }

    /**
     * @brief Removes a component from an entity.
     */
    template <typename Component>
    void removeComponent(Entity entity, Component component) {
        size_t type_hash = typeid(Component).hash_code();

        // TODO implement remove component
    }
};
