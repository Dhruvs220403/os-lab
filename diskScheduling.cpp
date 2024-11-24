#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;

void readDiskParameters(const string &filename, int &numCylinders, int &numSectors, int &bytesPerSector,
                    	int &rpm, double &avgSeekTime, int &initialHeadPosition, vector<int> &requests) {
	ifstream infile(filename);
	if (!infile) {
    	cerr << "Error opening file." << endl;
    	exit(1);
	}

	infile >> numCylinders;
	infile >> numSectors;
	infile >> bytesPerSector;
	infile >> rpm;
	infile >> avgSeekTime;
	infile >> initialHeadPosition;

	int request;
	while (infile >> request) {
    	requests.push_back(request);
	}

	infile.close();
}

double calculateAverageRotationalDelay(int numSectors, int rpm) {
	double rotationTimePerRevolution = 60.0 / rpm;  // in seconds
	double rotationalDelay = rotationTimePerRevolution / numSectors;
	return rotationalDelay;
}

double calculateSeekTime(int start, int end, double avgSeekTime) {
	return avgSeekTime * abs(start - end);
}

void fcfsScheduling(const vector<int> &requests, int initialHeadPosition, double avgSeekTime,
                	double rotationalDelay, int numSectors) {
	double totalSeekTime = 0.0;
	double totalRotationalDelay = 0.0;
	int currentPosition = initialHeadPosition;
    
	for (int request : requests) {
    	totalSeekTime += calculateSeekTime(currentPosition, request, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - request) % numSectors);
    	currentPosition = request;
	}
    
	double averageRotationalDelay = totalRotationalDelay / requests.size();

	cout << "FCFS Scheduling:" << endl;
	cout << "Average Rotational Delay: " << averageRotationalDelay << " seconds" << endl;
	cout << "Total Seek Time: " << totalSeekTime << " seconds" << endl;
}

void sstfScheduling(const vector<int> &requests, int initialHeadPosition, double avgSeekTime,
                	double rotationalDelay, int numSectors) {
	vector<int> sortedRequests = requests;
	sort(sortedRequests.begin(), sortedRequests.end());

	double totalSeekTime = 0.0;
	double totalRotationalDelay = 0.0;
	int currentPosition = initialHeadPosition;
	vector<int> servedRequests;

	while (!sortedRequests.empty()) {
    	auto closest = min_element(sortedRequests.begin(), sortedRequests.end(),
                               	[currentPosition](int a, int b) {
                                   	return abs(a - currentPosition) < abs(b - currentPosition);
                               	});

    	int request = *closest;
    	servedRequests.push_back(request);
    	sortedRequests.erase(closest);

    	totalSeekTime += calculateSeekTime(currentPosition, request, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - request) % numSectors);
    	currentPosition = request;
	}

	double averageRotationalDelay = totalRotationalDelay / requests.size();

	cout << "SSTF Scheduling:" << endl;
	cout << "Average Rotational Delay: " << averageRotationalDelay << " seconds" << endl;
	cout << "Total Seek Time: " << totalSeekTime << " seconds" << endl;
}

void lookScheduling(const vector<int> &requests, int initialHeadPosition, double avgSeekTime,
                	double rotationalDelay, int numSectors) {
	vector<int> sortedRequests = requests;
	sort(sortedRequests.begin(), sortedRequests.end());

	vector<int> left, right;
	for (int request : sortedRequests) {
    	if (request < initialHeadPosition) {
        	left.push_back(request);
    	} else {
        	right.push_back(request);
    	}
	}

	double totalSeekTime = 0.0;
	double totalRotationalDelay = 0.0;
	int currentPosition = initialHeadPosition;

	for (int request : right) {
    	totalSeekTime += calculateSeekTime(currentPosition, request, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - request) % numSectors);
    	currentPosition = request;
	}

	for (int request : left) {
    	totalSeekTime += calculateSeekTime(currentPosition, request, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - request) % numSectors);
    	currentPosition = request;
	}

	double averageRotationalDelay = totalRotationalDelay / requests.size();

	cout << "LOOK Scheduling:" << endl;
	cout << "Average Rotational Delay: " << averageRotationalDelay << " seconds" << endl;
	cout << "Total Seek Time: " << totalSeekTime << " seconds" << endl;
}

void cscanScheduling(const vector<int> &requests, int initialHeadPosition, double avgSeekTime,
                 	double rotationalDelay, int numCylinders) {
	vector<int> sortedRequests = requests;
	sort(sortedRequests.begin(), sortedRequests.end());

	vector<int> left, right;
	for (int request : sortedRequests) {
    	if (request < initialHeadPosition) {
        	left.push_back(request);
    	} else {
        	right.push_back(request);
    	}
	}

	double totalSeekTime = 0.0;
	double totalRotationalDelay = 0.0;
	int currentPosition = initialHeadPosition;

	for (int request : right) {
    	totalSeekTime += calculateSeekTime(currentPosition, request, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - request) % numCylinders);
    	currentPosition = request;
	}

	if (!right.empty()) {
    	totalSeekTime += calculateSeekTime(currentPosition, 0, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - 0) % numCylinders);
    	currentPosition = 0;
	}

	for (int request : left) {
    	totalSeekTime += calculateSeekTime(currentPosition, request, avgSeekTime);
    	totalRotationalDelay += rotationalDelay * (abs(currentPosition - request) % numCylinders);
    	currentPosition = request;
	}

	double averageRotationalDelay = totalRotationalDelay / requests.size();

	cout << "C-SCAN Scheduling:" << endl;
	cout << "Average Rotational Delay: " << averageRotationalDelay << " seconds" << endl;
	cout << "Total Seek Time: " << totalSeekTime << " seconds" << endl;
}

int main() {
	int numCylinders, numSectors, bytesPerSector, rpm, initialHeadPosition;
	double avgSeekTime;
	vector<int> requests;

	readDiskParameters("disk.dat", numCylinders, numSectors, bytesPerSector, rpm, avgSeekTime, initialHeadPosition, requests);

	double rotationalDelay = calculateAverageRotationalDelay(numSectors, rpm);

	fcfsScheduling(requests, initialHeadPosition, avgSeekTime, rotationalDelay, numSectors);
	sstfScheduling(requests, initialHeadPosition, avgSeekTime, rotationalDelay, numSectors);
	lookScheduling(requests, initialHeadPosition, avgSeekTime, rotationalDelay, numSectors);
	cscanScheduling(requests, initialHeadPosition, avgSeekTime, rotationalDelay, numSectors);

	return 0;
}
