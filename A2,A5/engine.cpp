#include <iomanip>
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <set>
#include <cassert>
#include <climits>
#include "board.hpp"
#include "engine.hpp"
#include "butils.hpp"
#include "constants.hpp"

using namespace std;
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
unsigned long long rand(unsigned long long a, unsigned long long b) {
    return uniform_int_distribution<unsigned long long>(a,b)(rng);
}
bool Engine::search() {
    long long end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); 
    if (end-start > time_give) {
        return false;
    }
    return true;
}




int pawn_eval[7][7] = {
    {0,0,0,0,0,0,0}
    ,{0,50,100,150,200,250,0}
    ,{0,0,0,0,0,300,0}
    ,{0,350,0,0,0,350,0}
    ,{0,300,0,0,0,0,0}
    ,{0,250,200,150,100,50,-100}
    ,{0,0,0,0,0,0,0}
};

int pawn_eval_8_4[8][8] = {
    {0,0,0,0,0,0,0,0}
    ,{0,50,100,150,200,250,300,0}
    ,{0,0,0,0,0,0,350,0}
    ,{0,450,0,0,0,0,400,0}
    ,{0,400,0,0,0,0,450,0}
    ,{0,350,0,0,0,0,0,0}
    ,{0,300,250,200,150,100,50,0}
    ,{0,0,0,0,0,0,0,0}
};

int pawn_eval_8_2[8][8] ={
    {0,0,0,0,0,0,0,0}
    ,{0,50,100,150,200,250,300,0}
    ,{0,0,0,150,200,250,350,0}
    ,{0,450,0,0,0,300,400,0}
    ,{0,400,0,0,0,350,450,0}
    ,{0,350,0,0,0,0,0,0}
    ,{0,300,250,200,150,100,50,0}
    ,{0,0,0,0,0,0,0,0}
};


int king_eval[7][7] = {
    {0,0,0,0,0,0,0}
    ,{0,100,5,5,5,100,0}
    ,{0,5,0,0,0,5,0}
    ,{0,5,0,0,0,5,0}
    ,{0,5,0,0,0,5,0}
    ,{0,100,5,5,5,100,0}
    ,{0,0,0,0,0,0,0}
};

int rook_eval[7][7] = {
    {30,30,30,30,30,30,30}
    ,{30,0,0,0,0,0,30}
    ,{30,0,0,0,0,0,30}
    ,{30,0,0,0,0,0,30}
    ,{30,0,0,0,0,0,30}
    ,{30,0,0,0,0,0,30}
    ,{30,30,30,30,30,30,30}
};

int rook_eval_8_4[8][8] = {
    {150,150,150,150,150,150,150,150}
    ,{150,30,30,30,30,30,30,150}
    ,{150,30,0,0,0,0,30,150}
    ,{150,30,0,0,0,0,30,150}
    ,{150,30,0,0,0,0,30,150}
    ,{150,30,0,0,0,0,30,150}
    ,{150,30,30,30,30,30,30,150}
    ,{150,150,150,150,150,150,150,150}
};

int rook_eval_8_2[8][8] = {
    {30,30,30,30,30,30,30,30}
    ,{30,30,30,30,30,30,30,30}
    ,{30,30,0,0,0,0,30,30}
    ,{30,30,0,0,0,0,30,30}
    ,{30,30,0,0,0,0,30,30}
    ,{30,30,0,0,0,0,30,30}
    ,{30,30,30,30,30,30,30,30}
    ,{30,30,30,30,30,30,30,30}
};

int bishop_eval[7][7] = {
    {0,0,0,150,0,0,0}
    ,{0,50,150,50,150,50,0}
    ,{0,150,50,0,0,150,0}
    ,{150,150,0,0,0,50,150}
    ,{0,150,0,0,0,150,0}
    ,{0,50,150,50,150,50,0}
    ,{0,0,0,150,0,0,0}
};

int bishop_eval_8_4[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,50,50,50,50,50,50,0},
    {0,50,0,0,0,0,50,0},
    {150,50,0,0,0,0,50,0},
    {0,50,0,0,0,0,50,0},
    {0,50,0,0,0,0,50,0},
    {0,50,50,50,50,50,50,0},
    {0,0,0,0,0,0,0,0}
};


int bishop_eval_8_2[8][8] = {
    {0,0,0,0,0,0,0,0}
    ,{0,50,50,50,50,50,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,50,50,50,50,50,0}
    ,{0,0,0,0,0,0,0,0}
};

int knight_eval[8][8] = {
    {0,0,0,0,0,0,0,0}
    ,{0,50,50,50,50,50,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,0,0,0,0,50,0}
    ,{0,50,50,50,50,50,50,0}
    ,{0,0,0,0,0,0,0,0}
};


