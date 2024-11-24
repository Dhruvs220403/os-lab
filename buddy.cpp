#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>

using namespace std;

class BuddySystem {
private:
    vector<bool> memory;
    int memorySize;
    int minBlockSize;
    
public:
    BuddySystem(int size, int minBlock) : memorySize(size), minBlockSize(minBlock) {
        int blocks = memorySize / minBlockSize;
        memory.resize(blocks, false);
    }

    bool allocateMemory(int requiredSize, int time, int duration, int processID) {
        int blockSize = minBlockSize;
        while (blockSize < requiredSize) {
            blockSize *= 2;
        }
        
        int blocksNeeded = blockSize / minBlockSize;
        for (int i = 0; i <= memory.size() - blocksNeeded; ++i) {
            if (checkAvailable(i, blocksNeeded)) {
                allocate(i, blocksNeeded, processID, time, duration);
                return true;
            }
        }
        return false;
    }

    bool checkAvailable(int start, int blocksNeeded) {
        for (int i = start; i < start + blocksNeeded; ++i) {
            if (memory[i] == true) {
                return false;
            }
        }
        return true;
    }

    void allocate(int start, int blocksNeeded, int processID, int time, int duration) {
        for (int i = start; i < start + blocksNeeded; ++i) {
            memory[i] = true;
        }
        cout << "Process " << processID << " allocated " << blocksNeeded * minBlockSize 
             << " units starting at block " << start << " at time " << time << " for " << duration << " units." << endl;
    }

    void deallocateMemory(int start, int blocksNeeded) {
        for (int i = start; i < start + blocksNeeded; ++i) {
            memory[i] = false;
        }
    }

    void mergeBuddies() {
        for (int i = 0; i < memory.size(); ++i) {
            if (memory[i] == false) {
            }
        }
    }
};

void readData(const string &filename, vector<tuple<int, int, int, int>> &processRequests) {
    ifstream file(filename);
    int processID, memoryRequired, requestTime, duration;
    while (file >> processID >> memoryRequired >> requestTime >> duration) {
        if (processID < 0) break;
        processRequests.push_back(make_tuple(processID, memoryRequired, requestTime, duration));
    }
}

int main() {
    string filename = "buddy.dat";
    vector<tuple<int, int, int, int>> processRequests;
    
    readData(filename, processRequests);

    BuddySystem buddy(512, 8);

    for (auto &req : processRequests) {
        int processID, memoryRequired, requestTime, duration;
        tie(processID, memoryRequired, requestTime, duration) = req;

        if (!buddy.allocateMemory(memoryRequired, requestTime, duration, processID)) {
            cout << "Process " << processID << " could not be allocated memory!" << endl;
        }

        cout << "Process " << processID << " will use memory until time " << requestTime + duration << endl;

        buddy.deallocateMemory(requestTime, memoryRequired / 8);
    }

    return 0;
}