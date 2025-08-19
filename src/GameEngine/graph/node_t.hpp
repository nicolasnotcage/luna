/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_NODE_T_HPP
#define GRAPH_NODE_T_HPP

#include "graph/scene_state.hpp"

#include <tuple>

namespace cge
{

template <typename BaseT, typename... ChildrenTs>
class NodeT : public BaseT
{
    using TupleT = std::tuple<ChildrenTs...>;

  public:
    void init_children(SceneState &scene_state) override
    {
        // Next line uses fold expressions, which are a bit confusing
        std::apply([&](auto &&...t) { (t.init(scene_state), ...); }, children_ts_);

        // Traditional tuple iteration, calling init on static children
        // init_static_child<0>(scene_state);

        BaseT::init_children(scene_state);
    }

    void destroy_children() override
    {
        // Next line uses fold expressions, which are a bit confusing
        std::apply([&](auto &&...t) { (t.destroy(), ...); }, children_ts_);

        // Traditional tuple iteration, calling destroy on static children
        // destroy_static_child<0>();

        BaseT::destroy_children();
    }

    void draw_children(SceneState &scene_state) override
    {
        // Next line uses fold expressions, which are a bit confusing
        std::apply([&](auto &&...t) { (t.draw(scene_state), ...); }, children_ts_);

        // Traditional tuple iteration, calling draw on static children
        // draw_static_child<0>(scene_state);

        BaseT::draw_children(scene_state);
    }

    void update_children(SceneState &scene_state) override
    {
        std::apply([&](auto &&...t) { (t.update(scene_state), ...); }, children_ts_);
        BaseT::update_children(scene_state);
    }

    template <size_t Idx>
    auto &get_child()
    {
        static_assert(Idx < std::tuple_size<TupleT>::value);
        return std::get<Idx>(children_ts_);
    }

    template <size_t Idx>
    const auto &get_child() const
    {
        static_assert(Idx < std::tuple_size<TupleT>::value);
        return std::get<Idx>(children_ts_);
    }

  protected:
    TupleT children_ts_;

    template <size_t Idx>
    void init_static_child(SceneState &scene_state)
    {
        if constexpr(Idx < std::tuple_size<TupleT>::value)
        {
            get_child<Idx>().init(scene_state);
            init_static_child<Idx + 1>(scene_state);
        }
    }

    template <size_t Idx>
    void destroy_static_child()
    {
        if constexpr(Idx < std::tuple_size<TupleT>::value)
        {
            get_child<Idx>().destroy();
            destroy_static_child<Idx + 1>();
        }
    }

    template <size_t Idx>
    void draw_static_child(SceneState &scene_state)
    {
        if constexpr(Idx < std::tuple_size<TupleT>::value)
        {
            get_child<Idx>().draw(scene_state);
            draw_static_child<Idx + 1>(scene_state);
        }
    }
};

} // namespace cge

#endif // GRAPH_NODE_T_HPP
