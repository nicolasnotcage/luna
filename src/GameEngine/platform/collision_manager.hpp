/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

/*
This header defines a CollisionManager class that abstracts collision handling
from the MainScene class. It manages collision components, boundaries, and responses.
*/

#ifndef EXAMPLES_COLLISION_MANAGER_HPP
#define EXAMPLES_COLLISION_MANAGER_HPP

#include "platform/collision_system.hpp"
#include "platform/collision_component.hpp"
#include "graph/transform_node.hpp"
#include "graph/scene_state.hpp"

#include <memory>
#include <functional>
#include <vector>

namespace cge
{

class CollisionManager
{
public:
    CollisionManager();
    ~CollisionManager() = default;
    
    // Initialize the collision manager
    void init(SceneState& scene_state);

    // Process collisions with the collision system
    void process_collisions();
    
    // Add a collision component to the collision system
    void add_component(std::shared_ptr<CollisionComponent> component, CollisionSystem::CollisionType type);
    
    // Register a collision callback related to boundaries. 
    void register_boundary_response(std::function<void(TransformNode*, TransformNode*)> response);

    // Register a collision callback related to entities. 
    void register_entity_response(std::function<void(TransformNode*, TransformNode*)> response);
    
    // Initialize boundary nodes
    void init_boundary(TransformNode& boundary, SceneState& scene_state);
    
    // Add colliders to boundaries
    std::shared_ptr<CollisionComponent> add_boundary_collider(
        TransformNode& boundary, 
        const Vector2& min, 
        const Vector2& max);
    
    // Add colliders to entities
    std::shared_ptr<CollisionComponent> add_entity_collider(
        TransformNode& entity, 
        const Vector2& min, 
        const Vector2& max);
    
private:
    CollisionSystem collision_system_;  // Stores and manages a collision system
};

} // namespace cge

#endif // EXAMPLES_COLLISION_MANAGER_HPP
