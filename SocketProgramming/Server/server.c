// Anuneet Anand
// 2018022
// CN Assignment - 1
// Server

#include<stdio.h> 
#include<sys/socket.h> 
#include<unistd.h> 
#include<netinet/in.h> 
#include<stdlib.h> 
#include<string.h>
#define PORT 9001

int main()
{
	int opt = 1;
	char Read_Buffer[1024];
	char Write_Buffer[1024];

	struct sockaddr_in server;						//Server Details
	server.sin_family = AF_INET; 
	server.sin_port = htons(PORT); 
	int Server_Size = sizeof(server);

	printf("[~] Starting Server...\n");
	int Socket_ID = socket(AF_INET, SOCK_STREAM, 0);	// Starting Server
	setsockopt(Socket_ID, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt));
	bind(Socket_ID, (struct sockaddr *)&server, Server_Size );
	
	while(1)
	{
		listen(Socket_ID,0);
		printf("[+] Server Ready!\n");
		
		printf("[~] Waiting for Client...\n");
		int client = accept(Socket_ID, (struct sockaddr *)&server, (socklen_t*)&Server_Size );
		printf("[+] Client Connected!\n");
		read(client, Read_Buffer, 1024);
		printf("[~] Resolving File Request\n");

		FILE *File = fopen(Read_Buffer,"rb");

		if (File != NULL)						// Searching File
		{
			int n;
			fseek(File, 0L, SEEK_END);
			int File_Size = ftell(File);		//Calculating File Size
			fseek(File, 0L, SEEK_SET);
			sprintf(Write_Buffer, "%d", File_Size);
			send(client,Write_Buffer ,1024, 0);

			printf("[~] Sending File...\n");
			bzero(Write_Buffer,1024);
			while ((n = fread(Write_Buffer, sizeof(char), 1024, File)) > 0)
			{
				send(client,Write_Buffer,1024,0);		// Sending File 
				bzero(Write_Buffer,1024);
			}
			printf("[+] File Sent\n"); 
		}
		else
		{
			send(client,"0", 1, 0); 
		}
	}
	printf("[~] Shutting Down...\n");
 	close(Socket_ID);								// Closing Connection
}

// End Of Code