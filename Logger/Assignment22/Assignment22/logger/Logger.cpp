#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <queue>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "Logger.h"



using namespace std;


int fd;
struct sockaddr_in servaddr;
  const int BUF_LEN=4096;
  bool is_running;
  const char IP_ADDR[]="127.0.0.1";
  const int PORT=5000;
  pthread_mutex_t lock_x;
  //queue<string> message1;
  void *recv_func(void *arg);
  char logLevel;
int  InitializeLog()
{
  int ret, len;
    
    socklen_t addrlen = sizeof(servaddr);
    char buf[BUF_LEN];


    fd=socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if(fd<0) {
        cout<<"Cannot create the socket"<<endl;
	cout<<strerror(errno)<<endl;
	return -1;
    }

    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    ret = inet_pton(AF_INET, IP_ADDR, &servaddr.sin_addr);
    if(ret==0) {
        cout<<"No such address"<<endl;
	cout<<strerror(errno)<<endl;
        close(fd);
        return -1;
    }
    servaddr.sin_port = htons(PORT);
   
    pthread_mutex_init(&lock_x, NULL); 
    is_running = true;
    pthread_t tid;
    ret = pthread_create(&tid, NULL, recv_func, &fd);
    if(ret!=0) {
        cout<<"Cannot start thread"<<endl;
	cout<<strerror(errno)<<endl;
        close(fd);
        return -1;
        
    //is_running=false;
    //pthread_join(tid, NULL);
    //close(fd);
    }
 
}

void SetLogLevel(LOG_LEVEL level)
{
  logLevel = level;
}

void Log(LOG_LEVEL level, const char *file, const char *func, int line, const char *message)
{
int ret;
 char buf[BUF_LEN];
   if(level<logLevel) return;

    time_t now = time(0);
    char *dt = ctime(&now);
    //Remove the carriage return
    int len=strlen(dt);
    dt[len-1]='\0';
    
    memset(buf, 0, BUF_LEN);
    char levelStr[][16]={"DEBUG", "WARNING", "ERROR", "CRITICAL"};
    len = sprintf(buf, "%s %s %s:%s:%d %s\n", dt, levelStr[level], file, func, line, message)+1;
    buf[len-1]='\0';
    //cout<<buf<<endl;
    ret = sendto(fd,buf , len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    

 
}

void ExitLog()
{
cout<<"entering"<<endl;
  is_running=false;
  close(fd);
}
void *recv_func(void *arg)
{
    int fd = *(int *)arg;
   // struct sockaddr_in servaddr;
    socklen_t addrlen = sizeof(servaddr);
    char buf[BUF_LEN];

cout << "server: read()" << endl;
cout<<"isrunning"<<is_running<<;endl;
    while(is_running) {
        memset(buf,0,100);
        cout<< "recv_func:pthread_mutex_lock()"<<endl;
        pthread_mutex_lock(&lock_x);
        int len = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr *)&servaddr, &addrlen)-1;
        cout << len <<"+++++++++"<< fd << buf << endl;
        pthread_mutex_unlock(&lock_x);
        if(len<0) sleep(1);
        else {
            cout << "tester" << endl;
            char logLevelStr[]="Set Log Level=";
            len=strlen(logLevelStr);
            if(strncmp(logLevelStr, buf, len)==0) {
                logLevel=atoi(&buf[len]);
                cout << logLevel << endl;
            }else{
               cout << logLevelStr<< endl;
               cout << buf << endl;
            }
            cout << "checker" << endl;
    }
    cout<<"pthread ecit"<<endl;
    pthread_exit(NULL);

}
}



