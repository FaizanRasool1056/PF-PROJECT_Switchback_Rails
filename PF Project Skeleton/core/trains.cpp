#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace std;

int plannedNextRow[100];
int plannedNextCol[100];
int plannedNextDir[100];
int trainStuckCounter[100];
int lastTrainRow[100];
int lastTrainCol[100];

void spawnTrainsForTick() {
    for(int i = 0; i < numTrains; i++) {
        if(trainSpawnTick[i] == currentTick && trainState[i] == TRAIN_WAITING) {
            int spawnR = trainRow[i];
            int spawnC = trainCol[i];
            
            if(!isInBounds(spawnR, spawnC)) {
                cout << "  ERROR: Train " << i << " spawn out of bounds!" << endl;
                trainState[i] = TRAIN_CRASHED;
                trainsCrashed++;
                continue;
            }
            
            char tile = grid[spawnR][spawnC];
            if(!isTrackTile(tile)) {
                cout << "  ERROR: Train " << i << " spawn not on track!" << endl;
                trainState[i] = TRAIN_CRASHED;
                trainsCrashed++;
                continue;
            }
            
            bool occupied = false;
            for(int j = 0; j < numTrains; j++) {
                if(i != j && trainState[j] == TRAIN_MOVING) {
                    if(trainRow[j] == spawnR && trainCol[j] == spawnC) {
                        occupied = true;
                        break;
                    }
                }
            }
            
            if(!occupied) {
                trainState[i] = TRAIN_MOVING;
                trainStuckCounter[i] = 0;
                lastTrainRow[i] = spawnR;
                lastTrainCol[i] = spawnC;
                
                int destIdx = trainDestinationIndex[i];
                cout << "  Train " << i << " SPAWNED at (" << spawnR << "," << spawnC 
                     << ") dir=" << trainDirection[i]
                     << " -> dest " << destIdx 
                     << " at (" << destRow[destIdx] << "," << destCol[destIdx] << ")" << endl;
            } else {
                trainSpawnTick[i]++;
                cout << "  Train " << i << " spawn delayed (occupied)" << endl;
            }
        }
    }
}

int calculateDistance(int fromRow, int fromCol, int toRow, int toCol) {
    int dr = fromRow - toRow;
    int dc = fromCol - toCol;
    if(dr < 0) dr = -dr;
    if(dc < 0) dc = -dc;
    return dr + dc;
}

bool canMoveInDirection(int row, int col, int dir) {
    int nextRow = row;
    int nextCol = col;
    
    if(dir == DIR_UP) nextRow--;
    else if(dir == DIR_RIGHT) nextCol++;
    else if(dir == DIR_DOWN) nextRow++;
    else if(dir == DIR_LEFT) nextCol--;
    
    if(!isInBounds(nextRow, nextCol)) return false;
    
    char nextTile = grid[nextRow][nextCol];
    return isTrackTile(nextTile);
}

int getSmartDirectionAtCrossing(int trainIndex) {
    int currentRow = trainRow[trainIndex];
    int currentCol = trainCol[trainIndex];
    int currentDir = trainDirection[trainIndex];
    
    int destIdx = trainDestinationIndex[trainIndex];
    int goalRow = destRow[destIdx];
    int goalCol = destCol[destIdx];
    
    int bestDir = currentDir;
    int bestDist = 999999;
    
    for(int dir = 0; dir < 4; dir++) {
        if(!canMoveInDirection(currentRow, currentCol, dir)) {
            continue;
        }
        
        int testRow = currentRow;
        int testCol = currentCol;
        
        if(dir == DIR_UP) testRow--;
        else if(dir == DIR_RIGHT) testCol++;
        else if(dir == DIR_DOWN) testRow++;
        else if(dir == DIR_LEFT) testCol--;
        
        int dist = calculateDistance(testRow, testCol, goalRow, goalCol);
        
        if(dist < bestDist) {
            bestDist = dist;
            bestDir = dir;
        }
    }
    
    return bestDir;
}

