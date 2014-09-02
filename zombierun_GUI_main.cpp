#include <iostream>
#include <sstream>
#include <math.h> // For sine
#include "Mercury.h"
#include "HgTexture.h"
#include "warehouse.h"
#include "zombierun_GUI_main.h"

void draw();
void update(int delta);
void newGame();
void loadMedia();
void unloadMedia();
void myLeftClick(int x, int y);
void spawnZombie(int index); // Spawns and plays a sound
string itos(int n);

MercuryEngine gMerc;

// Images
SDL_Color sdlMagenta = {255, 0, 255, 255}; // Don't forget to put the alpha at 255
SDL_Color sdlZombieColor = {255, 0, 0};
SDL_Color sdlKidColor = {0, 200, 255};
HgTexture hgCircle;
HgTexture hgZombie;
HgTexture hgKid;
HgTexture hgBack; // Background
HgTexture hgWall; // Brick wall
HgTexture hgExit; // Exit sign

// Font
TTF_Font * ttfFear = NULL;

// Text Images
HgTexture hgZombieTurn;
HgTexture hgKidsTurn;
HgTexture hgZombieWin;
HgTexture hgKidWins;
HgTexture hgEscape;
HgTexture hgEscapeTime;

// Audio
Mix_Music * musRavenous = NULL;
Mix_Chunk * mixRoar = NULL;
Mix_Chunk * mixOkay = NULL;
Mix_Chunk * mixDone = NULL;
Mix_Chunk * mixSelect = NULL;
Mix_Chunk * mixNoSelect = NULL;

// The MODEL of the game
warehouse wGame;

// Which space is selected by the player?
int iSelected = -1; // None selected = -1. Otherwise from 0 to 10
float fPulse = 0.0f; // This is going to allow our selection to pulse. It represents an angle, in radians (2*pi per circle)
int iZombieTimer = -1; // This spawns the zombies

int main(int argc, char * argv[]) {
    MercuryInfoStruct mInfo;
    mInfo.sTitle = "Zombie Run";
    mInfo.width = 800;
    mInfo.height = 600;
    mInfo.draw = &draw;
    mInfo.update = &update;
    mInfo.leftClick = &myLeftClick;
    gMerc.initialize(&mInfo);

    loadMedia();

    newGame();

    // For color-keying to work
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gMerc.activate();

    //==================//

    Mix_HaltMusic();

    unloadMedia();

    gMerc.deinitialize();
    return 0;
}

void newGame() {
    wGame.newGame();

    hgEscapeTime.becomeRenderedString(itos(MAX_TURNS - wGame.getTurnCount() + 1), ttfFear, sdlKidColor); // Reset turn counter

    // Stop any previous music, audio
    Mix_HaltChannel(-1);
    Mix_HaltMusic();

    wGame.spawnHuman();
    // I want to spawn the zombies after a delay
    iZombieTimer = 0; // Start counting!

    // Play that music! Loop eternally
    Mix_PlayMusic(musRavenous, -1);
}

void loadMedia() {
    hgCircle.loadColorKeyedImage("resources\\ring_01.bmp", sdlMagenta);
    hgZombie.loadColorKeyedImage("resources\\zombie_02.bmp", sdlMagenta);
    hgKid.loadColorKeyedImage("resources\\kid_02.bmp", sdlMagenta);
    hgBack.loadImage("resources\\background.png");
    hgWall.loadColorKeyedImage("resources\\wall.bmp", sdlMagenta);
    hgExit.loadColorKeyedImage("resources\\exit.bmp", sdlMagenta);

    // Font
    ttfFear = TTF_OpenFont("resources\\Face Your Fears.ttf", 24);

    // Text Images
    hgZombieTurn.becomeRenderedString("Zombie's Turn", ttfFear, sdlZombieColor);
    hgZombieWin.becomeRenderedString("Zombie's Win!", ttfFear, sdlZombieColor);
    hgEscape.becomeRenderedString("turns until sunrise", ttfFear, sdlKidColor);

    hgKidsTurn.becomeRenderedString("Kid's Turn", ttfFear, sdlKidColor);
    hgKidWins.becomeRenderedString("The Kid Escapes!", ttfFear, sdlKidColor);

    musRavenous = Mix_LoadMUS("resources\\Ravenous.mp3");
    if (musRavenous == NULL) {cout << "Failed to load Ravenous.mp3!" << Mix_GetError() << endl;}

    mixRoar = Mix_LoadWAV("resources\\zombie_roar.wav");
    mixDone = Mix_LoadWAV("resources\\done.wav");
    mixOkay = Mix_LoadWAV("resources\\okay.wav");
    mixSelect = Mix_LoadWAV("resources\\select.wav");
    mixNoSelect = Mix_LoadWAV("resources\\unselect.wav");
}

