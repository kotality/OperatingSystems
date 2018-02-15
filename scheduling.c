// Novira Farnaz, Kenia Castro, Sandy Demian
// COP 4600, Spring 2018
// Assignment 1: Scheduling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// int wait();
// int turnaround();
void printRR(int);
void fcfs();
void sjf();
void rr();
void parser();
void printInfo();

typedef struct
{ 
	char name[5];
	int arrival;
	int burst;
} proc;

int processCount, runFor, quantum;
char method[5], token[15];
proc *p;

int main()
{
	// parse the input file and store the info in the variables
	// processCount, runFor, quantum, method, and the array p
	parser();
	printInfo();

	// For testing only: print the info from the input file
	/*int i;
	printf("Processcount %d\n", processCount);
	printf("runfor %d\n", runFor);
	printf("Using %s\n", method);
	printf("Quantum %d\n", quantum);
	for(i=0 ; i<processCount ; i++)
	{
		printf("%s  ", p[i].name);
		printf("%d  ", p[i].arrival);
		printf("%d\n", p[i].burst);
	}*/

	// Call the correct scheduler based on the method
	if(strcmp(method, "fcfs") == 0)
	{
		fcfs();
	}
	else if(strcmp(method, "sjf") == 0)
	{
		sjf();
	}
	else if(strcmp(method, "rr") == 0)
	{
		rr();
	}

	return 0;
}

void fcfs()
{
	// struct used in the time array
	typedef struct
	{ 
		int selected;
		int arrived;
		int finished;
	} tick;

	int t, i, runUntil = 0, wait[processCount];
	tick time[runFor+1];

	// initialize all the information in the time array to -1
	for(t=0 ; t<(runFor+1) ; t++)
	{
		time[t].selected = -1;
		time[t].arrived = -1;
		time[t].finished = -1;
	}

	// record the arrival information in the time array
	for(i=0 ; i<processCount ; i++)
	{
		time[p[i].arrival].arrived = i;
	}

	// Schedule the processes
	for(t=0 ; t<(runFor+1) ; t++)
	{
		if(time[t].arrived != -1 && t >= runUntil)
		{
			i = time[t].arrived;
			wait[i] = t - p[i].arrival;
			time[t].selected = i;
			runUntil = t + p[i].burst;
			time[runUntil].finished = i;
		}
		else if(time[t].arrived != -1 && t < runUntil)
		{
			i = time[t].arrived;
			wait[i] = runUntil - p[i].arrival;
			time[runUntil].selected = i;
			runUntil = runUntil + p[i].burst;
			time[runUntil].finished = i;
		}
	}

	// Print to a file
	FILE *out = fopen("process.out", "w");

	fprintf(out, "%d processes\n", processCount);
	fprintf(out, "Using First Come First Served\n\n");

	for(t=0 ; t<(runFor+1) ; t++)
	{
		if(time[t].arrived != -1)
		{
			fprintf(out, "Time %d: %s arrived\n", t, p[time[t].arrived].name);
		}
		if(time[t].finished != -1)
		{
			fprintf(out, "Time %d: %s finished\n", t, p[time[t].finished].name);
		}
		if(time[t].selected != -1)
		{
			fprintf(out, "Time %d: %s selected ", t, p[time[t].selected].name);
			fprintf(out, "(burst %d)\n", p[time[t].selected].burst);
		}
	}
	fprintf(out, "Finished at time %d\n\n", t-1);

	for(i=0 ; i<processCount ; i++)
	{
		fprintf(out, "%s wait %d turnaround %d\n", p[i].name, wait[i], (wait[i] + p[i].burst));
	}

	fclose(out);
}

void sjf()
{
	int waitimes[10];
    int remt[10];
    int arr[10], sel[10];
    int turnt[10];
    int last = 0, time=0, sp, min = 2000, finishtime, l=0;
    int check;
     int nu=1;
    for(int a=0; a<processCount;a++){
        remt[a] = p[a].burst;
        arr[a] = 0;
        sel[a] = 0;
    }
    
    while (last != processCount) {
        for(int i = 0; i<processCount; i++){
            if(p[i].arrival == time && arr[i]==0){
                printf("Time %d: %s arrived\n",time, p[i].name);
		        arr[i] = 1;
		    }
            if ((p[i].arrival <= time) && (remt[i]<=min) && (remt[i]>0)){
				min = remt[i];
                sp = i;
				if(l!=i) 
				  sel[l] = 0;

                check = 1;
                l = i;
            }
        }
        if (check == 0){
            time++;
            continue;
        }
		
		if (sel[sp]==0){
           printf("Time %d: %s selected (burst %d)\n",time, p[sp].name, remt[sp]);
		   sel[sp] = 1;
		}
        
        remt[sp]--;
        min = remt[sp];
        if (min==0){
            min = 2000;
        }

        if (remt[sp]==0){
            last++;
            finishtime = time + 1;
			printf("Time %d: %s finished\n", finishtime, p[sp].name);
            waitimes[sp] = finishtime - p[sp].burst - p[sp].arrival;
            turnt[sp] = p[sp].burst + waitimes[sp];

            if (waitimes[sp]<0)
            waitimes[sp] = 0;
        }
		  
        time++;
    }

	if(finishtime<runFor){
		int sub = runFor - finishtime;
		for(int z=0;z<sub;z++){
            printf("Time %d: IDLE\n",finishtime++);
		}
           
		   printf("Finished at time %d\n",runFor);
		   for(int z=0;z<processCount;z++){
			   printf("%s wait %d turnaround %d\n",p[z].name, waitimes[z], turnt[z]);
		   }
	}
}

