#ifndef PLATFORM_COLLISION_SYSTEM_HPP
#define PLATFORM_COLLISION_SYSTEM_HPP

#include "platform/collision_component.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace cge
{

// Forward declarations
class CollisionComponent;
class TransformNode;

// Collision system to manage and check collisions between components
class CollisionSystem
{
  public:
    // Collision type enum
    enum class CollisionType
    {
        ENTITY,
        BOUNDARY,
        TRIGGER
    };

    CollisionSystem();
    ~CollisionSystem();

    // Add a collision component to the system, specifying the component's type.
    void add_component(std::shared_ptr<CollisionComponent> component, 
                       CollisionType type = CollisionType::ENTITY);

    // Remove a collision component from the system.
    void remove_component(std::shared_ptr<CollisionComponent> component);

    // Register a callback function for boundary-based collisions.
    void register_boundary_response(std::function<void(TransformNode*, TransformNode*)> handler);

    // Register a callback function for entity-based collisions.
    void register_entity_response(std::function<void(TransformNode*, TransformNode*)> handler);

    // Register a callback for trigger-zone-based collisions
    void register_trigger_response(std::function<void(TransformNode*, TransformNode*)> handler);

    // Process all collisions (detection and response)
    void process_collisions();

    // Clear all components
    void clear();

    // Get collision type for a component
    CollisionType get_component_type(const CollisionComponent* component) const;

  private:
    // Component storage with type information
    struct ComponentEntry
    {
        std::shared_ptr<CollisionComponent> component;
        CollisionType type;
    };
    
    std::vector<ComponentEntry> components_;                                // A collection of collision components
    
    // Response handlers
    std::function<void(TransformNode*, TransformNode*)> boundary_handler_;  // Boundary handler callback
    std::function<void(TransformNode*, TransformNode*)> entity_handler_;    // Entity handler callback
    std::function<void(TransformNode*, TransformNode*)> trigger_handler_;   // Trigger zone handler callback
};

} // namespace cge

#endif // PLATFORM_COLLISION_SYSTEM_HPP
