#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<event2/event.h>
void read_cb(int fd,short what,void* arg)
{
    char buf[1024];
    read(fd,buf,sizeof(buf));
    printf("read data %s\n",buf);
    sleep(1);
}
int main(){
    unlink("mkfifo");
    int ret=mkfifo("mkfifo",0664);
    if(ret<0){
        perror("mkfifo error");
    }
    int fd=open("mkfifo",O_RDONLY|O_NONBLOCK);
    if(fd<0){
        perror("fd error");
    }
    struct event_base *base=event_base_new();
    struct event *event=event_new(base,fd,EV_READ|EV_PERSIST,read_cb,NULL);
    event_add(event,NULL);
    event_base_dispatch(base);
    event_free(event);
    event_base_free(base);
    close(fd);
    return 0;
}