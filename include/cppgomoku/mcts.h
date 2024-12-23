#ifndef GOMOKU_MCTS_H
#define GOMOKU_MCTS_H

#include <unordered_map>
#include <memory>
#include <limits>
#include <omp.h>
#include "cppgomoku/common.h"
#include <algorithm>
#include "cppgomoku/board.h"
#include "cppgomoku/policy_functions.h"

#ifndef TIME_FUNC
#define TIME_FUNC
#include <sys/time.h>
extern double getTimeStamp();
#endif

namespace gomoku
{
    typedef decltype(MCTS_Expand_policy_fn) expandFunc;
    typedef decltype(rollout_policy_fn) rolloutFunc;

    struct MCTSTreeNode {
        MCTSTreeNode * parent;
        // move->child map
        std::unordered_map<int, MCTSTreeNode*> children;
        int visit_times;
        float Q_value;
        float U_value;
        float probability;
 
        MCTSTreeNode(){
            parent = nullptr; visit_times = 0; probability=0.0;
            Q_value = 0.0; U_value = 0.0;
        }
        MCTSTreeNode(MCTSTreeNode* parent, float prior_prob);
        ~MCTSTreeNode();
        void kill();
        MCTSTreeNode * leaveOneChild(int move);
        void expand(const std::vector<MoveProbPair> &policy);
        void expand(std::vector<MoveProbPair> &&polcy);
        void backPropagation(const float bp_value);
        float evaluate(float weight_c);
        void update(float bp_value);
        MCTSTreeNode* select(float weight_c, int &action);
        bool isLeaf() {return children.empty();}
        bool isRoot() {return parent == nullptr;}
        float getQValue() {return Q_value;}

        MCTSTreeNode *copyTree(const MCTSTreeNode *src);
    };

    class MonteCarloSearchTreeBase {
    public:
        MonteCarloSearchTreeBase() {}
        virtual ~MonteCarloSearchTreeBase() {}
        virtual void playout(Board &s) = 0;
        virtual void reset() = 0;
        virtual int getMove(Board &s, float exploration_level=1) = 0;
        virtual void updateWithMove(int last_move) = 0;
        virtual MCTSTreeNode * getRoot() = 0;
    };

    class PureMonteCarloSearchTree : public MonteCarloSearchTreeBase {
    private:
        MCTSTreeNode * root;
        float weight_c;
        int compute_budget;
        float time_budget;
        bool silent;
        int expand_bound;
        int rollout_limit;
        expandFunc *expand_func;
        rolloutFunc *rollout_func;
        bool DEBUG;
        std::vector<int> iter_count;
    
    public:
        PureMonteCarloSearchTree(){}
        PureMonteCarloSearchTree(float weight_c, int compute_budget, 
                                 int expand_bound, float time_budget, bool silent, 
                                 int rollout_limit, expandFunc *expand_fn,
                                 rolloutFunc *rollout_fn, bool DEBUG);
        ~PureMonteCarloSearchTree() {delete root;}
        void reset();
        float evaluateRollout(Board &board, int limit);
        void playout(Board &s);
        int getMove(Board &s, float exploration_level=1);
        void updateWithMove(int last_move);
        MCTSTreeNode * getRoot() {return root;}
        void setSilent() {silent = true;}
        void unsetSilent() {silent = false;}
        int getComputeBudget() {return compute_budget;}
        std::vector<int>& getIterCount() {return iter_count;}
    };

    // pp part
    class Ppc1_MonteCarloSearchTree;
    class Ppc2_MonteCarloSearchTree;
    
