#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include <iomanip>

using namespace std;

// Structure to represent a memory block
struct Block {
    int start;
    int size;
    bool allocated;
};

// Structure to represent a memory request
struct Request {
    int arrival_time;
    int size;
    int duration;
    int allocated_at = -1; // Default: not allocated
};

// Class to simulate memory allocation
class MemoryAllocator {
private:
    int memory_size;
    vector<Block> blocks;
    queue<Request> request_queue;
    int current_time = 0;
    int next_fit_index = 0;

    // For each strategy, we need to track fragmentation and allocation statistics separately
    struct StrategyStats {
        int successful_allocations = 0;
        int external_fragmentation = 0;
        int internal_fragmentation = 0;
        int total_requests = 0;
    };

    StrategyStats first_fit_stats;
    StrategyStats best_fit_stats;
    StrategyStats worst_fit_stats;
    StrategyStats next_fit_stats;

public:
    explicit MemoryAllocator(int size) : memory_size(size) {
        blocks.push_back({0, size, false});
    }

    void process_requests(const string& file_name) {
        ifstream file(file_name);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file " << file_name << "\n";
            return;
        }

        int time, size, duration;
        while (file >> time >> size >> duration) {
            if (time == -1 && size == -1 && duration == -1) break;
            request_queue.push({time, size, duration});
        }

        file.close();
        simulate();
    }

    void simulate() {
        while (!request_queue.empty()) {
            Request request = request_queue.front();
            request_queue.pop();
            
            // Increment the total request count for each strategy
            first_fit_stats.total_requests++;
            best_fit_stats.total_requests++;
            worst_fit_stats.total_requests++;
            next_fit_stats.total_requests++;

            // Advance time if needed
            if (request.arrival_time > current_time) {
                current_time = request.arrival_time;
                free_expired_blocks();
            }

            // Attempt allocation using different strategies
            allocate_memory(request, "First Fit");
            allocate_memory(request, "Best Fit");
            allocate_memory(request, "Worst Fit");
            allocate_memory(request, "Next Fit");

            // Free blocks and print status after every 10 requests
            if (request_queue.size() % 10 == 0) {
                free_expired_blocks();
                print_status("First Fit", first_fit_stats);
                print_status("Best Fit", best_fit_stats);
                print_status("Worst Fit", worst_fit_stats);
                print_status("Next Fit", next_fit_stats);
            }
        }
    }

    void allocate_memory(Request& request, const string& strategy) {
        int index = -1;
        if (strategy == "First Fit") {
            index = first_fit(request.size);
        } else if (strategy == "Best Fit") {
            index = best_fit(request.size);
        } else if (strategy == "Worst Fit") {
            index = worst_fit(request.size);
        } else if (strategy == "Next Fit") {
            index = next_fit(request.size);
        }

        if (index != -1) {
            blocks[index].allocated = true;
            int internal_frag = blocks[index].size - request.size;
            if (strategy == "First Fit") {
                first_fit_stats.successful_allocations++;
                first_fit_stats.internal_fragmentation += internal_frag;
            } else if (strategy == "Best Fit") {
                best_fit_stats.successful_allocations++;
                best_fit_stats.internal_fragmentation += internal_frag;
            } else if (strategy == "Worst Fit") {
                worst_fit_stats.successful_allocations++;
                worst_fit_stats.internal_fragmentation += internal_frag;
            } else if (strategy == "Next Fit") {
                next_fit_stats.successful_allocations++;
                next_fit_stats.internal_fragmentation += internal_frag;
            }
            blocks[index].size = request.size; // Split if needed
            request.allocated_at = current_time;
        } else {
            int external_frag = request.size;
            if (strategy == "First Fit") {
                first_fit_stats.external_fragmentation += external_frag;
            } else if (strategy == "Best Fit") {
                best_fit_stats.external_fragmentation += external_frag;
            } else if (strategy == "Worst Fit") {
                worst_fit_stats.external_fragmentation += external_frag;
            } else if (strategy == "Next Fit") {
                next_fit_stats.external_fragmentation += external_frag;
            }
        }
    }

    int first_fit(int size) {
        for (int i = 0; i < blocks.size(); ++i) {
            if (!blocks[i].allocated && blocks[i].size >= size) return i;
        }
        return -1;
    }

    int best_fit(int size) {
        int best_index = -1;
        int min_size_diff = memory_size + 1;

        for (int i = 0; i < blocks.size(); ++i) {
            if (!blocks[i].allocated && blocks[i].size >= size) {
                int size_diff = blocks[i].size - size;
                if (size_diff < min_size_diff) {
                    min_size_diff = size_diff;
                    best_index = i;
                }
            }
        }
        return best_index;
    }

    int worst_fit(int size) {
        int worst_index = -1;
        int max_size_diff = -1;

        for (int i = 0; i < blocks.size(); ++i) {
            if (!blocks[i].allocated && blocks[i].size >= size) {
                int size_diff = blocks[i].size - size;
                if (size_diff > max_size_diff) {
                    max_size_diff = size_diff;
                    worst_index = i;
                }
            }
        }
        return worst_index;
    }

    int next_fit(int size) {
        for (int i = 0; i < blocks.size(); ++i) {
            int index = (next_fit_index + i) % blocks.size();
            if (!blocks[index].allocated && blocks[index].size >= size) {
                next_fit_index = index;
                return index;
            }
        }
        return -1;
    }

    void free_expired_blocks() {
        for (auto& block : blocks) {
            if (block.allocated && current_time >= block.start + block.size) {
                block.allocated = false;
            }
        }
    }

    void print_status(const string& strategy, const StrategyStats& stats) {
        cout << "\nAfter " << stats.total_requests << " requests (" << strategy << "):\n";
        cout << "Successful allocations: " << stats.successful_allocations << "\n";
        cout << "External fragmentation: " << (stats.external_fragmentation * 100.0 / memory_size) << "%\n";
        cout << "Internal fragmentation: " << (stats.internal_fragmentation * 100.0 / memory_size) << "%\n";
    }
};

int main() {
    MemoryAllocator allocator(1024); // Initialize with 1024 KB memory
    allocator.process_requests("alloc.dat");
    return 0;
}