void unloadMedia() {
    hgCircle.unload();
    hgZombie.unload();
    hgKid.unload();
    hgBack.unload();
    hgWall.unload();
    hgExit.unload();

    TTF_CloseFont(ttfFear); ttfFear = NULL;

    hgZombieTurn.unload();
    hgZombieWin.unload();
    hgKidsTurn.unload();
    hgKidWins.unload();
    hgEscape.unload();
    hgEscapeTime.unload();

    Mix_FreeMusic(musRavenous); musRavenous = NULL;
    Mix_FreeChunk(mixRoar); mixRoar = NULL;
    Mix_FreeChunk(mixDone); mixDone = NULL;
    Mix_FreeChunk(mixOkay); mixOkay = NULL;
    Mix_FreeChunk(mixSelect); mixSelect = NULL;
    Mix_FreeChunk(mixNoSelect); mixNoSelect = NULL;
}

void myLeftClick(int x, int y) {
    unsigned char * B = wGame.getBoard(); // Get the board

    if (wGame.getState() != GAME_ONGOING) {
        // Reset the game
        newGame();
        return;
    }

    int index = -1;
    for (int i = 0; i < 11; i++) {
        // Are we in the radius of the circle?
        int dx = (CIRCLE_POS[i][0] - x);
        int dy = (CIRCLE_POS[i][1] - y);
        if ((dx*dx + dy*dy) < (64*64)) {index = i;} // We have clicked on an important spot! A circle. Let's save that index into which circle
    }

    if (index == -1) {iSelected = -1; Mix_PlayChannel(-1, mixNoSelect, 0); } // If we have not clicked on a circle, unselect.
    else {
        // Select source
        if (iSelected == -1) {
            // If nothing is selected, select something!
            // Yes, we clicked here, but is it a valid selection?
            if (((wGame.getWhoseTurn() == ZOMBIES) && (B[index] == ZOMBIE_SYMBOL)) || ((wGame.getWhoseTurn() == HUMAN) && (B[index] == HUMAN_SYMBOL))) {
                iSelected = index;
                // Play that sound
                Mix_PlayChannel(-1, mixSelect, 0);
            } else {
                // Not a valid source
                Mix_PlayChannel(-1, mixNoSelect, 0);
            }
        }
        // Select destination
        else {
            // Actually, we've already clicked and selected. Now we're selecting a place to go

            if (wGame.makeMove(iSelected, index)) {
                // Play a sound, but only if was a valid move!
                // Notice that the current turn will be the NEXT player, so we're checking the reverse logic here.
                if (wGame.getWhoseTurn() == HUMAN) {Mix_PlayChannel(-1, mixRoar, 0); Mix_PlayChannel(-1, mixSelect, 0);}
                else if (wGame.getWhoseTurn() == ZOMBIES) {Mix_PlayChannel(-1, mixOkay, 0); Mix_PlayChannel(-1, mixSelect, 0);}

                // A turn has been taken, update the turn count display
                hgEscapeTime.becomeRenderedString(itos(MAX_TURNS - wGame.getTurnCount() + 1), ttfFear, sdlKidColor);

            } else {
                // It was invalid
                Mix_PlayChannel(-1, mixNoSelect, 0);
            }

            // If we've now won, stop music
            if (wGame.getState() != GAME_ONGOING) {
                Mix_HaltChannel(-1);
                Mix_HaltMusic();
                Mix_PlayChannel(-1, mixDone, 0);
            }

            iSelected = -1; // And unselect now
        }
    }

}

// This is called at FPS. Delta in milliseconds
void update(int delta) {
    fPulse += 0.01*delta;
    if (fPulse > (2*3.1416)) {fPulse = 0;}

    static int iZombiesSpawned = 0;
    // Check on the timer. -1 means forget about it, we're not counting.
    if (iZombieTimer > -1) {
        iZombieTimer += delta;
        if ((iZombieTimer >= 750) && (iZombiesSpawned == 0)) {spawnZombie(2); iZombiesSpawned++;}
        if ((iZombieTimer >= 1250) && (iZombiesSpawned == 1)) {spawnZombie(1); iZombiesSpawned++;}
        if ((iZombieTimer >= 1750) && (iZombiesSpawned == 2)) {
                spawnZombie(3); // Spawn the last zombie
                iZombieTimer = -1; // Stop timing
                iZombiesSpawned = 0; // Reset
        }
    }

}

