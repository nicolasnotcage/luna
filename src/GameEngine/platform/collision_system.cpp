/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/collision_system.hpp"
#include "graph/transform_node.hpp"
#include <algorithm>

namespace cge
{

// Default constructor with no-op handlers
CollisionSystem::CollisionSystem()
{
    boundary_handler_ = [](TransformNode*, TransformNode*) {};
    entity_handler_ = [](TransformNode*, TransformNode*) {};
    trigger_handler_ = [](TransformNode*, TransformNode*) {};
}

CollisionSystem::~CollisionSystem() 
{ 
    clear(); 
}

// Add a collision component to the collection of components with its type.
void CollisionSystem::add_component(std::shared_ptr<CollisionComponent> component, CollisionType type)
{
    if(component)
    {
        // Check if component is already in the list
        for(auto& entry : components_)
        {
            if(entry.component == component)
            {
                entry.type = type; // Update type if already exists
                return;
            }
        }
        
        // Add new component
        components_.push_back({component, type});
    }
}

// Remove a component from the collection if it exists.
void CollisionSystem::remove_component(std::shared_ptr<CollisionComponent> component)
{
    auto it = std::remove_if(components_.begin(), components_.end(),
                           [&component](const ComponentEntry& entry) {
                               return entry.component == component;
                           });
    
    if(it != components_.end())
    {
        components_.erase(it, components_.end());
    }
}

// Register a handler for boundary collisions
void CollisionSystem::register_boundary_response(std::function<void(TransformNode*, TransformNode*)> handler)
{
    boundary_handler_ = handler;
}

// Register a handler for entity-entity collisions
void CollisionSystem::register_entity_response(std::function<void(TransformNode*, TransformNode*)> handler)
{
    entity_handler_ = handler;
}

// Register a handler for trigger collisions
void CollisionSystem::register_trigger_response(std::function<void(TransformNode*, TransformNode*)> handler)
{
    trigger_handler_ = handler;
}

// Process all collisions (detection and response)
void CollisionSystem::process_collisions()
{
    // Check all components against each other
    for(size_t i = 0; i < components_.size(); i++)
    {
        for(size_t j = i + 1; j < components_.size(); j++)
        {
            auto& entry_a = components_[i];
            auto& entry_b = components_[j];
            
            auto component_a = entry_a.component;
            auto component_b = entry_b.component;
            
            // Skip if either component is disabled
            if(!component_a->is_enabled() || !component_b->is_enabled())
                continue;
            
            // Check for collision
            if(component_a->collides_with(*component_b))
            {
                // Get owner transform nodes
                TransformNode* transform_a = component_a->get_owner();
                TransformNode* transform_b = component_b->get_owner();
                
                // Skip if either owner is null
                if(!transform_a || !transform_b)
                    continue;
                
                // Determine collision type and call appropriate handler
                if(entry_a.type == CollisionType::BOUNDARY)
                {
                    boundary_handler_(transform_b, transform_a);
                }
                else if(entry_b.type == CollisionType::BOUNDARY)
                {
                    boundary_handler_(transform_a, transform_b);
                }
                else if(entry_a.type == CollisionType::TRIGGER)
                {
                    trigger_handler_(transform_b, transform_a);
                }
                else if(entry_b.type == CollisionType::TRIGGER)
                {
                    trigger_handler_(transform_a, transform_b);
                }
                else
                {
                    entity_handler_(transform_a, transform_b);
                }
            }
        }
    }
}

// Get the collision type for a component
CollisionSystem::CollisionType CollisionSystem::get_component_type(const CollisionComponent* component) const
{
    for(const auto& entry : components_)
    {
        if(entry.component.get() == component)
        {
            return entry.type;
        }
    }
    
    // Default to entity type if not found
    return CollisionType::ENTITY;
}

// Clear all components
void CollisionSystem::clear()
{
    components_.clear();
}

} // namespace cge
