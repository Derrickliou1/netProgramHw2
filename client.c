#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int sockfd=0;
void *tSend(void *err)
{
	char message[512]={};
	char user[80]={};
	char arg[5];
	while(1)
	{
		fgets(arg,5,stdin);
		arg[strlen(arg)-1]='\0';
		send(sockfd,arg,sizeof(arg),0);
		if(strcmp(arg,"c")==0)
		{
			fgets(user,80,stdin);
			user[strlen(user)-1]='\0';
			send(sockfd,user,sizeof(user),0);
			fgets(message,512,stdin);
			message[strlen(message)-1]='\0';
			send(sockfd,message,sizeof(message),0);
		}
		else if(strcmp(arg,"s")==0)
		{
			fgets(message,512,stdin);
			message[strlen(message)-1]='\0';
			send(sockfd,message,sizeof(message),0);
		}
		
	}
}

void *tRecive(void *arg)
{
	int err;
	char message[512];
	while(1)
	{
		err=recv(sockfd,message,sizeof(message),0);
		if(err==-1)
		{
			sleep(500);
			continue;
		}
		if(strlen(message)!=0)
			printf("%s\n",message);
		else
			sleep(100);
	}
}

int main(int argc , char *argv[])
{
	if( argc != 2)
	{
		printf("What's your name.\n");
		exit(EXIT_FAILURE);
	}
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		perror("Fail to create a socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in info;
	bzero(&info,sizeof(info));
	info.sin_family=PF_INET;

	info.sin_addr.s_addr=inet_addr("127.0.0.1");
	info.sin_port=htons(9487);

	int err=connect(sockfd,(struct sockaddr*)&info,sizeof(info));
	if(err==-1)
	{
		perror("Connect Error");
		exit(EXIT_FAILURE);
	}
	
	char receiveMessage[4096]={};
	send(sockfd,argv[1],sizeof(argv[1]),0);
	recv(sockfd,receiveMessage,sizeof(receiveMessage),0);
	printf("%s",receiveMessage);
	
	pthread_t tid1,tid2;
	pthread_create(&tid1,NULL,tSend,NULL);
	pthread_create(&tid2,NULL,tRecive,NULL);
	
	pthread_join(tid2,NULL);
	printf("Close socket\n");
	close(sockfd);
	return 0;
}

