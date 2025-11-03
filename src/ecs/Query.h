
#pragma once

#include <set>
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
    const World &world;

   public:
    /**
     * @brief Iterator class for Query.
     * @details https://en.cppreference.com/w/cpp/iterator/iterator.html
     */
    class iter : public std::iterator<std::input_iterator_tag, Entity> {
       public:
        // TODO implement
        // explicit iter(long ??) ??
    };

    //
    // QUERY INSTANTIATION
    //

    Query() = delete;

    /**
     * @brief Default constructor taking a world reference.
     */
    Query(const World &world) : world(world) {}

    //
    // QUERY ITERATION
    //

    // /**
    //  * @brief Iterator.
    //  */
    // iter begin() { return entities.begin(); }

    // /**
    //  * @brief Iterator.
    //  */
    // iter end() { return entities.end(); }
};
