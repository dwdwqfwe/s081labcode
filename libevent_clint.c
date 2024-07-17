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
#define SERV_PORT 8000
void read_cb(struct bufferevent* bufferevent,void* arg){
    char buf[1024];
    bufferevent_read(bufferevent,buf,sizeof(buf));
    printf("clint recived message--%s",buf);
    // bufferevent_write(bufferevent,buf,strlen(buf)+1);
    sleep(1);
}
void write_cb(struct bufferevent* bufferevent,void* arg){

    printf("clint have sended message\n");
}
void event_cb(struct bufferevent* bufferevent,short what,void* arg){
    if(what&BEV_EVENT_CONNECTED){
        printf("服务器已连接----");
        return;
    }
    bufferevent_free(bufferevent);
}
void read_terminal(int fd,short what,void *arg){
    char buf[1024];
    int n=read(fd,buf,sizeof(buf));
    struct bufferevent* bufferevent=(struct bufferevent*)arg;
    puts("ssssss\n");
    bufferevent_write(bufferevent,buf,n+1);
}
int main(){
    struct event_base *base;
    base=event_base_new();
    struct sockaddr_in addr;
    addr.sin_port=htons(SERV_PORT);
    addr.sin_family=AF_INET;
    inet_pton(AF_INET,"192.168.200.129",&addr.sin_addr.s_addr);
    int fd=socket(AF_INET,SOCK_STREAM,0);

    struct bufferevent *bufferevent;
    bufferevent=bufferevent_socket_new(base,fd ,BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bufferevent,read_cb,write_cb,event_cb,NULL);
    bufferevent_socket_connect(bufferevent,(struct sockaddr*)&addr,sizeof(addr));
    struct event *event;
    event=event_new(base,STDIN_FILENO,EV_READ|EV_PERSIST,read_terminal,bufferevent);
    bufferevent_enable(bufferevent, EV_READ);
    event_add(event,NULL);
    event_base_dispatch(base);


    event_free(event);
    event_base_free(base);
    close(fd);
}