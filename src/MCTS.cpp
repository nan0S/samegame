#include "MCTS.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#define all(x) (x).begin(), (x).end()

void MCTS::init(const State& state) {
    root = new Node();
    root->parent = nullptr;
    root->state = state;
    root->sum = 0;
    root->sum2 = 0;
    root->visits = 0;
    root->nextActionIdx = 0;
    root->actions = state.getActions();
    std::random_shuffle(all(root->actions));
    root->children.reserve(root->actions.size());
}

Action MCTS::chooseAction(const State& state, float timeLimit) {
    Timer timer(timeLimit);

    while (timer.isTimeLeft()) {
        Node* selected = selection();

        if (shouldExpand(selected)) {
            assert(!selected->state.terminal());
            selected = expansion(selected); 
        }
        else
            assert(selected->state.terminal());

        int result = simulation(selected);
        backpropagation(selected, result);
    }

    assert(!root->children.empty());
    int bestChildIdx = std::max_element(all(root->children), [](Node* n1, Node* n2){
        return n1->visits < n2->visits;
    }) - root->children.begin();

    assert(bestChildIdx < int(root->actions.size()));
    const auto action = root->actions[bestChildIdx];
    assert(state.canPress(action.i, action.j));

    root = root->children[bestChildIdx];
    root->parent = nullptr;

    return action;
}

Node* MCTS::selection() const {
    Node* node = root;
    while (!terminal(node) && !shouldExpand(node))
        node = select(node);
    return node;
}

bool MCTS::terminal(Node* node) const {
    return node->actions.empty();
}

bool MCTS::shouldExpand(Node* node) const {
    assert(node->nextActionIdx <= int(node->actions.size()));
    return node->nextActionIdx < int(node->actions.size());
}

Node* MCTS::select(Node* node) const {
    assert(!shouldExpand(node));
    for (const auto& child : node->children)
        assert(child->parent == node);
    auto selected = std::max_element(all(node->children), [this](Node* n1, Node* n2){
        return eval(*n1) < eval(*n2);
    });
    return *selected;
}

MCTS::eval_t MCTS::eval(const Node& n) const {
    assert(n.parent);
    assert(n.visits);
    const auto& p = *n.parent;
    assert(p.visits);

    eval_t avg = eval_t(n.sum) / n.visits;
    eval_t e = avg +
               C * std::sqrt(std::log(p.visits) / n.visits) +
               std::sqrt(eval_t(n.sum2 - n.visits * avg * avg + D) / n.visits);
    return e;
}

Node* MCTS::expansion(Node* node) const {
    assert(shouldExpand(node));

    Node* child = new Node();
    node->children.push_back(child);

    child->parent = node;
    const auto& action = node->actions[node->nextActionIdx++];
    child->state = node->state;
    assert(child->state.canPress(action.i, action.j));
    child->state.apply(action);
    child->sum = 0;
    child->sum2 = 0;
    child->visits = 0;
    child->nextActionIdx = 0;
    child->actions = child->state.getActions();
    std::random_shuffle(all(child->actions));
    child->children.clear();
    child->children.reserve(child->actions.size());

    return child;
}

int MCTS::simulation(Node* node) const {
    auto state = node->state;

    while (!state.terminal()) {
        auto actions = state.getActions();
        assert(!actions.empty());
        const auto& action = actions[rand() % actions.size()];
        state.apply(action);
    }

    return state.realscore;
}

void MCTS::backpropagation(Node* node, int result) const {
    while (node) {
        node->sum += result;
        node->sum2 += result * result;
        assert(node->sum > 0 && node->sum2 > 0);
        ++node->visits;
        node = node->parent;
    }
}
