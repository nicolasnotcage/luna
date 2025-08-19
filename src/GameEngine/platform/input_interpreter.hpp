#ifndef PLATFORM_INPUT_INTERPRETER
#define PLATFORM_INPUT_INTERPRETER

#include "platform/event.hpp"
#include "platform/game_action.hpp"

namespace cge
{

// A simple interpreter class. Is used to map 
// raw SDL inputs to game-specific actions. 
class InputInterpreter
{
public:
    InputInterpreter() = default;
    ~InputInterpreter() = default;

    // Convert raw inputs into game actions
    GameActionList interpret_events(const SDLEventInfo &events);
};

} // namespace cge

#endif // PLATFORM_INPUT_INTERPRETER
