/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/io_handler.hpp"
#include "platform/movement_controller.hpp"
#include "platform/path.hpp"
#include "graph/transform_node.hpp"

#include <cmath>

namespace cge
{

MovementController::MovementController(TransformNode &transform_node) : transform_node_(transform_node) {}

// Track sprite orientation
void MovementController::update_sprite_orientation()
{
    if(current_direction_ == MoveDirection::LEFT) 
    { 
        facing_left_ = true; 
    }
    else if(current_direction_ == MoveDirection::RIGHT)
    {
        facing_left_ = false;
    } 
}

void PlayerController::update(SceneState &scene_state)
{
    is_moving_ = false;

    if(!scene_state.io_handler) return;

    const GameActionList &actions = scene_state.io_handler->get_game_actions();
    float move_amount = speed_ * scene_state.delta;

    // Default: no movement
    current_direction_ = MoveDirection::NONE;

    // Process movement actions
    for(uint8_t i = 0; i < actions.num_actions; ++i)
    {
        switch(actions.actions[i])
        {
            case GameAction::PLAYER_MOVE_UP:
                transform_node_.right_translate(0.0f, -move_amount);
                current_direction_ = MoveDirection::UP;
                is_moving_ = true;
                break;

            case GameAction::PLAYER_MOVE_DOWN:
                transform_node_.right_translate(0.0f, move_amount);
                current_direction_ = MoveDirection::DOWN;
                is_moving_ = true;
                break;

            case GameAction::PLAYER_MOVE_LEFT:
                transform_node_.right_translate(-move_amount, 0.0f);
                current_direction_ = MoveDirection::LEFT;
                is_moving_ = true;
                break;

            case GameAction::PLAYER_MOVE_RIGHT:
                transform_node_.right_translate(move_amount, 0.0f);
                current_direction_ = MoveDirection::RIGHT;
                is_moving_ = true;
                break;

            default:
                break;
        }
    }

    update_sprite_orientation();
}

// Player specific collision handling; should make this specific to the scenario; perhaps a list of entities?
void PlayerController::handle_collision() { is_moving_ = false; }

// Update movement along path
void PathController::update(SceneState &scene_state)
{
    is_moving_ = false;

    // Don't move if path is empty
    if(path_.size() == 0) return;

    // Handle pause timer
    if(pause_timer_ > 0.0f)
    {
        pause_timer_ -= scene_state.delta;
        return;
    }

    // Move towards current target
    move_towards_target(scene_state.delta);
}

// Move towards current target point
void PathController::move_towards_target(float delta)
{
    // Don't move if path is empty
    if(path_.size() == 0) return;

    // Get current target
    const PathPoint &target = path_.get_point(current_point_index_);

    // Get entity's current position in game world
    Matrix3 transform = transform_node_.get_transform();
    Vector2 position(transform.a[6], transform.a[7]);

    // Calculate direction to target
    Vector2 direction(target.x - position.x, target.y - position.y);
    float   distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // If we've reached the target (or very close to it)
    if(distance < 0.1f)
    {
        // Set position exactly to target
        transform_node_.right_translate(direction.x, direction.y);

        // Start pause timer
        pause_timer_ = target.pause_time;

        // Move to next point
        current_point_index_++;

        // Handle end of path
        if(current_point_index_ >= path_.size())
        {
            if(path_.is_looping()) 
            { 
                current_point_index_ = 0; 
            }
            else
            {
                current_direction_ = MoveDirection::NONE;
                is_moving_ = false;
                return;
            }
        }

        return;
    }

    // Normalize direction (unit vector). We must take 
    // this step. If we don't, then the speed of the 
    // entity will correlate with the magnitude of the
    // distance vector (fast for large distances, slow 
    // for small). Comment out this line for a demo.
    direction.x /= distance;
    direction.y /= distance;

    // Determine direction
    if(std::abs(direction.x) > std::abs(direction.y))
    {
        current_direction_ = direction.x > 0 ? MoveDirection::RIGHT : MoveDirection::LEFT;
    }
    else 
    { 
        current_direction_ = direction.y > 0 ? MoveDirection::DOWN : MoveDirection::UP; 
    }

    // Move towards target
    float move_amount = speed_ * delta;

    // If we are going to overshoot, just move to the target
    if(move_amount > distance)
    {
        transform_node_.right_translate(direction.x * distance, direction.y * distance);
    }
    else 
    { 
        transform_node_.right_translate(direction.x * move_amount, direction.y * move_amount); 
    }

    is_moving_ = true;

    update_sprite_orientation();
}

} // namespace cge
