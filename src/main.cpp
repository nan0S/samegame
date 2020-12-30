#include "SameGame.hpp"
#include "BeamSearch.hpp"
#include "NMCS.hpp"
#include "MCTS.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);
    
    // using StrategyT = BeamSearch;
    using StrategyT = NMCS;
    // using StrategyT = MCTS;

	SameGame<StrategyT>::init();
	SameGame<StrategyT>::run();

	return 0;
}
