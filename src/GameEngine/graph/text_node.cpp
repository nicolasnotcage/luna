#include "graph/text_node.hpp"

#include <memory>

namespace cge
{

void TextNode::init(SceneState &scene_state)
{
	init_children(scene_state);
}

void TextNode::destroy()
{
	destroy_children();
	clear_children();
}

void TextNode::draw(SceneState& scene_state)
{
	// Only draw text if it should render
	if (is_rendered_)
	{
		// Store old values
		TextureNode* prev_texture_node = scene_state.texture_node;
		bool         old_using_sprite_sheet = scene_state.using_sprite_sheet;

		scene_state.texture_node = text_textures_.at(curr_text);
		scene_state.using_sprite_sheet = false; 

		draw_children(scene_state);

		// Restore old values
		scene_state.texture_node = prev_texture_node;
		scene_state.using_sprite_sheet = old_using_sprite_sheet;
	}
}

void TextNode::update(SceneState& scene_state)
{
	// Only process updates if rendering
	if (is_rendered_)
	{
        // Handle camera controls from game actions
        if (scene_state.io_handler)
        {
            const GameActionList& action_list = scene_state.io_handler->get_game_actions();

            // Process all game actions that affect the text node
            for (uint8_t i = 0; i < action_list.num_actions; ++i)
            {
                switch (action_list.actions[i])
                {
					case GameAction::ADVANCE_TEXT:
						// Stop rendering if we've seen all game text
						curr_text++;
						if (curr_text >= text_textures_.size()) 
						{
							is_rendered_ = false;
							curr_text = text_textures_.size() - 1;  // Stay at last valid index
						}
						break;
					default: break;
                }
            }
		}
	}

	update_children(scene_state);
}

} // namespace cge
