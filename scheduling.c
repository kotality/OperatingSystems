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
FILE *in;
proc *p;

int main()
{
	parser();
	printInfo();

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
}

void sjf()
{
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
	in = fopen("processes.in", "r");
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
}
