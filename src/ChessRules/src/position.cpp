/*
    Basic implementation of Position class.
    
    REVISED: Now uses a move history stack for proper undo functionality
    
    Author: Nicolas Miller
    Date: 06/11/2025
*/

#include "bitboard.h"
#include "movegen.h"
#include "position.h"
#include "ChessEngine/include/zobrist.h"
#include <iostream>
#include <sstream>

using namespace luna;

// Constructor - sets up starting position
Position::Position()
{
    // Initialize bitboard attack tables
    Bitboard::init_attack_tables(); 
    
    // Initialize Zobrist hash tables
    ZobristHash::initialize();

    // Initialize to empty
    for (int c = 0; c < static_cast<int>(Color::NB); ++c)
    {
        for (int pt = 0; pt < static_cast<int>(PieceType::NB); ++pt)
        {
            pieces_[c][pt] = Bitboard();
        }
        occupied_by_color_[c] = Bitboard();
    }
    occupied_ = Bitboard();
    
    for (int sq = 0; sq < static_cast<int>(Square::NB); ++sq)
    {
        board_[sq] = Piece::None;
    }
    
    hash_key_ = 0;
    
    // Load starting position
    load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// Delegate to MoveGenerator
std::vector<Move> Position::generate_legal_moves() const
{
    return MoveGenerator::generate_legal_moves(*this);
}

// Delegate to MoveGenerator
bool Position::is_square_attacked(Square square, Color by_color) const
{
    return MoveGenerator::is_square_attacked(*this, square, by_color);
}

// Load position from FEN string
bool Position::load_fen(const std::string& fen)
{
    std::istringstream ss(fen);
    std::string board_part, active_color, castling, ep_square;
    
    // Parse FEN components
    if (!(ss >> board_part >> active_color >> castling >> ep_square >> halfmove_clock_ >> fullmove_number_))
        return false;
    
    // Clear current position
    for (int c = 0; c < static_cast<int>(Color::NB); c++)
    {
        for (int pt = 0; pt < static_cast<int>(PieceType::NB); pt++)
        {
            pieces_[c][pt] = Bitboard();
        }
    }
    
    for (int sq = 0; sq < static_cast<int>(Square::NB); sq++)
    {
        board_[sq] = Piece::None;
    }
    
    // Clear move history when loading a new position
    move_history_.clear();
    
    // Parse board
    int rank = 7;  // Start from rank 8
    int file = 0;
    
    for (char ch : board_part)
    {
        if (ch == '/')
        {
            rank--;
            file = 0;
        }
        else if (ch >= '1' && ch <= '8')
        {
            file += ch - '0';
        }
        else
        {
            // Determine piece type and color
            Color color = std::isupper(ch) ? Color::White : Color::Black;
            PieceType type;
            
            switch (std::tolower(ch))
            {
                case 'p': type = PieceType::Pawn; break;
                case 'n': type = PieceType::Knight; break;
                case 'b': type = PieceType::Bishop; break;
                case 'r': type = PieceType::Rook; break;
                case 'q': type = PieceType::Queen; break;
                case 'k': type = PieceType::King; break;
                default: return false;
            }
            
            Square sq = make_square(static_cast<File>(file), static_cast<Rank>(rank));
            Piece piece = make_piece(color, type);
            
            board_[static_cast<int>(sq)] = piece;
            pieces_[static_cast<int>(color)][static_cast<int>(type)].set_bit(sq);
            
            file++;
        }
    }
    
    // Parse side to move
    side_to_move_ = (active_color == "w") ? Color::White : Color::Black;
    
    // Parse castling rights
    castling_rights_ = 0;
    for (char ch : castling)
    {
        switch (ch)
        {
            case 'K': castling_rights_ |= (1 << static_cast<int>(CastlingRights::White_OO)); break;
            case 'Q': castling_rights_ |= (1 << static_cast<int>(CastlingRights::White_OOO)); break;
            case 'k': castling_rights_ |= (1 << static_cast<int>(CastlingRights::Black_OO)); break;
            case 'q': castling_rights_ |= (1 << static_cast<int>(CastlingRights::Black_OOO)); break;
        }
    }
    
    // Parse en passant square
    en_passant_square_ = (ep_square == "-") ? Square::None : string_to_square(ep_square);
    
    // Update aggregate bitboards
    update_bitboards();
    
    // Compute hash key for the loaded position
    hash_key_ = ZobristHash::hash_position(*this);
    
    return true;
}

// Get FEN string representation of current position
std::string Position::to_fen() const
{
    std::ostringstream fen;
    
    // Board representation
    for (int rank = 7; rank >= 0; rank--)
    {
        int empty_count = 0;
        
        for (int file = 0; file < 8; file++)
        {
            Square sq = make_square(static_cast<File>(file), static_cast<Rank>(rank));
            Piece piece = piece_on(sq);
            
            if (piece == Piece::None)
            {
                empty_count++;
            }
            else
            {
                if (empty_count > 0)
                {
                    fen << empty_count;
                    empty_count = 0;
                }
                fen << piece_to_string(piece);
            }
        }
        
        if (empty_count > 0)
        {
            fen << empty_count;
        }
        
        if (rank > 0)
        {
            fen << '/';
        }
    }
    
    // Side to move
    fen << ' ' << (side_to_move_ == Color::White ? 'w' : 'b');
    
    // Castling rights
    fen << ' ';
    bool has_castling = false;
    if (castling_rights_ & (1 << static_cast<int>(CastlingRights::White_OO)))
    {
        fen << 'K';
        has_castling = true;
    }
    if (castling_rights_ & (1 << static_cast<int>(CastlingRights::White_OOO)))
    {
        fen << 'Q';
        has_castling = true;
    }
    if (castling_rights_ & (1 << static_cast<int>(CastlingRights::Black_OO)))
    {
        fen << 'k';
        has_castling = true;
    }
    if (castling_rights_ & (1 << static_cast<int>(CastlingRights::Black_OOO)))
    {
        fen << 'q';
        has_castling = true;
    }
    if (!has_castling)
    {
        fen << '-';
    }
    
    // En passant square
    fen << ' ';
    if (en_passant_square_ == Square::None)
    {
        fen << '-';
    }
    else
    {
        fen << square_to_string(en_passant_square_);
    }
    
    // Halfmove clock and fullmove number
    fen << ' ' << halfmove_clock_ << ' ' << fullmove_number_;
    
    return fen.str();
}

// Update aggregate bitboards
void Position::update_bitboards()
{
    // Clear aggregate bitboards
    for (int c = 0; c < static_cast<int>(Color::NB); ++c)
    {
        occupied_by_color_[c] = Bitboard();
    }
    occupied_ = Bitboard();
    
    // Rebuild from piece bitboards
    for (int c = 0; c < static_cast<int>(Color::NB); ++c)
    {
        for (int pt = 0; pt < static_cast<int>(PieceType::NB); ++pt)
        {
            occupied_by_color_[c] |= pieces_[c][pt];
        }
        occupied_ |= occupied_by_color_[c];
    }
}

// Get piece on a square
Piece Position::piece_on(Square square) const
{
    return board_[static_cast<int>(square)];
}

// Print board for debugging
void Position::print_board() const
{
    std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    
    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << (rank + 1) << " |";
        
        for (int file = 0; file < 8; file++)
        {
            Square sq = make_square(static_cast<File>(file), static_cast<Rank>(rank));
            Piece piece = piece_on(sq);
            
            char ch = ' ';
            if (piece != Piece::None)
            {
                ch = piece_to_string(piece)[0];
            }
            
            std::cout << " " << ch << " |";
        }
        
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
    }
    
    std::cout << "    a   b   c   d   e   f   g   h\n\n";
    std::cout << "Side to move: " << (side_to_move_ == Color::White ? "White" : "Black") << "\n";
    std::cout << "Move history: " << move_history_.size() << " moves\n";
}

