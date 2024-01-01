#pragma once

#include "engine_base.hpp"
#include <atomic>

class Engine : public AbstractEngine {

    // add extra items here. 
    // Note that your engine will always be instantiated with the default 
    // constructor.
    
    public:
    void find_best_move(const Board& b) override;
    bool search();
    int minimax(Board &b, int depth,int ply, int alpha, int beta, unsigned long long hash, int* piece_ind,bool null_move);
    long long start;
    long long time_give;

};
