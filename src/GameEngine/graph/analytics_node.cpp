#include "graph/scene_state.hpp"
#include "analytics_node.hpp"
#include "platform/time_manager.hpp"

#include <iostream>

namespace cge
{
    
void AnalyticsNode::init(SceneState &scene_state) 
{ 
    init_children(scene_state); 
}

void AnalyticsNode::draw(SceneState &scene_state) 
{ 
    total_draws_ += 1;
    draw_children(scene_state); 
}

void AnalyticsNode::destroy()
{
    std::cout << "Total Updates: " << total_updates_ << "\n";
    std::cout << "Total Draws: " << total_draws_ << std::endl;

    destroy_children();
    clear_children();
}

void AnalyticsNode::update(SceneState &scene_state)
{ 
    total_updates_ += 1; 
    update_children(scene_state);
}

} // namespace cge
