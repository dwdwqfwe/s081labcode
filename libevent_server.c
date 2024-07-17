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
void read_cb(struct bufferevent *buffer,void* arg){
    char buf[1024];
    bufferevent_read(buffer,buf,sizeof(buf));
    printf("server received data--%s\n",buf);
    char* s="i have recieved";
    bufferevent_write(buffer,s,strlen(s)+1);

}

void write_cb(struct bufferevent *buffer,void* arg){
    printf("server have send message\n");
}
void event_cb(struct bufferevent* buffer,short what,void* arg){
    if(what&BEV_EVENT_EOF){
       printf("connect close.....");
    }
    if(what&BEV_EVENT_ERROR){
        perror("event error");
    }
     bufferevent_free(buffer);
     printf("资源已经释放");

}
void ev_cb(struct evconnlistener* evlisten,evutil_socket_t fd,struct sockaddr *addr,int len,void* ptr){
    struct event_base *base=(struct event_base*)ptr;
    struct bufferevent* bufferevent=bufferevent_socket_new(base,fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bufferevent,read_cb,write_cb,event_cb,NULL);

    bufferevent_enable(bufferevent,EV_READ);
}

int main(){
    struct event_base *base=event_base_new();
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(SERV_PORT);
    addr.sin_family=AF_INET;
    struct evconnlistener *evconnlistener=evconnlistener_new_bind(base, ev_cb,base,LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
    10,(struct sockaddr*)&addr,sizeof(addr));

    event_base_dispatch(base);
    evconnlistener_free(evconnlistener);
    event_base_free(base);
    return 0;
}