U16* transform_moves(U16* moves_start, U16* moves_end, U16* next_moves, const U8 *transform) {

    for (U16* m = moves_start; m != moves_end; m++) {
        *next_moves = move_promo(transform[getp0(*m)], transform[getp1(*m)], getpromo(*m));
        next_moves++;
    }
    return next_moves;
}
U16* construct_rook_moves(const U8 p0, const U8 *board, const U8 *bmask, U16* next_moves) {
    // U16* start = next_moves;
    PlayerColor color = color(board[p0]);
    PlayerColor oppcolor = oppcolor(board[p0]);

    // right - move one square 
    if (inboard(bmask, getx(p0)+1, gety(p0)) && 
        !occupied(board, p0+pos(1,0), color)) {
            *next_moves = move(p0, p0+pos(1,0)); 
            next_moves++;
        }

    // bottom - move one square 
    if (inboard(bmask, getx(p0), gety(p0)-1) && 
        !occupied(board, p0-pos(0,1), color)) {
            *next_moves = move(p0, p0-pos(0,1));
            next_moves++;
        }

    // top - move multiple if left end (forward), move one if right end
    if (inboard(bmask, getx(p0), gety(p0)+1)) {
        if (getx(p0) >= 4 && !occupied(board, p0+pos(0,1), color)) {
            // right end 
            *next_moves = move(p0, p0+pos(0,1));
            next_moves++;
        }
        else {
            for (int y=1; inboard(bmask, getx(p0), gety(p0)+y); y++) {
                U8 tgt_pos = p0+pos(0,y);
                if (occupied(board, tgt_pos, color)) break;
                *next_moves = move(p0, tgt_pos);
                next_moves++;
                if (occupied(board, tgt_pos, oppcolor)) break;
            }
        }
    }

    // left
    bool blocked = false;
    for (int x=1; inboard(bmask, getx(p0)-x, gety(p0)); x++) {
        U8 tgt_pos = p0-pos(x,0);
        if (occupied(board, tgt_pos, color)) { blocked = true; break; }
        *next_moves = move(p0, tgt_pos);
        next_moves++;
        if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
    }

    // reflect if on outer ring and not blocked yet
    if (!blocked && gety(p0) == 0) {
        for (int y=1; inboard(bmask, 0, y); y++) {
            U8 tgt_pos = pos(0,y);
            if (occupied(board, tgt_pos, color)) break;
            *next_moves = move(p0, tgt_pos);
            next_moves++;
            if (occupied(board, tgt_pos, oppcolor)) break;
        }
    }
    // cout << "Rook moves: " << next_moves-start << endl;
    // for (U16* m = start; m != next_moves; m++) {
    //     cout << move_to_str(*m) << " ";
    // }
    // cout << endl;
    return next_moves;
}

U16* construct_bishop_moves(const U8 p0, const U8 *board, const U8 *bmask, U16* next_moves) {
    // U16* start = next_moves;
    PlayerColor color = color(board[p0]);
    PlayerColor oppcolor = oppcolor(board[p0]);

    // top right - move one square 
    if (inboard(bmask, getx(p0)+1, gety(p0)+1) && 
        !occupied(board, p0+pos(1,1), color)) {
            *next_moves = move(p0, p0+pos(1,1));
            next_moves++;
        }

    // bottom right - move one square 
    if (inboard(bmask, getx(p0)+1, gety(p0)-1) && 
        !occupied(board, p0+pos(1,0)-pos(0,1), color)) {
            *next_moves = move(p0, p0+pos(1,0)-pos(0,1));
            next_moves++;
        }

    // top left - move till reflection, then reflect
    bool blocked = false;
    U8 tgt_pos = DEAD;
    for (int s=1; inboard(bmask, getx(p0)-s, gety(p0)+s); s++) {
        tgt_pos = p0-pos(s,0)+pos(0,s);
        if (occupied(board, tgt_pos, color)) { blocked = true; break; }
        *next_moves = move(p0, tgt_pos);
        next_moves++;
        if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
    }

    // reflect on left / top edge
    if (!blocked) {
        // std::cout << "Not blocked, reflecting\n";
        U8 p1 = tgt_pos;
        if (getx(tgt_pos) == 0) {
            // left edge 
            for (int s=1; inboard(bmask, getx(p1)+s, gety(p1)+s); s++) {
                U8 tgt_pos = p1+pos(s,s);
                if (occupied(board, tgt_pos, color)) { blocked = true; break; }
                *next_moves = move(p0, tgt_pos);
                next_moves++;
                if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
            }
        }
        else {
            // top edge
            for (int s=1; inboard(bmask, getx(p1)-s, gety(p1)-s); s++) {
                U8 tgt_pos = p1-pos(s,s);
                if (occupied(board, tgt_pos, color)) { blocked = true; break; }
                *next_moves = move(p0, tgt_pos);
                next_moves++;
                if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
            }
        }
    }

    // bottom left - move till reflection, then reflect
    blocked = false;
    tgt_pos = DEAD;
    for (int s=1; inboard(bmask, getx(p0)-s, gety(p0)-s); s++) {
        tgt_pos = p0-pos(s,s);
        if (occupied(board, tgt_pos, color)) { blocked = true; break; }
        *next_moves = move(p0, tgt_pos);
        next_moves++;
        if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
    }

    // reflect on bottom edge
    if (!blocked) {
        U8 p1 = tgt_pos;
        for (int s=1; inboard(bmask, getx(p1)-s, gety(p1)+s); s++) {
            tgt_pos = p1-pos(s,0)+pos(0,s);
            if (occupied(board, tgt_pos, color)) { blocked = true; break; }
            *next_moves = move(p0, tgt_pos);
            next_moves++;
            if (occupied(board, tgt_pos, oppcolor)) { blocked = true; break; }
        }
    }
    //print_moves(start, next_moves);
    // cout << "Bishop moves: " << next_moves-start << endl;
    // for (U16* m = start; m != next_moves; m++) {
    //     cout << move_to_str(*m) << " ";
    // }
    // cout << endl;
    return next_moves;
}

U16* construct_knight_moves(const U8 p0, const U8 *board, const U8 *bmask, U16* next_moves) {

    PlayerColor color = color(board[p0]);

    // similar to king moves
    static int x_incrs[8] = {1, 2,  2,  1, -1, -2, -2, -1};
    static int y_incrs[8] = {2, 1, -1, -2, -2, -1,  1,  2};

    for (int i=0; i<8; i++) {
        if (!inboard(bmask, getx(p0)+x_incrs[i], gety(p0)+y_incrs[i])) continue;
        U8 p1 = pos(getx(p0)+x_incrs[i], gety(p0)+y_incrs[i]);
        if (!occupied(board, p1, color)) {
            *next_moves = move(p0, p1);
            next_moves++;
        }
    }
    return next_moves;
}

