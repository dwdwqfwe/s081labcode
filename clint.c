#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<pthread.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<sys/socket.h>
int main()
{
    int fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    char* s="192.168.200.129";
    int p;
    int ret=inet_pton(AF_INET,s,&p);
    if(ret!=1){
        perror("inet error");
    }
    addr.sin_addr.s_addr=p;
    addr.sin_port=htons(9527);
    addr.sin_family=AF_INET;
    ret=connect(fd, (const struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1){
        perror("connect error");
    }
    int count=100;
    char buf[101];
    // for(int i=1;i<101;i++){
    //     buf[i]='0'+i%10;
    // }
    while(count--){
        char *s=fgets(buf,sizeof(buf),stdin);
        int n=strlen(s);
        puts("ccc");
        write(fd,&buf,n);
    }
    close(fd);

}