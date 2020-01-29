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

key_t key2;
int msgid2;

bool is_running;
queue<Message> message2;

void *recv_func2(void *arg);

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
    pthread_t tid2;
  
    signal(SIGINT, shutdownHandler);
    // ftok to generate unique key 
    key2 = ftok("serverclient2", 65); 

    // msgget creates a message queue 
    // and returns identifier 
    msgid2 = msgget(key2, 0666 | IPC_CREAT); 

    pthread_mutex_init(&lock_x, NULL);
    is_running=true;
    ret = pthread_create(&tid2, NULL, recv_func2, NULL);
    if(ret!=0) {
        is_running = false;
        cout<<strerror(errno)<<endl;
        return -1;
    }

    int dest=0;
    while(is_running) {
        while(message2.size()>0) {
            pthread_mutex_lock(&lock_x);
            Message recvMsg=message2.front();
	    message2.pop();
            pthread_mutex_unlock(&lock_x);
	    cout<<"client 2: received from client "<<recvMsg.msgBuf.source<<endl;
	}
	if(dest==1) dest=3;
	else        dest=1;
	Message sendMsg;
        sendMsg.mtype=1;// the first message from the queue is removed
	sendMsg.msgBuf.source = 2;
	sendMsg.msgBuf.dest = dest;
#if 0
        int len = sprintf(sendMsg.buf, "%d: It's not pining, it's passed on! This parrot is no more! It has ceased to be!\n\
It's expired and gone to meet its maker! This is a late parrot! It's a stiff!\n\
Bereft of life, it rests in peace! If you hadn't nailed it to the perch, it\n\
would be pushing up the daisies!\n\
It's rung down the curtain and joined the choir invisible. This is an ex-parrot!\n", getpid());
#endif
        sprintf(sendMsg.msgBuf.buf, "%d: Message from client 2\n", getpid());
	cout<<"client 2: sending to client "<<sendMsg.msgBuf.dest<<endl;
	msgsnd(msgid2, &sendMsg, sizeof(sendMsg), 0);
        sleep(1);
    }
    cout<<"client2: quitting..."<<endl;
    pthread_join(tid2, NULL);
    msgctl(msgid2, IPC_RMID, NULL); 

    return 0; 
} 

void *recv_func2(void *arg)
{
    while(is_running) {
        // msgrcv to receive message 
        Message msg;
        msgrcv(msgid2, &msg, sizeof(msg), 2, 0); 
	if(strncmp(msg.msgBuf.buf, "Quit", 4)==0) is_running=false;
	else {
            pthread_mutex_lock(&lock_x);
            message2.push(msg);
            pthread_mutex_unlock(&lock_x);
        }
    }
    pthread_exit(NULL);
}