bool can_promote_engine(U8 pos, U8 *promo, int n_promo) {
    for (int i=0; i<n_promo; i++) {
        if (promo[i] == pos) return true;
    }
    return false;
}

U16* construct_pawn_moves(const U8 p0, const U8 *board, const U8 *bmask,
        U8 *promo, int n_promo, bool promote, U16* next_moves) {
    
    PlayerColor color = color(board[p0]);
    // cout << "Constructing pawn moves for " << (color==WHITE?"WHITE":"BLACK") << endl;
    // U16* start = next_moves;
    for (int y = gety(p0)-1; y <= gety(p0) + 1; y++) {
        if (!inboard(bmask, getx(p0)-1, y)) continue;
        U8 p1 = pos(getx(p0)-1, y);
        if (!occupied(board, p1, color)) {
            if (promote && can_promote_engine(p1, promo, n_promo)) {
                *next_moves = move_promo(p0, p1, PAWN_ROOK);
                next_moves++;
                *next_moves = move_promo(p0, p1, PAWN_BISHOP);
                next_moves++;
            }
            else {
                *next_moves = move(p0, p1);
                next_moves++;
            }
        }
    }
    //print_moves(start, next_moves);
    // cout << "Pawn moves: " << next_moves-start << endl;
    // for (U16* m = start; m != next_moves; m++) {
    //     cout << move_to_str(*m) << " ";
    // }
    // cout << endl;
    return next_moves;
}

U16* construct_king_moves(const U8 p0, const U8 *board, const U8 *bmask, U16* next_moves) {

    PlayerColor color = color(board[p0]);
    std::unordered_set<U16> king_moves;

    static int x_incrs[8] = {1, 1,  1, 0,  0, -1, -1, -1};
    static int y_incrs[8] = {1, 0, -1, 1, -1,  1,  0, -1};

    for (int i=0; i<8; i++) {
        if (!inboard(bmask, getx(p0)+x_incrs[i], gety(p0)+y_incrs[i])) continue;
        U8 p1 = pos(getx(p0)+x_incrs[i], gety(p0)+y_incrs[i]);
        if (!occupied(board, p1, color)) {
            *next_moves = move(p0, p1);
            next_moves++;
        }
    }

    return next_moves;
}

U16* get_pseudolegal_moves_for_piece(U8 piece_pos, U16* next_moves, BoardData &data)   {
    U16* moves = next_moves;
    next_moves += 125;
    U8 piece_id = data.board_0[piece_pos];
    int board_idx = data.board_mask[piece_pos] - 2;
    const U8 *transform_arr = data.transform_array[board_idx];
    const U8 *inv_transform_arr = data.inverse_transform_array[board_idx];

    const U8 *board = data.board_0;
    if (board_idx == 1) board = data.board_270;
    if (board_idx == 2) board = data.board_180;
    if (board_idx == 3) board = data.board_90;

    if (piece_id & PAWN) {
        next_moves = construct_pawn_moves(inv_transform_arr[piece_pos], board, data.board_mask,
                (U8*)data.pawn_promo_squares, data.n_pawn_promo_squares,
                (board_idx==2 && (piece_id & WHITE)) || (board_idx==0 && (piece_id & BLACK)), next_moves);
    }
    else if (piece_id & ROOK) {
        next_moves = construct_rook_moves(inv_transform_arr[piece_pos], board, data.board_mask, next_moves);
    }
    else if (piece_id & BISHOP) {
        next_moves = construct_bishop_moves(inv_transform_arr[piece_pos], board, data.board_mask, next_moves);
    }
    else if (piece_id & KING) {
        next_moves = construct_king_moves(inv_transform_arr[piece_pos], board, data.board_mask, next_moves);
    }
    else if (piece_id & KNIGHT) {
        next_moves = construct_knight_moves(inv_transform_arr[piece_pos], board, data.board_mask, next_moves);
    }

    next_moves = transform_moves(moves+125, next_moves, moves, transform_arr);
    return next_moves;
}


U16* get_pseudolegal_moves_for_side(U8 color, U16* next_moves, BoardData &data)  {


    int si = (color>>7) * 10;

    U8 *pieces = (U8*)(&data);
    for (int i=0; i<data.n_pieces; i++) {
        U8 piece = pieces[si+i];
        if (piece == DEAD) continue;
        next_moves = get_pseudolegal_moves_for_piece(piece, next_moves, data);
    }

    return next_moves;
}


U16* get_pseudolegal_moves(U16* next_moves, BoardData &data)   {
    return get_pseudolegal_moves_for_side(data.player_to_play, next_moves, data);
}

U16* pseudo_moves= new U16[20000];
unsigned long long zobrist_table[64][64];
unsigned long long zobrist_player;
pair<pair<unsigned long long, int>, pair<int,pair<U16,int> > > trans_table[1<<18];
int mask = (1<<18)-1;
void update_tt(unsigned long long hash, int depth, int mno, U16 move, int eval) {
    auto val = trans_table[hash&mask];
    if (hash == val.first.first) {
        if (depth >= val.first.second)
            trans_table[hash&mask] = make_pair(make_pair(hash,depth),make_pair(mno,make_pair(move,eval)));
    }
    else {
        if ((val.first.second-depth)/2 + mno-val.second.first)
            trans_table[hash&mask] = make_pair(make_pair(hash,depth),make_pair(mno,make_pair(move,eval)));
    }
}

pair<U16,int> get_tt_move(unsigned long long hash, int depth) {
    auto val = trans_table[hash&mask];
    if (val.first.first == hash) {
        if (val.first.second >= depth)
            return val.second.second;
        else
            return make_pair(val.second.second.first,INT_MAX);
    }
    return make_pair(0,INT_MAX);
}

