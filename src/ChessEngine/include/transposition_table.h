/*
    Transposition table implementation for chess engine.
    Stores position evaluations and best moves for faster search.

    Author: Nicolas Miller
    Date: 08/12/2025
*/

#ifndef CHESS_ENGINE_TRANSPOSITION_TABLE_H
#define CHESS_ENGINE_TRANSPOSITION_TABLE_H

#include "types.h"
#include <cstdint>
#include <vector>

namespace luna 
{

// Bounds for search algorithm
enum class BoundType : uint8_t 
{
    NONE = 0,
    EXACT = 1,          // Exact score (PV node)
    LOWER_BOUND = 2,    // Beta cutoff (fail-high)
    UPPER_BOUND = 3     // Alpha didn't improve (fail-low)
};

// An entry in the transposition table
struct TTEntry 
{
    uint64_t key;           // Zobrist hash key (verification)
    int16_t score;          // Position evaluation
    int16_t depth;          // Search depth when stored
    uint8_t bound_type;     // BoundType enum value
    uint8_t age;            // Search generation for replacement
    Move best_move;         // Best move from this position
    
    TTEntry() : key(0), score(0), depth(-1), bound_type(0), age(0), best_move() {}
    
    // Check if entry is valid
    bool is_valid() const { return depth >= 0; }
};

class TranspositionTable 
{
public:
    explicit TranspositionTable(size_t size_mb = 64);
    
    ~TranspositionTable() = default;
    
    // Store a position in the hash table
    void store(uint64_t key, int score, int depth, BoundType bound, 
               const Move& best_move, int ply);
    
    // Probe the hash table for a position
    bool probe(uint64_t key, int& score, int& depth, BoundType& bound, 
               Move& best_move, int ply) const;
    
    // Clear the entire hash table
    void clear();
    
    // Start a new search (increment age)
    void new_search() { current_age_++; }
    
    // Resize the hash table
    void resize(size_t size_mb);
    
    // Get current size in MB
    size_t size_mb() const;
    
private:
    std::vector<TTEntry> table_;
    size_t table_size_;     // Table size
    uint8_t current_age_;   // Current search generation
    
    // Get hash table index from key
    size_t get_index(uint64_t key) const;
    
    // Check if we should replace an existing entry
    bool should_replace(const TTEntry& existing, int new_depth, uint8_t new_age) const;
};

} // namespace luna

#endif // CHESS_ENGINE_TRANSPOSITION_TABLE_H
