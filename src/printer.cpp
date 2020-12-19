#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Remove connected regions of the same color to obtain the best score.
 **/

int main()
{

    // game loop
    while (1) {
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j++) {
                int color; // Color of the tile
                cin >> color; cin.ignore();
                cerr << color << " ";
            }
            cerr << endl;
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "3 6 Hello SameGame\\n:-)" << endl; // Selected tile "x y [message]".
    }
}