void init_zobrist() {
    for (int i=0;i<64;i++) {
        for (int j=0;j<64;j++) {
            zobrist_table[i][j] = rand(0,ULLONG_MAX);
        }
    }
    zobrist_player = rand(0,ULLONG_MAX);
}

unsigned long long eval_hash(Board &b) {
    long long hash = 0;
    U8* pieces = (U8*)&b;
    for (int i=0;i<2*b.data.n_pieces;i++) {
        if (pieces[i] != DEAD) {
            hash ^= zobrist_table[pieces[i]][b.data.board_0[pieces[i]]&63];
        }
    }
    if (b.data.player_to_play == BLACK) {
        hash ^= zobrist_player;
    }
    return hash;
}

int mvv_lva[64][64];
U16 killer_moves[50][2];
int nodes =1;
int moves =1;



struct MoveStore {
    int last_killed_piece, last_killed_piece_idx, ind0, ind1, piece0, piece1;
};


unsigned long long do_move(U16 move, Board &b, int* piece_ind, MoveStore &m, unsigned long long hash) {
    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    U8 promo = getpromo(move);
    unsigned long long new_hash = hash^zobrist_table[p0][b.data.board_0[p0]&63]^zobrist_table[p1][b.data.board_0[p1]&63];

    U8 piecetype = b.data.board_0[p0];
    m.piece0 = piecetype;
    m.piece1 = b.data.board_0[p1];
    m.last_killed_piece = b.data.last_killed_piece;
    m.last_killed_piece_idx = b.data.last_killed_piece_idx;
    b.data.last_killed_piece = 0;
    b.data.last_killed_piece_idx = -1;

    // scan and get piece from coord
    U8 *pieces = (U8*)&b.data;
    int ind0 = piece_ind[p0];
    int ind1 = piece_ind[p1];
    m.ind0 = ind0;
    m.ind1 = ind1;
    pieces[ind0] = p1;
    piece_ind[p1] = ind0;
    piece_ind[p0] = 2*b.data.n_pieces;
    if (ind1 != 2*b.data.n_pieces) {
        pieces[ind1] = DEAD;
        b.data.last_killed_piece = b.data.board_0[p1];
        b.data.last_killed_piece_idx = ind1;
    } 

    if (promo == PAWN_ROOK) {
        piecetype = (piecetype & (WHITE | BLACK)) | ROOK;
    }
    else if (promo == PAWN_BISHOP) {
        piecetype = (piecetype & (WHITE | BLACK)) | BISHOP;
    }
    b.data.board_0  [b.data.transform_array[0][p1]] = piecetype;
    b.data.board_90 [b.data.transform_array[1][p1]] = piecetype;
    b.data.board_180[b.data.transform_array[2][p1]] = piecetype;
    b.data.board_270[b.data.transform_array[3][p1]] = piecetype;

    b.data.board_0  [b.data.transform_array[0][p0]] = 0;
    b.data.board_90 [b.data.transform_array[1][p0]] = 0;
    b.data.board_180[b.data.transform_array[2][p0]] = 0;
    b.data.board_270[b.data.transform_array[3][p0]] = 0;
    new_hash ^= zobrist_table[p0][b.data.board_0[p0]&63]^zobrist_table[p1][b.data.board_0[p1]&63];
    new_hash ^= zobrist_player;

    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
    return new_hash;

}

void undo_move(U16 move, Board &b, int* piece_ind,MoveStore &m) {
    U8 p0 = getp0(move);
    U8 p1 = getp1(move);
    b.data.last_killed_piece = m.last_killed_piece;
    b.data.last_killed_piece_idx = m.last_killed_piece_idx;
    U8 *pieces = (U8*)&b.data;
    pieces[m.ind0] = p0;
    piece_ind[p0] = m.ind0;
    piece_ind[p1] = m.ind1;
    if (m.ind1 != 2*b.data.n_pieces)
        pieces[m.ind1] = p1;


    b.data.board_0  [b.data.transform_array[0][p0]] = m.piece0;
    b.data.board_90 [b.data.transform_array[1][p0]] = m.piece0;
    b.data.board_180[b.data.transform_array[2][p0]] = m.piece0;
    b.data.board_270[b.data.transform_array[3][p0]] = m.piece0;

    b.data.board_0  [b.data.transform_array[0][p1]] = m.piece1;
    b.data.board_90 [b.data.transform_array[1][p1]] = m.piece1;
    b.data.board_180[b.data.transform_array[2][p1]] = m.piece1;
    b.data.board_270[b.data.transform_array[3][p1]] = m.piece1;

    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
}


int pawn_coupling(Board &b){
    int score = 0;
    //count the number of pawns on both sides
    int w_pawns = 0;
    int b_pawns = 0;
    w_pawns += (b.data.w_pawn_1 != DEAD);
    w_pawns += (b.data.w_pawn_2 != DEAD);
    b_pawns += (b.data.b_pawn_1 != DEAD);
    b_pawns += (b.data.b_pawn_2 != DEAD);
    //if there are more than 2 pawns on both sides, then check for coupling
    if(w_pawns>1){
        U8 x_pawn_1 = getx(b.data.w_pawn_1);
        U8 y_pawn_1 = gety(b.data.w_pawn_1);
        U8 x_pawn_2 = getx(b.data.w_pawn_2);
        U8 y_pawn_2 = gety(b.data.w_pawn_2);
        score -= abs(x_pawn_1-x_pawn_2);
    }
    if(b_pawns>1){
        U8 x_pawn_1 = getx(b.data.b_pawn_1);
        U8 y_pawn_1 = gety(b.data.b_pawn_1);
        U8 x_pawn_2 = getx(b.data.b_pawn_2);
        U8 y_pawn_2 = gety(b.data.b_pawn_2);
        score += abs(x_pawn_1-x_pawn_2);
    }
    return score;
}


