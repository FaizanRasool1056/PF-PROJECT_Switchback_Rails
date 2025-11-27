#ifndef GRID_H
#define GRID_H

// ============================================================================
// GRID.H - Grid manipulation functions
// ============================================================================
// Functions for working with the 2D grid map.
// ============================================================================

// Check if a position is within grid bounds
bool isInBounds(int row, int col);


// Check if a tile is a track (can trains move on it?)
bool isTrackTile(char tile);

// Check if a tile is a switch (A-Z)
bool isSwitchTile(char tile);

// Get the switch index (0-25) from a switch character (A-Z)
int getSwitchIndex(char tile);

// Check if a position is a spawn point
bool isSpawnPoint(int row , int col);

// Check if a position is a destination point
bool isDestinationPoint(int row , int col);

// Place or remove a safety tile at a position (for mouse editing)
// Returns true if successful
bool toggleSafetyTile(int row, int col);
// print grid
void printGrid();
#endif
