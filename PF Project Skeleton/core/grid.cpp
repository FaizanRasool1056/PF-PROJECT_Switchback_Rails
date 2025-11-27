#include "grid.h"
#include "simulation_state.h"
#include<iostream>
using namespace std;




char grid[100][100];
char originalTiles[100][100];
int rows;
int cols;




// ============================================================================
// GRID.CPP - Grid utilities
// ============================================================================

// ----------------------------------------------------------------------------
// Check if a position is inside the grid.
// ----------------------------------------------------------------------------
// Returns true if x,y are within bounds.
// ----------------------------------------------------------------------------
bool isInBounds(int row, int col) {
if (row>=0&&row<rows&&col>=0&&col<cols){//rows and cols are total no  of rows and  columns use in the grid
return true;}
else{
return false;}
}


// ----------------------------------------------------------------------------
// Check if a tile is a track tile.
// ----------------------------------------------------------------------------
// Returns true if the tile can be traversed by trains.
// ----------------------------------------------------------------------------
bool isTrackTile(char tile) {
if(tile == '-' || tile == '|' || tile == '/' || tile == '\\' || tile == '+' || tile == '=' || tile == 'S' || tile == 'D' || (tile >= 'A' && tile <= 'Z')) {
    return true;
}
return false;
}

// ----------------------------------------------------------------------------
// Check if a tile is a switch.
// ----------------------------------------------------------------------------
// Returns true if the tile is 'A'..'Z'.
// ----------------------------------------------------------------------------
bool isSwitchTile(char tile) {
    if(tile>='A'&&tile<='Z')
    return true;
    else
    return false;
}

// ----------------------------------------------------------------------------
// Get switch index from character.
// ----------------------------------------------------------------------------
// Maps 'A'..'Z' to 0..25, else -1.
// ----------------------------------------------------------------------------
int getSwitchIndex(char tile) {
    if(tile >= 'A' && tile <= 'Z' && tile != 'S' && tile != 'D') {
        return tile - 'A';
    }
    return -1;
}

// ----------------------------------------------------------------------------
// Check if a position is a spawn point.
// ----------------------------------------------------------------------------
// Returns true if x,y is a spawn.
// ----------------------------------------------------------------------------
bool isSpawnPoint(int row , int col) {
    if (!isInBounds(row, col)) {
        return false;}
      return grid[row][col]=='S';
}

// ----------------------------------------------------------------------------
// Check if a position is a destination.
// ----------------------------------------------------------------------------
// Returns true if x,y is a destination.
// ----------------------------------------------------------------------------
bool isDestinationPoint(int row , int col) {
    if (!isInBounds(row, col)) {
        return false;}
      return grid[row][col]=='D';
}

// ----------------------------------------------------------------------------
// Toggle a safety tile.
// ----------------------------------------------------------------------------
// Returns true if toggled successfully.
// ----------------------------------------------------------------------------
bool toggleSafetyTile(int row, int col) {
    // STEP 1: Check if position is valid
    if(!isInBounds(row, col)) {
        return false;  // Position doesn't exist on grid
    }
    
    // STEP 2: Get the current character at this position
    // This reads what's currently in the grid at (row, col)
    char current = grid[row][col];
    
    // STEP 3: Check if we can PLACE a safety tile here
    // Only allow on horizontal '-' or vertical '|' tracks
    if(current == '-' || current == '|') {
        // SAVE the original tile before changing it
        // This way we can restore it later when removing the safety tile
        originalTiles[row][col] = current;
        
        // CHANGE the grid tile to safety marker '='
        grid[row][col] = '=';
        
        return true;  // Successfully placed safety tile
    }
    
    // STEP 4: Check if we're REMOVING a safety tile
    else if(current == '=') {
        // RESTORE the original tile from our backup array
        // This puts back either '-' or '|' depending on what was there before
        grid[row][col] = originalTiles[row][col];
        
        return true;  // Successfully removed safety tile
    }
    
    // STEP 5: Can't toggle on other types of tiles
    // This includes: S (spawn), D (destination), + (crossing),
    //                / or \ (curves), A-Z (switches)
    return false;  // Toggle not allowed on this tile type
}
void printGrid() {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            cout << grid[i][j];
        }
        cout << endl;
    }
}
