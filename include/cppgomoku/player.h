#ifndef GOMUKU_PLAYER_H
#define GOMUKU_PLAYER_H

#include <string>
#include "cppgomoku/common.h"
#include "cppgomoku/board.h"
#include "cppgomoku/mcts.h"


namespace gomoku
{
    std::vector<MoveProbPair> rollout_policy_fn(Board &board); 
    std::vector<MoveProbPair> MCTS_Expand_policy_fn(Board &board);

    class Player {
    protected:
        int color;
        std::string name;
    public:
        Player() {}
        Player(int color, std::string name) {this->color = color; this->name = name;}
        virtual ~Player() {}
        virtual int getAction(Board &board) = 0;
        virtual std::string PlayerInfo() = 0; 
        int getColor() {return color;}
        void setColor(int color) {this->color = color;}
        std::string getName() {return name;}
        void setName(std::string name) {this->name = name;}
    };

    class HumanPlayer : public Player {
    private:
        HumanPlayerInputMode input_mode;
    public:
        HumanPlayer(int color, std::string name="John Doe(Human)", char input_mode='S');
        ~HumanPlayer(){}
        int getActionFromGUI(Board &board);
        int getActionFromStdin(Board &board);
        int getAction(Board &board);
        std::string PlayerInfo();
    };

    class PureMCTSPlayer : public Player {
    private:
        bool silent;
        PureMonteCarloSearchTree search_tree;
    public:
        PureMCTSPlayer(int color, std::string name="Pure MCTS player", float weight_c=10, 
                       int compute_budget=1E4, float time_budget = 5.0,bool silent=false);
        ~PureMCTSPlayer(){}
        void reset() {search_tree.reset();}
        int getAction(Board &board);
        std::string PlayerInfo();
        void setSilent() {silent = true; search_tree.setSilent();}
        void unsetSilent() {silent = false; search_tree.unsetSilent();}
    };
    
    // pp part 
    class Ppc1_MCTSPlayer : public Player {
    private:
        bool silent;
        Ppc1_MonteCarloSearchTree search_tree;
    public:
        Ppc1_MCTSPlayer(int color, std::string name="Ppc1_MCTSPlayer", float weight_c=10, 
                       int compute_budget=1E4, bool silent=false);
        ~Ppc1_MCTSPlayer(){}
        void reset() {search_tree.reset();}
        int getAction(Board &board);
        std::string PlayerInfo();
        void setSilent() {silent = true; search_tree.setSilent();}
        void unsetSilent() {silent = false; search_tree.unsetSilent();}
    };

    class Ppc2_MCTSPlayer : public Player {
    private:
        bool silent;
        Ppc2_MonteCarloSearchTree search_tree;
    public:
        Ppc2_MCTSPlayer(int color, std::string name="Ppc2_MCTSPlayer", float weight_c=10, 
                       int compute_budget=1E4, bool silent=false);
        ~Ppc2_MCTSPlayer(){}
        void reset() {search_tree.reset();}
        int getAction(Board &board);
        std::string PlayerInfo();
        void setSilent() {silent = true; search_tree.setSilent();}
        void unsetSilent() {silent = false; search_tree.unsetSilent();}
    };
    class Ppc3_MCTSPlayer : public Player {
    private:
        bool silent;
        Ppc3_MonteCarloSearchTree search_tree;
    public:
        Ppc3_MCTSPlayer(int color, std::string name="Ppc3_MCTSPlayer", float weight_c=10, 
                       int compute_budget=1E4, bool silent=false);
        ~Ppc3_MCTSPlayer(){}
        void reset() {search_tree.reset();}
        int getAction(Board &board);
        std::string PlayerInfo();
        void setSilent() {silent = true; search_tree.setSilent();}
        void unsetSilent() {silent = false; search_tree.unsetSilent();}
    };
    


    
} // gomoku

#endif // GOMUKU_PLAYER_H