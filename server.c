#include <netinet/in.h>
#include <stdio.h>
#include <sys/select.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<ctype.h>
#include<wait.h>
#define PORT 9527
void catch_child(int signal)
{
    while(waitpid(0,NULL,WNOHANG)>0);
}
int main()
{
    struct sockaddr_in server_addr,clint_addr;
    int wfd=socket(AF_INET,SOCK_STREAM,0);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(PORT);
    server_addr.sin_family=AF_INET;
    int ret=bind(wfd, ( struct sockaddr *)&server_addr,sizeof(server_addr));
    if(ret){
        perror("bind error");
    }
    ret=listen(wfd,10);
    if(ret){
        perror("listen error");
    }
    int pid;
    char buf[BUFSIZ];
    socklen_t len;
    while(1){
        int cfd=accept(wfd, (struct sockaddr *)&clint_addr, &len);
        puts("one\n");
        pid=fork();
        if(pid==0){
            close(wfd);
            while(1){
                ret=read(cfd,buf,sizeof(buf));
                if(ret==0){
                    close(cfd);
                    exit(1);
                }
                for(int i=0;i<ret;i++){
                    buf[i]=toupper(buf[i]);
                }
                write(cfd,buf,ret);
                write(STDOUT_FILENO,buf,ret);
            }
        }
        else if(pid<0){
            perror("fork error");        
        }
        else
        {
            struct sigaction sig;
            sigemptyset(&sig.sa_mask);
            sig.sa_flags=0;
            sig.sa_handler=catch_child;
            sigaction(SIGCHLD,&sig,NULL);
            close(cfd);
        }
    }   



}