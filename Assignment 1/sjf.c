#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sjf();
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

int main(){

    parser();
    printf("Processcount %d\n", processCount);
	printf("runfor %d\n", runFor);
	printf("Using %s\n", method);
	printf("Quantum %d\n", quantum);
	for(int i=0; i<processCount ; i++)
	{
		printf("%s  ", p[i].name);
		printf("%d  ", p[i].arrival);
		printf("%d\n", p[i].burst);
	}

    printf("\n\n%d processes", processCount);
    printf("\nUsing Shortest Job First (Pre)\n");
   
	if(strcmp(method, "sjf") == 0)
	{
		sjf();
	}
}


void parser()
{
	in = fopen("set4_process.in", "r");
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

//============Shortest Job First Scheduling==============
void sjf(){
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