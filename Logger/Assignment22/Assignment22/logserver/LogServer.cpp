#include <arpa/inet.h>
#include <iostream>
#include <queue>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <syslog.h>
#include <vector>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
using namespace std;

//Global Variable
const int PORT=5000;
const char IP_ADDR[]="127.0.0.1";
const int BUF_LEN=4096;
bool is_running;
struct sockaddr_in remaddr;

void *recv_func(void *arg);

queue<string> message;
pthread_mutex_t lock_x;

int fd, ret, len, selection;
struct sockaddr_in myaddr;
socklen_t addrlen = sizeof(remaddr);
char buf[BUF_LEN];
int loglevelNum;
char loglevelName[BUF_LEN];

int main(void)
{
    //Set socket addr into zero position
    memset(&myaddr, 0, sizeof(myaddr));

    //Create a non-blocking socket for UDP communications using SOCK_NONBLOCK 
    fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if(fd < 0) {
        cout<<"Cannot create the socket"<<endl;
	cout<<strerror(errno)<<endl;
	return -1;
    }

    //Bind the socket to any valid IP address with a specifc port number
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;

    ret = inet_pton(AF_INET, IP_ADDR, &myaddr.sin_addr);
    if(ret==0) {
        cout<<"No such address"<<endl;
	cout<<strerror(errno)<<endl;
        close(fd);
        return -1;
    }

    myaddr.sin_port = htons(PORT);

    ret = bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if(ret<0) {
        cout<<"Cannot bind the socket to the local address"<<endl;
	cout<<strerror(errno)<<endl;
	return -1;
    } 

    //Create thread function
    pthread_mutex_init(&lock_x, NULL);
    is_running = true;
    pthread_t tid;

    ret = pthread_create(&tid, NULL, recv_func, &fd);
    if(ret!=0) {
        cout<<"Cannot start thread"<<endl;
        cout<<strerror(errno)<<endl;
        close(fd);
        return -1;
    }

    //Ask the user for set log level
    system("clear");
    do{
      cout << "Choose from the following: " << endl;
      cout << "1. Set the log level" << endl;
      cout << "2. Dump the log file here" << endl;
      cout << "0. Shutdown" << endl;
      cin >> selection;
           
      switch(selection){
        case 1:
           cout << "Log Level: (1)DEBUG, (2)WARNING, (3)ERROR, (4)CRITICAL" << endl;
           cout << "Enter the log level: ";
           cin >> loglevelNum;
 
           /*if(loglevelNum == 1){
              strcpy(loglevelName, "Set Log Level=DEBUG");
           }else if(loglevelNum == 2){
              strcpy(loglevelName, "Set Log Level=WARNING");
           }else if(loglevelNum == 3){
              strcpy(loglevelName, "Set Log Level=ERROR");
           }else if(loglevelNum == 4){
              strcpy(loglevelName, "Set Log Level=CRITICAL");
           }*/

           memset(buf, 0, BUF_LEN);
           len = sprintf(buf, "Set Log Level=%d", loglevelNum-1)+1;
           cout << buf << endl;
           sendto(fd, buf, len, 0, (struct sockaddr *)&remaddr, addrlen);
	   break;
        case 2:
	   FILE *fp;
           fp = fopen("system.log", "r");
	   if(fp == NULL){
	      cout<<"Cannot open syslog file"<<endl;
	      cout<<strerror(errno)<<endl;
	   }else{
              memset(buf, 0, BUF_LEN);

   	      while(!feof(fp)){
                 fgets(buf, BUF_LEN, fp);
	         puts(buf);	 
              }
	   fclose(fp);       
	   }
	   break;	   
        case 0:
           cout << "LogServer is shutting down" << endl;
           is_running = false;
           selection = 0;
           break;
      }
      
      if(selection != 0){
         char key;
         cout << "Press any key to continues: ";
         cin >> key;
         system("clear");
      }
   }while(selection != 0);

    pthread_join(tid, NULL);

    close(fd);

    return 0;
}

void *recv_func(void *arg)
{
    FILE *fp;
    //fp = fopen("/var/log/system.log", "w");
    fp = fopen("system.log", "w");
    //fp = fopen(serverlogfile, "w");
    //fp = fopen("abc.txt", "w");

    if(fp == NULL){
       cout<<"Cannot open syslog file"<<endl;
       cout<<strerror(errno)<<endl;
    }
    
    memset(buf, 0, BUF_LEN);

    /*if(fp != NULL){
       fprintf(fp, "++++++++++++++++++++++++++somehting wrong\n");
    }else{
        cout << "not working!" << endl;
    }*/

    while(is_running)
    { 
       pthread_mutex_lock(&lock_x);
       len = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr *)&remaddr, &addrlen)-1;
       pthread_mutex_unlock(&lock_x);
       
       if(len < 0){
          sleep(1);
       }else{
         fprintf(fp, "%s\n", buf);
       }

       /*if(len > 0){
         fprintf(fp, "%s\n", buf);  
       }else{
         sleep(1);
       }*/
    }
    
    fclose(fp);
   
    pthread_exit(NULL);
}
