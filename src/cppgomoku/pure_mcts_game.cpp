#include "cppgomoku/common.h"
#include "cppgomoku/player.h"
#include "cppgomoku/board.h"
#include "cppgomoku/game_server.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace gomoku;

bool define_player(Player *&p, string& type, int color, double time_budget = 2.0,bool silent = false, bool DEBUG = false) {
    // human player : "h", computer player : "c", pp1 : "p1", pp2 : "p2", pp3 : "p3"
    if (type == "p1") {
        p = new Ppc1_MCTSPlayer(color, "Ppc1 MCTS player", 10.0, 80000, time_budget, silent, DEBUG); 
    } else if (type == "p2") {
        p = new Ppc2_MCTSPlayer(color, "Ppc2 MCTS player", 10.0, 80000, time_budget, silent, DEBUG); 
    } else if (type == "p3") {
        p = new Ppc3_MCTSPlayer(color, "Ppc3 MCTS player", 10.0, 80000, time_budget, silent, DEBUG); 
    } else if (type == "h") {
        p = new HumanPlayer(color, "Human Player", 'S');
    } else if (type == "c") {
        p = new PureMCTSPlayer(color, "Pure MCTS player", 10.0, 80000, time_budget, silent, DEBUG); 
    } else {
        printf("Invalid player type: %s\n", type.c_str());
        return false;
    }
    return true;
}

void auto_mode(Player* player1, Player* player2, int iteration_time) {
    if (dynamic_cast<HumanPlayer*>(player1) || dynamic_cast<HumanPlayer*>(player2)) {
        printf("Auto mode does not support human players.\n");
        return;
    }

    int white_wins = 0, black_wins = 0, no_results = 0;
    int white_moves = 0, black_moves = 0;


    for (int i = 0; i < iteration_time; ++i) {
        Board board(9, 9, 5);
        GameServer gs(&board, player1, player2, true);


        // gs.reset();
        int winner = gs.startGame(2,0);

        if (winner == Board::kPlayerWhite) {
            white_wins++;
            white_moves += board.getmMoved().size() / 2;
        } else if (winner == Board::kPlayerBlack) {
            black_wins++;
            black_moves += (board.getmMoved().size() + 1) / 2;
        } else {
            no_results++;
        }
        // // show getmMoved
        vector<int> moves = board.getmMoved();
        // for (int i = 0; i < moves.size(); ++i) {
        //     cout << moves[i] << " ";
        // }
        // // show size
        // cout << "size: " << moves.size() << endl;
        // show white_moves, black_moves
        cout << "white_moves: " << moves.size() / 2 << " black_moves: " << (moves.size() + 1) / 2 << endl;

        
        printf("Game %d result: %s\n", i + 1, (winner == Board::kPlayerWhite ? "White wins" : winner == Board::kPlayerBlack ? "Black wins" : "No result"));
    }

    int total_games = iteration_time;
    printf("\nSummary for %d games:\n", iteration_time);
    printf("White wins: %d, Black wins: %d, No result: %d\n", white_wins, black_wins, no_results);
    printf("White win rate: %.2f%%, Average moves when White wins: %.2f\n", 
           100.0 * white_wins / total_games, white_wins ? static_cast<double>(white_moves) / white_wins : 0.0);
    printf("Black win rate: %.2f%%, Average moves when Black wins: %.2f\n", 
           100.0 * black_wins / total_games, black_wins ? static_cast<double>(black_moves) / black_wins : 0.0);
}

void board_mode(Player* player1, Player* player2, const string& filepath, int stop_after_one, double time_budget) {
    ifstream file(filepath);
    if (!file.is_open()) {
        printf("Failed to open file: %s\n", filepath.c_str());
        return;
    }

    int width, height, number_to_win;
    file >> width >> height >> number_to_win;

    Board board(height, width, number_to_win);
    int move;

    while (file >> move) {
        board.play(move);
    }

    file.close();

    GameServer gs(&board, player1, player2, stop_after_one == 0);
    if (stop_after_one == 0) {
        // printf("Probability distribution (DEBUG = 1):\n");
        // Add probability distribution display here if applicable
        // DEBUG = 1;
        int winner = gs.startGame(3, 0);
        printf("Move result: %s\n", (winner == Board::kPlayerWhite ? "White wins" : winner == Board::kPlayerBlack ? "Black wins" : "No result"));
        // show Probability distribution

    } else {
        gs.startGame(3,1);
    }
}

int thd_num = 1;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./.exe [mode] [player1] [player2] ...\n");
        return 0;
    }
    // DEBUG = 0;
    string mode = argv[1];
    // 1 - normal, 2 - auto, 3 - board
    
    char **thd_arg_pos = find(argv, argv + argc, string("-c"));
    if(thd_arg_pos != (argv + argc)) {
        thd_num = atoi(*(thd_arg_pos + 1));
    }
    printf("thd_num = %d\n", thd_num);

    if (mode == "1") {
        if (argc < 4) {
            printf("Normal mode requires two players!\n");
            return 0;
        }

        string player1_type = argv[2];
        string player2_type = argv[3];
        Player *player1;
        Player *player2;
        // DEBUG = 0;
        if (!define_player(player1, player1_type, Board::kPlayerBlack)) return 0;
        if (!define_player(player2, player2_type, Board::kPlayerWhite)) return 0;

        Board b(9, 9, 5);
        GameServer gs(&b, player1, player2, false);
        int winner = gs.startGame(1,0);
        // // show getmMoved
        // vector<int> moves = b.getmMoved();
        // for (int i = 0; i < moves.size(); ++i) {
        //     cout << moves[i] << " ";
        // }
        printf("Move result: %s\n", (winner == Board::kPlayerWhite ? "White wins" : winner == Board::kPlayerBlack ? "Black wins" : "No result"));




        delete player1;
        delete player2;

    } else if (mode == "2") {
        if (argc < 5) {
            printf("Auto mode requires two players and iteration time!\n");
            return 0;
        }
        // DEBUG = 0;
        string player1_type = argv[2];
        string player2_type = argv[3];
        int iteration_time = stoi(argv[4]);
        Player *player1;
        Player *player2;

        if (!define_player(player1, player1_type, Board::kPlayerBlack)) return 0;
        if (!define_player(player2, player2_type, Board::kPlayerWhite)) return 0;

        auto_mode(player1, player2, iteration_time);

        delete player1;
        delete player2;

    } else if (mode == "3") {
        if (argc < 7) {
            printf("Board mode requires [0or1] [player1] [player2] [time] [file path] !\n");
            return 0;
        }

        int stop_after_one = stoi(argv[2]);
        string player1_type = argv[3];
        string player2_type = argv[4];
        double time_budget = stod(argv[5]);
        string filepath = argv[6];
        Player *player1, *player2;

        if (!define_player(player1, player1_type, Board::kPlayerBlack, time_budget, true, true)) return 0;
        if (!define_player(player2, player2_type, Board::kPlayerWhite, time_budget, true, true)) return 0;

        board_mode(player1, player2, filepath, stop_after_one, time_budget);

        delete player1;
        delete player2;
    } else {
        printf("Invalid mode: %s\n", mode.c_str());
    }

    return 0;
}
