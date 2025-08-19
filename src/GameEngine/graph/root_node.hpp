/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_ROOT_NODE_HPP
#define GRAPH_ROOT_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"

#include "platform/sdl.h"

namespace cge
{
class RootNode : public Node
{
  public:
    RootNode() = default;

    ~RootNode() = default;

    void init(SceneState &scene_state) override;

    void destroy() override;

    void draw(SceneState &scene_state) override;

    void update(SceneState &scene_state) override;

  protected:
};

template <typename... ChildrenTs>
using RootNodeT = NodeT<RootNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_ROOT_NODE_HPP
