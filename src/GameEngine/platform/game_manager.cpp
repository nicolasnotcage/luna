#include "game_manager.hpp"
#include "time_manager.hpp"

#include <chrono>
#include <iostream>
#include <thread> 

namespace cge
{

void GameManager::sleep(int milliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); }

} // namespace cge

