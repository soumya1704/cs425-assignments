#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int sockid;

void signal_handler(int signum){
    if(signum==SIGINT){
        close(sockid);
        printf("\nClosing client socket\n");
        exit(0);
    }
}


int main(int argc, char *argv[])
{
    if(signal(SIGINT, signal_handler)==SIG_ERR)
        printf("Error in catching SIG_INT\n");
    int sockid, portno, n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];
    unsigned char filebuffer[4096];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    while(1){
        sockid=socket(AF_INET, SOCK_STREAM, 0);
        if(sockid<0)
            perror("ERROR opening socket");
        server = gethostbyname(argv[1]);
        if (server==NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
        server_addr.sin_port = htons(portno);
        if (connect(sockid,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) 
            perror("ERROR connecting"); 
        else{
            bzero(buffer,256);
            if((n=read(sockid,buffer,255))<0) 
                perror("ERROR reading from socket");
            printf("%s",buffer);
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            n=write(sockid,buffer,strlen(buffer));
            if(n<0) 
                perror("ERROR writing to socket");
        }
        bzero(filebuffer,4096);
        n=read(sockid,filebuffer,4095);
        if(n<0) 
            perror("ERROR reading from socket");
        while(n>=0){
            if(filebuffer[0]=='\0')
                break;
            printf("%s",filebuffer);
            bzero(filebuffer,4096);
            n=read(sockid,filebuffer,4095);
        }
        close(sockid);
    }
    return 0;
}
