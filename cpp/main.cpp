#include "deadlock.h"
#include <cstring>

using namespace std;

// Global objects that Python will interact with
DeadlockDetector* detector = nullptr;
BankerAlgorithm* banker = nullptr;

// ==================== C Interface for Python ====================

extern "C" {
    
    // Initialize the system
    void init_system() {
        if(detector != nullptr) {
            delete detector;
        }
        if(banker != nullptr) {
            delete banker;
        }
        detector = new DeadlockDetector();
        banker = new BankerAlgorithm();
    }
    
    // Add a new process
    int add_process(int pid, const char* name, int* max_resources, int num_resources) {
        if(detector == nullptr) {
            return -1;
        }
        
        vector<int> max_vec;
        for(int i = 0; i < num_resources; i++) {
            max_vec.push_back(max_resources[i]);
        }
        
        Process* proc = new Process(pid, string(name), max_vec);
        detector->addProcess(proc);
        
        return 0;
    }
    
    // Add a new resource
    int add_resource(int rid, const char* name, int instances) {
        if(detector == nullptr) {
            return -1;
        }
        
        Resource* res = new Resource(rid, string(name), instances);
        detector->addResource(res);
        
        return 0;
    }
    
    // Allocate resource to process
    int allocate_resource(int pid, int resource_type, int amount) {
        if(detector == nullptr) {
            return -1;
        }
        
        // Find process
        Process* proc = nullptr;
        for(Process* p : detector->processes) {
            if(p->pid == pid) {
                proc = p;
                break;
            }
        }
        
        if(proc == nullptr) {
            return -1;
        }
        
        // Find resource
        if(resource_type < 0 || resource_type >= detector->resources.size()) {
            return -1;
        }
        
        Resource* res = detector->resources[resource_type];
        
        // Try to allocate
        if(res->allocate(amount)) {
            proc->allocateResource(resource_type, amount);
            return 0;
        }
        
        return -1; // Not enough resources available
    }
    
    // Release resource from process
    int release_resource(int pid, int resource_type, int amount) {
        if(detector == nullptr) {
            return -1;
        }
        
        // Find process
        Process* proc = nullptr;
        for(Process* p : detector->processes) {
            if(p->pid == pid) {
                proc = p;
                break;
            }
        }
        
        if(proc == nullptr) {
            return -1;
        }
        
        // Find resource
        if(resource_type < 0 || resource_type >= detector->resources.size()) {
            return -1;
        }
        
        Resource* res = detector->resources[resource_type];
        
        // Release
        proc->releaseResource(resource_type, amount);
        res->release(amount);
        
        return 0;
    }
    
    // Detect deadlock
    int detect_deadlock(int* deadlocked_pids, int max_size) {
        if(detector == nullptr) {
            return 0;
        }
        
        vector<int> deadlocked;
        bool hasDeadlock = detector->detectDeadlock(deadlocked);
        
        if(!hasDeadlock) {
            return 0;
        }
        
        // Copy to output array
        int count = 0;
        for(int i = 0; i < deadlocked.size() && i < max_size; i++) {
            deadlocked_pids[i] = deadlocked[i];
            count++;
        }
        
        return count;
    }
    
    // Check if request is safe (Banker's Algorithm)
    int check_safe_request(int pid, int* request, int num_resources, 
                          int* safe_sequence, int max_size) {
        if(detector == nullptr || banker == nullptr) {
            return -1;
        }
        
        // Update banker with current state
        banker->setProcesses(detector->processes);
        banker->setResources(detector->resources);
        
        vector<int> req_vec;
        for(int i = 0; i < num_resources; i++) {
            req_vec.push_back(request[i]);
        }
        
        vector<int> safe_seq;
        bool isSafe = banker->canGrantRequest(pid, req_vec, safe_seq);
        
        if(isSafe) {
            // Copy safe sequence
            for(int i = 0; i < safe_seq.size() && i < max_size; i++) {
                safe_sequence[i] = safe_seq[i];
            }
            return safe_seq.size();
        }
        
        return 0; // Not safe
    }
    
    // Get number of processes
    int get_process_count() {
        if(detector == nullptr) {
            return 0;
        }
        return detector->processes.size();
    }
    
    // Get number of resources
    int get_resource_count() {
        if(detector == nullptr) {
            return 0;
        }
        return detector->resources.size();
    }
    
    // Get process info
    int get_process_info(int index, int* pid, char* name, int* allocated, 
                        int* maximum, int* need, int max_resources) {
        if(detector == nullptr || index >= detector->processes.size()) {
            return -1;
        }
        
        Process* proc = detector->processes[index];
        *pid = proc->pid;
        strcpy(name, proc->name.c_str());
        
        for(int i = 0; i < proc->allocated.size() && i < max_resources; i++) {
            allocated[i] = proc->allocated[i];
            maximum[i] = proc->maximum[i];
            need[i] = proc->need[i];
        }
        
        return 0;
    }
    
    // Get resource info
    int get_resource_info(int index, int* rid, char* name, 
                         int* total, int* available) {
        if(detector == nullptr || index >= detector->resources.size()) {
            return -1;
        }
        
        Resource* res = detector->resources[index];
        *rid = res->rid;
        strcpy(name, res->name.c_str());
        *total = res->totalInstances;
        *available = res->availableInstances;
        
        return 0;
    }
    
    // Clear all data
    void clear_system() {
        if(detector != nullptr) {
            detector->clear();
        }
    }
    
    // Cleanup
    void cleanup_system() {
        if(detector != nullptr) {
            delete detector;
            detector = nullptr;
        }
        if(banker != nullptr) {
            delete banker;
            banker = nullptr;
        }
    }
}
