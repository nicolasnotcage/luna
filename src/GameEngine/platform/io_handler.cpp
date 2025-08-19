#include "platform/io_handler.hpp"
#include "platform/event.hpp"

#include <iostream>

namespace cge
{

IoHandler::IoHandler() :
    curr_events_(), interpreter_()
    { 
        game_actions_.num_actions = 0; 
    }

// Update raw events and map them to game actions
void IoHandler::update() 
{ 
    curr_events_ = cge::get_current_events(); 
    game_actions_ = interpreter_.interpret_events(curr_events_);
}   

// Check if a quit was requested
bool IoHandler::quit_requested() const
{
    for(uint8_t i = 0; i < game_actions_.num_actions; i++)
    {
        if(game_actions_.actions[i] == GameAction::QUIT) 
        { 
            printf("Quit detected...\n");
            return true;
        }
    }
    return false;
}

// Return reference to game actions
const GameActionList &IoHandler::get_game_actions() const { return game_actions_; }

} // namespace cge

