/**

A clone of the ancient "Hounds and Hare" game.
Three zombies have you cornered in an abandoned warehouse. Can you escape?
They are slow and single-minded; they cannot move backwards (either directly or diagonally).
After 10 consecutive directly vertical moves, the zombies lose rather than drawing the game (currently commented out).

*/

#include <iostream>
#include "warehouse.h"

using namespace std;

void printGame();
void askDoMove();

warehouse W;

int main()
{
    W.spawnHuman();
    W.spawnZombies();

    while (W.getState() == GAME_ONGOING){
        printGame();
        askDoMove();
    }
    // Someone has won. Who?
    if (W.getState() == HUMAN) {
        cout << "The human escaped!" << endl;
    } else if (W.getState() == ZOMBIES) {
        cout << endl << "The zombies have caught the human!" << endl;
    }
    W.printBoard();
    cout << endl;
    return 0;
}

void askDoMove() {
    int src, dest;

    cout << "Move piece FROM where? ";
    cin >> src;
    cout << "Move piece TO where? ";
    cin >> dest;

    while (W.makeMove(src, dest) == false) {
        cout << "Invalid move." << endl;
        cout << "Move piece FROM where? ";
        cin >> src;
        cout << "Move piece TO where? ";
        cin >> dest;
    }
}

void printGame(){
    cout << "It is the " << (W.getWhoseTurn() == ZOMBIES ? "ZOMBIES" : "HUMAN") << "'s turn." << endl;
    W.printBoard();
    cout << endl;
}
