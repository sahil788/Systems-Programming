// client1.cpp - C Program for Message Queue (Read/Write) 
//
// 19-Mar-19  M. Watler         Created.
//
#include <errno.h> 
#include <iostream> 
#include <queue> 
#include <signal.h> 
#include <string.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h>
#include "client.h"

using namespace std;

key_t key1;
int msgid1;

bool is_running;
queue<Message> message1;

void *recv_func1(void *arg);

/* shared mutex between receive thread and send */
pthread_mutex_t lock_x;

static void shutdownHandler(int sig)
{
    switch(sig) {
        case SIGINT:
            is_running=false;
            break;
    }
}
  
int main() 
{ 
    int ret;
    pthread_t tid1;
  
    signal(SIGINT, shutdownHandler);
    // ftok to generate unique key 
    key1 = ftok("serverclient1", 65); 

    // msgget creates a message queue 
    // and returns identifier 
    msgid1 = msgget(key1, 0666 | IPC_CREAT); 

    pthread_mutex_init(&lock_x, NULL);
    is_running=true;
    ret = pthread_create(&tid1, NULL, recv_func1, NULL);
    if(ret!=0) {
        is_running = false;
        cout<<strerror(errno)<<endl;
        return -1;
    }

    int dest=0;
    while(is_running) {
        while(message1.size()>0) {
	    pthread_mutex_lock(&lock_x);
            Message recvMsg=message1.front();
	    message1.pop();
	    pthread_mutex_unlock(&lock_x);
	    cout<<"client 1: received from client "<<recvMsg.msgBuf.source<<endl;
	}
	if(dest==2) dest=3;
	else        dest=2;
	Message sendMsg;
        sendMsg.mtype=1;// the first message from the queue is removed
	sendMsg.msgBuf.source = 1;
	sendMsg.msgBuf.dest = dest;
        sprintf(sendMsg.msgBuf.buf, "%d: Message from client 1\n", getpid());
	cout<<"client 1: sending to client "<<sendMsg.msgBuf.dest<<endl;
	msgsnd(msgid1, &sendMsg, sizeof(sendMsg), 0);
        sleep(1);
    }
    cout<<"client1: quitting..."<<endl;
    pthread_join(tid1, NULL);
    msgctl(msgid1, IPC_RMID, NULL); 

    return 0; 
} 

void *recv_func1(void *arg)
{
    while(is_running) {
        // msgrcv to receive message 
        Message msg;
        msgrcv(msgid1, &msg, sizeof(msg), 2, 0); 
	if(strncmp(msg.msgBuf.buf, "Quit", 4)==0) is_running=false;
	else {
	    pthread_mutex_lock(&lock_x);
            message1.push(msg);
	    pthread_mutex_unlock(&lock_x);
	}
    }
    pthread_exit(NULL);
}