#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h> 
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<signal.h>

struct client_mems_info{
	int connectfd;//存放客户端已连接socket
	struct sockaddr_in client_addr;//存放客户端ip地址
	bool used;
};
struct client_mems_info client_mems[10];

//初始化
int sock_init(){
	int sockfd;
	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(4000);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		perror("socket failed"),exit(0);
	
	if((bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))<0)
		perror("bind failed"),exit(0);
	
	if(listen(sockfd,10)<0)
		perror("listen failed"),exit(0);
	
	printf("server is running on port %d\n",ntohs(server_addr.sin_port));
	
	return sockfd;
}

//查找成员
int search_mems(struct client_mems_info* client_mems,struct sockaddr_in client){
	int n=0;
	for(int i=0;i<10;i++){
		if(client_mems[i].client_addr.sin_addr.s_addr==client.sin_addr.s_addr&&client_mems[i].client_addr.sin_port==client.sin_port){
			n=i;
			break;
		}
		else if(client_mems[i].used==0){
			n=i;
			break;
		}
	}
	return n;
}

//处理信息
void* deal_msg(void* arg){
	int n=*(int*)arg;
	while(1){
		sleep(1);
		char buf[100];
		memset(buf,0,sizeof(buf));
		if(recv(client_mems[n].connectfd,buf,100,0)<0)
			perror("recv failed"),exit(0);
		printf("server recv:%s\n",buf);
		for(int i=0;i<10;i++){
			if(client_mems[i].used){
				char all_msg[200];
				memset(all_msg,0,sizeof(all_msg));
				sprintf(all_msg,"%s:%d:%s",inet_ntoa(client_mems[n].client_addr.sin_addr),ntohs(client_mems[n].client_addr.sin_port),buf);
				if(i!=n) send(client_mems[i].connectfd,all_msg,strlen(all_msg),0);
				printf("server send:%s\n",all_msg);
			}
		}
	}
}

int main(){
	int sockfd=sock_init();
	
	while(1){
		int connectfd=0;
		struct sockaddr_in client;
		memset(&client,0,sizeof(client));
		socklen_t client_size=sizeof(client);
		if((connectfd=accept(sockfd,(struct sockaddr*)&client,&client_size))<0)//只对接入的每个socket响应一次，若放入死循环会卡住
			perror("accept failed"),exit(0);
	
		int n=search_mems(client_mems,client);//查询是否存在
		client_mems[n].client_addr=client;
		client_mems[n].connectfd=connectfd;
		client_mems[n].used=1;
		
		printf("%s:%d接入\n",inet_ntoa(client_mems[n].client_addr.sin_addr),ntohs(client_mems[n].client_addr.sin_port));
		pthread_t msg_id;
		pthread_create(&msg_id, 0, deal_msg, &n);
	}
	
	return 0;
}