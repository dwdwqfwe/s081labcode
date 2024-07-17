#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
int main(){
    struct sockaddr_in clint_addr,server_addr;
    char buf[BUFSIZ];
    inet_pton(AF_INET,"192.168.200.129",&clint_addr
    .sin_addr.s_addr);
    clint_addr.sin_family=AF_INET;
    clint_addr.sin_port=htons(6000);
    // printf("%d/n",ntohs(clint_addr.sin_port));
    inet_pton(AF_INET,"192.168.200.129",&server_addr
    .sin_addr.s_addr);
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8000);
    int cfd=socket(AF_INET, SOCK_DGRAM,0);
    int n=bind(cfd,(struct sockaddr*)&clint_addr,sizeof(clint_addr));
    if(n<0){
        perror("clint bind error");
    }
    while(fgets(buf,BUFSIZ,stdin)!=NULL){
        n=sendto(cfd,buf,strlen(buf),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
        if(n<0){
            perror("clint send error");
        }
        puts("ssssss");
        socklen_t len;
        n=recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&server_addr,&len);
        write(STDOUT_FILENO,buf,n);
    }

}