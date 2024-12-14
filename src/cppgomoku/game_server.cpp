#include "cppgomoku/game_server.h"
#include <stdexcept>


namespace gomoku
{
    GameServer::GameServer(Board *b, Player *p1, Player *p2, bool silent) {
        board = b;
        player1 = p1;
        player2 = p2;
        this->silent = silent;
    }

    void GameServer::showGameInfo() {
        if (player1) {
            printf("[--Player 1--]:\n------\n");
            printf("%s", player1->PlayerInfo().c_str());
            printf("\n");
        }

        if (player2) {
            printf("[--Player 2--]:\n------\n");
            printf("%s", player2->PlayerInfo().c_str());
            printf("\n");
        }

        board->printBoard();
    }

    int GameServer::startGame(int mode = 1, int flag = 0) {
        if (mode != 3){
            board->initBoard(player1->getColor());  
        }
        Player * color_to_player[2];
        color_to_player[player1->getColor()] = player1;
        color_to_player[player2->getColor()] = player2;

        if (!silent) showGameInfo();

        int next_move, winner_color = Board::kPlayerEmpty;
        while (true) {
            Player *current_player = color_to_player[board->currentPlayerColor()];
            if (mode != 2){
                printf("Before get action\n");
            }
            next_move = current_player->getAction(*board);
            if (mode != 2){
                printf("After get action\n");
            }
            if (!board->isValidMove(next_move)) {
                printf("Invalid move %d\n", next_move);
                throw std::runtime_error("Invalid move!");
            }
            board->play(next_move);
            // [debug]show mMoved
            std::vector<int> moves = board->getmMoved();
            for (int i = 0; i < moves.size(); ++i) {
                printf("%d ", moves[i]);
            }
            printf("\n");

            if (!silent) showGameInfo();
            bool is_end = board->gameEnd(winner_color);
            if (is_end) {
                if (!silent) {
                    if (winner_color != Board::kPlayerEmpty) 
                        printf("Game end with winner %s(color %c)\n", 
                                color_to_player[winner_color]->getName().c_str(), 
                                Board::stone(winner_color));
                    else 
                        printf("Game end with no winner.\n");
                }
                return winner_color;
            }else if (mode == 3 && flag == 0) {
                return winner_color;
            }
        }
    }
} // gomoku
