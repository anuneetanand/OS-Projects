// Anuneet Anand
// 2018022
// OS Optional Assignment 4
// Car Race

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// you can change the prototype of existing
// functions, add new routines, and global variables
// cheatmode, car1, car2, report are different processes
// they communicate with each other via pipes

int status;									//variable used to collect unnecessary read/write return values everywhere
int Pipe_Cheat_Car_1[2];					//cheatmode() -> car1()
int Pipe_Cheat_Car_2[2];					//cheatmode() -> car2()
int Pipe_Report_Car_1[2];					//report() <- car1()
int Pipe_Report_Car_2[2];					//report() <- car2()
int Pipe_Terminate_Car_1[2];				//report() -> car1()
int Pipe_Terminate_Car_2[2];				//report() -> car2()

// step-1
// ask user's if they want to cheat
// if yes, ask them if they want to relocate car1 or car2
// ask them the new location of car1/car2
// instruct car1/car2 to relocate to new position
// goto step-1
// Assumption : User will enter valid input values

void cheatmode()
{
	char buf[1000];
	close(Pipe_Cheat_Car_1[0]);
	close(Pipe_Cheat_Car_2[0]);
	while(1)
	{
		strcpy(buf,"");
		status = write(1,"Do you want to cheat? (Press Y)\n",32);
		status = scanf("%s",buf);
		if (buf[0] == 'Y'|| buf[0] =='y')
		{
			status = write(1,"Which car do you want to relocate? (1/2)\n",41);
			status = scanf("%s",buf);
			if (buf[0] == '1')
			{
				status = write(1,"Enter new location:\n",20);
				status = scanf("%s",buf);
				status = write(1,"\n",1);
				status = write(Pipe_Cheat_Car_1[1],buf,strlen(buf));
			}
			else if (buf[0] == '2')
			{
				status = write(1,"Enter new location:\n",20);
				status = scanf("%s",buf);
				status = write(1,"\n",1);
				status = write(Pipe_Cheat_Car_2[1],buf,strlen(buf));
			}
		}
	}
}

// step-1
// check if report wants me to terminate
// if yes, terminate
// sleep for a second
// generate a random number r between 0-10
// advance the current position by r steps
// check if cheat mode relocated me
// if yes set the current position to the new position
// send the current postion to report
// make sure that car1 and car2 generates a different
// random number
// goto step-1

void car1()
{
	int r;
	char buf[1000];
	int Position_1 = 0;
	srand(getpid());
	close(Pipe_Cheat_Car_1[1]);
	close(Pipe_Terminate_Car_1[1]);
	while(1)
	{
		strcpy(buf,"");
		status = read(Pipe_Terminate_Car_1[0],buf,1000);
		if (!strcmp(buf,"Terminate")){exit(0);}					// Checking for terminate signal
		sleep(1);
		r = (rand()%11);
		Position_1 += r;
		int Update = read(Pipe_Cheat_Car_1[0],buf,1000);
		if (Update>0){sscanf(buf,"%d",&Position_1);}			// Checking for update from cheatmode
		sprintf(buf,"%d",Position_1);
		status = write(Pipe_Report_Car_1[1],buf,strlen(buf));
	}
}

// step-1
// check if report wants me to terminate
// if yes, terminate
// sleep for a second
// generate a random number r between 0-10
// advance the current position by r steps
// check if cheat mode relocated me
// if yes set the current position to the new position
// send the current postion to report
// make sure that car1 and car2 generates a different
// random number
// goto step-1

void car2()
{
	int r;
	char buf[1000];
	int Position_2 = 0;
	srand(getpid());
	close(Pipe_Cheat_Car_2[1]);
	close(Pipe_Terminate_Car_2[1]);
	while(1)
	{
		strcpy(buf,"");
		status = read(Pipe_Terminate_Car_2[0],buf,1000);
		if (!strcmp(buf,"Terminate")){exit(0);}				// Checking for terminate signal
		sleep(1);
		r = (rand()%11);
		Position_2 += r;
		int Update = read(Pipe_Cheat_Car_2[0],buf,1000);	
		if (Update>0) {sscanf(buf,"%d",&Position_2);} 		// Checking for update from cheatmode
		sprintf(buf,"%d",Position_2);
		status = write(Pipe_Report_Car_2[1],buf,strlen(buf));
	}
}

