
#pragma once

#include <typeinfo>
#include <set>
#include <vector>

#include "Entity.h"

struct World {
   private:
    Entity next_id = 1;

    /**
     * @brief Set of all active entity IDs.
     */
    std::set<Entity> entities;

    // TODO entity-components mapping

    /**
     * @brief Resource allocation. Maps type hash to resource pointer.
     * Assumes the default constructor exists.
     */
    std::unordered_map<unsigned int, void *> resources;

   public:
    using iter = std::set<Entity>::iterator;
    using const_iter = std::set<Entity>::const_iterator;

    //
    // ENTITY MANAGEMENT
    //

    /**
     * @brief Creates a new entity and returns its identifier.
     * @return The unique identifier of the newly created entity.
     */
    Entity createEntity() {
        Entity eid = next_id++;
        entities.insert(eid);
        return eid;
    }

    /**
     * @brief Yields if the entity id exists or is removed.
     * @returns True if the entity exists, false otherwise.
     */
    bool existsEntity(Entity entity) {
        return entities.find(entity) != entities.end();
    }

    /**
     * @brief Destroys an entity given its identifier.
     * @param entity The unique identifier of the entity to destroy.
     */
    void destroyEntity(Entity entity) {
        entities.erase(entity);
    }

    /**
     * @brief Amount of entities in the world.
     * @return The number of entities in the world.
     */
    unsigned int entityCount() {
        return entities.size();
    }

    //
    // ENTITY-COMPONENT RELATION MANAGEMENT
    //

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

    //
    // RESOURCE MANAGEMENT
    //

    /**
     * @brief Adds a public resource.
     */
    template <typename Resource>
    Resource *addResource(Resource component) {
        size_t type_hash = typeid(Resource).hash_code();

        resources[type_hash] = component;
    }

    /**
     * @brief Gets a public resource or nullptr if not existing.
     */
    template <typename Resource>
    Resource *getResource() {
        size_t type_hash = typeid(Resource).hash_code();

        if (resources[type_hash] == nullptr) {
            return nullptr;
        }

        return static_cast<Resource *>(resources[type_hash]);
    }

    //
    // GLOBAL ITERATION
    //

    /**
     * @return Entity Id of the first matching entity. Yields
     * 0 if no entities exist.
     */
    iter begin() {
        return entities.begin();
    }

    /**
     * @return Entity Id of the last matching entity. Yields
     * 0 if no entities exist.
     */
    iter end() {
        return entities.end();
    }

    /**
     * @return Entity Id of the first matching entity. Yields
     * 0 if no entities exist.
     */
    const_iter begin() const {
        return entities.begin();
    }

    /**
     * @return Entity Id of the last matching entity. Yields
     * 0 if no entities exist.
     */
    const_iter end() const {
        return entities.end();
    }
};
