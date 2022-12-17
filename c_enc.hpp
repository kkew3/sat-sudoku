#ifndef _C_ENC_H_
#define _C_ENC_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

using Board = std::int64_t *;
using VClause = std::vector<int>;

void c_disp_board(int nsq, Board);
bool check_literals(int, const std::vector<VClause> &);
std::vector<VClause> minimal_encoding(int, int, int, Board);
std::vector<VClause> extended_encoding(int, int, int, Board);
std::vector<VClause> c_minimal_encode(int, Board);
std::vector<VClause> c_extended_encode(int, Board);

class VariableManager
{
   public:
    VariableManager() : index({0}) {}
    std::vector<VClause> convert(const std::vector<VClause> &);
    std::vector<int> back(const std::vector<int> &);

   private:
    std::map<int, int> pool;
    std::vector<int> index;
};

inline void setboard(int n, std::int64_t *board, int i, int j, int v)
{
    board[i * n + j] = static_cast<std::int64_t>(v);
}

inline int getboard(int n, std::int64_t *board, int i, int j)
{
    return static_cast<int>(board[i * n + j]);
}

inline int c_lit2var(int n, int n2, int x, int y, int v)
{
    int var = (x - 1) * n2 + (y - 1) * n + std::abs(v);
    if (v < 0)
    {
        var = -var;
    }
    return var;
}

inline void c_var2lit(int n, int var, int *xp, int *yp, int *vp)
{
    int varabs = std::abs(var);
    *vp = --varabs % n + 1;
    varabs = (varabs - *vp + 1) / n;
    if (var < 0)
    {
        *vp = -*vp;
    }
    *yp = varabs % n + 1;
    varabs = (varabs - *yp + 1) / n;
    *xp = varabs + 1;
}

VClause cell_def(int, int, int, int);
VClause cell_uni(int, int, int, int, int, int);
VClause row_def(int, int, int, int);
VClause row_uni(int, int, int, int, int, int);
VClause col_def(int, int, int, int);
VClause col_uni(int, int, int, int, int, int);
VClause blk_def(int, int, int, int, int, int);
VClause blk_uni1(int, int, int, int, int, int, int, int, int);
VClause blk_uni2(int, int, int, int, int, int, int, int, int, int);
std::vector<VClause> ass(const std::vector<int> &);
void pp_init(int, int, Board, std::vector<int> &);
std::set<int> pp_ns_existing_values(int, int, Board, int, int);
void pp_naked_single(int, int, int, Board, std::vector<int> &);
void pp_hidden_single(int, int, int, Board, std::vector<int> &);
bool clean_refresh_assigned(int, int, Board, std::vector<int> &);
void pp(int, int, int, Board, std::vector<int> &);
void rm_assigned(const std::vector<int> &, VClause &);

#endif