// step-1
// sleep for a second
// read the status of car1
// read the status of car2
// whoever completes a distance of 100 steps is decalared as winner
// if both cars complete 100 steps together then the match is tied
// if (any of the cars have completed 100 steps)
//    print the result of the match
//    ask cars to terminate themselves
//    kill cheatmode using kill system call
//    return to main if report is the main process
// else
// 	  print the status of car1 and car2
// goto step-1

void report(int pid_cheat_mode)
{
	int t = 0;
	char Status_1[1000];
	char Status_2[1000];
	int Location_1 = 0;
	int Location_2 = 0;
	close(Pipe_Report_Car_1[1]);
	close(Pipe_Report_Car_2[1]);
	close(Pipe_Terminate_Car_1[0]);
	close(Pipe_Terminate_Car_2[0]);
	while(1)
	{
		sleep(1);

		if (t == 1)
		{
			status = write(Pipe_Terminate_Car_1[1],"Terminate",9);
			status = write(Pipe_Terminate_Car_2[1],"Terminate",9);
			kill(pid_cheat_mode,SIGKILL);
			exit(0);
		}

		strcpy(Status_1,"0");
		strcpy(Status_2,"0");
		status = read(Pipe_Report_Car_1[0],Status_1,1000);		// Reading car1 location
		status = read(Pipe_Report_Car_2[0],Status_2,1000);		// Reading car2 location

		status = write(1,"--------------------\n",21);
		status = write(1,"Car 1 Position: ",16);
		status = write(1,Status_1,strlen(Status_1));
		status = write(1,"\n",1);
		status = write(1,"Car 2 Position: ",16);
		status = write(1,Status_2,strlen(Status_2));
		status = write(1,"\n",2);
		status = write(1,"--------------------\n",21);
		status = write(Pipe_Terminate_Car_1[1],"Continue!",9);
		status = write(Pipe_Terminate_Car_2[1],"Continue!",9);

		sscanf(Status_1,"%d",&Location_1);
		sscanf(Status_2,"%d",&Location_2);
		if ((Location_1>=100) && (Location_2>=100))
		{
			status = write(1,"It's a Tie!\n",12);
			t = 1;
		}
		else if (Location_1>=100)
		{
			status = write(1,"Car 1 Won The Match!\n",21);
			t = 1;
		}
		else if (Location_2>=100)
		{
			status = write(1,"Car 2 Won The Match!\n",21);
			t = 1;
		}
		else
		{
			t = 0 ;
		}
	}
}

// create pipes
// create all processes
// wait for all processes to terminate
int main()
{
	status = pipe(Pipe_Cheat_Car_1);
	status = pipe(Pipe_Cheat_Car_2);
	status = pipe(Pipe_Report_Car_1);
	status = pipe(Pipe_Report_Car_2);
	status = pipe(Pipe_Terminate_Car_1);
	status = pipe(Pipe_Terminate_Car_2);
	fcntl(Pipe_Cheat_Car_1[0], F_SETFL, O_NONBLOCK);
	fcntl(Pipe_Cheat_Car_2[0], F_SETFL, O_NONBLOCK);
	fcntl(Pipe_Terminate_Car_1[0], F_SETFL, O_NONBLOCK);
	fcntl(Pipe_Terminate_Car_2[0], F_SETFL, O_NONBLOCK);

	int pid_car_1 = fork();
	if (pid_car_1 == 0){car1();}

	int pid_car_2 = fork();
	if (pid_car_2 == 0){car2();}

	int pid_cheat_mode = fork();
	if (pid_cheat_mode == 0) {cheatmode();}

	report(pid_cheat_mode);

	while(wait(NULL)>=0);

	close(Pipe_Cheat_Car_1[0]);
	close(Pipe_Cheat_Car_1[1]);
	close(Pipe_Cheat_Car_2[0]);
	close(Pipe_Cheat_Car_2[1]);
	close(Pipe_Report_Car_1[0]);
	close(Pipe_Report_Car_1[1]);
	close(Pipe_Report_Car_2[0]);
	close(Pipe_Report_Car_2[1]);
	close(Pipe_Terminate_Car_1[0]);
	close(Pipe_Terminate_Car_1[1]);
	close(Pipe_Terminate_Car_2[0]);
	close(Pipe_Terminate_Car_2[1]);

	return 0;
}

// End Of Code
