#ifndef GRAPH_TEXT_NODE_HPP
#define GRAPH_TEXT_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "graph/texture_node.hpp"

#include "platform/io_handler.hpp"

#include <memory>
#include <vector>

namespace cge
{

/* 
* TextNodes are utility nodes designed to hold a collection of textures
* that contain text to be displayed in a game. Functions are provided to 
* control when these nodes are rendered in a scene so that rendering can 
* be driven by game state.
*/
class TextNode : public Node
{
  public:
    TextNode() = default;
    ~TextNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    // Set whether or not the node should be rendered
    void set_should_render(bool should_render) { is_rendered_ = should_render; }

    // Return whether or not the node is rendered
    bool is_rendered() const { return is_rendered_; }

    // Push a texture to the collection of textures
    void push_texture(TextureNode* texture) { text_textures_.push_back(texture); }

    // Empty the collection of textures so that the node can be re-used
    void clear_textures() { text_textures_.clear(); }

  private:
    std::vector<TextureNode*> text_textures_{}; // A collection of text textures to be rendered
    uint16_t curr_text{0};                      // The current texture to be rendered
    bool is_rendered_{false};                   // Whether or not the node is being rendered
};

template <typename... ChildrenTs>
using TextNodeT = NodeT<TextNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_TEXT_NODE_HPP
