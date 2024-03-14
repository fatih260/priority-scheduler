#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define MAX_PROCESS 10
#define MAX_INSTRUCTIONS 21
#define TIME_QUANTUM_SILVER 80
#define TIME_QUANTUM_GOLD 120
#define TIME_QUANTUM_PLATINUM 120
#define CONTEXT_SWITCH_TIME 10

// Instruction structure
typedef struct {
    char name[8];
    int duration;
} Instruction;


// Constant instructions
Instruction instructions[MAX_INSTRUCTIONS] = {
    {"instr1", 90}, {"instr2", 80}, {"instr3", 70}, {"instr4", 60}, {"instr5", 50},
    {"instr6", 40}, {"instr7", 30}, {"instr8", 20}, {"instr9", 30}, {"instr10", 40},
    {"instr11", 50}, {"instr12", 60}, {"instr13", 70}, {"instr14", 80}, {"instr15", 90},
    {"instr16", 80}, {"instr17", 70}, {"instr18", 60}, {"instr19", 50}, {"instr20", 40},
    {"exit", 10}
};


// Process structure
typedef struct {
    char name[3];
    int priority;
    int arrival_time;
    int updated_arrival_time;
    char type[9]; // 0: GOLD, 1: SILVER, 2: PLATINUM
    int completion_time;
    int num_instructions;
    int instruction_pointer;
    int aging;
    int active_quantum;
    int quantum;
    int turnaround_time;
    Instruction instructions_array[MAX_INSTRUCTIONS];
} Process;

// Read process definitions from file
Process processes[MAX_PROCESS];


// Function prototypes
void readProcesses(char *filename);
void readProcessInstructions(char *pfilename);
int getProcessIndex();

int numProcesses = 0;

//int round_robin_available = 0;
int platinum_check = 0;

int generalTime = 0;

int main() {
    //printf("Hello World!\n");
    //readProcesses("Example_Inputs_Outputs_v3/def11.txt");//"definition.txt");//
    readProcesses("definition.txt");

    

    int pre_index = -1;


    while (1) {

        
        // Get process index
        int index = getProcessIndex();
        
        if (index == -1) {
            break;
        }

        

        if (index != pre_index) {
            // Implement logic for context switch

            // Reset quantum and update waiting time
            //printf("Context switch %d\n", generalTime);
            generalTime += CONTEXT_SWITCH_TIME;
            //processes[pre_index].updated_arrival_time = generalTime;
            // if the process is preempted even if its quantum is not over, update its arrival time
            if (processes[pre_index].active_quantum != 0) {
                processes[pre_index].active_quantum = 0;
                processes[pre_index].aging++;

                processes[pre_index].updated_arrival_time = generalTime;

                if (processes[pre_index].aging == 3 && strcmp(processes[pre_index].type, "SILVER") == 0) {
                    strcpy(processes[pre_index].type,"GOLD");
                    processes[pre_index].quantum = TIME_QUANTUM_GOLD;

                    processes[pre_index].aging = 0;

                }
                if (processes[pre_index].aging == 5 && strcmp(processes[pre_index].type, "GOLD") == 0) {
                    strcpy(processes[pre_index].type,"PLATINUM");
                    processes[pre_index].quantum = TIME_QUANTUM_PLATINUM;

                    processes[pre_index].aging = 0;
                    
                }
            }

            
            
        }
        // update instruction pointer        
        // Update general time

        // if the process is platinum, all its instructions are executed 
        if (platinum_check == 1){
            

            //printf("PLATINUM %s %d\n", processes[index].name, generalTime);

            while (processes[index].instruction_pointer != processes[index].num_instructions) {
                //printf("in platinum generalTime: %d\n", generalTime);
                generalTime += processes[index].instructions_array[processes[index].instruction_pointer].duration;
                processes[index].instruction_pointer++;
                //processes[index].updated_arrival_time = generalTime;
                //printf("after in platinum generalTime: %d\n", generalTime);    
            }

            processes[index].completion_time = generalTime;
            processes[index].turnaround_time = processes[index].completion_time - processes[index].arrival_time;
            pre_index = index;
            continue;
        }
        //printf("not in platinum generalTime: %d\n", generalTime);
        //printf("Execution %s %d\n", processes[index].name, generalTime);
        int val = processes[index].instructions_array[processes[index].instruction_pointer].duration;
        generalTime += val;
        processes[index].instruction_pointer++;
        
        

        
        if (processes[index].instruction_pointer == processes[index].num_instructions) {
            // update for exit
            //process index is exiting, write process name
            //printf("in exit\n");
            

            processes[index].completion_time = generalTime;
            processes[index].turnaround_time = processes[index].completion_time - processes[index].arrival_time;
        }
        //printf("after not in platinum generalTime: %d\n", generalTime);

        // Update aging
        //printf("Current time and process name: %d %s\n", generalTime, processes[index].name);
        // printf("processes[index].aging: %d\n", processes[index].aging);
        //printf("val: %d\n", val);

        processes[index].active_quantum += val;
        if (processes[index].active_quantum >= processes[index].quantum) {

            //update arrival time
            processes[index].updated_arrival_time = generalTime;
            
            processes[index].active_quantum = 0;
            processes[index].aging++;
        } 
        //printf("after processes[index].active_quantum: %d\n", processes[index].active_quantum);
        //printf("after processes[index].quantum: %d\n", processes[index].quantum);
       // printf("after processes[index].aging: %d\n", processes[index].aging);

        //}
        //printf("updated_arrival_time: %d\n", processes[index].updated_arrival_time);

        //do the necessary type conversions

        if (processes[index].aging == 3 && strcmp(processes[index].type, "SILVER") == 0) {
            strcpy(processes[index].type,"GOLD");
            processes[index].quantum = TIME_QUANTUM_GOLD;

            processes[index].aging = 0;

        }
        if (processes[index].aging == 5 && strcmp(processes[index].type, "GOLD") == 0) {
            strcpy(processes[index].type,"PLATINUM");
            processes[index].quantum = TIME_QUANTUM_PLATINUM;

            processes[index].aging = 0;
            
        }


        pre_index = index;
    }
    

    // calculate average turnaround time and average waiting time
    float avg_turnaround_time = 0;
    float total_burst_time = 0;
    
    for (int i = 0; i < numProcesses; ++i) {
        avg_turnaround_time += processes[i].turnaround_time;

        for (int j=0; j < processes[i].num_instructions; j++) {
            total_burst_time += processes[i].instructions_array[j].duration;
        }
        
    }
    float avg_waiting_time = avg_turnaround_time - total_burst_time;

    avg_turnaround_time /= numProcesses;
    avg_waiting_time /= numProcesses;

    // Average waiting time:
    if (avg_waiting_time == (int)avg_waiting_time) {
        printf("%d\n", (int)avg_waiting_time);
    } else {
        printf("%.1f\n", avg_waiting_time);
    }

    // Average turnaround time:
    if (avg_turnaround_time == (int)avg_turnaround_time) {
        printf("%d\n", (int)avg_turnaround_time);
    } else {
        printf("%.1f\n", avg_turnaround_time);
    }
    

    return 0;
}

