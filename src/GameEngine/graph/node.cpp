/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "graph/node.hpp"

namespace cge
{

// Calls init() on each child with given scene state data
void Node::init_children(SceneState &scene_state)
{
    for(auto &child : children_) { child->init(scene_state); }
}

void Node::destroy_children()
{
    for(auto &child : children_) { child->destroy(); }
}

void Node::draw_children(SceneState &scene_state)
{
    for(auto &child : children_) { child->draw(scene_state); }
}

void Node::update_children(SceneState& scene_state)
{
    for(auto &child : children_) { child->update(scene_state); }
}

void Node::set_name(const std::string &n) { name_ = n; }

void Node::add_child(std::shared_ptr<Node> child) { children_.push_back(child); }

void Node::clear_children() { children_.clear(); }

} // namespace cge
