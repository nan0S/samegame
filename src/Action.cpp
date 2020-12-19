#include "Action.hpp"

std::ostream& operator<<(std::ostream& out, const Action& action) {
	return out << action.i << " " << action.j;
}

bool Action::operator==(const Action& action) const {
	return i == action.i && j == action.j;
}