int getNextDirection(char tile, int currentDirection, int currentRow, int currentCol, int trainIndex) {
    
    if(tile == '-' || tile == '=') {
        if(currentDirection == DIR_LEFT || currentDirection == DIR_RIGHT) {
            return currentDirection;
        }
        return DIR_RIGHT;
    }
    
    if(tile == '|') {
        if(currentDirection == DIR_UP || currentDirection == DIR_DOWN) {
            return currentDirection;
        }
        return DIR_DOWN;
    }
    
    if(tile == 'S' || tile == 'D') {
        return currentDirection;
    }
    
    if(tile == '/') {
        if(currentDirection == DIR_UP) return DIR_RIGHT;
        if(currentDirection == DIR_RIGHT) return DIR_UP;
        if(currentDirection == DIR_DOWN) return DIR_LEFT;
        if(currentDirection == DIR_LEFT) return DIR_DOWN;
    }
    
    if(tile == '\\') {
        if(currentDirection == DIR_UP) return DIR_LEFT;
        if(currentDirection == DIR_LEFT) return DIR_UP;
        if(currentDirection == DIR_DOWN) return DIR_RIGHT;
        if(currentDirection == DIR_RIGHT) return DIR_DOWN;
    }
    
    if(tile == '+') {
        int smartDir = getSmartDirectionAtCrossing(trainIndex);
        cout << "      Train " << trainIndex << " at crossing - choosing dir " << smartDir << endl;
        return smartDir;
    }
    
    if(tile >= 'A' && tile <= 'Z') {
        int switchIndex = getSwitchIndex(tile);
        
        if(switchIndex != -1) {
            int state = switchState[switchIndex];
            char letter = 'A' + switchIndex;
            
            bool canUp = canMoveInDirection(currentRow, currentCol, DIR_UP);
            bool canRight = canMoveInDirection(currentRow, currentCol, DIR_RIGHT);
            bool canDown = canMoveInDirection(currentRow, currentCol, DIR_DOWN);
            bool canLeft = canMoveInDirection(currentRow, currentCol, DIR_LEFT);
            
            cout << "      Switch " << letter << " state=" << state 
                 << " entering from dir=" << currentDirection << " | Exits: ";
            if(canUp) cout << "UP ";
            if(canRight) cout << "RIGHT ";
            if(canDown) cout << "DOWN ";
            if(canLeft) cout << "LEFT ";
            cout << endl;
            
            if(state == 0) {
                if(canMoveInDirection(currentRow, currentCol, currentDirection)) {
                    cout << "        -> STRAIGHT" << endl;
                    return currentDirection;
                }
                
                if(currentDirection == DIR_RIGHT || currentDirection == DIR_LEFT) {
                    if(canDown) {
                        cout << "        -> Can't go straight, turning DOWN" << endl;
                        return DIR_DOWN;
                    }
                    if(canUp) {
                        cout << "        -> Can't go straight, turning UP" << endl;
                        return DIR_UP;
                    }
                }
                if(currentDirection == DIR_UP || currentDirection == DIR_DOWN) {
                    if(canRight) {
                        cout << "        -> Can't go straight, turning RIGHT" << endl;
                        return DIR_RIGHT;
                    }
                    if(canLeft) {
                        cout << "        -> Can't go straight, turning LEFT" << endl;
                        return DIR_LEFT;
                    }
                }
            }
            else {
                if(currentDirection == DIR_RIGHT || currentDirection == DIR_LEFT) {
                    if(canDown) {
                        cout << "        -> TURN DOWN" << endl;
                        return DIR_DOWN;
                    }
                    if(canUp) {
                        cout << "        -> TURN UP" << endl;
                        return DIR_UP;
                    }
                }
                
                if(currentDirection == DIR_UP || currentDirection == DIR_DOWN) {
                    if(canRight) {
                        cout << "        -> TURN RIGHT" << endl;
                        return DIR_RIGHT;
                    }
                    if(canLeft) {
                        cout << "        -> TURN LEFT" << endl;
                        return DIR_LEFT;
                    }
                }
                
                if(canMoveInDirection(currentRow, currentCol, currentDirection)) {
                    cout << "        -> Can't turn, going STRAIGHT" << endl;
                    return currentDirection;
                }
            }
            
            if(canUp) return DIR_UP;
            if(canRight) return DIR_RIGHT;
            if(canDown) return DIR_DOWN;
            if(canLeft) return DIR_LEFT;
        }
    }
    
    return currentDirection;
}

