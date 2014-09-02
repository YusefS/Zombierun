/**

Defines the class WAREHOUSE, which represents the complete game state -- the world in which the players act.

*/

#include <iostream>
#include "warehouse.h"

warehouse::warehouse(){
    newGame();
}

// Unused
warehouse::~warehouse() {
}

void warehouse::newGame() {
    // Clear out the board
    for (int i = 0; i < 11; i++) {
        board[i] = EMPTY_SYMBOL;
    }

    turn_count = 1;
    state = GAME_ONGOING;
}

void warehouse::spawnZombies(){
    spawnZombieHere(0);
    spawnZombieHere(1);
    spawnZombieHere(3);
}

void warehouse::spawnZombieHere(int index) {
    board[index] = ZOMBIE_SYMBOL;
}

void warehouse::spawnHuman() {
    board[10] = HUMAN_SYMBOL;
}

/** If valid, makes a move, increments turn count. If not valid returns FALSE. Increments turn count if successful. */
bool warehouse::makeMove(unsigned char src, unsigned char dest) {
    // First check for array out of bounds
    if ((src < 0) || (src > 10) || (dest < 0) || (dest > 10)) {return false;}

    // Check if the move is legal
    const bool (*adjacencyMatrix)[11][11] = 0; // The pointer may still be changed, but cannot modify contents!!!
    if (getWhoseTurn() == ZOMBIES) {adjacencyMatrix = &zombie_adjacency;}
    else {adjacencyMatrix = &human_adjacency;}

    // Note that you MUST dereference WITH PARENTHESES! Otherwise GNU GCC interprets it differently...
    if ((*adjacencyMatrix)[src][dest] != true) {return false;} // Not a valid move for sure

    // Possibly a valid move, unless blocked by one of the other pieces.
    if (board[dest] != EMPTY_SYMBOL) {return false;}
    // The board must have a piece of the right type there.
    if (board[src] != ZOMBIE_SYMBOL && getWhoseTurn() == ZOMBIES) {return false;}
    if (board[src] != HUMAN_SYMBOL && getWhoseTurn() == HUMAN) {return false;}

    // Make the move and return true
    board[dest] = board[src];
    board[src] = EMPTY_SYMBOL;
    turn_count++;
    // Check status...did the game end?
    calcState();
    return true;
}


/** Determines (recalculates) the current state of the game: win for HUMAN, win for ZOMBIES, or GAME_ONGOING. */
unsigned char warehouse::calcState(){
    // If the human gets to position 0, humans win.
    if (board[0] == HUMAN_SYMBOL) {state = HUMAN; return state;}

    // If the human cannot move because he is surrounded on all sides, then zombies win. This can only happen in 3 ways:
    // We need access to the board, zombies are indistinguishable.
    if ((board[10] == HUMAN_SYMBOL) && (board[7] == ZOMBIE_SYMBOL) && (board[8] == ZOMBIE_SYMBOL) && (board[9] == ZOMBIE_SYMBOL)) {state = ZOMBIES; return state;}
    if ((board[4] == HUMAN_SYMBOL) && (board[1] == ZOMBIE_SYMBOL) && (board[5] == ZOMBIE_SYMBOL) && (board[7] == ZOMBIE_SYMBOL)) {state = ZOMBIES; return state;}
    if ((board[6] == HUMAN_SYMBOL) && (board[3] == ZOMBIE_SYMBOL) && (board[5] == ZOMBIE_SYMBOL) && (board[9] == ZOMBIE_SYMBOL)) {state = ZOMBIES; return state;}

    // If the turn count gets too high, end the game with a win for the human
    if (turn_count > MAX_TURNS) {state = HUMAN; return state;}

    // Otherwise, no win yet.
    state = GAME_ONGOING;
    return state;
}

/** Prints a simple textual representation to the screen. */
void warehouse::printBoard() {
    // Print a Z for zombie, P for person, - (dash) otherwise.
    std::cout << "    " << board[1] << "--" << board[4] << "--" << board[7] << "    " << std::endl; // First row
    std::cout << "  / | \\|/ | \\ " << std::endl; // Second row
    std::cout << " " << board[0] << "--" << board[2] << "--" << board[5] << "--" << board[8] << "--" << board[10] << std::endl; // Third
    std::cout << "  \\ | /|\\ | / " << std::endl; // Fourth row
    std::cout << "    " << board[3] << "--" << board[6] << "--" << board[9] << "    " << std::endl; // Final row
/*
    1--4--7
  / | \|/ | \
 0--2--5--8--10
  \ | /|\ | /
    3--6--9
*/
}
