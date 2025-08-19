/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_GEOMETRY_NODE_HPP
#define GRAPH_GEOMETRY_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"

#include "platform/sdl.h"

namespace cge
{
class GeometryNode : public Node
{
  public:
    GeometryNode() = default;

    ~GeometryNode() = default;

    void init(SceneState &scene_state) override;

    void destroy() override;

    void draw(SceneState &scene_state) override;

    void update(SceneState &scene_state) override;
};

template <typename... ChildrenTs>
using GeometryNodeT = NodeT<GeometryNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_GEOMETRY_NODE_HPP
