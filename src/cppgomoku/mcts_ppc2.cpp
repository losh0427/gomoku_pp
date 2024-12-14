#include "cppgomoku/mcts.h"
#include "cppgomoku/common.h"
#include <pthread.h>

namespace gomoku
{
    //ppc2 part
    Ppc2_MonteCarloSearchTree::Ppc2_MonteCarloSearchTree(float weight_c, int compute_budget, double time_budget,
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
        this->time_budget = time_budget;
        this->DEBUG = DEBUG;
        this->iter_count = std::vector<int>();
    }

    void Ppc2_MonteCarloSearchTree::reset() {
        root->kill();
        delete root;
        root = new MCTSTreeNode(nullptr, 1.0);
    }

    float Ppc2_MonteCarloSearchTree::evaluateRollout(Board &board, int limit) {
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

    void Ppc2_MonteCarloSearchTree::playout(Board &s) {
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


    void Ppc2_MonteCarloSearchTree::playoutLocal(thread_data *t_data) {
        // thread_data *t_data = (thread_data*)arguments;
        MCTSTreeNode *curr_node = t_data->local_root;
        while (true) { // first find a leaf node use UCB
            if (curr_node->isLeaf()) break; // if leaf or single root.
            
            int next_action;
            curr_node = curr_node->select(t_data->_this->weight_c, next_action);
            t_data->local_board->play(next_action);
        }
        

        // check game is end
        int winner_color;
        bool is_end = t_data->local_board->gameEnd(winner_color);

        // if game not over and this node's visit_times beyonds the bound.
        if (!is_end && curr_node->visit_times >= t_data->_this->expand_bound) {
            curr_node->expand((*t_data->_this->expand_func)(*t_data->local_board));
        }

        // evaluate this "leaf(expanded before)" node。
        float bp_value = t_data->_this->evaluateRollout(*t_data->local_board, t_data->_this->rollout_limit);
        curr_node->backPropagation(-bp_value);
    }

    void *Ppc2_MonteCarloSearchTree::loopPlayoutLocal(void *arguments) {
        thread_data *t_data = (thread_data*)arguments;
        double local_start = getTimeStamp();
        double local_end = local_start;
        int iter = 0;
        while((local_end - local_start) < t_data->_this->getTimeBudget()) {
            t_data->local_board = new Board(*t_data->global_board);
            t_data->_this->playoutLocal(t_data);
            local_end = getTimeStamp();
            ++iter;
        }
        t_data->iter = iter;
        pthread_exit(NULL);
    }

    int Ppc2_MonteCarloSearchTree::getMove(Board &s, float exploration_level) {
        // the first move is at the center of board
        if (s.isEmpty()) return (s.getHeight() * s.getWidth()) / 2;

        if (!silent) printf("PPC2 Thinking...\n");
        // for (int i=0;i<compute_budget;++i) {
        //     Board board_for_search(s);
        //     playout(board_for_search);
        // }
        int ncpus = thd_num;
        pthread_t threads[ncpus];
        thread_data t_datas[ncpus];
        for(int t = 0; t < ncpus; ++t) {
            t_datas[t] = (thread_data){
                .iter = 0,
                .global_board = &s,
                .local_root = root->copyTree(root),
                ._this = this,
            };
            pthread_create(&threads[t], NULL, this->loopPlayoutLocal, (void*)&t_datas[t]);
        }

        double think_start = getTimeStamp();
        for(int t = 0; t < ncpus; ++t) {
            pthread_join(threads[t], NULL);
        }
        double think_end = getTimeStamp();
        int avg_iter = 0;
        for(int t = 0; t < ncpus; ++t) {
            printf("tid %d iteration: %d\n", t, t_datas[t].iter);
            avg_iter += t_datas[t].iter;
        }
        // avg_iter /= ncpus;
        iter_count.push_back(avg_iter);
        if(!silent) {
            printf("PPC2 Thinking time: %f\n", think_end - think_start);
            // printf("PPC2 Iteration count(average): %d\n", avg_iter);
            printf("PPC2 Iteration count(total): %d\n", avg_iter);
        }

        if (DEBUG) {
            std::vector<MoveProbPair> debug_output_vec;
            for(auto td: t_datas)
                for (auto i: td.local_root->children) {
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

        std::unordered_map<int, int> aggregated_visit_times;
        std::unordered_set<int> all_moves;
        for(int i = 0; i < ncpus; ++i) {
            for(auto &child_pair: t_datas[i].local_root->children) {
                all_moves.insert(child_pair.first);
            }
        }
        // float max_Q_values = root->children.begin()->second->Q_value;
        // int return_move = root->children.begin()->first;
        // for(auto i: root->children) {
        //     if (i.second->Q_value > max_Q_values) {
        //         max_Q_values = i.second->Q_value;
        //         return_move = i.first;
        //     }
        // }
        int max_visit_times = -1;
        int return_move = -1;
        for(int move: all_moves) {
            int sum_visit_times = 0;
            for(auto lr: t_datas) {
                auto it = lr.local_root->children.find(move);
                if(it != lr.local_root->children.end()) {
                    sum_visit_times += it->second->visit_times;
                }
            }
            aggregated_visit_times[move] = sum_visit_times;
            if(sum_visit_times > max_visit_times) {
                max_visit_times = sum_visit_times;
                return_move = move;
            }
        }
        
        if (DEBUG) {
            printf("Next Move: %d, Value: %d\n", return_move, max_visit_times);
        }

        for(int t = 0; t < ncpus; ++t) {
            delete(t_datas[t].local_board);
            delete(t_datas[t].local_root);
        }
        return return_move;
    }

    void Ppc2_MonteCarloSearchTree::updateWithMove(int last_move) {
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
