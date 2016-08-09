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

int sockid;

void signal_handler(int signum){
	if(signum==SIGINT){
		close(sockid);
		printf("\nClosing server socket\n");
		exit(0);
	}
}

int main(int argc, char *argv[]){
	if(signal(SIGINT, signal_handler)==SIG_ERR)
		printf("Error in catching SIG_INT\n");
	
	FILE *fp;
	int client_sockid, portno;
	socklen_t client_len;
	char buffer[256];
	unsigned char filebuffer[4096];
	struct sockaddr_in server_addr, client_addr;
	int n;
	if (argc < 2){
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockid=socket(AF_INET, SOCK_STREAM, 0);
	if (sockid < 0) 
	perror("ERROR opening socket");
	bzero((char *) &server_addr, sizeof(server_addr));
	portno = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);
	if (bind(sockid, (struct sockaddr *) &server_addr,sizeof(server_addr))<0) 
		perror("ERROR on binding");
	while(1){
		listen(sockid,5);
		client_len=sizeof(client_addr);
		client_sockid=accept(sockid, (struct sockaddr *) &client_addr, &client_len);
		if(client_sockid<0) 
			perror("ERROR on accept");
		else{
			n=write(client_sockid,"\nText file to be fetched:\n",26);
			if(n<0) 
				perror("ERROR writing to socket");
		}
		bzero(buffer,256);
		n=read(client_sockid,buffer,255);
		// printf("%s",buffer);
		buffer[strlen(buffer)-1]='\0';
		fp=fopen(buffer,"r+");
		
		if(fp!=NULL){
			int read=0;
			while((read=fread(filebuffer,1,4095,fp))>0){
				n=write(client_sockid,filebuffer,4095);
				if(n<0) 
					perror("ERROR writing to socket");
			}
			fclose(fp);
		}
		else{
			n = write(client_sockid,"ERROR: File Not Found",21);
			if (n < 0) 
				perror("ERROR writing to socket");
		}
		close(client_sockid);
	}	
	close(sockid);
	return 0; 
}
