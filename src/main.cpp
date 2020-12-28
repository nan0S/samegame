#include "SameGame.hpp"
#include "BeamSearch.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);
    
    using StrategyT = BeamSearch;

	SameGame<StrategyT>::init();
	SameGame<StrategyT>::run();

	return 0;
}
