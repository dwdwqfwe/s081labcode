#include <asm-generic/errno.h>
// #include <errno.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include<stdio.h>
// #include<stdlib.h>
#include<arpa/inet.h>
#include <sys/poll.h>
#include <sys/select.h>
#include<unistd.h>
#include<sys/socket.h>
#include<ctype.h>
#include<poll.h>
#include<sys/select.h>
#include<sys/epoll.h>
int main()
{
    puts("ffff");
    struct sockaddr_in server_addr,clint_addr;
    char buf[1];
    socklen_t csize=sizeof(clint_addr);
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(9527);
    server_addr.sin_family=AF_INET;
    int ret=bind(lfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret<0){
        perror("bind error");
    }
    listen(lfd,100);
    int pot=1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,&pot,sizeof(pot));
    int efd=epoll_create(100);
    struct epoll_event eplTemp,eplAll[100];
    eplTemp.events=EPOLLIN;
    eplTemp.data.fd=lfd;
    ret=epoll_ctl(efd,EPOLL_CTL_ADD,lfd,&eplTemp);
    if(ret==-1){
        perror("ctl error");
    }
    while(1){
        int nready=epoll_wait(efd,eplAll,100,-1);
        for(int i=0;i<nready;i++){
            if(!(eplAll[i].events&EPOLLIN)){
                continue;
            }
            if(eplAll[i].data.fd==lfd){
                int cfd=accept(lfd,(struct sockaddr*)&clint_addr,&csize);
                if(cfd<0){
                    perror("accept error");
                }
                // if(--nready==0){
                //     continue;
                // }
                eplTemp.data.fd=cfd;
                eplTemp.events=EPOLLIN|EPOLLET;
                ret=epoll_ctl(efd,EPOLL_CTL_ADD,cfd,&eplTemp);
                if(ret==-1){
                    perror("ctl error");
                }
            }
            else{
                int n;
                while((n=read(eplAll[i].data.fd,buf,sizeof(buf)))>0){
                // if(){
                //     close(eplAll[i].data.fd);
                //     epoll_ctl(efd,EPOLL_CTL_DEL,eplAll[i].data.fd,NULL);
                // }
                // else if(n<0){
                //     if(errno==ECONNRESET){
                //         close(eplAll[i].data.fd);
                //         epoll_ctl(efd,EPOLL_CTL_DEL,eplAll[i].data.fd,NULL);
                //     }
                //     else{
                //         perror("read error");
                //     }
                // }
                // else{
                    puts("read");
                    for(int i=0;i<n;i++){
                        buf[i]=toupper(buf[i]);
                    }
                    write(STDOUT_FILENO,buf,n);
                // }
                }
            }
        }
    }
}