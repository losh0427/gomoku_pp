#!/bin/bash
make clean
make
./bin/cppgomoku/pure_mcts_game 1 c c

## mode 2:
## ./exe 2 [player1] [player2] [iter_time]
# ./bin/cppgomoku/pure_mcts_game 2 c c 2



## mode 3:
## ./exe 3 [0or1] [player1] [player2] [time_budget] [filename] (0 : do ont time , 1 : do to end)
# ./bin/cppgomoku/pure_mcts_game 3 0 c c 2 ./testcase/01.txt
# ./bin/cppgomoku/pure_mcts_game 3 1 c c 2 ./testcase/01.txt

