/*
    Implementation of transposition table for chess engine.
    Stores position evaluations and best moves for faster search.

    Author: Nicolas Miller
    Date: 08/12/2025
*/

#include "transposition_table.h"
#include "constants.h"
#include <algorithm>
#include <cstring>

namespace luna 
{

TranspositionTable::TranspositionTable(size_t size_mb) : current_age_(0) 
{
    resize(size_mb);
}

void TranspositionTable::resize(size_t size_mb) {
    // Calculate number of entries
    size_t bytes = size_mb * 1024 * 1024;
    table_size_ = bytes / sizeof(TTEntry);
    
    // Resize and clear the table
    table_.resize(table_size_);
    clear();
}

void TranspositionTable::clear() 
{
    std::fill(table_.begin(), table_.end(), TTEntry());
}

void TranspositionTable::store(uint64_t key, int score, int depth, BoundType bound, 
                              const Move& best_move, int ply) 
{
    size_t index = get_index(key);
    TTEntry& entry = table_[index];
    
    // Adjust mate scores for storage
    int store_score = score;
    if (score > MATE_SCORE - MAX_PLY) 
    {
        store_score += ply;
    } 
    else if (score < -MATE_SCORE + MAX_PLY) 
    {
        store_score -= ply;
    }
    
    // Check if we should replace this entry
    if (!entry.is_valid() || should_replace(entry, depth, current_age_)) 
    {
        entry.key = key;
        entry.score = static_cast<int16_t>(store_score);
        entry.depth = static_cast<int16_t>(depth);
        entry.bound_type = static_cast<uint8_t>(bound);
        entry.age = current_age_;
        entry.best_move = best_move;
    }
}

bool TranspositionTable::probe(uint64_t key, int& score, int& depth, BoundType& bound, 
                              Move& best_move, int ply) const 
{
    size_t index = get_index(key);
    const TTEntry& entry = table_[index];
    
    // Check if entry is valid and keys match
    if (!entry.is_valid() || entry.key != key) return false;
    
    // Extract values
    score = entry.score;
    depth = entry.depth;
    bound = static_cast<BoundType>(entry.bound_type);
    best_move = entry.best_move;
    
    // Adjust mate scores for current ply (make them relative to current position)
    if (score > MATE_SCORE - MAX_PLY) 
    {
        score -= ply;
    } 
    else if (score < -MATE_SCORE + MAX_PLY) 
    {
        score += ply;
    }
    
    return true;
}

size_t TranspositionTable::size_mb() const 
{
    return (table_.size() * sizeof(TTEntry)) / (1024 * 1024);
}

size_t TranspositionTable::get_index(uint64_t key) const 
{
    return key % table_size_;
}

bool TranspositionTable::should_replace(const TTEntry& existing, int new_depth, uint8_t new_age) const 
{
    // Always replace invalid entries
    if (!existing.is_valid()) return true;
     
    // Replace if the new entry is from a more recent search
    if (new_age != existing.age) return true;
    
    // Replace if the new entry has greater or equal depth
    return new_depth >= existing.depth;
}

} // namespace luna
