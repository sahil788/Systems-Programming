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

key_t key3;
int msgid3;

bool is_running;
queue<Message> message3;

void *recv_func3(void *arg);

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
    pthread_t tid3;
  
    signal(SIGINT, shutdownHandler);
    // ftok to generate unique key 
    key3 = ftok("serverclient3", 65); 

    // msgget creates a message queue 
    // and returns identifier 
    msgid3 = msgget(key3, 0666 | IPC_CREAT); 

    pthread_mutex_init(&lock_x, NULL);
    is_running=true;
    ret = pthread_create(&tid3, NULL, recv_func3, NULL);
    if(ret!=0) {
        is_running = false;
        cout<<strerror(errno)<<endl;
        return -1;
    }

    int dest=0;
    while(is_running) {
        while(message3.size()>0) {
            pthread_mutex_lock(&lock_x);
            Message recvMsg=message3.front();
	    message3.pop();
            pthread_mutex_unlock(&lock_x);
	    cout<<"client 3: received from client "<<recvMsg.msgBuf.source<<endl;
	}
	if(dest==1) dest=2;
	else        dest=1;
	Message sendMsg;
        sendMsg.mtype=1;// the first message from the queue is removed
	sendMsg.msgBuf.source = 3;
	sendMsg.msgBuf.dest = dest;
#if 0
        int len = sprintf(sendMsg.buf, "%d: It's not pining, it's passed on! This parrot is no more! It has ceased to be!\n\
It's expired and gone to meet its maker! This is a late parrot! It's a stiff!\n\
Bereft of life, it rests in peace! If you hadn't nailed it to the perch, it\n\
would be pushing up the daisies!\n\
It's rung down the curtain and joined the choir invisible. This is an ex-parrot!\n", getpid());
#endif
        sprintf(sendMsg.msgBuf.buf, "%d: Message from client 3\n", getpid());
	cout<<"client 3: sending to client "<<sendMsg.msgBuf.dest<<endl;
	msgsnd(msgid3, &sendMsg, sizeof(sendMsg), 0);
        sleep(1);
    }
    cout<<"client3: quitting..."<<endl;
    pthread_join(tid3, NULL);
    msgctl(msgid3, IPC_RMID, NULL); 

    return 0; 
} 

void *recv_func3(void *arg)
{
    while(is_running) {
        // msgrcv to receive message 
        Message msg;
        msgrcv(msgid3, &msg, sizeof(msg), 2, 0); 
	if(strncmp(msg.msgBuf.buf, "Quit", 4)==0) is_running=false;
	else {
            pthread_mutex_lock(&lock_x);
            message3.push(msg);
            pthread_mutex_unlock(&lock_x);
        }
    }
    pthread_exit(NULL);
}