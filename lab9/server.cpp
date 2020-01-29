#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <queue>

using namespace std;

bool is_running;
bool condition = false;
const int BUF_LEN=4096;
const int MAX_CLIENTS=3;

void *recv_func(void *arg);
static void shutdownHandler(int sig);

queue<string> message;
pthread_mutex_t lock;

int master_fd,max_fd,cl[MAX_CLIENTS],rc;
fd_set activefds;
fd_set readfds;
int ret;

struct sockaddr_in addr, cli_addr[MAX_CLIENTS];
socklen_t clilen;
int len;
int numClients = 0;


int main(int argc, char *argv[])
{
    signal(SIGINT, shutdownHandler);
    char buf[BUF_LEN];

    if(argc<2) {
        cout<<"usage: server <port number>"<<endl;
	return -1;
    }

    memset(&addr, 0, sizeof(addr));
    if ( (master_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "server: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(master_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(master_fd);
        exit(-1);
    }

    cout<<"Waiting for the client..."<<endl;
    if (listen(master_fd, 5) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(master_fd);
        exit(-1);
    }

    pthread_mutex_init(&lock, NULL);
    pthread_t tid1, tid2, tid3;


    ret = pthread_create(&tid1, NULL, recv_func, &master_fd);
    if(ret!=0) {
        cout<<"Cannot start thread"<<endl;
        cout<<strerror(errno)<<endl;
        close(master_fd);
        return -1;
    }
    ret = pthread_create(&tid2, NULL, recv_func, &master_fd);
    if(ret!=0) {
        cout<<"Cannot start thread"<<endl;
        cout<<strerror(errno)<<endl;
        close(master_fd);
        return -1;
    }
    ret = pthread_create(&tid3, NULL, recv_func, &master_fd);
    if(ret!=0) {
        cout<<"Cannot start thread"<<endl;
        cout<<strerror(errno)<<endl;
        close(master_fd);
        return -1;
    }
   
    is_running = true;
    
    while(is_running)
    {
	int size = message.size();

	if(size){
	   pthread_mutex_lock(&lock);
           string msg=message.front();
           message.pop();
	   cout<<msg<<endl;
           pthread_mutex_unlock(&lock);
	}else{
           sleep(1);
	}

	if(condition == true){
          memset(buf,0,100);
          int len=sprintf(buf, "Quit")+1;
	  for(int i=0; i < numClients; i++){
	     write(cl[i], buf, len);
	  }
	}
    } 
   
    for(int i=0; i<numClients; ++i) {
        close(cl[i]);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    close(master_fd);
   
    return 0;
}

void *recv_func(void *arg)
{
    int fd = *(int *)arg;
    char buf[BUF_LEN];
    clilen = sizeof(cli_addr[0]);
    is_running = true;

    FD_ZERO(&readfds);
    FD_ZERO(&activefds);

    FD_SET(master_fd, &activefds);
    max_fd = master_fd;

    while(is_running)
    {
        readfds = activefds;
        struct timeval tv;
        
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
	    ret = select(max_fd+1, &readfds, NULL, NULL, &tv);
 
	if(FD_ISSET(master_fd, &readfds)){

        cl[numClients] = accept(master_fd, (struct sockaddr *) &cli_addr[numClients], &clilen);

	    FD_SET(cl[numClients], &activefds);

            if(max_fd<cl[numClients]) max_fd=cl[numClients];

                memset(buf, 0, BUF_LEN);

            if(ret==-1) {
                cout<<"server: Write Error"<<endl;
                cout<<strerror(errno)<<endl;
            }
            ++numClients;

    } else if(FD_ISSET(cl[0], &readfds)) {
            memset(buf, 0, BUF_LEN);
	    pthread_mutex_lock(&lock);
            ret=read(cl[0], buf, BUF_LEN);
	    pthread_mutex_unlock(&lock);
	} else if(FD_ISSET(cl[1], &readfds)) {
            memset(buf, 0, BUF_LEN);
	    pthread_mutex_lock(&lock);
            ret=read(cl[1], buf, BUF_LEN);
	    pthread_mutex_unlock(&lock);
        } else if(FD_ISSET(cl[2], &readfds)) {
            memset(buf, 0, BUF_LEN);
	    pthread_mutex_lock(&lock);
            ret=read(cl[2], buf, BUF_LEN);
	    pthread_mutex_unlock(&lock);
	}


    if(ret > 0){
        pthread_mutex_lock(&lock);
        message.push(buf);
        pthread_mutex_unlock(&lock);
	}else{
       is_running = false;
	   condition = true;
	}
    }

    pthread_exit(NULL);
}

static void shutdownHandler(int sig)
{
    switch(sig) {
        case SIGINT:
            is_running = false;
	    break;
    }
}

// One of the major differences is that in Synchronous Transmission, the sender and receiver should have synchronized clocks before data transmission. Whereas Asynchronous Transmission does not require a clock but it adds a parity bit to the data before transmission.
// In Synchronous data transfer, data is transmitted in the form of blocks or frames whereas in asynchronous data transfer, transmission is done in 1 byte or character at a time.


// I prefer socket reads in a receive function as it is easy to manage or change something in a function then whole code and it can be called whenever needed thus improving the efficiency of the code.