U16* mobility = new U16[800];

int mobility_in_check(Board &b, int mobility_weight, int check_weight)
{
    int score = 0;
    // get all pseudo legal moves for the player to play
    U16* moves_start = mobility;
    U16* moves_end = get_pseudolegal_moves(moves_start, b.data);
    // get all pseudo legal moves for the opponent
    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));
    U16* opp_moves_start = mobility+400;
    U16* opp_moves_end = get_pseudolegal_moves(opp_moves_start, b.data);
    b.data.player_to_play = (PlayerColor)(b.data.player_to_play ^ (WHITE | BLACK));

    score += mobility_weight*((moves_end-moves_start)-(opp_moves_end-opp_moves_start));
    
    // check if the king is in check
    for (int i = 0; i < moves_end-moves_start; i++) {
        if ((b.data.board_0[getp1(moves_start[i])]&63) == KING) {
            return 1000000;
        }
    }
    for (int i = 0; i < opp_moves_end-opp_moves_start; i++) {
        if ((b.data.board_0[getp1(opp_moves_start[i])]&63) == KING) {
            score -= check_weight;
        }
    }
    
    return score;
}   
int evaluation_function_7_3(Board &b)
{
    // implement this
    static int piece_weights[3] = {1000, 9000, 5400};
    int white_score = 0;
    
    white_score += (b.data.w_rook_1 != DEAD) * (piece_weights[1] + rook_eval[getx(b.data.w_rook_1)][gety(b.data.w_rook_1)]*2);
    white_score += (b.data.w_rook_2 != DEAD) * (piece_weights[1] + rook_eval[getx(b.data.w_rook_2)][gety(b.data.w_rook_2)]*2);
    white_score += (b.data.w_bishop != DEAD) * (piece_weights[2] + bishop_eval[getx(b.data.w_bishop)][gety(b.data.w_bishop)]*2);
    // if (b.data.w_rook_1)
    // white_score -= 
    if (b.data.w_pawn_1 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_1] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] +  pawn_eval[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_1)+gety(b.data.w_pawn_1))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)]*2;
            break;
        case BISHOP:
            white_score += piece_weights[2] +  bishop_eval[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)]*2;
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    if (b.data.w_pawn_2 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_2] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_2)+gety(b.data.w_pawn_2))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)]*2;
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)]*2;
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    int black_score = 0;
    black_score += (b.data.b_rook_1 != DEAD) * (piece_weights[1] + rook_eval[getx(b.data.b_rook_1)][gety(b.data.b_rook_1)]*2);
    black_score += (b.data.b_rook_2 != DEAD) * (piece_weights[1] + rook_eval[getx(b.data.b_rook_2)][gety(b.data.b_rook_2)]*2);
    black_score += (b.data.b_bishop != DEAD) * (piece_weights[2] + bishop_eval[getx(b.data.b_bishop)][gety(b.data.b_bishop)]*2);
    if (b.data.b_pawn_1 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_1] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] +  pawn_eval[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)]*4;
            // black_score += piece_weights[0] + (12-getx(b.data.b_pawn_1)-gety(b.data.b_pawn_1))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)]*2;
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)]*2;
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_2 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_2] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)]*4;
            // black_score += piece_weights[0] + (12-getx(b.data.b_pawn_2)-gety(b.data.b_pawn_2))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)]*2;
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)]*2;
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    int score = white_score - black_score;
    score += pawn_coupling(b)*5;


    if(b.data.player_to_play == BLACK) {
        score = -score;
    }
    int mob = mobility_in_check(b, 10, 1000);
    if (mob>=1000000) return mob;
    score += mob;
    return score;
}


int evaluation_function_8_4(Board &b)
{
    // implement this
    static int piece_weights[3] = {1000, 9000, 5400};
    int white_score = 0;
    
    white_score += (b.data.w_rook_1 != DEAD) * (piece_weights[1] + rook_eval_8_4[getx(b.data.w_rook_1)][gety(b.data.w_rook_1)]);
    white_score += (b.data.w_rook_2 != DEAD) * (piece_weights[1] + rook_eval_8_4[getx(b.data.w_rook_2)][gety(b.data.w_rook_2)]);
    white_score += (b.data.w_bishop != DEAD) * (piece_weights[2] + bishop_eval[getx(b.data.w_bishop)][gety(b.data.w_bishop)]);
    // if (b.data.w_rook_1)
    // white_score -= 
    if (b.data.w_pawn_1 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_1] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] +  pawn_eval_8_4[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_1)+gety(b.data.w_pawn_1))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_4[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)];
            break;
        case BISHOP:
            white_score += piece_weights[2] +  bishop_eval[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    if (b.data.w_pawn_2 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_2] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval_8_4[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_2)+gety(b.data.w_pawn_2))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_4[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)];
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    if(b.data.w_pawn_3!=DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_3] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval_8_4[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_3)+gety(b.data.w_pawn_3))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_4[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)];
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    if(b.data.w_pawn_4 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_4] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval_8_4[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_4)+gety(b.data.w_pawn_4))*40
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_4[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)];
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    int black_score = 0;
    black_score += (b.data.b_rook_1 != DEAD) * (piece_weights[1] + rook_eval_8_4[getx(b.data.b_rook_1)][gety(b.data.b_rook_1)]);
    black_score += (b.data.b_rook_2 != DEAD) * (piece_weights[1] + rook_eval_8_4[getx(b.data.b_rook_2)][gety(b.data.b_rook_2)]);
    black_score += (b.data.b_bishop != DEAD) * (piece_weights[2] + bishop_eval[getx(b.data.b_bishop)][gety(b.data.b_bishop)]);
    if (b.data.b_pawn_1 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_1] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] +  pawn_eval_8_4[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)]*4;
            // black_score += piece_weights[0] + (12-getx(b.data.b_pawn_1)-gety(b.data.b_pawn_1))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_4[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_2 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_2] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval_8_4[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)]*4;
            // black_score += piece_weights[0] + (12-getx(b.data.b_pawn_2)-gety(b.data.b_pawn_2))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_4[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_3 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_3] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval_8_4[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)]*4;
            // black_score += piece_weights[0] + (12-getx(b.data.b_pawn_3)-gety(b.data.b_pawn_3))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_4[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_4 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_4] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval_8_4[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)]*4;
            // black_score += piece_weights[0] + (12-getx(b.data.b_pawn_4)-gety(b.data.b_pawn_4))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_4[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }



    int score = white_score - black_score;
    score += pawn_coupling(b)*5;

    if(b.data.player_to_play == BLACK) {
        score = -score;
    }
    int mob = mobility_in_check(b, 10, 1000);
    if (mob>=1000000) return mob;
    score += mob;
    return score;
}

