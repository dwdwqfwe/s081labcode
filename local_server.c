#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<stddef.h>
int main(){
    char buf[BUFSIZ];
    struct sockaddr_un server_addr;
    strcpy(server_addr.sun_path,"socket.socke");
    server_addr.sun_family=AF_UNIX;
    int lfd=socket(AF_UNIX,SOCK_STREAM,0);
    int len=offsetof(struct sockaddr_un,sun_path)+strlen(server_addr.sun_path);
    unlink(server_addr.sun_path);
    int ret=bind(lfd,(struct sockaddr*)&server_addr,len);
    if(ret<0){
        perror("server bind error");
    }
    listen(lfd,20);
    int cfd=accept(lfd,(struct sockaddr*)&server_addr,(socklen_t*)&len);
    while((len=read(cfd,buf,sizeof(buf)))>0){
        for(int i=0;i<len;i++){
            buf[i]=toupper(buf[i]);
        }
        write(cfd,buf,len);
    }

}