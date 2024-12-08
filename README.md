# gomoku_pp
#### pp24_final ####

## Introduction
A game demo that explores advancements in parallel computing techniques applied to the game of Gomoku. This project demonstrates different parallelization strategies for computer players, showcasing performance and decision-making improvements.

### Game
To be continued.

### Human Player
To be continued.

### Computer Player
To be continued.

---

## Run

### Compile
To build the project, simply run the `make` command in the terminal:
```bash
make
```
This will compile all necessary source files and generate the executable binaries.

### Execute

#### Introduction to Player Types
There are several types of players supported in the game:
- **h**: Human Player  
  A player controlled through manual input.
- **c**: Pure MCTS Player  
  A computer player that uses the Monte Carlo Tree Search (MCTS) algorithm without parallelization.
- **p1**: Pthread MCTS Player  
  A computer player leveraging POSIX threads (pthreads) to parallelize the MCTS algorithm.
- **p2**: OpenMP MCTS Player  
  A computer player utilizing OpenMP to achieve parallelized MCTS computations.
- **p3**: MPI MCTS Player  
  A computer player designed for distributed computing, using MPI (Message Passing Interface) to run MCTS across multiple processes.

#### How to Execute

1. **Quick Demo (Computer vs. Computer)**  
   To see an immediate demonstration where two computer players compete against each other:
   ```bash
   ./run.sh
   ```

2. **Detailed Demo (Custom Player Setup)**  
   For more control over the game setup, you can run the executable directly:
   ```bash
   ./bin/cppgomoku/pure_mcts_game (player1) (player2)
   ```
   Replace `(player1)` and `(player2)` with the respective player types (e.g., `h`, `c`, `p1`, `p2`, `p3`).  
   Example:
   ```bash
   ./bin/cppgomoku/pure_mcts_game h c
   ```
   This command starts a game between a Human Player and a Pure MCTS Player.

---

## Reference
For more details and the original source of the game:
[gomoku]: https://github.com/lygztq/gomoku