int evaluation_function_8_2(Board &b)
{
    // implement this
    static int piece_weights[4] = {1800, 8000, 8100, 3200};
    int white_score = 0;
    white_score += (b.data.w_rook_1 != DEAD) * (piece_weights[1] + rook_eval_8_2[getx(b.data.w_rook_1)][gety(b.data.w_rook_1)]);
    white_score += (b.data.w_rook_2 != DEAD) * (piece_weights[1] + rook_eval_8_2[getx(b.data.w_rook_2)][gety(b.data.w_rook_2)]);
    white_score += (b.data.w_bishop != DEAD) * (piece_weights[2] + bishop_eval_8_2[getx(b.data.w_bishop)][gety(b.data.w_bishop)]);
    white_score += (b.data.w_knight_1 != DEAD) * (piece_weights[3] + knight_eval[getx(b.data.w_knight_1)][gety(b.data.w_knight_1)]);
    white_score += (b.data.w_knight_2 != DEAD) * (piece_weights[3] + knight_eval[getx(b.data.w_knight_2)][gety(b.data.w_knight_2)]);
    // if (b.data.w_rook_1)
    // white_score -= 
    if (b.data.w_pawn_1 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_1] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] +  pawn_eval_8_2[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_1)+gety(b.data.w_pawn_1))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_2[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)];
            break;
        case BISHOP:
            white_score += piece_weights[2] +  bishop_eval_8_2[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)];
            break;
        case KNIGHT:
            white_score += piece_weights[3] +  knight_eval[getx(b.data.w_pawn_1)][gety(b.data.w_pawn_1)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    if (b.data.w_pawn_2 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_2] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval_8_2[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_2)+gety(b.data.w_pawn_2))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_2[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)];
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)];
            break;
        case KNIGHT:
            white_score += piece_weights[3] + knight_eval[getx(b.data.w_pawn_2)][gety(b.data.w_pawn_2)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }
    if (b.data.w_pawn_3 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_3] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval_8_2[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_3)+gety(b.data.w_pawn_3))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_2[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)];
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)];
            break;
        case KNIGHT:
            white_score += piece_weights[3] + knight_eval[getx(b.data.w_pawn_3)][gety(b.data.w_pawn_3)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }

    if (b.data.w_pawn_4 != DEAD)
    {
        switch (b.data.board_0[b.data.w_pawn_4] & 63)
        {
        case PAWN:
            white_score += piece_weights[0] + pawn_eval_8_2[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)]*4;
            // white_score += piece_weights[0] + (getx(b.data.w_pawn_4)+gety(b.data.w_pawn_4))*40;
            break;
        case ROOK:
            white_score += piece_weights[1] + rook_eval_8_2[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)];
            break;
        case BISHOP:
            white_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)];
            break;
        case KNIGHT:
            white_score += piece_weights[3] + knight_eval[getx(b.data.w_pawn_4)][gety(b.data.w_pawn_4)];
            break;
        case KING:
            white_score += piece_weights[2];
            break;
        }
    }

    int black_score = 0;
    black_score += (b.data.b_rook_1 != DEAD) * (piece_weights[1] + rook_eval_8_2[getx(b.data.b_rook_1)][gety(b.data.b_rook_1)]);
    black_score += (b.data.b_rook_2 != DEAD) * (piece_weights[1] + rook_eval_8_2[getx(b.data.b_rook_2)][gety(b.data.b_rook_2)]);
    black_score += (b.data.b_bishop != DEAD) * (piece_weights[2] + bishop_eval[getx(b.data.b_bishop)][gety(b.data.b_bishop)]);
    black_score += (b.data.b_knight_1 != DEAD) * (piece_weights[3] + knight_eval[getx(b.data.b_knight_1)][gety(b.data.b_knight_1)]);
    black_score += (b.data.b_knight_2 != DEAD) * (piece_weights[3] + knight_eval[getx(b.data.b_knight_2)][gety(b.data.b_knight_2)]);
    if (b.data.b_pawn_1 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_1] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] +  pawn_eval_8_2[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)]*4;
            // black_score += piece_weights[0] + (16-getx(b.data.b_pawn_1)-gety(b.data.b_pawn_1))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_2[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)];
            break;
        case KNIGHT:
            black_score += piece_weights[3] + knight_eval[getx(b.data.b_pawn_1)][gety(b.data.b_pawn_1)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_2 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_2] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval_8_2[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)]*4;
            // black_score += piece_weights[0] + (16-getx(b.data.b_pawn_2)-gety(b.data.b_pawn_2))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_2[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)];
            break;
        case KNIGHT:
            black_score += piece_weights[3] + knight_eval[getx(b.data.b_pawn_2)][gety(b.data.b_pawn_2)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_3 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_3] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval_8_2[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)]*4;
            // black_score += piece_weights[0] + (16-getx(b.data.b_pawn_3)-gety(b.data.b_pawn_3))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_2[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)];
            break;
        case KNIGHT:
            black_score += piece_weights[3] + knight_eval[getx(b.data.b_pawn_3)][gety(b.data.b_pawn_3)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }
    if (b.data.b_pawn_4 != DEAD)
    {
        switch (b.data.board_0[b.data.b_pawn_4] & 63)
        {
        case PAWN:
            black_score += piece_weights[0] + pawn_eval_8_2[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)]*4;
            // black_score += piece_weights[0] + (16-getx(b.data.b_pawn_4)-gety(b.data.b_pawn_4))*40;
            break;
        case ROOK:
            black_score += piece_weights[1] + rook_eval_8_2[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)];
            break;
        case BISHOP:
            black_score += piece_weights[2] + bishop_eval_8_2[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)];
            break;
        case KNIGHT:
            black_score += piece_weights[3] + knight_eval[getx(b.data.b_pawn_4)][gety(b.data.b_pawn_4)];
            break;
        case KING:
            black_score += piece_weights[2];
            break;
        }
    }


    int score = white_score - black_score;
    score += pawn_coupling(b)*5;
    if(b.data.player_to_play == BLACK) {
        score = -score;
    }
    int mob = mobility_in_check(b, 10, 1000);
    if (mob>=1000000) return mob;
    score += mob;
    return score;
}
int evaluation_function(Board &b){
    if(b.data.board_type==SEVEN_THREE){
        return evaluation_function_7_3(b);
    }
    else if(b.data.board_type==EIGHT_FOUR){
        return evaluation_function_8_4(b);
    }
    else{
        return evaluation_function_8_2(b);
    }
}

