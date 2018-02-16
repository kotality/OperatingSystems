// Novira Farnaz, Kenia Castro, Sandy Demian
// COP 4600, Spring 2018
// Assignment 1: Scheduling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fcfs();
void sjf();
void rr();
void printRR(int, char*, int);
int waitingTime(int, int);
int turnaround(int, int);
void parser();

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
	// parse the input file and store the info in the variables
	// processCount, runFor, quantum, method, and the array p
	parser();

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
	FILE *out = fopen("processes.out", "w");
	int timeCounter, prevTimeCounter, processNum = processCount, finished = 0;
	int i, j, k, m, n, w;
	int temp, temp2;
	char temp3[10];

	int *remainingBurst = malloc(sizeof(int) * processCount);
	int *wait = malloc(sizeof(int) * processCount);
	int *turn = malloc(sizeof(int) * processCount);

	// Print scheduling and quantum info
	fprintf(out, "%d processes\n", processCount);
	fprintf(out, "Using Round-Robin\n");
	fprintf(out, "Quantum %d\n\n", quantum);

	// Sort processes by arrival time
	for (k = 0; k < processCount; k++)
	{
		for (m = 0; m < processCount; m++)
		{
			if (p[m].arrival > p[k].arrival)
			{
				temp = p[k].arrival;
				p[k].arrival = p[m].arrival;
				p[m].arrival = temp;

				temp2 = p[k].burst;
				p[k].burst = p[m].burst;
				p[m].burst = temp2;

				strcpy(temp3, p[k].name);
				strcpy(p[k].name, p[m].name);
				strcpy(p[m].name, temp3);
			}
		}
	}

	// Set up arrays for wait time, turnaround time, burst time
	for (i = 0; i < processCount; i++)
	{
		remainingBurst[i] = p[i].burst;
		wait[i] = 0;
		turn[i] = 0;
		// fprintf(out,"NAME %s ARRIVAL %d BURST %d\n", p[i].name, p[i].arrival, p[i].burst);
	}

	prevTimeCounter = 0;
	for (timeCounter = 0, j = 0; processCount != 0;)
	{
		// Prints first process arrival
		if (p[j].arrival == timeCounter)
		{
			fprintf(out, "Time %d: %s arrived\n", timeCounter, p[j].name);
		}

		if (remainingBurst[j] <= quantum && remainingBurst[j] > 0)
		{
			fprintf(out, "Time %d: %s selected (burst %d)\n", timeCounter, p[j].name, remainingBurst[j]);
			prevTimeCounter = timeCounter;
			timeCounter += remainingBurst[j];
			remainingBurst[j] = 0;
			finished = 1;
		}
		else if (remainingBurst[j] > 0)
		{
			// Check for arrival times in between new quantum
			while (p[j].arrival > prevTimeCounter && p[j].arrival <= timeCounter)
			{
				fprintf(out, "Time %d: %s arrived\n", p[j].arrival, p[j].name);
				prevTimeCounter++;
			}

			fprintf(out, "Time %d: %s selected (burst %d)\n", timeCounter, p[j].name, remainingBurst[j]);
			remainingBurst[j] -= quantum;
			prevTimeCounter = timeCounter;
			timeCounter += quantum;
		}

		if (remainingBurst[j] == 0 && finished == 1)
		{
			fprintf(out, "Time %d: %s finished\n", timeCounter, p[j].name);
			processCount--;
			wait[j] += waitingTime(timeCounter, j);
			turn[j] += turnaround(timeCounter, j);
			finished = 0;
		}

		if (j == (processCount - 1))
		{
			j = 0;
		}
		else if (p[j+1].arrival <= timeCounter)
		{
			j++;
		}
		else
		{
			j = 0;
		}

		// For idle processes
		if (processCount == 0 && timeCounter < runFor)
		{			
			fprintf(out, "Time %d: Idle\n", timeCounter);
		}
	}

	// Print wait and turnaround 
	fprintf(out, "Finished at time %d\n", runFor);
	for (n = processNum - 1; n > -1; n--)
	{
		fprintf(out, "\n%s wait %d turnaround %d", p[n].name, wait[n], turn[n]);
	}

	free(wait);
	free(turn);
	free(remainingBurst);
}

int waitingTime(int timeCounter, int count)
{
	return (timeCounter - p[count].arrival - p[count].burst);
}

int turnaround(int timeCounter, int count)
{
	return (timeCounter - p[count].arrival);
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

	fclose(in);
}