// Get king square
Square Position::king_square(Color color) const
{
    Bitboard king_bb = pieces(color, PieceType::King);
    return static_cast<Square>(king_bb.get_lsb_index());
}

// Check if current side is in check
bool Position::is_in_check() const
{
    Square king_sq = king_square(side_to_move_);
    Color enemy_color = opposite_color(side_to_move_);
    return is_square_attacked(king_sq, enemy_color);
}

// Make a move (handles all move types)
void Position::make_move(const Move& move)
{
    // Create a copy of the move that we'll store in history
    Move move_with_state = move;
    
    // Store previous state for undoing
    move_with_state.previous_castling_rights = castling_rights_;
    move_with_state.previous_en_passant_square = en_passant_square_;
    move_with_state.previous_halfmove_clock = halfmove_clock_;
    
    Piece moving_piece = piece_on(move.from_square);
    Piece captured_piece = piece_on(move.to_square);
    move_with_state.captured_piece = captured_piece;
    
    Color color = color_of(moving_piece);
    PieceType type = type_of(moving_piece);
    
    // Remove piece from source square
    pieces_[static_cast<int>(color)][static_cast<int>(type)].clear_bit(move.from_square);
    board_[static_cast<int>(move.from_square)] = Piece::None;
    
    // Handle different move types
    switch (move.move_type)
    {
        case MoveType::Normal:
        case MoveType::Capture:
            // Remove captured piece if any
            if (captured_piece != Piece::None)
            {
                Color cap_color = color_of(captured_piece);
                PieceType cap_type = type_of(captured_piece);
                pieces_[static_cast<int>(cap_color)][static_cast<int>(cap_type)].clear_bit(move.to_square);
                halfmove_clock_ = 0;  // Reset on capture
            }
            else if (type != PieceType::Pawn)
            {
                halfmove_clock_++;  // Increment if not pawn move or capture
            }
            else
            {
                halfmove_clock_ = 0;  // Reset on pawn move
            }
            
            // Place piece on destination square
            pieces_[static_cast<int>(color)][static_cast<int>(type)].set_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = moving_piece;
            break;
            
        case MoveType::Castle:
        {
            // Move king
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::King)].set_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = moving_piece;
            
            // Move rook
            Square rook_from, rook_to;
            if (move.to_square == Square::G1)  // White kingside
            {
                rook_from = Square::H1;
                rook_to = Square::F1;
            }
            else if (move.to_square == Square::C1)  // White queenside
            {
                rook_from = Square::A1;
                rook_to = Square::D1;
            }
            else if (move.to_square == Square::G8)  // Black kingside
            {
                rook_from = Square::H8;
                rook_to = Square::F8;
            }
            else  // Black queenside (C8)
            {
                rook_from = Square::A8;
                rook_to = Square::D8;
            }
            
            // Move the rook
            Piece rook = piece_on(rook_from);
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Rook)].clear_bit(rook_from);
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Rook)].set_bit(rook_to);
            board_[static_cast<int>(rook_from)] = Piece::None;
            board_[static_cast<int>(rook_to)] = rook;
            
            halfmove_clock_++;
            break;
        }
            
        case MoveType::EnPassant:
        {
            // Place pawn on destination
            pieces_[static_cast<int>(color)][static_cast<int>(type)].set_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = moving_piece;
            
            // Remove captured pawn (it's not on the destination square)
            Square captured_pawn_sq;
            if (color == Color::White)
            {
                captured_pawn_sq = static_cast<Square>(static_cast<int>(move.to_square) - 8);
            }
            else
            {
                captured_pawn_sq = static_cast<Square>(static_cast<int>(move.to_square) + 8);
            }
            
            // Store the captured pawn for undo
            Color enemy_color = opposite_color(color);
            move_with_state.captured_piece = make_piece(enemy_color, PieceType::Pawn);
            
            pieces_[static_cast<int>(enemy_color)][static_cast<int>(PieceType::Pawn)].clear_bit(captured_pawn_sq);
            board_[static_cast<int>(captured_pawn_sq)] = Piece::None;
            
            halfmove_clock_ = 0;  // Reset on pawn move
            break;
        }
            
        case MoveType::Promotion:
            // Remove captured piece if any
            if (captured_piece != Piece::None)
            {
                Color cap_color = color_of(captured_piece);
                PieceType cap_type = type_of(captured_piece);
                pieces_[static_cast<int>(cap_color)][static_cast<int>(cap_type)].clear_bit(move.to_square);
            }
            
            // Place promoted piece
            PieceType promo_type = type_of(move.promotion_piece);
            pieces_[static_cast<int>(color)][static_cast<int>(promo_type)].set_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = move.promotion_piece;
            
            halfmove_clock_ = 0;  // Reset on pawn move
            break;
    }
    
    // Update castling rights
    // Remove castling rights if king moves
    if (type == PieceType::King)
    {
        if (color == Color::White)
        {
            castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::White_OO));
            castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::White_OOO));
        }
        else
        {
            castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::Black_OO));
            castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::Black_OOO));
        }
    }
    
    // Remove castling rights if rook moves from original square
    if (move.from_square == Square::A1)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::White_OOO));
    if (move.from_square == Square::H1)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::White_OO));
    if (move.from_square == Square::A8)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::Black_OOO));
    if (move.from_square == Square::H8)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::Black_OO));
    
    // Remove castling rights if rook is captured on original square
    if (move.to_square == Square::A1)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::White_OOO));
    if (move.to_square == Square::H1)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::White_OO));
    if (move.to_square == Square::A8)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::Black_OOO));
    if (move.to_square == Square::H8)
        castling_rights_ &= ~(1 << static_cast<int>(CastlingRights::Black_OO));
    
    // Update en passant square
    en_passant_square_ = Square::None;
    
    // Set en passant square if pawn double push
    if (type == PieceType::Pawn)
    {
        int from_rank = static_cast<int>(rank_of(move.from_square));
        int to_rank = static_cast<int>(rank_of(move.to_square));
        
        if (abs(to_rank - from_rank) == 2)
        {
            // Set en passant square to the square behind the pawn
            if (color == Color::White)
            {
                en_passant_square_ = static_cast<Square>(static_cast<int>(move.to_square) - 8);
            }
            else
            {
                en_passant_square_ = static_cast<Square>(static_cast<int>(move.to_square) + 8);
            }
        }
    }
    
    // Update aggregate bitboards
    update_bitboards();
    
    // Switch side to move
    side_to_move_ = opposite_color(side_to_move_);
    
    // Increment fullmove number after black's move
    if (side_to_move_ == Color::White)
    {
        fullmove_number_++;
    }
    
    // Add the move to history
    move_history_.push_back(move_with_state);
    
    // Update hash key (recompute from current position after move is made)
    hash_key_ = ZobristHash::hash_position(*this);
}