void update_killer_moves(U16 move, int ply) {
    killer_moves[ply][1] = killer_moves[ply][0];
    killer_moves[ply][0] = move;
}



int Engine::minimax(Board &b, int depth,int ply, int alpha, int beta, unsigned long long hash, int* piece_ind,bool null_move=false) {
    if (!search()) {
        return 1000000;
    }
    nodes++;
    if (depth == 0) { 
        return evaluation_function(b);
    }
    U16* moves_start = pseudo_moves+ply*400;
    U16* moves_end = get_pseudolegal_moves(moves_start,b.data);
    if (moves_end == moves_start) {
        if (b.in_check()) {
            return -1000000;
        }
        else {
            return -evaluation_function(b);
        }
    }
    if (depth>=3 && !null_move) {
        if (b.data.player_to_play == WHITE) {
            // Board* b_copy = new Board(b);
            b.data.player_to_play = BLACK;
            int score;
            if ((score = -minimax(b,depth-3,ply+1,-beta,-beta+1,hash^zobrist_player,piece_ind,true )) >= beta) {
                b.data.player_to_play = WHITE;
                // delete b_copy;
                return score;
            }
            b.data.player_to_play = WHITE;
            // delete b_copy;
        }
        else if (b.data.player_to_play == BLACK) {
            b.data.player_to_play = WHITE;
            int score;
            if ((score = -minimax(b,depth-3,ply+1,-beta,-beta+1, hash^zobrist_player,piece_ind,true )) >= beta) {
                // delete b_copy;
                b.data.player_to_play = BLACK;
                return score;
            }
            b.data.player_to_play = BLACK;
            // delete b_copy;
        }
    }
    auto tt_move = get_tt_move(hash,depth);
    int best_score = -1000000;
    static int ini_sizes[7] = {0,5,15,25,35,45,50};
    int sizes[7] = {0,5,15,25,35,45,50};
    U16* ord_moves = moves_start+250;
    for (U16* m = moves_start; m != moves_end; m++) {
        if (b.data.board_0[getp1(*m)] != 0) {
            if ((b.data.board_0[getp1(*m)]&63)==KING) {
                return 1000000;
            }
        }
        if (*m == tt_move.first) {
            if (tt_move.second!=INT_MAX)
                return tt_move.second;
            sizes[0]++;
            ord_moves[0] = *m;
            continue;
        }
        if (b.data.board_0[getp1(*m)] != 0) {
            switch (b.data.board_0[getp1(*m)]&63) {
                case PAWN:
                    ord_moves[sizes[4]] = *m;
                    sizes[4]++;
                    break;
                case ROOK:
                    ord_moves[sizes[1]] = *m;
                    sizes[1]++;
                    break;
                case BISHOP:
                    ord_moves[sizes[2]] = *m;
                    sizes[2]++;
                    break;
                case KNIGHT:
                    ord_moves[sizes[3]] = *m;
                    sizes[3]++;
                    break;
            }
        }
        else {
            if (*m == killer_moves[ply][0] || *m == killer_moves[ply][1]) {
                ord_moves[sizes[5]] = *m;
                sizes[5]++;
            }
            else {
                ord_moves[sizes[6]] = *m;
                sizes[6]++;
            }
        }
    }
    auto best_move = tt_move.first;
    int ct=0;
    for (int i =0; i<7;i++) {
        bool beta_cutoff = false;
        for (int j = ini_sizes[i]; j < sizes[i]; j++) {
            ct++;
            auto m = ord_moves[j];
            MoveStore mstore;
            unsigned long long new_hash = do_move(m,b,piece_ind,mstore,hash);
            int score;
            if (ct>2 && i>=5 && depth>=2) {
                score = -minimax(b, depth-2,ply+1,-alpha-1,-alpha,new_hash,piece_ind);
                if (score > alpha) {
                    score = -minimax(b, depth-1,ply+1,-beta,-alpha,new_hash,piece_ind);
                }
            }
            else {
                score = -minimax(b, depth-1,ply+1,-beta,-alpha,new_hash,piece_ind);
            }
            
            undo_move(m,b,piece_ind,mstore);    
            // delete b_copy;
            if (score > best_score) {
                best_score = score;
                best_move = m;
            }
            alpha = std::max(alpha,score);
            if (alpha >= beta) {
                if (i == 6) {
                    update_killer_moves(m,ply);
                }
                beta_cutoff = true;
                break;
            }
        }
        if (beta_cutoff) {
            break;
        }
    }
    if (!search()) {
        return 1000000;
    }
    if (best_score <= -1000000) {
        if (b.get_legal_moves().size() == 0 && !b.in_check())
            best_score = -evaluation_function(b);
    }
    update_tt(hash,depth,moves,best_move,best_score);
    
    return best_score;
}

