#include "c_enc.hpp"

void c_disp_board(int nsq, Board board)
{
    int n = nsq * nsq;
    for (int i = 0; i < n; ++i)
    {
        if (i % nsq == 0)
        {
            std::cout << '\n';
        }
        for (int j = 0; j < n; ++j)
        {
            int v = getboard(n, board, i, j);
            if (j % nsq == 0)
            {
                std::cout << ' ';
            }
            if (v == 0)
            {
                std::cout << "   ";
            }
            else if (v < 10)
            {
                std::cout << ' ' << v << ' ';
            }
            else
            {
                std::cout << v << ' ';
            }
        }
        std::cout << '\n';
    }
}

bool check_literals(int n3, const std::vector<VClause> &clauses)
{
    std::set<int> appeared, expected;
    std::vector<int> missed;
    for (int i = 1; i <= n3; i++)
    {
        expected.insert(i);
    }
    for (const auto &cl : clauses)
    {
        for (int lit : cl)
        {
            appeared.insert(std::abs(lit));
        }
    }
    std::set_difference(
        expected.begin(), expected.end(), appeared.begin(), appeared.end(),
        std::back_inserter(missed));

    return missed.empty();
}

VClause cell_def(int n, int n2, int x, int y)
{
    VClause cl;
    for (int v = 1; v <= n; ++v)
    {
        cl.push_back(c_lit2var(n, n2, x, y, v));
    }
    return cl;
}

VClause cell_uni(int n, int n2, int x, int y, int vi, int vj)
{
    return {c_lit2var(n, n2, x, y, -vi), c_lit2var(n, n2, x, y, -vj)};
}

VClause row_def(int n, int n2, int x, int v)
{
    VClause cl;
    for (int y = 1; y <= n; ++y)
    {
        cl.push_back(c_lit2var(n, n2, x, y, v));
    }
    return cl;
}

VClause row_uni(int n, int n2, int x, int v, int yi, int yj)
{
    return {c_lit2var(n, n2, x, yi, -v), c_lit2var(n, n2, x, yj, -v)};
}

VClause col_def(int n, int n2, int y, int v)
{
    VClause cl;
    for (int x = 1; x <= n; ++x)
    {
        cl.push_back(c_lit2var(n, n2, x, y, v));
    }
    return cl;
}

VClause col_uni(int n, int n2, int y, int v, int xi, int xj)
{
    return {c_lit2var(n, n2, xi, y, -v), c_lit2var(n, n2, xj, y, -v)};
}

VClause blk_def(int nsq, int n, int n2, int xo, int yo, int v)
{
    VClause cl;
    for (int x = 1; x <= nsq; ++x)
    {
        for (int y = 1; y <= nsq; ++y)
        {
            cl.push_back(c_lit2var(n, n2, xo * nsq + x, yo * nsq + y, v));
        }
    }
    return cl;
}

VClause blk_uni1(
    int nsq, int n, int n2, int v, int i, int j, int x, int y, int k)
{
    return {
        c_lit2var(n, n2, i * nsq + x, j * nsq + y, -v),
        c_lit2var(n, n2, i * nsq + x, j * nsq + k, -v)};
}

VClause blk_uni2(
    int nsq, int n, int n2, int v, int i, int j, int x, int y, int k, int l)
{
    return {
        c_lit2var(n, n2, i * nsq + x, j * nsq + y, -v),
        c_lit2var(n, n2, i * nsq + k, j * nsq + l, -v)};
}

std::vector<VClause> ass(const std::vector<int> &assigned)
{
    std::vector<VClause> clauses;
    for (int a : assigned)
    {
        clauses.push_back({a});
    }
    return clauses;
}

void pp_init(int n, int n2, Board board, std::vector<int> &assigned)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            int v = getboard(n, board, i, j);
            if (v != 0)
            {
                assigned.push_back(c_lit2var(n, n2, i + 1, j + 1, v));
            }
        }
    }
}

