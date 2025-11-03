
#pragma once

#include <set>
#include <typeinfo>
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
     * @example
     * ```c++
     * World world();
     * Entity e1 = world.createEntity();    // e1 == 1
     * Entity e2 = world.createEntity();    // e2 == 2
     * ```
     */
    Entity createEntity() {
        Entity eid = next_id++;
        entities.insert(eid);
        return eid;
    }

    /**
     * @brief Yields if the entity id exists or is removed.
     * @returns True if the entity exists, false otherwise.
     * @example
     * ```c++
     * World world();
     * world.createEntity();                // eidd == 1
     * world.existsEntity(1);               // exists == true
     * world.existsEntity(2);               // exists == false
     * ```
     */
    bool existsEntity(Entity entity) { return entities.find(entity) != entities.end(); }

    /**
     * @brief Destroys an entity given its identifier.
     * @param entity The unique identifier of the entity to destroy.
     * @example
     * ```c++
     * World world();
     * Entity eid = world.createEntity();   // eid == 1
     * world.existsEntity(eid);             // exists == true
     * world.destroyEntity(eid);
     * world.existsEntity(eid);             // exists == false
     * ```
     */
    void destroyEntity(Entity entity) { entities.erase(entity); }

    /**
     * @brief Amount of entities in the world.
     * @return The number of entities in the world.
     * @example
     * ```c++
     * World world();
     * world.createEntity();
     * world.entityCount();                 // count == 1
     * world.createEntity();
     * world.entityCount();                 // count == 2
     * ```
     */
    unsigned int entityCount() { return entities.size(); }

    //
    // ENTITY-COMPONENT RELATION MANAGEMENT
    //

    /**
     * @brief Adds a component to an entity.
     * @details Adds a component of the specified type to the given
     * entity, overriding the previous component of the same type. If
     * overridden, returns the previous component, otherwise returns nullptr.
     * @example
     * ```c++
     * World world();
     *
     * Entity eid = world.createEntity();
     * Position pos(0.0, 1.0, 0.0);
     * Velocity vel(0.1, 0.0, 0.0);
     *
     * world.addComponent<Position>(eid, pos);
     * world.addComponent<Velocity>(eid, vel);
     * ```
     */
    template <typename Component>
    Component *addComponent(Entity entity, Component component) {
        size_t type_hash = typeid(Component).hash_code();

        // TODO implement add component
    }

    /**
     * @brief Gets a component from an entity.
     * @details Uses generic template to get component of the specified type.
     * @example
     * ```c++
     * World world();
     *
     * Entity eid = world.createEntity();
     * Position pos(0.0, 1.0, 0.0);
     * Velocity vel(0.1, 0.0, 0.0);
     *
     * world.addComponent<Position>(eid, Position(0.0, 1.0, 0.0));
     * world.addComponent<Velocity>(eid, Velocity(0.1, 0.0, 0.0));
     *
     * ASSERT(world.getComponent<Position>(eid).y == 1.0);
     * ```
     */
    template <typename Component>
    Component *getComponent(Entity entity) {
        size_t type_hash = typeid(Component).hash_code();

        // TODO implement get component
    }

    /**
     * @brief Removes a component from an entity.
     * @example
     * ```c++
     * World world();
     *
     * Entity eid = world.createEntity();
     * Position pos(0.0, 1.0, 0.0);
     * Velocity vel(0.1, 0.0, 0.0);
     *
     * world.addComponent<Position>(eid, Position(0.0, 1.0, 0.0));
     * world.addComponent<Velocity>(eid, Velocity(0.1, 0.0, 0.0));
     * world.removeComponent<Position>(eid);
     * world.removeComponent<Velocity>(eid);
     * ```
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
     * @example
     * ```c++
     * World world();
     *
     * struct TimeResource {
     *    public:
     *     double elapsedTime;
     *     double delta;
     *
     *     TimeResource(double dt) : elapsedTime(0), delta(dt) {}
     * }
     *
     * TimeResource time_res(0.01);
     *
     * world.addResource<TimeResource>(time_res);
     * ```
     */
    template <typename Resource>
    Resource *addResource(Resource component) {
        size_t type_hash = typeid(Resource).hash_code();

        resources[type_hash] = component;
    }

    /**
     * @brief Gets a public resource or nullptr if not existing.
     * @example
     * ```c++
     * World world();
     *
     * struct TimeResource {
     *    public:
     *     double elapsedTime;
     *     double delta;
     *
     *     TimeResource(double dt) : elapsedTime(0), delta(dt) {}
     * }
     *
     * TimeResource time_res(0.01);
     *
     * world.addResource<TimeResource>(time_res);
     *
     * TimeResource res = world.getResource<TimeResource>();
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
    // SYSTEM MANAGEMENT
    //

    /**
     * @brief Calls a system function for all entities having the required components.
     * @example
     * ```c++
     * World world();
     *
     * Entity eid1 = world.createEntity();
     * Entity eid2 = world.createEntity();
     * 
     * Position pos(0.0, 1.0, 0.0);
     * Velocity vel(0.1, 0.0, 0.0);
     *
     * world.addComponent<Position>(eid1, Position(0.0, 1.0, 0.0));
     * world.addComponent<Velocity>(eid1, Velocity(0.1, 0.0, 0.0));
     * 
     * void updatePosition(Position &pos, Velocity &vel) {
     *     *pos += *vel;
     * }
     * 
     * world.invoke<Position, Velocity>(updatePosition);
     * ```
     */
    // https://stackoverflow.com/questions/14441410/function-signature-as-template-parameter
    // https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
    // https://en.cppreference.com/w/cpp/language/fold.html
    template <typename... Components>
    void invoke(void (*callback)(Components&...)) {
        // TODO refactor

        for (Entity entity : entities) {
            // Iterate over every entity, get all components and call the system.
            callback(
                (*getComponent<Components>(entity))...
            );
        }
    }

    //
    // GLOBAL ENTITY ITERATION
    //

    /**
     * @brief Iterator.
     */
    iter begin() { return entities.begin(); }

    /**
     * @brief Iterator.
     */
    iter end() { return entities.end(); }

    /**
     * @brief Const iterator.
     */
    const_iter begin() const { return entities.begin(); }

    /**
     * @brief Const iterator.
     */
    const_iter end() const { return entities.end(); }
};
