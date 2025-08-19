/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/collision_manager.hpp"

namespace cge
{

CollisionManager::CollisionManager()
{
}

void CollisionManager::init(SceneState& scene_state)
{
    // Initialize collision manager
    // Currently no-op
}

// Process all collisions using the collision system
void CollisionManager::process_collisions()
{
    collision_system_.process_collisions();
}

void CollisionManager::add_component(std::shared_ptr<CollisionComponent> component, CollisionSystem::CollisionType type)
{
    collision_system_.add_component(component, type);
}

void CollisionManager::register_boundary_response(std::function<void(TransformNode*, TransformNode*)> response)
{
    collision_system_.register_boundary_response(response);
}

void CollisionManager::register_entity_response(std::function<void(TransformNode*, TransformNode*)> response)
{
    collision_system_.register_entity_response(response);
}

void CollisionManager::init_boundary(TransformNode& boundary, SceneState& scene_state)
{
    boundary.init(scene_state);
}

std::shared_ptr<CollisionComponent> CollisionManager::add_boundary_collider(
    TransformNode& boundary, 
    const Vector2& min, 
    const Vector2& max)
{
    auto collider = boundary.add_aabb_collider(min, max);
    add_component(collider, CollisionSystem::CollisionType::BOUNDARY);
    return collider;
}

std::shared_ptr<CollisionComponent> CollisionManager::add_entity_collider(
    TransformNode& entity, 
    const Vector2& min, 
    const Vector2& max)
{
    auto collider = entity.add_aabb_collider(min, max);
    add_component(collider, CollisionSystem::CollisionType::ENTITY);
    return collider;
}

} // namespace cge
