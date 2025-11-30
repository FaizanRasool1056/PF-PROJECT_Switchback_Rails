#include "switches.h"
#include "simulation_state.h"
#include "grid.h"
#include "io.h"
#include <iostream>

using namespace std;

bool wasOnSwitch[100];
int lastSwitchIndex[100];

void updateSwitchCounters() {
    
    for(int i = 0; i < numTrains; i++) {
        
        if(trainState[i] != TRAIN_MOVING) {
            wasOnSwitch[i] = false;
            lastSwitchIndex[i] = -1;
            continue;
        }
        
        int row = trainRow[i];
        int col = trainCol[i];
        
        char tile = grid[row][col];
        if(!isSwitchTile(tile)) {
            wasOnSwitch[i] = false;
            lastSwitchIndex[i] = -1;
            continue;
        }
        
        int switchIndex = getSwitchIndex(tile);
        if(switchIndex == -1) {
            continue;
        }
        
        // Only increment if train just entered
        if(!wasOnSwitch[i] || lastSwitchIndex[i] != switchIndex) {
            
            int entryDir = trainDirection[i];
            
            if(switchMode[switchIndex] == SWITCH_PER_DIR) {
                switchCounter[switchIndex][entryDir]++;
                cout << "    Train " << i << " entered Switch " << tile 
                     << " from dir " << entryDir 
                     << ", counter[" << entryDir << "] = " 
                     << switchCounter[switchIndex][entryDir] << "/" << switchKValue[switchIndex][entryDir] << endl;
            }
            else {
                for(int d = 0; d < 4; d++) {
                    switchCounter[switchIndex][d]++;
                }
                cout << "    Train " << i << " entered Switch " << tile 
                     << ", global counter = " 
                     << switchCounter[switchIndex][0] << "/" << switchKValue[switchIndex][0] << endl;
            }
            
            wasOnSwitch[i] = true;
            lastSwitchIndex[i] = switchIndex;
        }
    }
}

void queueSwitchFlips() {
    
    for(int i = 0; i < 26; i++) {
        
        if(switchRow[i] == -1) {
            continue;
        }
        
        bool shouldFlip = false;
        
        if(switchMode[i] == SWITCH_PER_DIR) {
            for(int d = 0; d < 4; d++) {
                if(switchKValue[i][d] > 0 && 
                   switchCounter[i][d] >= switchKValue[i][d]) {
                    shouldFlip = true;
                    break;
                }
            }
        }
        else {
            if(switchKValue[i][0] > 0 && 
               switchCounter[i][0] >= switchKValue[i][0]) {
                shouldFlip = true;
            }
        }
        
        if(shouldFlip) {
            switchFlipQueued[i] = true;
            char letter = 'A' + i;
            cout << "    Switch " << letter << " QUEUED FOR FLIP" << endl;
        }
    }
}

void applyDeferredFlips() {
    
    for(int i = 0; i < 26; i++) {
        
        if(!switchFlipQueued[i]) {
            continue;
        }
        
        if(switchState[i] == 0) {
            switchState[i] = 1;
        }
        else {
            switchState[i] = 0;
        }
        
        for(int d = 0; d < 4; d++) {
            switchCounter[i][d] = 0;
        }
        
        switchFlipQueued[i] = false;
        totalSwitchFlips++;
        
        char letter = 'A' + i;
        cout << "    Switch " << letter << " FLIPPED to state " 
             << switchState[i] << " (STRAIGHT=" << (switchState[i]==0?"YES":"NO") 
             << " TURN=" << (switchState[i]==1?"YES":"NO") << ")" << endl;
    }
}

void updateSignalLights() {
    
    for(int i = 0; i < 26; i++) {
        
        if(switchRow[i] == -1) {
            continue;
        }
        
        switchSignal[i] = SIGNAL_GREEN;
        
        int sRow = switchRow[i];
        int sCol = switchCol[i];
        
        for(int t = 0; t < numTrains; t++) {
            if(trainState[t] != TRAIN_MOVING) {
                continue;
            }
            
            int tRow = trainRow[t];
            int tCol = trainCol[t];
            
            int dist = abs(tRow - sRow) + abs(tCol - sCol);
            
            if(dist == 0) {
                switchSignal[i] = SIGNAL_RED;
                break;
            }
            else if(dist <= 2) {
                if(switchSignal[i] == SIGNAL_GREEN) {
                    switchSignal[i] = SIGNAL_YELLOW;
                }
            }
        }
    }
}

void toggleSwitchState() {}
int getSwitchStateForDirection() { return 0; }