void Engine::find_best_move(const Board& b) {
    if (moves == 1) {
        init_zobrist();
        mvv_lva[PAWN][KING] = 10, mvv_lva[PAWN][ROOK] = 11, mvv_lva[PAWN][KNIGHT] = 12, mvv_lva[PAWN][BISHOP] = 13, mvv_lva[PAWN][PAWN] = 14;
        mvv_lva[BISHOP][KING] = 20, mvv_lva[BISHOP][ROOK] = 21, mvv_lva[BISHOP][KNIGHT] = 22, mvv_lva[BISHOP][BISHOP] = 23, mvv_lva[BISHOP][PAWN] = 24;
        mvv_lva[KNIGHT][KING] = 30, mvv_lva[KNIGHT][ROOK] = 31, mvv_lva[KNIGHT][KNIGHT] = 32, mvv_lva[KNIGHT][BISHOP] = 33, mvv_lva[KNIGHT][PAWN] = 34;
        mvv_lva[ROOK][KING] = 40, mvv_lva[ROOK][ROOK] = 41, mvv_lva[ROOK][KNIGHT] = 42, mvv_lva[ROOK][BISHOP] = 43, mvv_lva[ROOK][PAWN] = 44;
        mvv_lva[KING][KING] = 50, mvv_lva[KING][ROOK] = 51, mvv_lva[KING][KNIGHT] = 52, mvv_lva[KING][BISHOP] = 53, mvv_lva[KING][PAWN] = 54;
    }
    time_give = b.data.board_type==SEVEN_THREE ? 2000000 : b.data.board_type==EIGHT_FOUR ? 3000000 : 4000000;
    time_give = min(time_give, (long long)50*time_left.count());
    start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // pick a random move
    moves++;
    nodes =1;
    auto moveset = b.get_legal_moves();
    
    std::vector<std::pair<int,U16> > moveset_vec;
    for (auto m:moveset) {
        moveset_vec.push_back(std::make_pair(rand(0,9),m));
    }
    unsigned long long hash = eval_hash(*(new Board(b)));
    int* piece_ind = new int[64];
    for (int i =0;i<64;i++) {
        piece_ind[i] = 2*b.data.n_pieces;
    }
    U8 *pieces = (U8*)&b.data;
    for (int i=0;i<2*b.data.n_pieces;i++) {
        if (pieces[i] != DEAD) {
            piece_ind[pieces[i]] = i;
        }
    }
    Board* b_copy = new Board(b);
    long long end=start;
    if (moveset.size() == 0) {
        this->best_move = 0;
    }
    else {
        auto tt_move = get_tt_move(hash,2).first;
        this->best_move = moveset_vec[0].second;
        for (int i=0;i < moveset_vec.size();i++) {
            auto m = moveset_vec[i].second;
            if (m == tt_move) {
                this->best_move = m;
                break;
            }
        }
        int best_score = -1000000;
        int depth = 0;
        while(search() && best_score < 1000000 && depth < 30)  {
            bool current_depth_move = false;
            int alpha = -1000000;
            int beta = 1000000;
            //place best moves at the front
            depth+=1;
            for (int i=0;i < moveset_vec.size();i++) {
                auto m = moveset_vec[i].second;
                int move_score = mvv_lva[b.data.board_0[getp1(m)]&63][b.data.board_0[getp0(m)]&63]+5;
                if (m == this->best_move) {
                    move_score = 1000;
                }
                if (move_score==0) {
                    //check if killer move
                    if (m == killer_moves[1][0]) {
                        move_score = 11;
                    }
                    else if (m == killer_moves[1][1]) {
                        move_score = 10;
                    }
                }
                if (move_score < 10)
                    move_score = rand(0,9);
                moveset_vec[i].first = move_score;
                
            }
            std::sort(moveset_vec.begin(), moveset_vec.end(), std::greater<std::pair<int,U16> >());
            for (int i=0;i < moveset_vec.size();i++) {
                auto m = moveset_vec[i].second;
                MoveStore mstore;
                unsigned long long new_hash = do_move(m,*b_copy,piece_ind,mstore, hash);
                int score = -minimax(*b_copy, depth-1,2,-beta,-alpha,new_hash,piece_ind);
                undo_move(m,*b_copy,piece_ind,mstore);
                if(!search()) {
                    break;
                }
                if (score > best_score||!current_depth_move) {
                    best_score = score;
                    current_depth_move = true;
                    this->best_move = m;
                }
                alpha = std::max(alpha,score);
                if (alpha >= beta) {
                    if (moveset_vec[i].first < 15) {
                        update_killer_moves(m,1);
                    }
                    break;
                }
            }
        }

    }
}