void draw() {

    glColor3f(1, 1, 1);
    hgBack.render(0, 0); // Background image

    switch (wGame.getState()) {
    case GAME_ONGOING:
        if (wGame.getWhoseTurn() == ZOMBIES) {hgZombieTurn.render(16, 16);}
        else {hgKidsTurn.render(650, 16);}
        break;
    case ZOMBIES:
        hgZombieWin.render(16, 16);
        break;
    case HUMAN:
        hgKidWins.render(600, 16);
        break;
    default:
        break;
    }

    // How many turns until the Kid auto-wins.
    if (MAX_TURNS - wGame.getTurnCount() < 5) {
        // pulse
        glColor3f(0.5*sin(fPulse)+0.5, 0.1*sin(fPulse)+0.5, 0.5*sin(fPulse)+0.5);
    }
    hgEscapeTime.render(24, 560);
    hgEscape.render(64, 560);

    // Draw the connecting lines
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_LINES);
        glColor3f(1, 1, 0); // Yellow
        glVertex2f(CIRCLE_POS[0][0], CIRCLE_POS[0][1]); glVertex2f(CIRCLE_POS[10][0], CIRCLE_POS[10][1]); // Long horizontal
        glVertex2f(CIRCLE_POS[1][0], CIRCLE_POS[1][1]); glVertex2f(CIRCLE_POS[7][0], CIRCLE_POS[7][1]); // Top horizontal
        glVertex2f(CIRCLE_POS[3][0], CIRCLE_POS[3][1]); glVertex2f(CIRCLE_POS[9][0], CIRCLE_POS[9][1]); // Bottom horizontal

        glVertex2f(CIRCLE_POS[1][0], CIRCLE_POS[1][1]); glVertex2f(CIRCLE_POS[3][0], CIRCLE_POS[3][1]); // Left vertical
        glVertex2f(CIRCLE_POS[4][0], CIRCLE_POS[4][1]); glVertex2f(CIRCLE_POS[6][0], CIRCLE_POS[6][1]); // Middle
        glVertex2f(CIRCLE_POS[7][0], CIRCLE_POS[7][1]); glVertex2f(CIRCLE_POS[9][0], CIRCLE_POS[9][1]); // Right

        glVertex2f(CIRCLE_POS[0][0], CIRCLE_POS[0][1]); glVertex2f(CIRCLE_POS[1][0], CIRCLE_POS[1][1]); // Top left diag
        glVertex2f(CIRCLE_POS[0][0], CIRCLE_POS[0][1]); glVertex2f(CIRCLE_POS[3][0], CIRCLE_POS[3][1]); // Bottom left diag

        glVertex2f(CIRCLE_POS[7][0], CIRCLE_POS[7][1]); glVertex2f(CIRCLE_POS[10][0], CIRCLE_POS[10][1]); // Top right diag
        glVertex2f(CIRCLE_POS[9][0], CIRCLE_POS[9][1]); glVertex2f(CIRCLE_POS[10][0], CIRCLE_POS[10][1]); // Bottom right diag

        glVertex2f(CIRCLE_POS[7][0], CIRCLE_POS[7][1]); glVertex2f(CIRCLE_POS[3][0], CIRCLE_POS[3][1]); //
        glVertex2f(CIRCLE_POS[9][0], CIRCLE_POS[9][1]); glVertex2f(CIRCLE_POS[1][0], CIRCLE_POS[1][1]); //
    glEnd();

    glColor3f(1, 1, 1); // Back to WHITE

    // Draw the game board as a series of circles
    unsigned char * B = wGame.getBoard(); // Get a pointer to the game board
    for (int i = 0; i < 11; i++) {

        if (iSelected == i) {glColor3f(0.5*sin(fPulse)+0.5, 0.1*sin(fPulse)+0.5, 0.5*sin(fPulse)+0.5);} // Pulsing Greenish if selected
        else {glColor3f(1, 1, 1);} // White otherwise


        if (B[i] == ZOMBIE_SYMBOL) {
                hgZombie.renderCentered(CIRCLE_POS[i][0], CIRCLE_POS[i][1]); // YES! You can use a pointer as an array like this! You have to know the size...
                glColor3f(sdlZombieColor.r / 255.0, sdlZombieColor.g / 255.0, sdlZombieColor.b / 255.0);
        }
        else if (B[i] == HUMAN_SYMBOL) {
            hgKid.renderCentered(CIRCLE_POS[i][0], CIRCLE_POS[i][1]);
            glColor3f(sdlKidColor.r / 255.0, sdlKidColor.g / 255.0, sdlKidColor.b / 255.0);
        }
        else {
            glColor3f(1, 1, 1); // White
            // draw the brick wall
            if (i == 0) {hgExit.renderCentered(CIRCLE_POS[i][0], CIRCLE_POS[i][1]);} // The first spot is the exit
            else {hgWall.renderCentered(CIRCLE_POS[i][0], CIRCLE_POS[i][1]);}
        }

        // This circle will be rendered according to the if statements above, color-wise
        hgCircle.renderCentered(CIRCLE_POS[i][0], CIRCLE_POS[i][1]); // Outer ring
    }
}

void spawnZombie(int index) {
    wGame.spawnZombieHere(index);
    Mix_PlayChannel(-1, mixRoar, 0); // Zombie sound
}

// Converts an integer into a string
string itos(int n) {
    ostringstream os;
    os << n;
    return os.str();
}