std::set<int> pp_ns_existing_values(int nsq, int n, Board board, int i, int j)
{
    std::set<int> existing_values;
    // in a row
    for (int jj = 0; jj < n; ++jj)
    {
        int v = getboard(n, board, i, jj);
        if (v != 0)
        {
            existing_values.insert(v);
        }
    }
    // in a column
    for (int ii = 0; ii < n; ++ii)
    {
        int v = getboard(n, board, ii, j);
        if (v != 0)
        {
            existing_values.insert(v);
        }
    }
    // in a block
    int bi = i / nsq, bj = j / nsq;
    for (int ii = bi * nsq; ii < (bi + 1) * nsq; ++ii)
    {
        for (int jj = bj * nsq; jj < (bj + 1) * nsq; ++jj)
        {
            int v = getboard(n, board, ii, jj);
            if (v != 0)
            {
                existing_values.insert(v);
            }
        }
    }
    return existing_values;
}

void pp_naked_single(
    int nsq, int n, int n2, Board board, std::vector<int> &assigned)
{
    std::set<int> all_values;
    for (int v = 1; v <= n; ++v)
    {
        all_values.insert(v);
    }
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (getboard(n, board, i, j) == 0)
            {
                std::vector<int> diff;
                std::set<int> existing_values =
                    pp_ns_existing_values(nsq, n, board, i, j);
                std::set_difference(
                    all_values.begin(), all_values.end(),
                    existing_values.begin(), existing_values.end(),
                    std::back_inserter(diff));
                if (diff.size() == 1)
                {
                    assigned.push_back(
                        c_lit2var(n, n2, i + 1, j + 1, diff.back()));
                    clean_refresh_assigned(n, n2, board, assigned);
                }
            }
        }
    }
}

void pp_hidden_single(
    int nsq, int n, int n2, Board board, std::vector<int> &assigned)
{
    std::set<int> all_values;
    for (int v = 1; v <= n; ++v)
    {
        all_values.insert(v);
    }
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (getboard(n, board, i, j) == 0)
            {
                for (int v = 1; v <= n; ++v)
                {
                    std::vector<int> curr_available;
                    auto curr_existing_values =
                        pp_ns_existing_values(nsq, n, board, i, j);
                    std::set_difference(
                        all_values.begin(), all_values.end(),
                        curr_existing_values.begin(),
                        curr_existing_values.end(),
                        std::back_inserter(curr_available));
                    if (std::find(
                            curr_available.begin(), curr_available.end(), v) ==
                        curr_available.end())
                    {
                        continue;
                    }

                    bool others_unavailable;
                    // search row
                    others_unavailable = true;
                    for (int jj = 0; jj < n; ++jj)
                    {
                        if (jj != j && getboard(n, board, i, jj) == 0)
                        {
                            std::vector<int> available;
                            auto existing_values =
                                pp_ns_existing_values(nsq, n, board, i, jj);
                            std::set_difference(
                                all_values.begin(), all_values.end(),
                                existing_values.begin(), existing_values.end(),
                                std::back_inserter(available));
                            if (std::find(
                                    available.begin(), available.end(), v) !=
                                available.end())
                            {
                                others_unavailable = false;
                                break;
                            }
                        }
                    }
                    if (others_unavailable)
                    {
                        assigned.push_back(c_lit2var(n, n2, i + 1, j + 1, v));
                        clean_refresh_assigned(n, n2, board, assigned);
                        break;
                    }

                    // search column
                    others_unavailable = true;
                    for (int ii = 0; ii < n; ++ii)
                    {
                        if (ii != i && getboard(n, board, ii, j) == 0)
                        {
                            std::vector<int> available;
                            auto existing_values =
                                pp_ns_existing_values(nsq, n, board, ii, j);
                            std::set_difference(
                                all_values.begin(), all_values.end(),
                                existing_values.begin(), existing_values.end(),
                                std::back_inserter(available));
                            if (std::find(
                                    available.begin(), available.end(), v) !=
                                available.end())
                            {
                                others_unavailable = false;
                                break;
                            }
                        }
                    }
                    if (others_unavailable)
                    {
                        assigned.push_back(c_lit2var(n, n2, i + 1, j + 1, v));
                        clean_refresh_assigned(n, n2, board, assigned);
                        break;
                    }

                    // search block
                    others_unavailable = true;
                    int bi = i / nsq, bj = j / nsq;
                    for (int ii = bi * nsq; ii < (bi + 1) * nsq; ++ii)
                    {
                        for (int jj = bj * nsq; jj < (bj + 1) * nsq; ++jj)
                        {
                            if ((ii != i || jj != j) &&
                                getboard(n, board, ii, jj) == 0)
                            {
                                std::vector<int> available;
                                auto existing_values = pp_ns_existing_values(
                                    nsq, n, board, ii, jj);
                                std::set_difference(
                                    all_values.begin(), all_values.end(),
                                    existing_values.begin(),
                                    existing_values.end(),
                                    std::back_inserter(available));
                                if (std::find(
                                        available.begin(), available.end(),
                                        v) != available.end())
                                {
                                    others_unavailable = false;
                                    break;
                                }
                            }
                        }
                        if (!others_unavailable)
                        {
                            break;
                        }
                    }
                    if (others_unavailable)
                    {
                        assigned.push_back(c_lit2var(n, n2, i + 1, j + 1, v));
                        clean_refresh_assigned(n, n2, board, assigned);
                        break;
                    }
                }
            }
        }
    }
}