void rr()
{
	int *remainingBurst = malloc(sizeof(int)*processCount);
	int timeCounter = 0;
	int running = 1;
	int loop = 0;

	// Copy burst times of each process into separate tracking array
	for (int j = 0; j < processCount; j++)
	{
		remainingBurst[j] = p[j].burst;
	}

	printRR(timeCounter);

	while (running)
	{
		running = 0;

		for (int i = 0; i < runFor; i++)
		{
			if (remainingBurst[i] > quantum)
			{		
				loop++;		
				running = 1;
				timeCounter += quantum;
				remainingBurst[i] -= quantum;

				printRR(timeCounter);
				// printf("loop count: %d\n", loop);
			}
			else  // smaller than quantum left
			{
				timeCounter += remainingBurst[i];
				// wait();
				remainingBurst[i] = 0;
				// printRR(timeCounter);
			}
		}

		if (running == 0);
			break;
	}

	free(remainingBurst);
}

// int wait()
// {
// 	int wait = completionTime - burstTime;
// 	return wait;
// }

// int turnaround()
// {
// 	int turnaround = completionTime - arrivalTime;
// 	return turnaround;
// }

void printInfo()
{
	// Outputs number of processes, type of scheduling, and quantum value
	printf("%d processes\n", processCount);
	if (strcmp(method, "fcfs") == 0)
	{
		printf("Using First Come First Served\n\n");
	}
	else if (strcmp(method, "sjf") == 0)
	{
		printf("Using Shortest Job First\n\n");
	}
	else if (strcmp(method, "rr") == 0)
	{
		printf("Using Round-Robin\n");
		printf("Quantum %d\n\n", quantum);
	}

	// I used this for testing only
	// printf("Processcount %d\n", processCount);
	// printf("runfor %d\n", runFor);
	// printf("Using %s\n", method);
	// printf("Quantum %d\n", quantum);
	// for(int i=0; i<processCount ; i++)
	// {
	// 	printf("%s  ", p[i].name);
	// 	printf("%d  ", p[i].arrival);
	// 	printf("%d\n", p[i].burst);
	// }
}

void printRR(int timeCounter)
{
	int i;
	char arrival[10] = "";

	// printf("%d process\n", processCount);
	// printf("timecounter %d\n", timeCounter);
	
	// Outputs the execution timeline
	for (i = 0; i < processCount; i++)
	{
		printf("Time %d: ", timeCounter);

		if (timeCounter == p[i].arrival)
		{
			strcpy(arrival, "arrived");
			printf("%s %s", p[i].name, arrival);
		}
		else
		{
			strcpy(arrival, "selected");
			printf("%s %s (burst %d)", p[i].name, arrival, p[i].burst);
		}
	
		printf("\n");
	}
}


void parser()
{
	FILE *in = fopen("process.in", "r");
	fscanf(in, "%s", token);

	// Parser
	while(strcmp(token, "end") != 0 && !feof(in))
	{
		if(strcmp(token, "processcount") == 0)
		{
			fscanf(in, "%d", &processCount);
			fscanf(in, "%s", token);
		}
		else if(strcmp(token, "runfor") == 0)
		{
			fscanf(in, "%d", &runFor);
			fscanf(in, "%s", token);
		}
		else if(strcmp(token, "use") == 0)
		{
			fscanf(in, "%s", method);
			fscanf(in, "%s", token);
		}
		else if(strcmp(token, "quantum") == 0)
		{
			fscanf(in, "%d", &quantum);
			fscanf(in, "%s", token);	
		}
		else if(strcmp(token, "#") == 0)
		{
			char c = fgetc(in);;
			while(c != '\n')
			{
				c = fgetc(in);
			}
			fscanf(in, "%s", token);
		}
		else if(strcmp(token, "process") == 0)
		{
			p = malloc(sizeof(proc)*processCount);

			for(int i=0; i<processCount ; i++)
			{
				fscanf(in, "%s", token);

				if(strcmp(token, "name") == 0 )
				{
					fscanf(in, "%s", p[i].name);
					fscanf(in, "%s", token);
				}
				if(strcmp(token, "arrival") == 0)
				{
					fscanf(in, "%d", &(p[i].arrival));
					fscanf(in, "%s", token);
				}
				if(strcmp(token, "burst") == 0)
				{
					fscanf(in, "%d", &(p[i].burst));
					fscanf(in, "%s", token);
				}
				
			}

		}

	}

	fclose(in);
}
