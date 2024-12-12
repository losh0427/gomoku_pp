#!/bin/bash
# make clean
# make
MODE="$1"
echo "$MODE"
if [ "$MODE" = "1" ]; then
    ./bin/cppgomoku/pure_mcts_game 1 c p1
## mode 2:
## ./exe 2 [player1] [player2] [iter_time]
elif [ "$MODE" = "2" ]; then
    ./bin/cppgomoku/pure_mcts_game 2 c p1 2
## mode 3:
## ./exe 3 [0or1] [player1] [player2] [time_budget] [filename] (0 : do ont time , 1 : do to end)
elif [ "$MODE" = "3" ]; then
    ./bin/cppgomoku/pure_mcts_game 3 0 c c 2 ./testcase/01.txt
    # ./bin/cppgomoku/pure_mcts_game 3 1 c c 2 ./testcase/01.txt
else 
    echo "mode number needed"
fi
