#include <netinet/in.h>
#include <stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define PORT 9527
int main()
{
    struct sockaddr_in addr,clint_addr;
    socklen_t st;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(PORT);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    st=sizeof(clint_addr);
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    int ret=bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret==-1){
        perror("bind error");
    }
    ret=listen(sfd,10);
    if(ret==-1){
        perror("listen error");
    }
    int rfd=accept(sfd, (struct sockaddr*)&addr,&st );
    char b[1024];
    printf("sss\n");
    printf("clint ip=%s, pot=%d\n",inet_ntop(AF_INET,&addr.sin_addr.s_addr,b,sizeof(b)),ntohs(addr.sin_port));
    char buf[BUFSIZ];
    while(1){
        ret=read(rfd,buf,BUFSIZ);
        write(STDOUT_FILENO,buf,ret);
        for(int i=0;i<ret;i++){
            buf[i]=toupper(buf[i]);
        }
        write(rfd,buf,ret);
    }
    close(sfd);
    close(rfd);
}