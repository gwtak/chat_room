#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>


//连接服务器
void sock_connect(int sockfd){
	struct sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr("192.168.111.150");//服务器地址
	server_addr.sin_port=htons(4000);
	
	if(connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
		perror("connect failed"),exit(0);
	
	printf("NOW CONNECTED!START CHAT\n");
}

//接受消息
void* pthread_recv(void* arg){
	int sockfd=*(int*)arg;
	char buf[200]; 
	while(1){
		int n=0;
		memset(buf,0,sizeof(buf));
		if((n=recv(sockfd,buf,200,0))<0)
			perror("recv failed"),exit(0);
		else
			printf("%s\n",buf);
	}
}

//发送消息
void* pthread_send(void* arg){
	int sockfd=*(int*)arg;
	char buf[100];
	while(1){
		memset(buf,0,sizeof(buf));
		fgets(buf,100,stdin);
		if(send(sockfd,buf,sizeof(buf),0)<0)
			perror("send failed"),exit(0);
	}
}

int main(){
	int sockfd;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		perror("socket failed"),exit(0);
	
	sock_connect(sockfd);
	
	pthread_t pid1,pid2;
	pthread_create(&pid2,0,pthread_recv,&sockfd);
	pthread_create(&pid1,0,pthread_send,&sockfd);
	
	while(1);
	
	return 0;
}