/*
    Chess game popup manager implementation.

    Author: Nicolas Miller
    Date: 06/25/2025
*/

#include "chess_game/popup_manager.hpp"
#include "chess_game/constants.h"
#include "platform/audio_engine.hpp"

namespace cge
{

PopupManager::PopupManager()
    : show_game_over_popup_(false),
      popup_display_timer_(0.0)
{
}

void PopupManager::init(SceneState& scene_state)
{
    scene_state_ = &scene_state;
    load_textures(scene_state);
}

void PopupManager::load_textures(SceneState& scene_state)
{
    // Popup images
    promotion_texture_ = std::make_unique<TextureNode>();
    promotion_texture_->set_filepath("images/pop-ups/promotion_prompt.png");
    promotion_texture_->init(scene_state);

    you_win_texture_ = std::make_unique<TextureNode>();
    you_win_texture_->set_filepath("images/pop-ups/you_win.png");
    you_win_texture_->init(scene_state);

    you_lose_texture_ = std::make_unique<TextureNode>();
    you_lose_texture_->set_filepath("images/pop-ups/you_lose.png");
    you_lose_texture_->init(scene_state);

    stalemate_texture_ = std::make_unique<TextureNode>();
    stalemate_texture_->set_filepath("images/pop-ups/stalemate.png");
    stalemate_texture_->init(scene_state);
}


void PopupManager::destroy()
{
    if (promotion_texture_) promotion_texture_->destroy();
    if (you_win_texture_) you_win_texture_->destroy();
    if (you_lose_texture_) you_lose_texture_->destroy();
    if (stalemate_texture_) stalemate_texture_->destroy();
}

void PopupManager::show_promotion_prompt()
{
    if (!promotion_prompt_) return;
    
    auto& promotion_sprite = promotion_prompt_->get_child<0>();
    promotion_sprite.set_should_render(true);
}

void PopupManager::hide_promotion_prompt()
{
    if (!promotion_prompt_) return;
    
    auto& promotion_sprite = promotion_prompt_->get_child<0>();
    promotion_sprite.set_should_render(false);
}

void PopupManager::show_game_over_popup(const Position& chess_position, Color player_color)
{
    if (!player_won_ || !player_lost_ || !game_tied_) return;
    
    if (chess_position.is_in_check())
    {
        // Checkmate
        if (chess_position.side_to_move() == player_color)
        {
            // Player is checkmated - show you lose
            auto& lose_sprite = player_lost_->get_child<0>();
            lose_sprite.set_should_render(true);
        }
        else
        {
            // Computer is checkmated - show you win
            auto& win_sprite = player_won_->get_child<0>();
            win_sprite.set_should_render(true);
        }
    }
    else
    {
        // Stalemate - show stalemate
        auto& stalemate_sprite = game_tied_->get_child<0>();
        stalemate_sprite.set_should_render(true);
    }
    
    show_game_over_popup_ = true;
    popup_display_timer_ = 0.0;
}

void PopupManager::show_game_over_popup_with_result(luna::GameResult game_result, Color player_color)
{
    if (!player_won_ || !player_lost_ || !game_tied_) return;
    
    switch (game_result)
    {
        case luna::GameResult::WhiteWins:
        {
            if (player_color == Color::White)
            {
                // Player (white) wins
                auto& win_sprite = player_won_->get_child<0>();
                win_sprite.set_should_render(true);
            }
            else
            {
                // Computer (white) wins, player (black) loses
                auto& lose_sprite = player_lost_->get_child<0>();
                lose_sprite.set_should_render(true);
            }
            break;
        }
            
        case luna::GameResult::BlackWins:
        {
            if (player_color == Color::Black)
            {
                // Player (black) wins
                auto& win_sprite = player_won_->get_child<0>();
                win_sprite.set_should_render(true);
            }
            else
            {
                // Computer (black) wins, player (white) loses
                auto& lose_sprite = player_lost_->get_child<0>();
                lose_sprite.set_should_render(true);
            }
            break;
        }
            
        case luna::GameResult::Draw:
        {
            // Draw/stalemate
            auto& stalemate_sprite = game_tied_->get_child<0>();
            stalemate_sprite.set_should_render(true);
            break;
        }
            
        case luna::GameResult::None:
        default:
        {
            // Game not over - shouldn't happen, but fall back to standard logic
            show_game_over_popup(Position(), player_color);
            return;
        }
    }
    
    show_game_over_popup_ = true;
    popup_display_timer_ = 0.0;
}

void PopupManager::hide_all_popups()
{
    if (!promotion_prompt_ || !player_won_ || !player_lost_ || !game_tied_) return;
    
    // Hide all popups by setting render flag to false
    promotion_prompt_->get_child<0>().set_should_render(false);
    player_won_->get_child<0>().set_should_render(false);
    player_lost_->get_child<0>().set_should_render(false);
    game_tied_->get_child<0>().set_should_render(false);
    
    show_game_over_popup_ = false;
}

void PopupManager::update_popup_timer(double delta)
{
    if (show_game_over_popup_)
    {
        popup_display_timer_ += delta;
    }
}

void PopupManager::setup_promotion_popup(float camera_width, float camera_height)
{
    if (!promotion_prompt_ || !promotion_texture_) return;
    
    float popup_size = camera_width * POPUP_SCALE_FACTOR;
    
    auto& promotion_sprite = promotion_prompt_->get_child<0>();
    promotion_sprite.set_filepath(promotion_texture_->get_filepath());
    promotion_sprite.init(*scene_state_);
    
    promotion_prompt_->right_scale(popup_size, popup_size);
    promotion_prompt_->set_position(0.0f, 0.0f);  
    promotion_sprite.set_should_render(false); 
}

void PopupManager::setup_game_over_popups(float camera_width, float camera_height)
{
    float popup_size = camera_width * POPUP_SCALE_FACTOR;
    float popup_height = popup_size * 0.4f;
    
    // Setup you win popup
    if (player_won_ && you_win_texture_) 
    {
        auto& win_sprite = player_won_->get_child<0>();
        win_sprite.set_filepath(you_win_texture_->get_filepath());
        win_sprite.init(*scene_state_);
        player_won_->right_scale(popup_size, popup_height);
        player_won_->set_position(0.0f, 0.0f);
        win_sprite.set_should_render(false);
    }
    
    // Setup you lose popup
    if (player_lost_ && you_lose_texture_) 
    {
        auto& lose_sprite = player_lost_->get_child<0>();
        lose_sprite.set_filepath(you_lose_texture_->get_filepath());
        lose_sprite.init(*scene_state_);
        player_lost_->right_scale(popup_size, popup_height);
        player_lost_->set_position(0.0f, 0.0f);
        lose_sprite.set_should_render(false);
    }
    
    // Setup stalemate popup
    if (game_tied_ && stalemate_texture_) 
    {
        auto& stalemate_sprite = game_tied_->get_child<0>();
        stalemate_sprite.set_filepath(stalemate_texture_->get_filepath());
        stalemate_sprite.init(*scene_state_);
        game_tied_->right_scale(popup_size, popup_height);
        game_tied_->set_position(0.0f, 0.0f);
        stalemate_sprite.set_should_render(false);
    }
}

void PopupManager::setup_popup_nodes(
        PromotionPrompt* promotion_prompt,
        PlayerWon* player_won,
        PlayerLost* player_lost,
        GameTied* game_tied,
        float camera_width,
        float camera_height)
{
        promotion_prompt_ = promotion_prompt;
        player_won_ = player_won;
        player_lost_ = player_lost;
        game_tied_ = game_tied;
        
        // Setup each popup
        setup_promotion_popup(camera_width, camera_height);
        setup_game_over_popups(camera_width, camera_height);
}

} // namespace cge