// Undo the last move
void Position::undo_move()
{
    // Check if there's a move to undo
    if (move_history_.empty())
    {
        std::cerr << "Warning: No moves to undo!\n";
        return;
    }
    
    // Get the last move from history
    const Move& move = move_history_.back();
    
    // Switch side to move back
    side_to_move_ = opposite_color(side_to_move_);
    
    // Decrement fullmove number if we're undoing a black move
    if (side_to_move_ == Color::Black)
    {
        fullmove_number_--;
    }
    
    // Restore previous state
    castling_rights_ = move.previous_castling_rights;
    en_passant_square_ = move.previous_en_passant_square;
    halfmove_clock_ = move.previous_halfmove_clock;
    
    // Get the piece that was moved
    Piece moving_piece;
    if (move.move_type == MoveType::Promotion)
    {
        // For promotions, the original piece was a pawn
        moving_piece = make_piece(side_to_move_, PieceType::Pawn);
    }
    else
    {
        // For other moves, get the piece from the destination square
        moving_piece = piece_on(move.to_square);
    }
    
    Color color = color_of(moving_piece);
    PieceType type = type_of(moving_piece);
    
    // Handle different move types
    switch (move.move_type)
    {
        case MoveType::Normal:
        case MoveType::Capture:
            // Remove piece from destination square
            pieces_[static_cast<int>(color)][static_cast<int>(type)].clear_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = Piece::None;
            
            // Place piece back on source square
            pieces_[static_cast<int>(color)][static_cast<int>(type)].set_bit(move.from_square);
            board_[static_cast<int>(move.from_square)] = moving_piece;
            
            // Restore captured piece if any
            if (move.captured_piece != Piece::None)
            {
                Color cap_color = color_of(move.captured_piece);
                PieceType cap_type = type_of(move.captured_piece);
                pieces_[static_cast<int>(cap_color)][static_cast<int>(cap_type)].set_bit(move.to_square);
                board_[static_cast<int>(move.to_square)] = move.captured_piece;
            }
            break;
            
        case MoveType::Castle:
        {
            // Move king back
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::King)].clear_bit(move.to_square);
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::King)].set_bit(move.from_square);
            board_[static_cast<int>(move.to_square)] = Piece::None;
            board_[static_cast<int>(move.from_square)] = moving_piece;
            
            // Move rook back
            Square rook_from, rook_to;
            if (move.to_square == Square::G1)  // White kingside
            {
                rook_from = Square::F1;
                rook_to = Square::H1;
            }
            else if (move.to_square == Square::C1)  // White queenside
            {
                rook_from = Square::D1;
                rook_to = Square::A1;
            }
            else if (move.to_square == Square::G8)  // Black kingside
            {
                rook_from = Square::F8;
                rook_to = Square::H8;
            }
            else  // Black queenside (C8)
            {
                rook_from = Square::D8;
                rook_to = Square::A8;
            }
            
            // Move the rook back
            Piece rook = piece_on(rook_from);
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Rook)].clear_bit(rook_from);
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Rook)].set_bit(rook_to);
            board_[static_cast<int>(rook_from)] = Piece::None;
            board_[static_cast<int>(rook_to)] = rook;
            break;
        }
            
        case MoveType::EnPassant:
        {
            // Remove pawn from destination square
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Pawn)].clear_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = Piece::None;
            
            // Place pawn back on source square
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Pawn)].set_bit(move.from_square);
            board_[static_cast<int>(move.from_square)] = moving_piece;
            
            // Restore captured pawn
            Square captured_pawn_sq;
            if (color == Color::White)
            {
                captured_pawn_sq = static_cast<Square>(static_cast<int>(move.to_square) - 8);
            }
            else
            {
                captured_pawn_sq = static_cast<Square>(static_cast<int>(move.to_square) + 8);
            }
            
            Color enemy_color = opposite_color(color);
            pieces_[static_cast<int>(enemy_color)][static_cast<int>(PieceType::Pawn)].set_bit(captured_pawn_sq);
            board_[static_cast<int>(captured_pawn_sq)] = move.captured_piece;
            break;
        }
            
        case MoveType::Promotion:
            // Remove promoted piece from destination square
            PieceType promo_type = type_of(move.promotion_piece);
            pieces_[static_cast<int>(color)][static_cast<int>(promo_type)].clear_bit(move.to_square);
            board_[static_cast<int>(move.to_square)] = Piece::None;
            
            // Place pawn back on source square
            pieces_[static_cast<int>(color)][static_cast<int>(PieceType::Pawn)].set_bit(move.from_square);
            board_[static_cast<int>(move.from_square)] = make_piece(color, PieceType::Pawn);
            
            // Restore captured piece if any
            if (move.captured_piece != Piece::None)
            {
                Color cap_color = color_of(move.captured_piece);
                PieceType cap_type = type_of(move.captured_piece);
                pieces_[static_cast<int>(cap_color)][static_cast<int>(cap_type)].set_bit(move.to_square);
                board_[static_cast<int>(move.to_square)] = move.captured_piece;
            }
            break;
    }
    
    // Update aggregate bitboards
    update_bitboards();
    
    // Remove the move from history
    move_history_.pop_back();
    
    // Update hash key (recompute from current position)
    hash_key_ = ZobristHash::hash_position(*this);
}

// Undo multiple moves
void Position::undo_moves(int count)
{
    for (int i = 0; i < count && !move_history_.empty(); ++i)
    {
        undo_move();
    }
}
