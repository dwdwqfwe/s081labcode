#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define SERV_PORT 8000
int main(){
    char buf[BUFSIZ];
    struct sockaddr_in server_addr,clint_addr;
    socklen_t len=sizeof(clint_addr);
    char str[BUFSIZ];
    int lfd=socket(AF_INET,SOCK_DGRAM,0);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(SERV_PORT);
    int ret=bind(lfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret<0){
        perror("bind error");
    }
    while(1){
        int n=recvfrom(lfd, buf,BUFSIZ,0,(struct sockaddr*)&clint_addr,&len);
        if(n==-1){
            perror("serve recv error");
        }
        printf("server recived ip %s,port=%d\n",inet_ntop(AF_INET,&clint_addr.sin_addr.s_addr,str,sizeof(str)),ntohs(clint_addr.sin_port));
        for(int i=0;i<n;i++){
            buf[i]=toupper(buf[i]);
        }
        n=sendto(lfd,buf,n,0,(struct sockaddr*)&clint_addr,sizeof(clint_addr));
        if(n==-1){
            perror("server send error");
        }
        
    }
}