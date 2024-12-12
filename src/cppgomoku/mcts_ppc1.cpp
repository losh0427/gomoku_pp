#include "cppgomoku/mcts.h"
// bool checked = false;
#include "cppgomoku/common.h"

namespace gomoku
{
    MCTSTreeNode *copyTree(const MCTSTreeNode *src) {
        MCTSTreeNode *new_node = new MCTSTreeNode(nullptr, src->probability);
        new_node->Q_value = src->Q_value;
        new_node->visit_times = src->visit_times;
        new_node->U_value = src->U_value;

        for(auto &kv: src->children) {
            MCTSTreeNode *child_copy = copyTree(kv.second);
            child_copy->parent = new_node;
            new_node->children.insert({kv.first, kv.second});
        }

        return new_node;
    }

    // ppc1 part
    Ppc1_MonteCarloSearchTree::Ppc1_MonteCarloSearchTree(float weight_c, int compute_budget, double time_budget,
                                                       int expand_bound, bool silent, int rollout_limit,
                                                       expandFunc *expand_fn, rolloutFunc *rollout_fn, bool DEBUG) {
        root = new MCTSTreeNode(nullptr, 1.0);
        this->weight_c = weight_c;
        this->compute_budget = compute_budget;
        this->time_budget = time_budget;
        this->silent = silent;
        this->expand_bound = expand_bound;
        this->rollout_limit = rollout_limit;
        this->expand_func = expand_fn;
        this->rollout_func = rollout_fn;
        this->DEBUG = DEBUG;
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

    void Ppc1_MonteCarloSearchTree::playoutLocal(Board &s, MCTSTreeNode *local_root) {
        MCTSTreeNode *curr_node = local_root;
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

        if (!silent) printf("PPC1 Thinking...\n");
        // cpu_set_t cpu_set;
        // sched_getaffinity(0, sizeof(cpu_set), &cpu_set);
        // printf("%d cpus availale\n", CPU_COUNT(&cpu_set));

        int num_threads = thd_num;
        std::vector<MCTSTreeNode*> local_roots(num_threads, nullptr);
        int avg_iter = 0;

        double think_start = getTimeStamp();
        #pragma omp parallel num_threads(num_threads)
        {
            int tid = omp_get_thread_num();
            MCTSTreeNode *local_root = copyTree(root);
            Board local_board(s);

            // for (int i = 0; i < compute_budget; ++i) {
            //     Board board_for_search(local_board);
            //     playoutLocal(board_for_search, local_root);
            // }
            double local_start = getTimeStamp();
            double local_end = local_start;
            int iter = 0;
            while((local_end - local_start) < time_budget) {
                Board board_for_search(local_board);
                playoutLocal(board_for_search, local_root);
                local_end = getTimeStamp();
                ++iter;
            }

            local_roots[tid] = local_root;

            #pragma omp critical
            {
                printf("tid %d iteration: %d\n", tid, iter);
                avg_iter += iter;
            }
            // printf("tid %d children size: %d\n", tid, local_root->children.size());
        }
        // for (int i=0;i<compute_budget;++i) {
        //     Board board_for_search(s);
        //     playout(board_for_search);
        // }
        double think_end = getTimeStamp();
        avg_iter /= num_threads;
        if (!silent) {
            printf("PPC1 Thinking time: %f\n", think_end - think_start);
            printf("PPC1 Iteration count(average): %d\n", avg_iter);
        }
        // printf("PPC1 Thinking time: %f\n", think_end - think_start);
        // printf("PPC1 Iteration count(average): %d\n", avg_iter);

        if (DEBUG) {
            std::vector<MoveProbPair> debug_output_vec;
            for(auto lr: local_roots)
                for (auto i: lr->children) {
                    MoveProbPair p(i.first, i.second->Q_value);
                    debug_output_vec.push_back(p);
                }

            std::sort(debug_output_vec.begin(), debug_output_vec.end(),
                     [](const MoveProbPair &p1, const MoveProbPair &p2)
                     {return p1.prob > p2.prob;});

            printf("[--DEBUG OUT--]\nMove probabilities %d:", debug_output_vec.size());
            for (auto mpp: debug_output_vec) {
                printf("(Move: %d, Value: %f), ", mpp.move, mpp.prob);
            }
            printf("\n");
        }

        std::unordered_map<int, int> aggregated_visit_times;
        std::unordered_set<int> all_moves;
        for(int i = 0; i < local_roots.size(); ++i) {
            for(auto &child_pair: local_roots[i]->children) {
                all_moves.insert(child_pair.first);
            }
        }

        int max_visit_times = -1;
        int return_move = -1;
        for(int move: all_moves) {
            int sum_visit_times = 0;
            for(auto lr: local_roots) {
                auto it = lr->children.find(move);
                if(it != lr->children.end()) {
                    sum_visit_times += it->second->visit_times;
                }
            }
            aggregated_visit_times[move] = sum_visit_times;
            if(sum_visit_times > max_visit_times) {
                max_visit_times = sum_visit_times;
                return_move = move;
            }
        }


        // int max_visit_times = -1;
        // int return_move = -1;
        // for(auto i: root->children) {
        //     if (i.second->visit_times > max_visit_times) {
        //         max_visit_times = i.second->visit_times;
        //         return_move = i.first;
        //     }
        // }
        if (DEBUG) {
            printf("Next Move: %d, Value: %d\n", return_move, max_visit_times);
        }
        for(auto &lr: local_roots) {
            delete lr;
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
