 /* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int sockid, portno;
struct sockaddr_in server_addr, client_addr;
socklen_t client_len;	

// Need to close socket when SIGINT is thrown
void signal_handler(int signum){
	if(signum==SIGINT){
		close(sockid);
		printf("\nClosing server socket\n");
		exit(0);
	}
}
// Initialise server socket
void init(char *port_num){
	sockid=socket(AF_INET, SOCK_STREAM, 0);
	if (sockid < 0) 
	perror("ERROR opening socket");
	bzero((char *)&server_addr, sizeof(server_addr));
	portno = atoi(port_num);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
}

// Bind server sockid to sockaddr
void bind_server(){
	if (bind(sockid, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) 
		perror("ERROR on binding");
}

int main(int argc, char *argv[]){
	if(signal(SIGINT, signal_handler)==SIG_ERR)
		printf("Error in catching SIG_INT\n");
	FILE *fp;
	int client_sockid;	
	
	// buffer stores name of file requested
	char buffer[256];
	// filebuffer stores content of file
	unsigned char filebuffer[4096];
	int n;
	
	if (argc < 2){
		fprintf(stderr,"ERROR: No Port Number\n");
		exit(1);
	}
	
	init(argv[1]);
	bind_server();
	
	while(1){
		//Listen on server socket
		listen(sockid,1);
		client_len=sizeof(client_addr);
		client_sockid=accept(sockid, (struct sockaddr *) &client_addr, &client_len);
		
		if(client_sockid<0) 
			perror("ERROR on accept");
		else{
			//Send prompt for file request
			n=write(client_sockid,"\nText file to be fetched:\n",26);
			if(n<0)
				perror("ERROR writing to socket");
		}

		//read filename from client
		bzero(buffer,256);
		n=read(client_sockid,buffer,255);
		
		//remove additional /n from the back
		buffer[strlen(buffer)-1]='\0';

		//Open file. Read in chunks of 4096 bytes
		fp=fopen(buffer,"r+");
		if(fp!=NULL){
			int read=0;
			while((read=fread(filebuffer,1,4095,fp))>0){
				//Write file chunk to client_socketid
				n=write(client_sockid,filebuffer,4095);
				if(n<0) 
					perror("ERROR writing to socket");
			}
			fclose(fp);
		}
		else{
			//Write appropriate error message
			n = write(client_sockid,"ERROR: File Not Found",21);
			if (n < 0) 
				perror("ERROR writing to socket");
		}
		close(client_sockid);
	}
	return 0; 
}
