#include "cppgomoku/player.h"
#include <stdexcept>


namespace gomoku
{
    HumanPlayer::HumanPlayer(int color, std::string name, 
                             char input_mode):Player(color, name)
    {
        switch (input_mode) {
            case 'G': 
                this->input_mode = HumanPlayerInputMode::kFromGUI; 
                break;
            case 'S':
                this->input_mode = HumanPlayerInputMode::kFromStdin;
                break;
            default:
                this->input_mode = HumanPlayerInputMode::kFromStdin;
        }
    }

    int HumanPlayer::getActionFromGUI(Board &b) {
        // unfinished, leave for future
        return 0;
    }

    int HumanPlayer::getActionFromStdin(Board &b) {
        while (true) {
            Location next_location;
            printf("Your movement [index starts from 0]\n(format: vertical_index horizontal_index): ");
            
            if (scanf("%d %d", &next_location.h_index, &next_location.w_index) != 2) {
                printf("Invalid input! Please enter two integers.\n");

                while (getchar() != '\n');
                continue;
            }

            printf("\n");

            int next_move = b.locationToMove(next_location);
            if (b.isValidMove(next_move)) {
                return next_move;
            } else {
                printf("Invalid movement! Please try again.\n");
            }
        }
    }

    int HumanPlayer::getAction(Board &b) {
        if (b.currentPlayerColor() != color) {
            throw std::runtime_error("The color of next move in board "
                                     "is not the same color of current player.\n");
        }
        switch (input_mode) {
            case HumanPlayerInputMode::kFromGUI:
                return getActionFromGUI(b);
            case HumanPlayerInputMode::kFromStdin:
                return getActionFromStdin(b);
            default:
                return getActionFromStdin(b);
        }
    }

    std::string HumanPlayer::PlayerInfo() {
        std::string color_info;
        switch (color) {
            case Board::kPlayerBlack:
                color_info = "Black[@]\n";
                break;
            case Board::kPlayerWhite:
                color_info = "White[O]\n";
                break;
            default:
                color_info = "None[+]\n";
        }

        std::string player_info = "-----\n[--Player Information--]\nName: ";
        player_info += name;
        player_info += "\nColor: ";
        player_info += color_info;
        switch (input_mode) {
            case HumanPlayerInputMode::kFromGUI:
                player_info += "Input Mode: from GUI\n\n";
                break;
            default:
                player_info += "Input Mode: from stdin\n\n";
        }

        return player_info;
    }


    PureMCTSPlayer::PureMCTSPlayer(int color, std::string name, float weight_c, 
                                   int compute_budget,float time_budget, bool silent, bool DEBUG)
    :Player(color, name), 
    search_tree(PureMonteCarloSearchTree(weight_c, compute_budget, compute_budget/100, time_budget, 
                                         silent, 1000, MCTS_Expand_policy_fn,
                                         rollout_policy_fn, DEBUG)) {
        this->silent = silent;
    }

    int PureMCTSPlayer::getAction(Board &b) {
        // check color
        if (b.currentPlayerColor() != color) {
            throw std::runtime_error("The color of next move in board "
                                     "is not the same color of current player.\n");
        }

        // update the MCST with last move, because we only use 
        // PureMCTSPlayer to play with others, not itself.
        if (!silent) printf("Update with move\n");
        // printf("Update with move\n");
        search_tree.updateWithMove(b.lastMove());
        if (!silent) printf("get move\n");
        // printf("get move\n");
        // get next move
        int next_move = search_tree.getMove(b);
        search_tree.updateWithMove(next_move);
        return next_move;
    }

    std::string PureMCTSPlayer::PlayerInfo() {
        std::string color_info;
        switch (color) {
            case Board::kPlayerBlack:
                color_info = "Black[@]\n";
                break;
            case Board::kPlayerWhite:
                color_info = "White[O]\n";
                break;
            default:
                color_info = "None[+]\n";
        }

        std::string player_info = "-----\n[--Player Information--]\nName: ";
        player_info += name;
        player_info += "\nColor: ";
        player_info += color_info;
        player_info += "Compute Budget: ";
        player_info += std::to_string(search_tree.getComputeBudget());
        player_info += "\n\n";
        return player_info;
    }

    // ppc1 part
    Ppc1_MCTSPlayer::Ppc1_MCTSPlayer(int color, std::string name, float weight_c, 
                                   int compute_budget, double time_budget, bool silent, bool DEBUG)
    :Player(color, name), 
    search_tree(Ppc1_MonteCarloSearchTree(weight_c, compute_budget, time_budget, compute_budget/100, 
                                         silent, 1000, MCTS_Expand_policy_fn,
                                         rollout_policy_fn, DEBUG)) {
        this->silent = silent;
    }

