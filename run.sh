#!/bin/bash
make clean
make
MODE="$1"
THD_NUM=""

# 逐個解析參數
while [[ $# -gt 0 ]]; do
    case "$1" in
        -c)
            THD_NUM="-c $2"
            shift 2  # 移動兩個參數（-c 和它的值）
            ;;
        *)
            MODE="$1"
            shift  # 移動到下一個參數
            ;;
    esac
done

echo "MODE is: $MODE"
if [ "$MODE" = "1" ]; then
    ./bin/cppgomoku/pure_mcts_game 1 c p1 ${THD_NUM}
## mode 2:
## ./exe 2 [player1] [player2] [iter_time]
elif [ "$MODE" = "2" ]; then
    ./bin/cppgomoku/pure_mcts_game 2 c p1 2 ${THD_NUM}
## mode 3:
## ./exe 3 [0or1] [player1] [player2] [time_budget] [filename] (0 : do ont time , 1 : do to end)
elif [ "$MODE" = "3" ]; then
    ./bin/cppgomoku/pure_mcts_game 3 0 c c 2 ./testcase/01.txt ${THD_NUM}
    # ./bin/cppgomoku/pure_mcts_game 3 1 c c 2 ./testcase/01.txt
else 
    echo "mode number needed"
fi
