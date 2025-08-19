#ifndef GRAPH_ANALYTICS_NODE_HPP
#define GRAPH_ANALYTICS_NODE_HPP

#include "graph/node.hpp"
#include "graph/node_t.hpp"
#include "platform/time_manager.hpp"

namespace cge
{

/**
 * Analytics node used for statistics tracking.
 **/
class AnalyticsNode : public Node
{
  public:
    AnalyticsNode() = default;
    ~AnalyticsNode() = default;

    // Overrides
    void init(SceneState &scene_state) override;
    void destroy() override;
    void draw(SceneState &scene_state) override;
    void update(SceneState &scene_state) override;

    int get_draws() const { return total_draws_; }
    int get_updates() const { return total_updates_; }

private:
    int          total_updates_{0};
    int          total_draws_{0};
};

template <typename... ChildrenTs>
using AnalyticsNodeT = NodeT<AnalyticsNode, ChildrenTs...>;

} // namespace cge

#endif // GRAPH_ANALYTICS_NODE_HPP
