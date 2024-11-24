#include <iostream>

#include <fstream>

#include <vector>

#include <queue>

#include <iomanip>
using namespace std;

struct Process {

   int arrivalTime;

   int processId;

   int burstTime;

   int priority;

   int startTime;

   int endTime;

   bool operator>(const Process &p) const {
        return (priority > p.priority) || (priority == p.priority && arrivalTime > p.arrivalTime);
   }

};

void readProcessData(const string &filename, vector<Process> &processes) {

   ifstream file(filename);

   if (!file) {

   	cerr << "Error opening file!" << endl;

   	return;

   }


   int arrival, id, burst, priority;

   char comma;

   while (file >> arrival >> comma >> id >> comma >> burst >> comma >> priority) {

   	if (arrival < 0) break;

   	processes.push_back({arrival, id, burst, priority, -1, -1});

   }

   file.close();

}


void printGanttChart(const vector<Process> &completed, const string &processorName) {

   cout << "\nGantt Chart for " << processorName << ":\n";

   for (const auto &p : completed) {

   	cout << "| P" << p.processId << " ";

   }

   cout << "|\n";

}


void calculateAndPrintStats(const vector<Process> &completed) {

   int totalTurnaround = 0;

   int totalWaiting = 0;


   cout << "\nProcess-wise details:\n";

   cout << "Process ID | Turnaround Time | Waiting Time\n";


   for (const auto &p : completed) {

   	int turnaroundTime = abs(p.endTime - p.arrivalTime);

   	int waitingTime = abs(turnaroundTime - p.burstTime);


   	totalTurnaround += turnaroundTime;

   	totalWaiting += waitingTime;


   	cout << "P" << p.processId<< "       	" << setw(8) << turnaroundTime
             	<< "          	" << setw(8) << waitingTime << "\n";

   }


   cout << "\nAverage Turnaround Time: " << (totalTurnaround / (double)completed.size()) << "\n";

   cout << "Average Waiting Time: " << (totalWaiting / (double)completed.size()) << "\n";

}

void scheduleSingleQueue(const vector<Process> &processes) {

   priority_queue<Process, vector<Process>, greater<Process>> queue;

   vector<Process> completed;

   int timeProcessor1 = 0, timeProcessor2 = 0;


 

   for (const auto &process : processes) {

   	queue.push(process);

   }


   while (!queue.empty()) {

   	Process current = queue.top();

   	queue.pop();


   	if (timeProcessor1 <= timeProcessor2) {

       	current.startTime = max(timeProcessor1, current.arrivalTime);

       	current.endTime = current.startTime + current.burstTime / 2; // Processor 1 is twice as fast

       	timeProcessor1 = current.endTime;

   	} else {

       	current.startTime = max(timeProcessor2, current.arrivalTime);

       	current.endTime = current.startTime + current.burstTime; // Processor 2 speed

       	timeProcessor2 = current.endTime;

   	}


   	completed.push_back(current);

   }


   printGanttChart(completed, "Single Queue (Both Processors)");

   calculateAndPrintStats(completed);

}


void scheduleTwoQueues(const vector<Process> &processes) {

   priority_queue<Process, vector<Process>, greater<Process>> queue1, queue2;

   vector<Process> completed1, completed2;

   int timeProcessor1 = 0, timeProcessor2 = 0;



   for (const auto &process : processes) {

   	if (process.priority <= 5) {

       	queue1.push(process);	// Higher priority for Processor 1

   	} else {

       	queue2.push(process);	// Lower priority for Processor 2

   	}

   }



   while (!queue1.empty()) {

   	Process current = queue1.top();

   	queue1.pop();


   	current.startTime = max(timeProcessor1, current.arrivalTime);

   	current.endTime = current.startTime + current.burstTime / 2;

   	timeProcessor1 = current.endTime;


   	completed1.push_back(current);

   }


	while (!queue2.empty()) {

   	Process current = queue2.top();

   	queue2.pop();


   	current.startTime = max(timeProcessor2, current.arrivalTime);

   	current.endTime = current.startTime + current.burstTime; // Processor 2 speed

   	timeProcessor2 = current.endTime;


   	completed2.push_back(current);

   }


   printGanttChart(completed1, "Processor 1");

   calculateAndPrintStats(completed1);


   printGanttChart(completed2, "Processor 2");

   calculateAndPrintStats(completed2);

}

int main() {

   vector<Process> processes;

   readProcessData("sched2.dat", processes);


   cout << "Version 1: Single Queue Scheduling\n";

   
   
   scheduleSingleQueue(processes);


   cout << "\nVersion 2: Separate Queue Scheduling\n";

   scheduleTwoQueues(processes);

   return 0;

}