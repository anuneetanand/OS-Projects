// Anuneet Anand
// 2018022
// CN Assignment - 1
// Client

#include<stdio.h> 
#include<sys/socket.h> 
#include<unistd.h> 
#include<netinet/in.h> 
#include<stdlib.h> 
#include<string.h>
#define PORT 9001

int main()
{
	int File_Size;
	char Read_Buffer[1024];
	char Write_Buffer[1024];

	struct sockaddr_in server;						//Server Details
	server.sin_family = AF_INET; 
	server.sin_port = htons(PORT); 
	server.sin_addr.s_addr = INADDR_ANY;

	printf("[~] Connecting to Server...\n");
	int Socket = socket(AF_INET, SOCK_STREAM, 0);
	int Status = connect(Socket, (struct sockaddr * )&server, sizeof(server));	// Client Initiates Connection

	if (Status>=0)
	{
		printf("[+] Connected to Server!\n");
		printf("[-] Enter File Name : ");
		scanf("%s",Write_Buffer);					// Taking File Name
		send(Socket, Write_Buffer, 1024, 0); 		// Sending File Name
		
		read(Socket, Read_Buffer, 1024);
		sscanf(Read_Buffer, "%d", &File_Size);

		if (File_Size>0)
		{

			printf("[~] Downloading File...\n");
			FILE *File = fopen(Write_Buffer,"wb");
			while (File_Size>0)
			{
				int c = 1024;
				bzero(Read_Buffer,1024);
				read(Socket, Read_Buffer, 1024);
				if (File_Size<1024)
					c = File_Size;
				fwrite(Read_Buffer,sizeof(char),c,File);
				File_Size -= 1024;
			}
			printf("[+] Download Completed\n");
		}
		else
		{
			printf("[+] File does not exist!\n");
		}

		printf("[~] Closing Connection...\n");
		close(Socket);								// Closing Connection
	}

	else
	{
		printf("[+] Unable to Connect\n");
	}
}

// End Of Code