bool determineNextPosition(int trainIndex) {
    int currentRow = trainRow[trainIndex];
    int currentCol = trainCol[trainIndex];
    int currentDir = trainDirection[trainIndex];
    
    int destIdx = trainDestinationIndex[trainIndex];
    int goalRow = destRow[destIdx];
    int goalCol = destCol[destIdx];
    
    if(currentRow == goalRow && currentCol == goalCol) {
        plannedNextRow[trainIndex] = currentRow;
        plannedNextCol[trainIndex] = currentCol;
        plannedNextDir[trainIndex] = currentDir;
        return true;
    }
    
    char currentTile = grid[currentRow][currentCol];
    
    int nextDir = getNextDirection(currentTile, currentDir, currentRow, currentCol, trainIndex);
    
    int nextRow = currentRow;
    int nextCol = currentCol;
    
    if(nextDir == DIR_UP) {
        nextRow = currentRow - 1;
    } 
    else if(nextDir == DIR_RIGHT) {
        nextCol = currentCol + 1;
    } 
    else if(nextDir == DIR_DOWN) {
        nextRow = currentRow + 1;
    } 
    else if(nextDir == DIR_LEFT) {
        nextCol = currentCol - 1;
    }
    
    if(!isInBounds(nextRow, nextCol)) {
        cout << "    Train " << trainIndex << " would go out of bounds!" << endl;
        return false;
    }
    
    char nextTile = grid[nextRow][nextCol];
    if(!isTrackTile(nextTile)) {
        cout << "    Train " << trainIndex << " would hit non-track tile '" << nextTile << "'!" << endl;
        return false;
    }
    
    plannedNextRow[trainIndex] = nextRow;
    plannedNextCol[trainIndex] = nextCol;
    plannedNextDir[trainIndex] = nextDir;
    
    return true;
}

void determineAllRoutes() {
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] != TRAIN_MOVING) continue;
        if(trainDelayCounter[i] > 0) continue;
        
        bool valid = determineNextPosition(i);
        
        if(!valid) {
            plannedNextRow[i] = -1;
            plannedNextCol[i] = -1;
            cout << "    Train " << i << " cannot determine valid next position!" << endl;
        }
    }
}

void detectCollisions() {
    bool waitThisTick[100];
    for(int i = 0; i < 100; i++) {
        waitThisTick[i] = false;
    }
    
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] != TRAIN_MOVING) continue;
        if(plannedNextRow[i] == -1) continue;
        if(waitThisTick[i]) continue;
        
        for(int j = i + 1; j < numTrains; j++) {
            if(trainState[j] != TRAIN_MOVING) continue;
            if(plannedNextRow[j] == -1) continue;
            if(waitThisTick[j]) continue;
            
            // Same tile collision
            if(plannedNextRow[i] == plannedNextRow[j] && 
               plannedNextCol[i] == plannedNextCol[j]) {
                
                int destI = trainDestinationIndex[i];
                int distI = calculateDistance(trainRow[i], trainCol[i], 
                                             destRow[destI], destCol[destI]);
                
                int destJ = trainDestinationIndex[j];
                int distJ = calculateDistance(trainRow[j], trainCol[j], 
                                             destRow[destJ], destCol[destJ]);
                
                cout << "    COLLISION: Trains " << i << " and " << j 
                     << " targeting (" << plannedNextRow[i] << "," << plannedNextCol[i] << ")" << endl;
                
                if(distI > distJ) {
                    plannedNextRow[j] = trainRow[j];
                    plannedNextCol[j] = trainCol[j];
                    plannedNextDir[j] = trainDirection[j];
                    waitThisTick[j] = true;
                    totalWaitTicks++;
                    cout << "      Train " << j << " WAITS" << endl;
                }
                else if(distJ > distI) {
                    plannedNextRow[i] = trainRow[i];
                    plannedNextCol[i] = trainCol[i];
                    plannedNextDir[i] = trainDirection[i];
                    waitThisTick[i] = true;
                    totalWaitTicks++;
                    cout << "      Train " << i << " WAITS" << endl;
                }
                else {
                    plannedNextRow[i] = trainRow[i];
                    plannedNextCol[i] = trainCol[i];
                    plannedNextDir[i] = trainDirection[i];
                    plannedNextRow[j] = trainRow[j];
                    plannedNextCol[j] = trainCol[j];
                    plannedNextDir[j] = trainDirection[j];
                    waitThisTick[i] = true;
                    waitThisTick[j] = true;
                    totalWaitTicks += 2;
                    cout << "      Both WAIT" << endl;
                }
            }
            
            // Head-on collision
            if(plannedNextRow[i] == trainRow[j] && 
               plannedNextCol[i] == trainCol[j] &&
               plannedNextRow[j] == trainRow[i] && 
               plannedNextCol[j] == trainCol[i]) {
                
                int destI = trainDestinationIndex[i];
                int distI = calculateDistance(trainRow[i], trainCol[i], 
                                             destRow[destI], destCol[destI]);
                
                int destJ = trainDestinationIndex[j];
                int distJ = calculateDistance(trainRow[j], trainCol[j], 
                                             destRow[destJ], destCol[destJ]);
                
                cout << "    HEAD-ON: Trains " << i << " and " << j << endl;
                
                if(distI > distJ) {
                    plannedNextRow[j] = trainRow[j];
                    plannedNextCol[j] = trainCol[j];
                    plannedNextDir[j] = trainDirection[j];
                    waitThisTick[j] = true;
                    totalWaitTicks++;
                }
                else if(distJ > distI) {
                    plannedNextRow[i] = trainRow[i];
                    plannedNextCol[i] = trainCol[i];
                    plannedNextDir[i] = trainDirection[i];
                    waitThisTick[i] = true;
                    totalWaitTicks++;
                }
                else {
                    plannedNextRow[i] = trainRow[i];
                    plannedNextCol[i] = trainCol[i];
                    plannedNextDir[i] = trainDirection[i];
                    plannedNextRow[j] = trainRow[j];
                    plannedNextCol[j] = trainCol[j];
                    plannedNextDir[j] = trainDirection[j];
                    waitThisTick[i] = true;
                    waitThisTick[j] = true;
                    totalWaitTicks += 2;
                }
            }
        }
    }
}

