#ifndef GRAPH_SPRITE_NODE_HPP
#define GRAPH_SPRITE_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"

#include "platform/animation.hpp"
#include "platform/movement_controller.hpp"

#include <memory>
#include <unordered_map>

namespace cge
{

// A full-featured sprite node object. Intended to 
// provide an interface for sprite animation and 
// movement. 
class SpriteNode : public Node
{
  public:
    SpriteNode();
    ~SpriteNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Texture management
    void         set_texture(TextureNode *texture);
    TextureNode *get_texture() const;

    // Animation delegation methods
    void add_animation(const Animation &animation);
    void add_animation_with_texture(const Animation &animation, TextureNode *texture);
    void play(const std::string &animation_name);
    void pause();
    void resume();
    void reset();
    void set_playback_speed(float speed);
    void set_looping(bool looping);
    bool is_playing() const;
    const std::string &get_current_animation_name() const;
   
    // Animation integration with movement
    void update_animation_for_movement();

    // Update sprite based on movement state
    void set_movement_state(bool is_moving, MoveDirection direction, bool facing_left);
    
    // Control automatic animation switching
    void set_auto_animation_enabled(bool enabled) { auto_animation_enabled_ = enabled; }
    bool is_auto_animation_enabled() const { return auto_animation_enabled_; }

  private:
    // Current texture and frame
    TextureNode *current_texture_;
    uint32_t     current_frame_id_;

    // Map of animation names to textures
    std::unordered_map<std::string, TextureNode *> animation_textures_;

    // Animation component
    std::unique_ptr<Animator> animator_;

    // Movement state
    bool is_moving_{false};
    MoveDirection current_direction_{MoveDirection::NONE};
    bool facing_left_{false};
    
    // Animation control
    bool auto_animation_enabled_{true};
};

template <typename... ChildrenTs>
using SpriteNodeT = NodeT<SpriteNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_SPRITE_NODE_HPP
