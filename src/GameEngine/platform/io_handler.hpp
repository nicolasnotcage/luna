#ifndef PLATFORM_IO_HANDLER
#define PLATFORM_IO_HANDLER

#include "platform/event.hpp"
#include "platform/game_action.hpp"
#include "platform/input_interpreter.hpp"

namespace cge
{

// The central hub of input mapping. Contains 
// raw SDL events, a list of game-specific actions, 
// and an interpreter for translating raw events into 
// game actions.
class IoHandler
{
public:
    IoHandler();
    ~IoHandler() = default;

    void update();
    bool quit_requested() const;

    // Access current game actions
    const GameActionList &get_game_actions() const;

private:
    SDLEventInfo curr_events_;
    InputInterpreter interpreter_;
    GameActionList   game_actions_;
};

} // namespace cge

#endif // PLATFORM_IO_HANDLER
