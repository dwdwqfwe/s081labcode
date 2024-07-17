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
    struct sockaddr_un clint_addr,server_addr;
    strcpy(clint_addr.sun_path,"clint.socke");
    clint_addr.sun_family=AF_UNIX;
    int len=offsetof(struct sockaddr_un, sun_path)+strlen(clint_addr.sun_path);
    int cfd=socket(AF_UNIX,SOCK_STREAM,0);
    unlink(clint_addr.sun_path);
    int ret=bind(cfd,(struct sockaddr*)&clint_addr,len);
    if(ret<0){
        perror("clint bind error");
    }

    server_addr.sun_family=AF_UNIX;
    strcpy(server_addr.sun_path,"socket.socke");
    len=offsetof(struct sockaddr_un,sun_path)+strlen(server_addr.sun_path);
    ret=connect(cfd, (const struct sockaddr *)&server_addr, len);
    if(ret<0){
        perror("clint connect error");
    }
    char buf[BUFSIZ];
    while(strcmp((fgets(buf,BUFSIZ,stdin)),"aaaa\n")!=0){
    write(cfd,buf,26);
    int n=read(cfd,buf,sizeof(buf));
    write(STDIN_FILENO,buf,n);
    }
    sleep(10);
}