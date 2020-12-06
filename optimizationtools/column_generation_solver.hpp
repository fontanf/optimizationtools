#pragma once

#include <vector>
#include <cstdint>

#if CPLEX_FOUND
#include <ilcplex/ilocplex.h>
#endif

#if COINOR_FOUND
#include <coin/ClpModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
#endif

namespace optimizationtools
{

class ColumnGenerationSolver
{

public:

    typedef int64_t ColIdx;
    typedef int64_t RowIdx;
    typedef double  Value;

    virtual ~ColumnGenerationSolver() { }
    virtual void add_column(
            const std::vector<RowIdx>& row_indices,
            const std::vector<Value>& row_values,
            Value objective_value,
            Value lower_bound = 0,
            Value upper_bound = 1) = 0;

    virtual void solve() = 0;

    virtual Value objective()        const = 0;
    virtual Value dual(RowIdx row)   const = 0;
    virtual Value primal(ColIdx col) const = 0;
    virtual ColIdx column_number() const = 0;

};

#if COINOR_FOUND

class ColumnGenerationSolverClp: public ColumnGenerationSolver
{

public:

    ColumnGenerationSolverClp(
            const std::vector<Value>& row_lower_bounds,
            const std::vector<Value>& row_upper_bounds):
        duals_(row_lower_bounds.size())
    {
        model_.messageHandler()->setLogLevel(0);
        for (RowIdx i = 0; i < (RowIdx)row_lower_bounds.size(); ++i)
            model_.addRow(
                    0, NULL, NULL,
                    (row_lower_bounds[i] != -std::numeric_limits<Value>::infinity())? row_lower_bounds[i]: -COIN_DBL_MAX,
                    (row_upper_bounds[i] !=  std::numeric_limits<Value>::infinity())? row_upper_bounds[i]: COIN_DBL_MAX);
    }

    virtual ~ColumnGenerationSolverClp() { }

    void add_column(
            const std::vector<RowIdx>& row_indices,
            const std::vector<Value>& row_values,
            Value objective_value,
            Value lower_bound = 0,
            Value upper_bound = 1)
    {
        model_.addColumn(
                rows_indices.size(),
                row_indices.data(),
                row_values.data(),
                lower_bound,
                upper_bound,
                objective_value);
    }

    void solve()
    {
        model_.primal();
        // Primals
        const double* solution = model_.getColSolution();
        primals_.resize(model_.numberColumns());
        for (ColIdx col = 0; col < model_.numberColumns(); ++col)
            primals_[col] = solution[col];
        // Dual
        double* duals = model_.dualRowSolution();
        for (RowIdx row = 0; row < model.numberRows(); ++ row)
            duals_[row] = duals[row];
    }

    Value objective()        const { return model_.objectiveValue(); }
    Value dual(RowIdx row)   const { return duals_[row]; }
    Value primal(ColIdx col) const { return primals_[col]; }
    ColIdx column_number()   const { return primals_.size(); };

private:

    ClpSimplex model_;
    std::vector<Value> duals_;
    std::vector<Value> primals_;

};

#endif

#if CPLEX_FOUND

ILOSTLBEGIN

class ColumnGenerationSolverCplex: public ColumnGenerationSolver
{

public:

    ColumnGenerationSolverCplex(
            const std::vector<Value>& row_lower_bounds,
            const std::vector<Value>& row_upper_bounds):
        env_(),
        model_(env_),
        obj_(env_),
        range_(env_),
        cplex_(model_),
        duals_tmp_(env_, row_lower_bounds.size()),
        duals_(row_lower_bounds.size())
    {
        obj_.setSense(IloObjective::Minimize);
        model_.add(obj_);
        for (RowIdx i = 0; i < (RowIdx)row_lower_bounds.size(); ++i)
            range_.add(IloRange(
                        env_,
                        (row_lower_bounds[i] != -std::numeric_limits<Value>::infinity())? row_lower_bounds[i]: -IloInfinity,
                        (row_upper_bounds[i] !=  std::numeric_limits<Value>::infinity())? row_upper_bounds[i]: IloInfinity));
        model_.add(range_);
        cplex_.setOut(env_.getNullStream()); // Remove standard output
    }

    virtual ~ColumnGenerationSolverCplex()
    {
        env_.end();
    }

    void add_column(
            const std::vector<RowIdx>& row_indices,
            const std::vector<Value>& row_values,
            Value objective_value,
            Value lower_bound = 0,
            Value upper_bound = 1)
    {
        IloNumColumn col = obj_(objective_value);
        for (RowIdx i = 0; i < (RowIdx)row_indices.size(); ++i)
            col += range_[row_indices[i]](row_values[i]);
        vars_.push_back(IloNumVar(col, lower_bound, upper_bound, ILOFLOAT));
        model_.add(vars_.back());
    }

    void solve()
    {
        cplex_.solve();
        // Primal
        primals_.resize(vars_.size());
        for (ColIdx col = 0; col < (ColIdx)vars_.size(); ++col)
            primals_[col] = cplex_.getValue(vars_[col]);
        // Dual
        cplex_.getDuals(duals_tmp_, range_);
        for (RowIdx row = 0; row < duals_tmp_.getSize(); ++ row)
            duals_[row] = duals_tmp_[row];
    }

    Value objective()        const { return cplex_.getObjValue(); }
    Value dual(RowIdx row)   const { return duals_[row]; }
    Value primal(ColIdx col) const { return primals_[col]; }
    ColIdx column_number()   const { return primals_.size(); };

private:

    IloEnv env_;
    IloModel model_;
    IloObjective obj_;
    IloRangeArray range_;
    IloCplex cplex_;
    std::vector<IloNumVar> vars_;
    IloNumArray duals_tmp_;
    std::vector<Value> duals_;
    std::vector<Value> primals_;

};

#endif

}

