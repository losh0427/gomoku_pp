#include "cppgomoku/mcts.h"


namespace gomoku
{
    // ppc1 part
    Ppc1_MonteCarloSearchTree::Ppc1_MonteCarloSearchTree(float weight_c, int compute_budget,
                                                       int expand_bound, bool silent, int rollout_limit,
                                                       expandFunc *expand_fn, rolloutFunc *rollout_fn) {
        root = new MCTSTreeNode(nullptr, 1.0);
        this->weight_c = weight_c;
        this->compute_budget = compute_budget;
        this->silent = silent;
        this->expand_bound = expand_bound;
        this->rollout_limit = rollout_limit;
        this->expand_func = expand_fn;
        this->rollout_func = rollout_fn;
    }

    void Ppc1_MonteCarloSearchTree::reset() {
        root->kill();
        delete root;
        root = new MCTSTreeNode(nullptr, 1.0);
    }

    float Ppc1_MonteCarloSearchTree::evaluateRollout(Board &board, int limit) {
        int player_color = board.currentPlayerColor();
        int i, winner_color;
        for (i=0; i<limit; ++i) {
            // first check game end
            bool is_end = board.gameEnd(winner_color);
            if (is_end) break;

            int next_action = most_likely_move((*rollout_func)(board));
            board.play(next_action);
        } if (i == limit && !silent) {
            printf("[Warning]: rollout exceeds the limit: %d", limit);
        }

        if (winner_color == Board::kPlayerEmpty) return 0.0;
        else return (winner_color == player_color) ? 1.0 : -1.0;
    }

    void Ppc1_MonteCarloSearchTree::playout(Board &s) {
        MCTSTreeNode *curr_node = root;
        while (true) { // first find a leaf node use UCB
            if (curr_node->isLeaf()) break; // if leaf or single root.
            
            int next_action;
            curr_node = curr_node->select(weight_c, next_action);
            s.play(next_action);
        }

        // check game is end
        int winner_color;
        bool is_end = s.gameEnd(winner_color);

        // if game not over and this node's visit_times beyonds the bound.
        if (!is_end && curr_node->visit_times >= expand_bound) {
            curr_node->expand((*expand_func)(s));
        }

        // evaluate this "leaf(expanded before)" node。
        float bp_value = evaluateRollout(s, rollout_limit);
        curr_node->backPropagation(-bp_value);
    }

    int Ppc1_MonteCarloSearchTree::getMove(Board &s, float exploration_level) {
        // the first move is at the center of board
        if (s.isEmpty()) return (s.getHeight() * s.getWidth()) / 2;

        if (!silent) printf("Thinking...\n");
        
        double think_start = getTimeStamp();
        for (int i=0;i<compute_budget;++i) {
            Board board_for_search(s);
            playout(board_for_search);
        }
        double think_end = getTimeStamp();

        if (DEBUG) {
            printf("Thinking time: %f\n", think_end - think_start);
            std::vector<MoveProbPair> debug_output_vec;
            for (auto i: root->children) {
                MoveProbPair p(i.first, i.second->Q_value);
                debug_output_vec.push_back(p);
            }

            std::sort(debug_output_vec.begin(), debug_output_vec.end(),
                     [](const MoveProbPair &p1, const MoveProbPair &p2)
                     {return p1.prob > p2.prob;});

            printf("[--DEBUG OUT--]\nMove probabilities:");
            for (auto mpp: debug_output_vec) {
                printf("(Move: %d, Value: %f), ", mpp.move, mpp.prob);
            }
            printf("\n");
        }

        // float max_Q_values = root->children.begin()->second->Q_value;
        // int return_move = root->children.begin()->first;
        // for(auto i: root->children) {
        //     if (i.second->Q_value > max_Q_values) {
        //         max_Q_values = i.second->Q_value;
        //         return_move = i.first;
        //     }
        // }
        int max_visit_times = root->children.begin()->second->visit_times;
        int return_move = root->children.begin()->first;
        for(auto i: root->children) {
            if (i.second->visit_times > max_visit_times) {
                max_visit_times = i.second->visit_times;
                return_move = i.first;
            }
        }
        if (DEBUG) {
            printf("Next Move: %d, Value: %d\n", return_move, max_visit_times);
        }
        return return_move;
    }

    void Ppc1_MonteCarloSearchTree::updateWithMove(int last_move) {
        if (last_move == Board::kPlayerEmpty) return;
        if (root->children.find(last_move) != root->children.end()) {
            MCTSTreeNode * next_root = root->leaveOneChild(last_move);
            delete root;
            root = next_root;
        } else {
            delete root;
            root = new MCTSTreeNode(nullptr, 1.0);
        }
    }

} // gomoku