void moveAllTrains() {
    detectCollisions();
    
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] != TRAIN_MOVING) continue;
        
        if(trainDelayCounter[i] > 0) {
            trainDelayCounter[i]--;
            totalWaitTicks++;
            continue;
        }
        
        if(plannedNextRow[i] == -1 || plannedNextCol[i] == -1) {
            trainState[i] = TRAIN_CRASHED;
            trainsCrashed++;
            cout << "    Train " << i << " CRASHED (invalid path)" << endl;
            continue;
        }
        
        if(trainRow[i] == lastTrainRow[i] && trainCol[i] == lastTrainCol[i]) {
            trainStuckCounter[i]++;
            if(trainStuckCounter[i] > 100) {
                cout << "    Train " << i << " STUCK for 100 ticks - CRASHED" << endl;
                trainState[i] = TRAIN_CRASHED;
                trainsCrashed++;
                continue;
            }
        } else {
            trainStuckCounter[i] = 0;
        }
        
        lastTrainRow[i] = trainRow[i];
        lastTrainCol[i] = trainCol[i];
        
        trainRow[i] = plannedNextRow[i];
        trainCol[i] = plannedNextCol[i];
        trainDirection[i] = plannedNextDir[i];
        
        char currentTile = grid[trainRow[i]][trainCol[i]];
        if(currentTile == '=') {
            trainDelayCounter[i] = 1;
        }
    }
}

void checkArrivals() {
    for(int i = 0; i < numTrains; i++) {
        if(trainState[i] != TRAIN_MOVING) continue;
        
        int destIndex = trainDestinationIndex[i];
        
        if(trainRow[i] == destRow[destIndex] && 
           trainCol[i] == destCol[destIndex]) {
            trainState[i] = TRAIN_DELIVERED;
            trainsDelivered++;
            cout << "  *** Train " << i << " DELIVERED to dest " << destIndex << " ***" << endl;
        }
    }
}

void applyEmergencyHalt(int switchRow, int switchCol) {
    emergencyHaltActive = true;
    emergencyHaltTicks = 3;
    emergencyHaltRow = switchRow;
    emergencyHaltCol = switchCol;
}

void updateEmergencyHalt() {
    if(emergencyHaltActive) {
        emergencyHaltTicks--;
        if(emergencyHaltTicks <= 0) {
            emergencyHaltActive = false;
        }
    }
}