//read the input files 
void readProcessInstructions(char *pfilename) {
    FILE *file = fopen(pfilename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", pfilename);
        exit(1);
    }

    int numInstructions = 0;

    while (fscanf(file, "%s", processes[numProcesses].instructions_array[numInstructions].name) == 1) {
        if (strcmp(processes[numProcesses].instructions_array[numInstructions].name, "exit") == 0) {
            processes[numProcesses].instructions_array[numInstructions].duration = 10;
            numInstructions++;
            processes[numProcesses].num_instructions = numInstructions;
            break;
        }
        else {

            // Find the corresponding duration from the global instructions array
            for (int i = 0; i < MAX_INSTRUCTIONS; ++i) {
                if (strcmp(processes[numProcesses].instructions_array[numInstructions].name, instructions[i].name) == 0) {
                    processes[numProcesses].instructions_array[numInstructions].duration = instructions[i].duration;
                    break;
                }
            }
        }

        numInstructions++;
    }

    fclose(file);
}

void readProcesses(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(1);
    }
    //printf("file opened\n");
    while (fscanf(file, "%s %d %d %s", processes[numProcesses].name, &processes[numProcesses].priority,
              &processes[numProcesses].arrival_time, processes[numProcesses].type) == 4) {
        //printf("reading process\n");
        //printf("%s %d %d %s\n", processes[numProcesses].name, processes[numProcesses].priority,
                 // processes[numProcesses].arrival_time, processes[numProcesses].type);
        // Read instructions for process i

        processes[numProcesses].updated_arrival_time = processes[numProcesses].arrival_time;
        if (strcmp(processes[numProcesses].type, "PLATINUM") == 0) {
            processes[numProcesses].quantum = TIME_QUANTUM_PLATINUM;
            
        } else if (strcmp(processes[numProcesses].type, "GOLD") == 0) {
            processes[numProcesses].quantum = TIME_QUANTUM_GOLD;
           
        } else if (strcmp(processes[numProcesses].type, "SILVER") == 0) {
            processes[numProcesses].quantum = TIME_QUANTUM_SILVER;
           
        }
        char pfilename[20];
        sprintf(pfilename, "%s.txt", processes[numProcesses].name);
        readProcessInstructions(pfilename);
        

        numProcesses++;
    }

    fclose(file);
}


