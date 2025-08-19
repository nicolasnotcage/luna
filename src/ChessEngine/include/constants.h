/*
    Constants and configuration values for the chess engine.
    Centralizes all tunable parameters and limits.

    Author: Nicolas Miller
    Date: 8/13/2025
*/

#ifndef CHESS_ENGINE_CONSTANTS_H
#define CHESS_ENGINE_CONSTANTS_H

namespace luna {

// Search Constants
constexpr int MAX_PLY = 64;                    // Maximum search depth
constexpr int MAX_QUIESCENCE_DEPTH = 8;        // Maximum quiescence search depth
constexpr int INFINITY_SCORE = 1000000;         // Infinity for alpha-beta
constexpr int MATE_SCORE = 100000;              // Base mate score
constexpr int DRAW_SCORE = 0;                   // Draw score

// Time Management
constexpr int DEFAULT_SEARCH_TIME_MS = 5000;   // Default search time in milliseconds
constexpr int MIN_SEARCH_TIME_MS = 100;         // Minimum search time
constexpr int CHECK_FREQUENCY = 2048;           // Check time every N nodes


// Evaluation Constants
constexpr int PAWN_VALUE = 100;                 // Pawn material value
constexpr int KNIGHT_VALUE = 320;               // Knight material value
constexpr int BISHOP_VALUE = 330;               // Bishop material value
constexpr int ROOK_VALUE = 500;                 // Rook material value
constexpr int QUEEN_VALUE = 900;                // Queen material value
constexpr int KING_VALUE = 20000;               // King value (for safety)

// Positional Bonuses/Penalties
constexpr int DOUBLED_PAWN_PENALTY = 15;        // Penalty for doubled pawns
constexpr int ISOLATED_PAWN_PENALTY = 25;       // Penalty for isolated pawns
constexpr int PASSED_PAWN_BONUS = 30;           // Bonus per rank for passed pawns
constexpr int BISHOP_PAIR_BONUS = 50;           // Bonus for having both bishops 
constexpr int ROOK_ON_SEVENTH_BONUS = 30;       // Bonus for rook on 7th rank
constexpr int ROOK_ON_OPEN_FILE_BONUS = 15;     // Bonus for rook on open file
constexpr int KING_PAWN_SHIELD_BONUS = 15;      // Bonus per pawn in king shield
constexpr int CASTLING_RIGHTS_BONUS = 30;       // Bonus for having castling rights
constexpr int MOBILITY_SCORE_MULTIPLIER = 3;    // Multiplier for mobility score
constexpr int CENTER_CONTROL_BONUS = 8;         // Bonus for controlling center squares 

// Move Ordering Constants
constexpr int WINNING_CAPTURE_SCORE = 10000;    // Base score for winning captures
constexpr int PROMOTION_SCORE = 9500;           // Base score for promotions
constexpr int KILLER_MOVE_1_SCORE = 8000;       // Score for first killer move
constexpr int KILLER_MOVE_2_SCORE = 7000;       // Score for second killer move
constexpr int LOSING_CAPTURE_SCORE = -500;      // Score for losing captures

// MVV-LVA (Most Valuable Victim - Least Valuable Attacker) Scores
// These are added to WINNING_CAPTURE_SCORE
constexpr int MVV_LVA_OFFSET[6][6] = {
    // Victim:     P    N    B    R    Q    K
    /* P */ {     105, 205, 305, 405, 505, 605 },
    /* N */ {     104, 204, 304, 404, 504, 604 },
    /* B */ {     103, 203, 303, 403, 503, 603 },
    /* R */ {     102, 202, 302, 402, 502, 602 },
    /* Q */ {     101, 201, 301, 401, 501, 601 },
    /* K */ {     100, 200, 300, 400, 500, 600 }
};

// Search Parameters
constexpr int DEFAULT_SEARCH_DEPTH = 12;        // Default search depth 
constexpr int MAX_SEARCH_DEPTH = 30;            // Maximum allowed search depth
constexpr int ENDGAME_MATERIAL_THRESHOLD = 1800; // Material threshold for endgame

// History Heuristic - Optimized for better move ordering
constexpr int HISTORY_MAX = 4000;               // Maximum history value
constexpr int HISTORY_DIVISOR = 2;              // Divisor when history gets too large

// Transposition Table Constants
constexpr size_t DEFAULT_HASH_SIZE_MB = 64;         // Default hash table size in MB
constexpr size_t MIN_HASH_SIZE_MB = 1;              // Minimum hash table size in MB
constexpr size_t MAX_HASH_SIZE_MB = 1024;           // Maximum hash table size in MB
constexpr int TT_MOVE_SCORE = 15000;                // Score for TT best move (higher than captures)
constexpr int MATE_BOUND = MATE_SCORE - MAX_PLY;    // Mate score boundary for TT storage

// Null Move Pruning (for future implementation)
constexpr int NULL_MOVE_REDUCTION = 3;          // Depth reduction for null move
constexpr int NULL_MOVE_MIN_DEPTH = 2;          // Minimum depth for null move

// Late Move Reductions (for future implementation)
constexpr int LMR_MIN_DEPTH = 2;                // Minimum depth for LMR
constexpr int LMR_MOVE_COUNT_THRESHOLD = 3;     // Apply LMR after N moves

// Debug and Display
constexpr bool DISPLAY_SEARCH_INFO = true;      // Show search information
constexpr int PV_MAX_LENGTH = 20;               // Maximum PV length to display

} // namespace luna

#endif // CHESS_ENGINE_CONSTANTS_H
