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
void write_cb(int fd,short what,void* arg)
{
    char s[1024];
    static int count=0;
    sprintf(s,"hello-world %d",count++);
    write(fd,s,strlen(s)+1);
    sleep(1);
}
int main(){
    int fd=open("mkfifo",O_WRONLY|O_NONBLOCK);
    if(fd<0){
        perror("fd error");
    }
    struct event_base *base=event_base_new();
    struct event *event=event_new(base,fd,EV_PERSIST|EV_WRITE,write_cb,NULL);
    event_add(event,NULL);
    event_base_dispatch(base);
    event_free(event);
    event_base_free(base);
    close(fd);
    return 0;

}