#ifndef DEADLOCK_H
#define DEADLOCK_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// Process class
class Process {
public:
    int pid;
    string name;
    vector<int> allocated;      // Resources currently allocated
    vector<int> maximum;        // Maximum resource needs
    vector<int> need;           // Remaining resource needs
    bool finished;

    Process(int id, string pname, vector<int> max_resources);
    
    void allocateResource(int resourceType, int amount);
    void releaseResource(int resourceType, int amount);
    void calculateNeed();
};

// Resource class
class Resource {
public:
    int rid;
    string name;
    int totalInstances;
    int availableInstances;

    Resource(int id, string rname, int instances);
    
    bool allocate(int amount);
    void release(int amount);
};

// DeadlockDetector class
class DeadlockDetector {
public:
    vector<Process*> processes;
    vector<Resource*> resources;
    
    DeadlockDetector();
    ~DeadlockDetector();
    
    void addProcess(Process* proc);
    void addResource(Resource* res);
    
    // Detect deadlock
    bool detectDeadlock(vector<int>& deadlockedProcesses);
    
    // Helper function for cycle detection
    bool hasCycle(int node, vector<bool>& visited, vector<bool>& recStack, 
                  map<int, vector<int>>& graph);
    
    void clear();
};

// BankerAlgorithm class
class BankerAlgorithm {
public:
    vector<Process*> processes;
    vector<Resource*> resources;
    vector<int> available;
    
    BankerAlgorithm();
    
    void setProcesses(vector<Process*> procs);
    void setResources(vector<Resource*> res);
    void setAvailable(vector<int> avail);
    
    // Check if resource request is safe
    bool isSafeState(vector<int>& safeSequence);
    
    // Check if request can be granted
    bool canGrantRequest(int processId, vector<int> request, vector<int>& safeSequence);
};

#endif
