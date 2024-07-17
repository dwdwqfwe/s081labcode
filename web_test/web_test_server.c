#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <event2/event.h>
#include <event2/util.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<event2/bufferevent.h>
#include<event2/listener.h>
#include<stdlib.h>
#include<sys/epoll.h>
#define MAX_SIZE 1024
char *file_type(char* name){
    char* temp=strchr(name,'.');
    if(temp==NULL){
        return "text/plain; charset=utf-8";
    }
    if(strcmp(temp,".html")==0||strcmp(temp,".htm")==0)
        return "text/html; charset=utf-8";
    if(strcmp(temp,".jpg")==0||strcmp(temp,".jpeg")==0)
        return "image/jpeg";
    if(strcmp(temp,".gif")==0)
        return "image/gif";
    if(strcmp(temp,".png")==0)
        return "image/png";
    if(strcmp(temp,".css")==0)
        return "image/css";
    if(strcmp(temp,".au")==0)
        return "audio/basic";
    if(strcmp(temp,".wav")==0)
        return "audio/wav";
    if(strcmp(temp,".avi")==0)
        return "video/x-msvideo";
    if(strcmp(temp,".mv")==0||strcmp(temp,".qt")==0)
        return "video/quicktime";
    if(strcmp(temp,".mpe")==0||strcmp(temp,".mpeg")==0)
        return "video/mpeg";
    if(strcmp(temp,".vrml")==0||strcmp(temp,".vrl")==0)
        return "model/vrml";
    if(strcmp(temp,".mid")==0||strcmp(temp,".midi")==0)
        return "audio/midi";
    if(strcmp(temp,".mp3")==0)
        return "audio/mpeg";
    if(strcmp(temp,".ogg")==0)
        return "application/ogg";
    if(strcmp(temp,".pac")==0)
        return "application/x-ns-proxy-autoconfig";
    return "text/plain; charset=utf-8";
}
void http_respond(int cfd,int num,char *descripe,char *type,int len){
    char buf[MAX_SIZE];
    sprintf(buf,"HTTP/1.1 %d %s\r\n",num,descripe);
    sprintf(buf+strlen(buf),"%s\r\n",type);
    sprintf(buf+strlen(buf), "Conten-Length: %d\r\n",len);
    send(cfd,buf,strlen(buf),0);
    send(cfd,"\r\n",2,0);
}
void http_file(int cfd,char *file){
    int fd=open(file,O_RDONLY);
    if(fd<0){
        perror("open http_file error");
    }
    int n;
    char buf[MAX_SIZE];
    while((n=read(fd,buf,MAX_SIZE))>0){
        int ret=send(cfd,buf,n,0);
        if(ret<0){
            if(errno==EAGAIN||errno==EINTR){
                continue;
            }
            perror("file send error");
        }
    }
    close(fd);
}
void http_request(int cfd,char* file)
{
    struct stat statbuf;
    int ret=stat(file,&statbuf);
    if(ret!=0){
        perror("http_requst error");
    }
    if(S_ISREG(statbuf.st_mode)){
        // printf("---it is a regular file\n");
        char *type;
        sscanf(type,"Content-Type: %s",file_type(file));
        http_respond(cfd,200,"OK",type,statbuf.st_size);
        http_file(cfd,file);
    }

}
void disconnect(int cfd,int evd)
{
    int ret=epoll_ctl(evd,EPOLL_CTL_DEL,cfd,NULL);
    if(ret<0){
        perror("disconnect error");
    }
    close(cfd);
}
int get_line(int cfd,char* buf,int size)
{
    int i=0;
    char c='\0';
    int n;
    while((i<size-1)&&(c!='\n')){
        int n=recv(cfd,&c,1,0);
        if(n>0){
            if(c=='\r'){
                n=recv(cfd,&c,1,MSG_PEEK);
                if(n>0&&c=='\n'){
                    recv(cfd,&c,1,0);
                }
                else{
                    c='\r';
                }
            }
            buf[i++]=c;
        }
        else{
            c='\n';
        }
    }
    buf[i]='\0';
    if(n==-1){
        i=n;
    }
    return i;
}
void do_read(int fd,int evd)
{
    char line[MAX_SIZE];
    int len=get_line(fd,line,MAX_SIZE);
    if(len==-1){
        perror("get_line error");
    }
    else if(len==0){
        disconnect(fd,evd);
    }
    else{
        char method[16],path[256],protocal[16];
        sscanf(line,"%[^ ] %[^ ] %[^ ]",method,path,protocal);
        printf("method=%s, path=%s, protocal=%s",method,path,protocal);
        while(1){
            char buf[MAX_SIZE];
            len=get_line(fd,buf,MAX_SIZE);
            if(len==0){
                break;
            }else if(len==-1){
                perror("do_read error");
            }
            printf("----%s\n",buf);
        }
            char *file=path+1;
            http_request(fd,file);
    }
}
void do_accept(int lfd,int evd)
{
    struct sockaddr_in clint_addr;
    int len=sizeof(clint_addr);
    int fd=accept(lfd,(struct sockaddr*)&clint_addr,(socklen_t*)&len);
    char clint_ip[64];
    printf("clint_ip=%s port=%d cfd=%d\n",inet_ntop(AF_INET,&clint_addr.sin_addr.s_addr,clint_ip,
    sizeof(clint_ip)),ntohs(clint_addr.sin_port),fd);

    int flags=fcntl(fd,F_GETFL);
    flags|=O_NONBLOCK;
    fcntl(fd,F_SETFL,flags);

    struct epoll_event ev;
    ev.events=EPOLLIN|EPOLLET;
    ev.data.fd=fd;

    int ret=epoll_ctl(evd,EPOLL_CTL_ADD,fd,&ev);
    if(ret<0){
        perror("ad_accept error");
    }

}
int listen_fd_init(int port,int evd)
{
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=htons(INADDR_ANY);
     int opt=1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEPORT,&opt,opt);
    int ret=bind(lfd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret<0){
        perror("server bind error");
    }
    listen(lfd,MAX_SIZE);
    struct epoll_event temp_event;
    temp_event.events=EPOLLIN;
    temp_event.data.fd=lfd;
    ret=epoll_ctl(evd,EPOLL_CTL_ADD,lfd,&temp_event);
    if(ret<0){
        perror("epoll_ctl error");
    }
    return lfd;
}
void epoll_init(int port)
{
    struct epoll_event all_events[MAX_SIZE];
    int evd=epoll_create(MAX_SIZE);
    int lfd=listen_fd_init(port,evd);
    while(1){
        int nready=epoll_wait(evd,all_events,MAX_SIZE,-1);
        for(int i=0;i<nready;i++){
            if(!(all_events[i].events&EPOLLIN)){
                continue;
            }
            if(all_events[i].data.fd==lfd){
                do_accept(lfd,evd);
            }
            else{
                do_read(all_events[i].data.fd,evd);
            }
        }

    }
}
int main(int argc,char* argv[])
{
    if(argc<3){
        perror("arg number error");
    }
    int port=atoi(argv[1]);
    int ret=chdir(argv[2]);
    if(ret==-1){
        perror("chdir error");
    }
    // int evd=epoll_create(1024);
    epoll_init(port);

}