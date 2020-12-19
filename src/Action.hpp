#ifndef ACTION_HPP
#define ACTION_HPP

#include <iostream>

struct Action {
	int i;
	int j;

	friend std::ostream& operator<<(std::ostream& out,
		const Action& action);
	bool operator==(const Action& action) const;
};

#endif /* ACTION_HPP */