    class Ppc1_MonteCarloSearchTree : public MonteCarloSearchTreeBase {
    private:
        MCTSTreeNode * root;
        float weight_c;
        int compute_budget;
        double time_budget;
        bool silent;
        int expand_bound;
        int rollout_limit;
        expandFunc *expand_func;
        rolloutFunc *rollout_func;
        bool DEBUG;
        std::vector<int> iter_count;

    
    public:
        Ppc1_MonteCarloSearchTree(){}
        Ppc1_MonteCarloSearchTree(float weight_c, int compute_budget, double time_budget,
                                 int expand_bound, bool silent, 
                                 int rollout_limit, expandFunc *expand_fn,
                                 rolloutFunc *rollout_fn, bool DEBUG);
        ~Ppc1_MonteCarloSearchTree() {delete root;}
        void reset();
        float evaluateRollout(Board &board, int limit);
        void playout(Board &s);
        void playoutLocal(Board &s, MCTSTreeNode *local_root);
        int getMove(Board &s, float exploration_level=1);
        void updateWithMove(int last_move);
        MCTSTreeNode * getRoot() {return root;}
        void setSilent() {silent = true;}
        void unsetSilent() {silent = false;}
        int getComputeBudget() {return compute_budget;}
        std::vector<int>& getIterCount() {return iter_count;}
    };

    typedef struct ThreadData{
        int iter;
        Board *global_board;
        Board *local_board;
        MCTSTreeNode *local_root;
        Ppc2_MonteCarloSearchTree *_this;
    } thread_data;

    class Ppc2_MonteCarloSearchTree : public MonteCarloSearchTreeBase {
    private:
        MCTSTreeNode * root;
        float weight_c;
        int compute_budget;
        double time_budget;
        bool silent;
        int expand_bound;
        int rollout_limit;
        expandFunc *expand_func;
        rolloutFunc *rollout_func;
        bool DEBUG;
        std::vector<int> iter_count;

    
    public:
        Ppc2_MonteCarloSearchTree(){}
        Ppc2_MonteCarloSearchTree(float weight_c, int compute_budget, double time_budget, 
                                 int expand_bound, bool silent, 
                                 int rollout_limit, expandFunc *expand_fn,
                                 rolloutFunc *rollout_fn, bool DEBUG);
        ~Ppc2_MonteCarloSearchTree() {delete root;}
        void reset();
        float evaluateRollout(Board &board, int limit);
        void playout(Board &s);
        int getMove(Board &s, float exploration_level=1);
        void updateWithMove(int last_move);
        MCTSTreeNode * getRoot() {return root;}
        void setSilent() {silent = true;}
        void unsetSilent() {silent = false;}
        int getComputeBudget() {return compute_budget;}
        double getTimeBudget() {return time_budget;}
        std::vector<int>& getIterCount() {return iter_count;}

        static void *loopPlayoutLocal(void *arguments);
        static void playoutLocal(thread_data *t_data);
    };
    class Ppc3_MonteCarloSearchTree : public MonteCarloSearchTreeBase {
    private:
        MCTSTreeNode * root;
        float weight_c;
        int compute_budget;
        double time_budget;
        bool silent;
        int expand_bound;
        int rollout_limit;
        expandFunc *expand_func;
        rolloutFunc *rollout_func;
        bool DEBUG;
        std::vector<int> iter_count;

    
    public:
        Ppc3_MonteCarloSearchTree(){}
        Ppc3_MonteCarloSearchTree(float weight_c, int compute_budget, double time_budget, 
                                 int expand_bound, bool silent, 
                                 int rollout_limit, expandFunc *expand_fn,
                                 rolloutFunc *rollout_fn, bool DEBUG);
        ~Ppc3_MonteCarloSearchTree() {delete root;}
        void reset();
        float evaluateRollout(Board &board, int limit);
        void playout(Board &s);
        int getMove(Board &s, float exploration_level=1);
        void updateWithMove(int last_move);
        MCTSTreeNode * getRoot() {return root;}
        void setSilent() {silent = true;}
        void unsetSilent() {silent = false;}
        int getComputeBudget() {return compute_budget;}
        std::vector<int>& getIterCount() {return iter_count;}
    };




} // gomoku


#endif // GOMOKU_MCTS_H