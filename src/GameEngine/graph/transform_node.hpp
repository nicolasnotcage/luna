#ifndef GRAPH_TRANSFORM_NODE_HPP
#define GRAPH_TRANSFORM_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "platform/math.hpp"
#include "platform/path.hpp"

#include <memory>
#include <iostream>

namespace cge
{

// Forward declarations
class SpriteNode;
class MovementController;
enum class MoveDirection;
class CollisionComponent;
class CircleCollisionComponent;
class AABBCollisionComponent;
class AudioComponent;

class TransformNode : public Node
{
  public:
    TransformNode() = default;
    ~TransformNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Transformation functions
    void set_identity();
    void left_scale(float x, float y);
    void right_scale(float x, float y);
    void left_rotate_degrees(float angle_deg);
    void right_rotate_degrees(float angle_deg);
    void left_rotate(float rad_deg);
    void right_rotate(float rad_deg);
    void left_translate(float x, float y);
    void right_translate(float x, float y);

    // Getters for transformation data
    float get_scale_x() const { return scale_x_; }
    float get_scale_y() const { return scale_y_; }
    Matrix3 &get_transform() { return transform_; }

    // Movement controller methods
    void                set_player_controlled();
    void                set_path_controlled(Path &path);
    bool                is_moving() const;
    MovementController *get_movement_controller() { return movement_controller_.get(); }
    
    
    // Return movement direction
    MoveDirection get_move_direction() const;

    // Return if facing left
    bool is_facing_left() const;

    // Connect a sprite to this transform
    void set_associated_sprite(SpriteNode *sprite);

    // ------------------------------------
    //          Collision Code
    // ------------------------------------
    std::shared_ptr<CircleCollisionComponent> add_circle_collider(float radius);
    std::shared_ptr<AABBCollisionComponent> add_aabb_collider(const Vector2 &min, const Vector2 &max);
    std::shared_ptr<CollisionComponent> get_collision_component() const { return collision_component_; }

    // Position-related functions; primarily used when handling collisions. 
    void store_previous_transform() { previous_transform_ = transform_; }
    float get_position_x() const { return transform_.a[6]; }
    float get_position_y() const { return transform_.a[7]; }
    float get_prev_position_x() const { return previous_transform_.a[6]; }
    float get_prev_position_y() const { return previous_transform_.a[7]; }

    void set_position(float x, float y)
    {
        store_previous_transform();

        // Update only the translation components of the matrix
        transform_.a[6] = x;
        transform_.a[7] = y;
    }

    // ------------------------------------
    //             Audio Code
    // ------------------------------------
    AudioComponent *add_audio_component();
    AudioComponent *get_audio_component() const { return audio_component_; }

private:
    Matrix3 transform_;
    Matrix3 previous_transform_;
    std::unique_ptr<MovementController> movement_controller_;
    SpriteNode                         *associated_sprite_{nullptr};

    // Collision Component
    std::shared_ptr<CollisionComponent> collision_component_;

    // Audio Component
    AudioComponent *audio_component_;

    // Transform information
    float scale_x_{1.0};
    float scale_y_{1.0};
};

template <typename... ChildrenTs>
using TransformNodeT = NodeT<TransformNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TRANSFORM_NODE_HPP