int getProcessIndex() {
    //first check if all processes are done
    int allDone = 1;
    for (int i = 0; i < numProcesses; ++i) {
        if (processes[i].instruction_pointer < processes[i].num_instructions) {
            allDone = 0;
            break;
        }
    }
    if (allDone) {
        return -1;
    }
    
    int numCandidates = 0;
    int candidateIndexes[MAX_PROCESS];

    // Find processes that have arrived and have instructions remaining
    for (int i = 0; i < numProcesses; ++i) {
        if (processes[i].arrival_time <= generalTime && processes[i].instruction_pointer < processes[i].num_instructions) {
            candidateIndexes[numCandidates++] = i;
        }
    }
    //printf("numCandidates: %d\n", numCandidates);

    //print candidate indexes
    for (int i = 0; i < numCandidates; ++i) {
        //printf("%d ", candidateIndexes[i]);
    }

    platinum_check = 0;
    // Check if there are any platinum processes in the candidate list
    int platinumCandidates[MAX_PROCESS];
    int numPlatinumCandidates = 0;
    for (int i = 0; i < numCandidates; ++i) {
        if (strcmp(processes[candidateIndexes[i]].type, "PLATINUM") == 0) {
            platinumCandidates[numPlatinumCandidates++] = candidateIndexes[i];
            platinum_check = 1;
        }
    }

    // If there is more than one platinum candidate, sort them by priority
    if (numPlatinumCandidates >= 1) {
        for (int i = 0; i < numPlatinumCandidates - 1; ++i) {
            for (int j = 0; j < numPlatinumCandidates - i - 1; ++j) {
                // Sort by priority
                if (processes[platinumCandidates[j]].priority < processes[platinumCandidates[j + 1]].priority) {
                    int temp = platinumCandidates[j];
                    platinumCandidates[j] = platinumCandidates[j + 1];
                    platinumCandidates[j + 1] = temp;
                } else if (processes[platinumCandidates[j]].priority == processes[platinumCandidates[j + 1]].priority) {
                
                    if (processes[platinumCandidates[j]].arrival_time > processes[platinumCandidates[j + 1]].arrival_time) {
                        int temp = platinumCandidates[j];
                        platinumCandidates[j] = platinumCandidates[j + 1];
                        platinumCandidates[j + 1] = temp;
                    } else if (processes[platinumCandidates[j]].arrival_time == processes[platinumCandidates[j + 1]].arrival_time) {
                        
                        // If arrival times are equal, sort by names
                        if (strcmp(processes[platinumCandidates[j]].name, processes[platinumCandidates[j + 1]].name) > 0) {
                            int temp = platinumCandidates[j];
                            platinumCandidates[j] = platinumCandidates[j + 1];
                            platinumCandidates[j + 1] = temp;
                        }                            
                                    
                    }
                }
            }
        }
        return platinumCandidates[0]; // Return the platinum process with the highest priority
    }

    // if there is no candidate, make generalTime equal to the next process arrival time
    if (numCandidates == 0) {
        int min_ = 100000;
        for (int i = 0; i < numProcesses; ++i) {
            //printf("processes[i].arrival_time: %d\n", processes[i].arrival_time);
            if(processes[i].instruction_pointer < processes[i].num_instructions && processes[i].arrival_time < min_) {
                min_ = processes[i].arrival_time;
            }
            
        }
        generalTime = min_;
        return getProcessIndex();
    }
    
    
    // Sort processes based on priority, arrival time, and names
    for (int i = 0; i < numCandidates - 1; ++i) {
        for (int j = 0; j < numCandidates - i - 1; ++j) {
            // Sort by priority
            if (processes[candidateIndexes[j]].priority < processes[candidateIndexes[j + 1]].priority) {
                int temp = candidateIndexes[j];
                candidateIndexes[j] = candidateIndexes[j + 1];
                candidateIndexes[j + 1] = temp;
            } else if (processes[candidateIndexes[j]].priority == processes[candidateIndexes[j + 1]].priority) {

                if (processes[candidateIndexes[j]].updated_arrival_time > processes[candidateIndexes[j + 1]].updated_arrival_time) {
                    int temp = candidateIndexes[j];
                    candidateIndexes[j] = candidateIndexes[j + 1];
                    candidateIndexes[j + 1] = temp;
                } else if (processes[candidateIndexes[j]].updated_arrival_time == processes[candidateIndexes[j + 1]].updated_arrival_time) { 

                    // If arrival times are equal, sort by names
                    if (strcmp(processes[candidateIndexes[j]].name, processes[candidateIndexes[j + 1]].name) > 0) {
                        int temp = candidateIndexes[j];
                        candidateIndexes[j] = candidateIndexes[j + 1];
                        candidateIndexes[j + 1] = temp;
                    }
                    
                }
                
                               
            }
        }
    }

    
    // Return the index of the first process in the sorted order
    return candidateIndexes[0];
}
