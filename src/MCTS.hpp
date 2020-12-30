#ifndef MCTS_HPP
#define MCTS_HPP

#include "Strategy.hpp"
#include <vector>

struct Node {
    Node* parent = nullptr;
    State state;
    double sum = 0;
    double sum2 = 0;
    int visits = 0;
    int nextActionIdx = 0;
    std::vector<Action> actions;
    std::vector<Node*> children;
};

class MCTS : public Strategy {
public:
    using eval_t = double;

    void init(const State& state) override;
    Action chooseAction(const State& state, float timeLimit) override; 

private:
    Node* selection() const;
    bool terminal(Node* node) const;
    bool shouldExpand(Node* node) const;
    Node* select(Node* node) const;
    eval_t eval(const Node& node) const;
    Node* expansion(Node* node) const;
    int simulation(Node* node) const;
    void backpropagation(Node* node, int result) const;

private:
    Node* root;
    static constexpr eval_t C = 0.1;
    static constexpr eval_t D = 500000;
};
    
#endif /* MCTS_HPP */
