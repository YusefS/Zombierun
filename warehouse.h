#ifndef WAREHOUSE_H_INCLUDED
#define WAREHOUSE_H_INCLUDED

// Adjacency Matrices
// Leave diagonals blank, moving to the same position is not a valid move. It's not a "move"!
// One is an edge, zero is absence of an edge

const bool human_adjacency[11][11] = {
                            {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                            {1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
                            {1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0},
                            {1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0},
                            {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
                            {0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0},
                            {0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
                            {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
                            {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1},
                            {0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1},
                            {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0}
                        };

const bool zombie_adjacency[11][11] = {
                            {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
                            {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0},
                            {0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
                            {0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0},
                            {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
                            {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1},
                            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
                        };

// For example, zombie_adjacency[7][5] = false. You can't go from position 7 to position 5. But zombie_adjacency[5][7] = true; You can go forward.

/*
Game Board:

// We'll use indices 0 - 10 in the game code

    1--4--7
  / | \|/ | \
 0--2--5--8--10
  \ | /|\ | /
    3--6--9

*/

// Used for various things (needs to be type char???)
enum {ZOMBIES, HUMAN, GAME_ONGOING};

const int MAX_TURNS = 21; // When this many moves have passed, the Kid automatically wins

const char ZOMBIE_SYMBOL = 'Z';
const char HUMAN_SYMBOL = 'P';
const char EMPTY_SYMBOL = 'O';


class warehouse {
    private:
        unsigned char turn_count; // Starts at 1, move for the zombies. Even moves are then for the human.
        unsigned char state; // Win for zombies, human, or still ongoing game? There is no draw.
        unsigned char board[11]; // The game board

    public:
        unsigned char getState() {return state;}
        unsigned char calcState();
        unsigned char getTurnCount() {return turn_count;}
        unsigned char getWhoseTurn() {return (turn_count % 2 == 0 ? HUMAN : ZOMBIES);}

        bool makeMove(unsigned char, unsigned char); // Attempt to play from src index to destination index. Returns true if successful.
        void newGame(); // Clear out this instance
        void spawnHuman(); // Sets the 10th index with a human
        void spawnZombies(); // Fills the board with three zombies at correct indices.
        void spawnZombieHere(int index); // Spawns a zombie at the given location
        unsigned char * getBoard() {return board;} // Returns an array[11] of the game board, where a space can be EMPTY_SYMBOL, ZOMBIE_SYMBOL, or HUMAN_SYMBOL
        void printBoard(); // A textual visual

        warehouse();
        ~warehouse();

};

#endif // WAREHOUSE_H_INCLUDED