    int Ppc1_MCTSPlayer::getAction(Board &b) {
        // check color
        if (b.currentPlayerColor() != color) {
            throw std::runtime_error("The color of next move in board "
                                     "is not the same color of current player.\n");
        }

        // update the MCST with last move, because we only use 
        // Ppc1_MCTSPlayer to play with others, not itself.
        if (!silent) printf("Update with move\n");
        // printf("Update with move\n");
        search_tree.updateWithMove(b.lastMove());
        if (!silent) printf("get move\n");
        // printf("get move\n");
        // get next move
        int next_move = search_tree.getMove(b);
        search_tree.updateWithMove(next_move);
        return next_move;
    }

    std::string Ppc1_MCTSPlayer::PlayerInfo() {
        std::string color_info;
        switch (color) {
            case Board::kPlayerBlack:
                color_info = "Black[@]\n";
                break;
            case Board::kPlayerWhite:
                color_info = "White[O]\n";
                break;
            default:
                color_info = "None[+]\n";
        }

        std::string player_info = "-----\n[--Player Information--]\nName: ";
        player_info += name;
        player_info += "\nColor: ";
        player_info += color_info;
        player_info += "Compute Budget: ";
        player_info += std::to_string(search_tree.getComputeBudget());
        player_info += "\n\n";
        return player_info;
    }
    // ppc2 part
    Ppc2_MCTSPlayer::Ppc2_MCTSPlayer(int color, std::string name, float weight_c, 
                                   int compute_budget, double time_budget, bool silent, bool DEBUG)
    :Player(color, name), 
    search_tree(Ppc2_MonteCarloSearchTree(weight_c, compute_budget, time_budget, compute_budget/100,
                                         silent, 1000, MCTS_Expand_policy_fn,
                                         rollout_policy_fn, DEBUG)) {
        this->silent = silent;
    }

    int Ppc2_MCTSPlayer::getAction(Board &b) {
        // check color
        if (b.currentPlayerColor() != color) {
            throw std::runtime_error("The color of next move in board "
                                     "is not the same color of current player.\n");
        }

        // update the MCST with last move, because we only use 
        // Ppc2_MCTSPlayer to play with others, not itself.
        if (!silent) printf("Update with move\n");
        // printf("Update with move\n");
        search_tree.updateWithMove(b.lastMove());
        if (!silent) printf("get move\n");
        // printf("get move\n");
        // get next move
        int next_move = search_tree.getMove(b);
        search_tree.updateWithMove(next_move);
        return next_move;
    }

    std::string Ppc2_MCTSPlayer::PlayerInfo() {
        std::string color_info;
        switch (color) {
            case Board::kPlayerBlack:
                color_info = "Black[@]\n";
                break;
            case Board::kPlayerWhite:
                color_info = "White[O]\n";
                break;
            default:
                color_info = "None[+]\n";
        }

        std::string player_info = "-----\n[--Player Information--]\nName: ";
        player_info += name;
        player_info += "\nColor: ";
        player_info += color_info;
        player_info += "Compute Budget: ";
        player_info += std::to_string(search_tree.getComputeBudget());
        player_info += "\n\n";
        return player_info;
    }
    // ppc3 part
    Ppc3_MCTSPlayer::Ppc3_MCTSPlayer(int color, std::string name, float weight_c, 
                                   int compute_budget, double time_budget, bool silent, bool DEBUG)
    :Player(color, name), 
    search_tree(Ppc3_MonteCarloSearchTree(weight_c, compute_budget, time_budget, compute_budget/100,  
                                         silent, 1000, MCTS_Expand_policy_fn,
                                         rollout_policy_fn, DEBUG)) {
        this->silent = silent;
    }

    int Ppc3_MCTSPlayer::getAction(Board &b) {
        // check color
        if (b.currentPlayerColor() != color) {
            throw std::runtime_error("The color of next move in board "
                                     "is not the same color of current player.\n");
        }

        // update the MCST with last move, because we only use 
        // Ppc3_MCTSPlayer to play with others, not itself.
        printf("Update with move\n");
        search_tree.updateWithMove(b.lastMove());
        printf("get move\n");
        // get next move
        int next_move = search_tree.getMove(b);
        search_tree.updateWithMove(next_move);
        return next_move;
    }

    std::string Ppc3_MCTSPlayer::PlayerInfo() {
        std::string color_info;
        switch (color) {
            case Board::kPlayerBlack:
                color_info = "Black[@]\n";
                break;
            case Board::kPlayerWhite:
                color_info = "White[O]\n";
                break;
            default:
                color_info = "None[+]\n";
        }

        std::string player_info = "-----\n[--Player Information--]\nName: ";
        player_info += name;
        player_info += "\nColor: ";
        player_info += color_info;
        player_info += "Compute Budget: ";
        player_info += std::to_string(search_tree.getComputeBudget());
        player_info += "\n\n";
        return player_info;
    }
    






} // gomoku
