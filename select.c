#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include <sys/poll.h>
#include <sys/select.h>
#include<unistd.h>
#include<sys/socket.h>
#include<ctype.h>
#include<poll.h>
#include<sys/select.h>

#define port 9527
int main()
{
    struct sockaddr_in clint_addr,sever_addr;
    socklen_t len=sizeof(clint_addr);
    char buf[BUFSIZ];
    struct pollfd pfds[INET_ADDRSTRLEN];
    for(int i=0;i<INET6_ADDRSTRLEN;i++){
        pfds[i].fd=-1;
    }
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd<0)
        perror("sock error");
    sever_addr.sin_port=htons(port);
    sever_addr.sin_family=AF_INET;
    sever_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    int ret=bind(lfd,(struct sockaddr*)&sever_addr,sizeof(sever_addr));
    if(ret)
        perror("bind error");
    listen(lfd,10);
    // fd_set allset,rset;
    // FD_ZERO(&allset);
    // FD_SET(lfd,&allset);
    pfds[0].events=POLLIN;
    pfds[0].revents=0;
    pfds[0].fd=lfd;
    int maxfd=lfd;
    int maxi=0;
    int cfd;
    while(1){
        // rset=allset;
        int nready=poll(pfds,maxi+1,-1);
        if(pfds[0].revents&POLLIN){
            cfd=accept(lfd,(struct sockaddr*)&clint_addr,&len);
            // FD_SET(cfd,&allset);
            int i;
            for( i=1;i<INET_ADDRSTRLEN;i++){
                if(pfds[i].fd<0){
                    pfds[i].fd=cfd;
                    pfds[i].events=POLLIN;
                    break;
                }
            }
            if(i==INET_ADDRSTRLEN){
                perror("addr too much");
            }
            if(i>maxi){
                maxi=i;
            }
            if(cfd>maxfd)
                maxfd=cfd;
            if(--nready==0)
                continue;
        }
        for(int i=1;i<=maxi;i++){
            if(pfds[i].fd>=0){
                int n=0;
                if((n=read(pfds[i].fd,&buf,sizeof(buf)))==0){
                    close(i);
                    pfds[i].fd=-1;
                }
                else if(n<0){
                    if(errno==ECONNRESET){
                        close(pfds[i].fd);
                        pfds[i].fd=-1;
                    }
                    else{
                        perror("read error");
                    }
                }
                else{
                    for(int j=0;j<n;j++){
                        buf[j]=toupper(buf[j]);
                    }
                        puts("server");
                        write(STDOUT_FILENO,&buf,n);
                }
                if(--nready==0){
                    break;
                }
            }
        }
    }
}