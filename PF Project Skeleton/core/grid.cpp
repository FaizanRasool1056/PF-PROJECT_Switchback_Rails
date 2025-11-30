#include "grid.h"
#include "simulation_state.h"
#include <iostream>
using namespace std;

bool isInBounds(int row, int col) {
    if (row>=0 && row<rows && col>=0 && col<cols) {
        return true;
    }
    else {
        return false;
    }
}

bool isTrackTile(char tile) {
    if(tile == '-' || tile == '|' || tile == '/' || tile == '\\' || 
       tile == '+' || tile == '=' || tile == 'S' || tile == 'D' || 
       (tile >= 'A' && tile <= 'Z')) {
        return true;
    }
    return false;
}

bool isSwitchTile(char tile) {
    if(tile >= 'A' && tile <= 'Z') {
        int index = tile - 'A';
        if(switchRow[index] != -1) {
            return true;
        }
    }
    return false;
}

int getSwitchIndex(char tile) {
    if(tile >= 'A' && tile <= 'Z') {
        int index = tile - 'A';
        if(switchRow[index] != -1) {
            return index;
        }
    }
    return -1;
}

bool isSpawnPoint(int row , int col) {
    if (!isInBounds(row, col)) {
        return false;
    }
    
    for(int i = 0; i < numSpawns; i++) {
        if(spawnRow[i] == row && spawnCol[i] == col) {
            return true;
        }
    }
    return false;
}

bool isDestinationPoint(int row , int col) {
    if (!isInBounds(row, col)) {
        return false;
    }
    
    for(int i = 0; i < numDestinations; i++) {
        if(destRow[i] == row && destCol[i] == col) {
            return true;
        }
    }
    return false;
}

bool toggleSafetyTile(int row, int col) {
    if(!isInBounds(row, col)) {
        return false;
    }
    
    char current = grid[row][col];
    
    if(current == '-' || current == '|') {
        originalTiles[row][col] = current;
        grid[row][col] = '=';
        currentSafetyBuffers++;
        return true;
    }
    else if(current == '=') {
        grid[row][col] = originalTiles[row][col];
        currentSafetyBuffers--;
        return true;
    }
    
    return false;
}

void printGrid() {
    char displayGrid[100][100];
    
    // Copy grid
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            displayGrid[i][j] = grid[i][j];
        }
    }
    
    // Add trains to display
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] == TRAIN_MOVING) {
            int r = trainRow[i];
            int c = trainCol[i];
            if(isInBounds(r, c)) {
                if(i < 10) {
                    displayGrid[r][c] = '0' + i;
                } else {
                    displayGrid[r][c] = 'T';
                }
            }
        }
    }
    
    // Print to terminal
    cout << "\nGrid State (Tick " << currentTick << "):" << endl;
    for(int i = 0; i < rows; i++) {
        cout << "  ";
        for(int j = 0; j < cols; j++) {
            char ch = displayGrid[i][j];
            cout << ch;
        }
        cout << endl;
    }
}