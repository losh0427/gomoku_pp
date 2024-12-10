#include "cppgomoku/common.h"
#include "cppgomoku/player.h"
#include "cppgomoku/board.h"
#include "cppgomoku/game_server.h"
using namespace std;
using namespace gomoku;

bool define_player(Player *&p, string& type, int color) {
    // human player : "h", computer player : "c", pp1 : "p1", pp2 : "p2", pp3 : "p3"
    if (type == "p1") {
        p = new Ppc1_MCTSPlayer(color, "Ppc1 MCTS player", 10.0, 80000); 
    } else if (type == "p2") {
        p = new Ppc2_MCTSPlayer(color, "Ppc2 MCTS player", 10.0, 80000); 
    } else if (type == "p3") {
        p = new Ppc3_MCTSPlayer(color, "Ppc3 MCTS player", 10.0, 80000); 
    } else if (type == "h") {
        p = new HumanPlayer(color, "Human Player", 'S');
    } else if (type == "c") {
        p = new PureMCTSPlayer(color, "Pure MCTS player", 10.0, 80000, 5.0); 
    } else {
        printf("Invalid player type: %s\n", type.c_str());
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    // parse parameters
    // first parameter is the type of player1, 'h' for human, 'c' for computer.
    // second parameter is the type of player2 'h' for human, 'c' for computer.
    if (argc < 3) {
        printf("At least two parameters!\n");
        return 0;
    } 
    string player1_type = argv[1];
    string player2_type = argv[2];
    Player *player1;
    Player *player2;

    // define player    
    if (!define_player(player1, player1_type, Board::kPlayerBlack)) return 0;
    if (!define_player(player2, player2_type, Board::kPlayerWhite)) return 0;

    Board b(9,9,5);
    GameServer gs(&b, player1, player2, false);
    gs.startGame();

    delete player1;
    delete player2;

    return 0;
}