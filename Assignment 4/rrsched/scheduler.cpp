/// =========================================================================
/// Copyright (C) 2023 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading
//Name: Muhammad Taimur Rizwan || Assignment 4 Q2

#include "scheduler.h"
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <climits>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <deque>
#include <algorithm>
using namespace std;

//removed because it's already defined in scheduler.h
// struct Process {
//     int id;
//     int64_t arrival, burst;
//     int64_t start_time, finish_time;

// };

// this is the function you should implement
//
// simulate_rr() implements a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//
void simulate_rr(int64_t quantum, int64_t max_seq_len, std::vector<Process> & processes, std::vector<int> & seq) {

    // replace the wrong implementation below with your own!!!!
    seq.clear();

    if(processes.empty()) return;

    //initializing variables
    bool jumpFlag = false;
    int64_t rem_time = 0;
    std::deque<int> tempQueue;
    std::vector<bool> endProc;
    std::vector<bool> begin(processes.size());
    std::vector<int64_t> proc(processes.size());
    int temp = 0;
    std::queue<int> firstQueue;
    std::vector<bool> end(processes.size());
    int convProcSizeToInt = int(processes.size());

    


    while(1){ //simulation loop

        while(processes[temp].arrival <= rem_time && temp < convProcSizeToInt){ //pushing processes in queue
            begin[temp] = false;
            end[temp] = false;
            firstQueue.push(temp);
            proc[temp] = processes[temp].burst;

            temp = temp + 1;

        }
        //setting false for every iteration of the simulation loop
        jumpFlag = false;

        if(firstQueue.empty()){ //queue is empty
            seq.push_back(-1);
            if(temp < convProcSizeToInt) rem_time = processes[temp].arrival;
                
            }

        else { //for processes in the queue

            int queue_size = int(firstQueue.size());

            int currProc = firstQueue.front();

            firstQueue.pop();

            if(seq.empty()) seq.push_back(currProc);
            if(seq.back() != currProc) seq.push_back(currProc);

            

            //initiating the process
            if(!begin[currProc]){
                begin[currProc] = true;
                processes[currProc].start_time = rem_time;
                
            }

            //Case handling
            if(temp == convProcSizeToInt && queue_size == 1){ //Last process is temp

                
                rem_time = rem_time + proc[currProc]; 
                proc[currProc] = 0;
                



            }
            //temp process finishes before the next 
            if(processes[temp].arrival >= rem_time + proc[currProc] && queue_size == 1){ 
                
                processes[currProc].finish_time = proc[currProc] + rem_time;      
                proc[currProc] = 0; 
                end[currProc] = true;
                                            
                


            }
            //half jump
            else if( processes[temp].arrival - rem_time >= quantum && queue_size == 1){ //move to the next process

                int64_t next = quantum * ((processes[temp].arrival - rem_time) / quantum + 1);
                rem_time = rem_time + next;
                proc[currProc] = proc[currProc] - next;
                

            }


            
            else{
                //-------- initialzing second queue variables
                std::vector<int> vector_Queue;
                std::queue<int> secondQueue = firstQueue;
                std:int x = 0;
                vector_Queue.push_back(currProc);

                while(!secondQueue.empty()){ //initializing second queue to copy the fist queue into

                    vector_Queue.push_back(secondQueue.front());
                    secondQueue.pop();

                }

                int64_t minProc = LLONG_MAX;
                //while loop to find the min process
                auto it = vector_Queue.begin();
                while(it != vector_Queue.end()){
                    int a = *it;

                    if(minProc > proc[a]) minProc = proc[a];

                    it++;
                    
                }

                if(convProcSizeToInt > temp){

                    minProc = std::min(minProc, processes[temp].arrival - rem_time);

                    minProc = minProc / vector_Queue.size();
                }
                minProc = minProc - (minProc % quantum); //padding minProc with Quantum by decrementing minProc

                if(quantum >= minProc){ //regular quantum jumps

                    if(proc[currProc] <= quantum){ //if quantum jumps finished
                        rem_time = rem_time + proc[currProc];
                        proc[currProc] = 0;
                    }

                    else { 
                        proc[currProc] = proc[currProc] - quantum;
                        rem_time = rem_time + quantum;
                    }
                }

                else if(quantum <= minProc){ //Burst jump 

                    jumpFlag = true;
                    std::queue<int> nQueue; //new queue
                    int64_t start_time = rem_time;

                    

                    for(int a :vector_Queue){ //Burst every process
                        rem_time = rem_time + minProc;
                        proc[a] = proc[a] - minProc;

                        if(proc[a] != 0) nQueue.push(a); //Requeue
                        
                    }



                    firstQueue = nQueue;

                    //for test files 3-7 debug (finish time for processes)
                    int i = 0;
                    auto it = vector_Queue.begin();

                    while(it != vector_Queue.end()){
                        int a = *it;



                        if(proc[a] == 0){
                            if(!end[a]){
                            processes[a].finish_time = rem_time - quantum * (vector_Queue.size() - 1) + quantum * i;
                            end[a] = true;
                            }

                        }
                        if(!begin[a]){
                            processes[a].start_time = start_time + quantum * i;
                            begin[a] = true;
                        }                        
                        i++;
                        it++;
                    }
                    

                    
                    int64_t m = 0; //to populate list
                    while(int64_t(seq.size()) <= max_seq_len && m < (minProc / quantum)){
                        for(int a : vector_Queue){
                            if(seq.back()!= a) seq.push_back(a); //update a into seq list
                            
                        }
                        m++;
                    }

                }


            }

                if(!jumpFlag){

                    if(proc[currProc] != 0){ //queing the new arrived processes 

                        while(rem_time > processes[temp].arrival && convProcSizeToInt > temp){

                            firstQueue.push(temp); 

                            proc[temp] = processes[temp].burst; //update burst

                            begin[temp] = false;

                            end[temp] = false; //set begin and end temp processes to false

                            temp = temp + 1; //update temp

                        }
                        if(proc[currProc] != 0) firstQueue.push(currProc); //push proc back to first queue if not complete

                    }

                    else{ //finished process

                        if(proc[currProc] == 0){

                            if(!end[currProc]){

                                processes[currProc].finish_time = rem_time; //update the finish time of the current proc with rem time
                                
                            }
                        }
                    }
                }


        }


        //brek to end processes
        if(firstQueue.empty() && convProcSizeToInt == temp) break;
            

    }

    seq.resize(std::min(max_seq_len, int64_t(seq.size())));

}

