#include "deadlock.h"
#include <algorithm>

using namespace std;

// ==================== Process Implementation ====================

Process::Process(int id, string pname, vector<int> max_resources) {
    pid = id;
    name = pname;
    maximum = max_resources;
    finished = false;
    
    // Initialize allocated to zeros
    for(int i = 0; i < max_resources.size(); i++) {
        allocated.push_back(0);
    }
    
    // Calculate initial need
    calculateNeed();
}

void Process::allocateResource(int resourceType, int amount) {
    if(resourceType >= 0 && resourceType < allocated.size()) {
        allocated[resourceType] += amount;
        calculateNeed();
    }
}

void Process::releaseResource(int resourceType, int amount) {
    if(resourceType >= 0 && resourceType < allocated.size()) {
        allocated[resourceType] -= amount;
        if(allocated[resourceType] < 0) {
            allocated[resourceType] = 0;
        }
        calculateNeed();
    }
}

void Process::calculateNeed() {
    need.clear();
    for(int i = 0; i < maximum.size(); i++) {
        need.push_back(maximum[i] - allocated[i]);
    }
}

// ==================== Resource Implementation ====================

Resource::Resource(int id, string rname, int instances) {
    rid = id;
    name = rname;
    totalInstances = instances;
    availableInstances = instances;
}

bool Resource::allocate(int amount) {
    if(amount <= availableInstances) {
        availableInstances -= amount;
        return true;
    }
    return false;
}

void Resource::release(int amount) {
    availableInstances += amount;
    if(availableInstances > totalInstances) {
        availableInstances = totalInstances;
    }
}

// ==================== DeadlockDetector Implementation ====================

DeadlockDetector::DeadlockDetector() {
    // Constructor
}

DeadlockDetector::~DeadlockDetector() {
    clear();
}

void DeadlockDetector::addProcess(Process* proc) {
    processes.push_back(proc);
}

void DeadlockDetector::addResource(Resource* res) {
    resources.push_back(res);
}

bool DeadlockDetector::hasCycle(int node, vector<bool>& visited, vector<bool>& recStack,
                                map<int, vector<int>>& graph) {
    visited[node] = true;
    recStack[node] = true;
    
    // Check all neighbors
    if(graph.find(node) != graph.end()) {
        for(int neighbor : graph[node]) {
            if(!visited[neighbor]) {
                if(hasCycle(neighbor, visited, recStack, graph)) {
                    return true;
                }
            } else if(recStack[neighbor]) {
                return true; // Cycle found
            }
        }
    }
    
    recStack[node] = false;
    return false;
}

bool DeadlockDetector::detectDeadlock(vector<int>& deadlockedProcesses) {
    deadlockedProcesses.clear();
    
    if(processes.empty()) {
        return false;
    }
    
    // Build Resource Allocation Graph
    map<int, vector<int>> graph;
    
    // Add edges: Process -> Resource (request edge)
    // Add edges: Resource -> Process (allocation edge)
    for(int i = 0; i < processes.size(); i++) {
        Process* p = processes[i];
        
        // Check if process is waiting for any resource
        for(int j = 0; j < resources.size(); j++) {
            // If process needs this resource and it's not available
            if(p->need[j] > 0 && resources[j]->availableInstances < p->need[j]) {
                // Add edge from process to resource (waiting)
                graph[p->pid].push_back(1000 + j); // Use 1000+ for resource IDs
            }
            
            // If process has allocated this resource
            if(p->allocated[j] > 0) {
                // Add edge from resource to process (allocated)
                graph[1000 + j].push_back(p->pid);
            }
        }
    }
    
    // Detect cycle using DFS
    int maxNodes = processes.size() + resources.size() + 1000;
    vector<bool> visited(maxNodes, false);
    vector<bool> recStack(maxNodes, false);
    
    // Check for cycle starting from each process
    for(int i = 0; i < processes.size(); i++) {
        int pid = processes[i]->pid;
        if(!visited[pid]) {
            if(hasCycle(pid, visited, recStack, graph)) {
                // Found deadlock - collect all processes in cycle
                for(int j = 0; j < processes.size(); j++) {
                    if(recStack[processes[j]->pid]) {
                        deadlockedProcesses.push_back(processes[j]->pid);
                    }
                }
                return true;
            }
        }
    }
    
    return false;
}

void DeadlockDetector::clear() {
    for(Process* p : processes) {
        delete p;
    }
    for(Resource* r : resources) {
        delete r;
    }
    processes.clear();
    resources.clear();
}

// ==================== BankerAlgorithm Implementation ====================

BankerAlgorithm::BankerAlgorithm() {
    // Constructor
}

void BankerAlgorithm::setProcesses(vector<Process*> procs) {
    processes = procs;
}

void BankerAlgorithm::setResources(vector<Resource*> res) {
    resources = res;
    
    // Initialize available vector
    available.clear();
    for(Resource* r : resources) {
        available.push_back(r->availableInstances);
    }
}

void BankerAlgorithm::setAvailable(vector<int> avail) {
    available = avail;
}

bool BankerAlgorithm::isSafeState(vector<int>& safeSequence) {
    safeSequence.clear();
    
    if(processes.empty()) {
        return true;
    }
    
    int n = processes.size();
    int m = resources.size();
    
    // Work vector (copy of available)
    vector<int> work = available;
    
    // Finish vector
    vector<bool> finish(n, false);
    
    // Try to find safe sequence
    int count = 0;
    while(count < n) {
        bool found = false;
        
        for(int i = 0; i < n; i++) {
            if(!finish[i]) {
                // Check if need <= work
                bool canFinish = true;
                for(int j = 0; j < m; j++) {
                    if(processes[i]->need[j] > work[j]) {
                        canFinish = false;
                        break;
                    }
                }
                
                if(canFinish) {
                    // Add allocated resources back to work
                    for(int j = 0; j < m; j++) {
                        work[j] += processes[i]->allocated[j];
                    }
                    
                    safeSequence.push_back(processes[i]->pid);
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        
        if(!found) {
            return false; // No safe sequence found
        }
    }
    
    return true;
}

bool BankerAlgorithm::canGrantRequest(int processId, vector<int> request, 
                                      vector<int>& safeSequence) {
    // Find the process
    Process* proc = nullptr;
    for(Process* p : processes) {
        if(p->pid == processId) {
            proc = p;
            break;
        }
    }
    
    if(proc == nullptr) {
        return false;
    }
    
    // Check if request <= need
    for(int i = 0; i < request.size(); i++) {
        if(request[i] > proc->need[i]) {
            return false; // Error: exceeds max claim
        }
    }
    
    // Check if request <= available
    for(int i = 0; i < request.size(); i++) {
        if(request[i] > available[i]) {
            return false; // Resources not available
        }
    }
    
    // Try allocating (pretend allocation)
    vector<int> oldAvailable = available;
    vector<int> oldAllocated = proc->allocated;
    vector<int> oldNeed = proc->need;
    
    for(int i = 0; i < request.size(); i++) {
        available[i] -= request[i];
        proc->allocated[i] += request[i];
        proc->need[i] -= request[i];
    }
    
    // Check if safe state
    bool safe = isSafeState(safeSequence);
    
    // Restore state
    available = oldAvailable;
    proc->allocated = oldAllocated;
    proc->need = oldNeed;
    
    return safe;
}
