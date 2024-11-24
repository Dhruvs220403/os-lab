#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;

const int num_person = 5;

mutex forks[num_person];
condition_variable cond_vars[num_person];
bool fork_available[num_person] = {true, true, true, true, true};
void pick_up_forks(int id) {
	int left = id;
	int right = (id + 1) % num_person;

	unique_lock<mutex> left_lock(forks[left]);
	unique_lock<mutex> right_lock(forks[right]);

	while (!fork_available[left] || !fork_available[right]) {
    	if (!fork_available[left]) {
        	cond_vars[left].wait(left_lock);
    	}
    	if (!fork_available[right]) {
        	cond_vars[right].wait(right_lock);
    	}
	}

	fork_available[left] = false;
	fork_available[right] = false;
}

void put_down_forks(int id) {
	int left = id;
	int right = (id + 1) % num_person;

	fork_available[left] = true;
	fork_available[right] = true;

	cond_vars[left].notify_all();
	cond_vars[right].notify_all();
}

void dine(int id) {
	while (true) {
    	this_thread::sleep_for(chrono::milliseconds(1000));

    	cout << "Person " << id << " is hungry.\n";

    	pick_up_forks(id);

    	cout << "Person " << id << " is eating.\n";
    	this_thread::sleep_for(chrono::milliseconds(1000));

    	put_down_forks(id);
	}
}

int main() {
	vector<thread> person;

	for (int i = 0; i < num_person; ++i) {
    	person.push_back(thread(dine, i));
	}

	for (auto& t : person) {
    	t.join();
	}

	return 0;
}