bool clean_refresh_assigned(
    int n, int n2, Board board, std::vector<int> &assigned)
{
    std::sort(assigned.begin(), assigned.end());
    auto last = std::unique(assigned.begin(), assigned.end());
    assigned.erase(last, assigned.end());
    bool changed = false;
    for (int var : assigned)
    {
        int x, y, v;
        c_var2lit(n, var, &x, &y, &v);
        if (getboard(n, board, x - 1, y - 1) == 0)
        {
            setboard(n, board, x - 1, y - 1, v);
            changed = true;
        }
    }
    return changed;
}

void pp(int nsq, int n, int n2, Board board, std::vector<int> &assigned)
{
    pp_init(n, n2, board, assigned);
#ifdef EXTRA_PP
    pp_naked_single(nsq, n, n2, board, assigned);
    pp_hidden_single(nsq, n, n2, board, assigned);
#endif
}

void rm_assigned(const std::vector<int> &assigned, VClause &cl)
{
    for (auto &a : assigned)
    {
        if (std::find(cl.begin(), cl.end(), a) != cl.end())
        {
            cl.clear();
            break;
        }
        std::remove(cl.begin(), cl.end(), -a);
    }
}

std::vector<VClause> minimal_encoding(int nsq, int n, int n2, Board board)
{
    std::vector<int> assigned;
    pp(nsq, n, n2, board, assigned);
    std::vector<VClause> clauses;

    for (int x = 1; x <= n; ++x)
    {
        for (int y = 1; y <= n; ++y)
        {
            VClause cl = cell_def(n, n2, x, y);
#ifdef RM_ASSIGNED
            rm_assigned(assigned, cl);
#endif
            if (!cl.empty())
            {
                clauses.push_back(cl);
            }
        }
    }

    for (int x = 1; x <= n; ++x)
    {
        for (int v = 1; v <= n; ++v)
        {
            for (int yi = 1; yi < n; ++yi)
            {
                for (int yj = yi + 1; yj <= n; ++yj)
                {
                    VClause cl = row_uni(n, n2, x, v, yi, yj);
#ifdef RM_ASSIGNED
                    rm_assigned(assigned, cl);
#endif
                    if (!cl.empty())
                    {
                        clauses.push_back(cl);
                    }
                }
            }
        }
    }

    for (int y = 1; y <= n; ++y)
    {
        for (int v = 1; v <= n; ++v)
        {
            for (int xi = 1; xi < n; ++xi)
            {
                for (int xj = xi + 1; xj <= n; ++xj)
                {
                    VClause cl = col_uni(n, n2, y, v, xi, xj);
#ifdef RM_ASSIGNED
                    rm_assigned(assigned, cl);
#endif
                    if (!cl.empty())
                    {
                        clauses.push_back(cl);
                    }
                }
            }
        }
    }

    for (int v = 1; v <= n; ++v)
    {
        for (int i = 0; i < nsq; ++i)
        {
            for (int j = 0; j < nsq; ++j)
            {
                for (int x = 1; x <= nsq; ++x)
                {
                    for (int y = 1; y <= nsq; ++y)
                    {
                        for (int k = y + 1; k <= nsq; ++k)
                        {
                            VClause cl = blk_uni1(nsq, n, n2, v, i, j, x, y, k);
#ifdef RM_ASSIGNED
                            rm_assigned(assigned, cl);
#endif
                            if (!cl.empty())
                            {
                                clauses.push_back(cl);
                            }
                        }
                    }
                }
            }
        }
    }
    for (int v = 1; v <= n; ++v)
    {
        for (int i = 0; i < nsq; ++i)
        {
            for (int j = 0; j < nsq; ++j)
            {
                for (int x = 1; x <= nsq; ++x)
                {
                    for (int y = 1; y <= nsq; ++y)
                    {
                        for (int k = x + 1; k <= nsq; ++k)
                        {
                            for (int l = 1; l <= nsq; ++l)
                            {
                                VClause cl =
                                    blk_uni2(nsq, n, n2, v, i, j, x, y, k, l);
#ifdef RM_ASSIGNED
                                rm_assigned(assigned, cl);
#endif
                                if (!cl.empty())
                                {
                                    clauses.push_back(cl);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Note on reference *A Sudoku-Solver for Large Puzzles using SAT*:
    //
    // > In a Sudoku puzzle some values are assigned to some cells initially.
    // > This can be used to remove redundancies by reducing the definedness
    // > and uniqueness formulas. For example, if it is known that the cell
    // > (x, y) is set to v, then any clause which contains (x, y, v) can be
    // > removed and from every remaining clause the literal ¬(x, y, z) can
    // > be deleted.
    //
    // The assignments themselves should not be reduced, as done below.
    std::vector<VClause> ass_clauses = ass(assigned);
    clauses.insert(clauses.end(), ass_clauses.begin(), ass_clauses.end());

    return clauses;
}

std::vector<VClause> extended_encoding(int nsq, int n, int n2, Board board)
{
    std::vector<int> assigned;
    pp(nsq, n, n2, board, assigned);
    std::vector<VClause> clauses;

    for (int x = 1; x <= n; ++x)
    {
        for (int y = 1; y <= n; ++y)
        {
            VClause cl = cell_def(n, n2, x, y);
#ifdef RM_ASSIGNED
            rm_assigned(assigned, cl);
#endif
            if (!cl.empty())
            {
                clauses.push_back(cl);
            }
        }
    }

    for (int x = 1; x <= n; ++x)
    {
        for (int y = 1; y <= n; ++y)
        {
            for (int vi = 1; vi < n; ++vi)
            {
                for (int vj = vi + 1; vj <= n; ++vj)
                {
                    VClause cl = cell_uni(n, n2, x, y, vi, vj);
#ifdef RM_ASSIGNED
                    rm_assigned(assigned, cl);
#endif
                    if (!cl.empty())
                    {
                        clauses.push_back(cl);
                    }
                }
            }
        }
    }
    for (int x = 1; x <= n; ++x)
    {
        for (int v = 1; v <= n; ++v)
        {
            VClause cl = row_def(n, n2, x, v);
#ifdef RM_ASSIGNED
            rm_assigned(assigned, cl);
#endif
            if (!cl.empty())
            {
                clauses.push_back(cl);
            }
        }
    }

    for (int x = 1; x <= n; ++x)
    {
        for (int v = 1; v <= n; ++v)
        {
            for (int yi = 1; yi < n; ++yi)
            {
                for (int yj = yi + 1; yj <= n; ++yj)
                {
                    VClause cl = row_uni(n, n2, x, v, yi, yj);
#ifdef RM_ASSIGNED
                    rm_assigned(assigned, cl);
#endif
                    if (!cl.empty())
                    {
                        clauses.push_back(cl);
                    }
                }
            }
        }
    }

    for (int y = 1; y <= n; ++y)
    {
        for (int v = 1; v <= n; ++v)
        {
            VClause cl = col_def(n, n2, y, v);
#ifdef RM_ASSIGNED
            rm_assigned(assigned, cl);
#endif
            if (!cl.empty())
            {
                clauses.push_back(cl);
            }
        }
    }

    for (int y = 1; y <= n; ++y)
    {
        for (int v = 1; v <= n; ++v)
        {
            for (int xi = 1; xi < n; ++xi)
            {
                for (int xj = xi + 1; xj <= n; ++xj)
                {
                    VClause cl = col_uni(n, n2, y, v, xi, xj);
#ifdef RM_ASSIGNED
                    rm_assigned(assigned, cl);
#endif
                    if (!cl.empty())
                    {
                        clauses.push_back(cl);
                    }
                }
            }
        }
    }

    for (int xo = 0; xo < nsq; ++xo)
    {
        for (int yo = 0; yo < nsq; ++yo)
        {
            for (int v = 1; v <= n; ++v)
            {
                VClause cl = blk_def(nsq, n, n2, xo, yo, v);
#ifdef RM_ASSIGNED
                rm_assigned(assigned, cl);
#endif
                if (!cl.empty())
                {
                    clauses.push_back(cl);
                }
            }
        }
    }

    for (int v = 1; v <= n; ++v)
    {
        for (int i = 0; i < nsq; ++i)
        {
            for (int j = 0; j < nsq; ++j)
            {
                for (int x = 1; x <= nsq; ++x)
                {
                    for (int y = 1; y <= nsq; ++y)
                    {
                        for (int k = y + 1; k <= nsq; ++k)
                        {
                            VClause cl = blk_uni1(nsq, n, n2, v, i, j, x, y, k);
#ifdef RM_ASSIGNED
                            rm_assigned(assigned, cl);
#endif
                            if (!cl.empty())
                            {
                                clauses.push_back(cl);
                            }
                        }
                    }
                }
            }
        }
    }
    for (int v = 1; v <= n; ++v)
    {
        for (int i = 0; i < nsq; ++i)
        {
            for (int j = 0; j < nsq; ++j)
            {
                for (int x = 1; x <= nsq; ++x)
                {
                    for (int y = 1; y <= nsq; ++y)
                    {
                        for (int k = x + 1; k <= nsq; ++k)
                        {
                            for (int l = 1; l <= nsq; ++l)
                            {
                                VClause cl =
                                    blk_uni2(nsq, n, n2, v, i, j, x, y, k, l);
#ifdef RM_ASSIGNED
                                rm_assigned(assigned, cl);
#endif
                                if (!cl.empty())
                                {
                                    clauses.push_back(cl);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Note on reference *A Sudoku-Solver for Large Puzzles using SAT*:
    //
    // > In a Sudoku puzzle some values are assigned to some cells initially.
    // > This can be used to remove redundancies by reducing the definedness
    // > and uniqueness formulas. For example, if it is known that the cell
    // > (x, y) is set to v, then any clause which contains (x, y, v) can be
    // > removed and from every remaining clause the literal ¬(x, y, z) can
    // > be deleted.
    //
    // The assignments themselves should not be reduced, as done below.
    std::vector<VClause> ass_clauses = ass(assigned);
    clauses.insert(clauses.end(), ass_clauses.begin(), ass_clauses.end());

    return clauses;
}

std::vector<VClause> c_minimal_encode(int nsq, Board board)
{
    int n = nsq * nsq;
    int n2 = n * n;
    int n3 = n * n * n;
    std::vector<VClause> clauses = minimal_encoding(nsq, n, n2, board);
    if (!check_literals(n3, clauses))
    {
        return {};
    }
    return clauses;
}

std::vector<VClause> c_extended_encode(int nsq, Board board)
{
    int n = nsq * nsq;
    int n2 = n * n;
    int n3 = n * n * n;
    std::vector<VClause> clauses = extended_encoding(nsq, n, n2, board);
    if (!check_literals(n3, clauses))
    {
        return {};
    }
    return clauses;
}

std::vector<VClause> VariableManager::convert(const std::vector<VClause> &clauses)
{
    std::vector<VClause> cvt_clauses;
    for (const VClause &cl : clauses)
    {
        VClause cvt_cl;
        for (int lit : cl)
        {
            int abslit = std::abs(lit);
            auto vidp = pool.find(abslit);
            int vid;
            if (vidp == pool.end())
            {
                vid = index.size();
                pool.insert({abslit, vid});
                index.push_back(abslit);
            }
            else
            {
                vid = vidp->second;
            }
            cvt_cl.push_back(lit > 0 ? vid : -vid);
        }
        cvt_clauses.push_back(std::move(cvt_cl));
    }

    return cvt_clauses;
}

std::vector<int> VariableManager::back(const std::vector<int> &solution)
{
    std::vector<int> psolution; // 'p' means positive
    for (int signed_vid : solution)
    {
        if (signed_vid > 0)
        {
            psolution.push_back(index[signed_vid]);
        }
    }
    return psolution;
}
