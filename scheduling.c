// Novira Farnaz, Kenia Castro, Sandy Demian
// COP 4600, Spring 2018
// Assignment 1: Scheduling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void fcfs();
void sjf();
void rr();
void parser();

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

}

void rr()
{

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
