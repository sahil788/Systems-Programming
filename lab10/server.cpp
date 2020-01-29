

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

queue<Message> message1, message2, message3;
pthread_mutex_t lock_x1, lock_x2, lock_x3;
bool is_running;

key_t key1, key2, key3;
const int MAX_CLIENTS=3;
int msgid[MAX_CLIENTS];

void *recv_func1(void *arg) {
    while(is_running){
        // msgrcv to receive message 
        Message msg;
        msgrcv(msgid[0], &msg, sizeof(msg), 1, 0); 
	    pthread_mutex_lock(&lock_x1);
        message1.push(msg);
	    pthread_mutex_unlock(&lock_x1);
	}
    pthread_exit(NULL);
}
void *recv_func2(void *arg) {
    while(is_running){
        // msgrcv to receive message 
        Message msg;
        msgrcv(msgid[1], &msg, sizeof(msg), 1, 0); 
	    pthread_mutex_lock(&lock_x2);
        message2.push(msg);
	    pthread_mutex_unlock(&lock_x2);
	}
    pthread_exit(NULL);
}
void *recv_func3(void *arg) {
    while(is_running){
        // msgrcv to receive message 
        Message msg;
        msgrcv(msgid[2], &msg, sizeof(msg), 1, 0); 
	    pthread_mutex_lock(&lock_x3);
        message3.push(msg);
	    pthread_mutex_unlock(&lock_x3);
	}
    pthread_exit(NULL);
}


static void shutdownHandler(int sig) {
    switch(sig) {
        case SIGINT:
            is_running=false;
            break;
    }
}

int main() {
    int ret;
    pthread_t tid[MAX_CLIENTS];
    
    signal(SIGINT, shutdownHandler);
    
    key1=ftok("serverclient1", 65);
    key2=ftok("serverclient2", 65);
    key3=ftok("serverclient3", 65);
    
    msgid[0] = msgget(key1, 0666 | IPC_CREAT); 
    if(msgid[0]<0) {
        cout<<"server: "<<strerror(errno)<<endl;
	return -1;
    }
    msgid[1] = msgget(key2, 0666 | IPC_CREAT); 
    if(msgid[1]<0) {
        cout<<"server: "<<strerror(errno)<<endl;
	    return -1;
    }
    msgid[2] = msgget(key3, 0666 | IPC_CREAT); 
    if(msgid[2]<0) {
        cout<<"server: "<<strerror(errno)<<endl;
	    return -1;
    }
    
    pthread_mutex_init(&lock_x1, NULL);
    pthread_mutex_init(&lock_x2, NULL);
    pthread_mutex_init(&lock_x3, NULL);
    is_running = true;
    
    ret = pthread_create(&tid[0], NULL, recv_func1, NULL);
    if(ret!=0) {
        cout<<strerror(errno)<<endl;
        is_running = false;
        msgctl(msgid[0], IPC_RMID, NULL); 
        return -1;
    }
    
    ret = pthread_create(&tid[1], NULL, recv_func2, NULL);
    if(ret!=0) {
        cout<<strerror(errno)<<endl;
        is_running = false;
        msgctl(msgid[1], IPC_RMID, NULL); 
        return -1;
    }
    
    ret = pthread_create(&tid[2], NULL, recv_func3, NULL);
    if(ret!=0) {
        cout<<strerror(errno)<<endl;
        is_running = false;
        msgctl(msgid[2], IPC_RMID, NULL); 
        return -1;
    }
    
    while(is_running) {
        while(message1.size() > 0){
            pthread_mutex_lock(&lock_x1);
            Message recvMsg=message1.front();
            
            cout << recvMsg.msgBuf.buf << endl;
	        message1.pop();
	        pthread_mutex_unlock(&lock_x1);
	        recvMsg.mtype=2;

	        msgsnd(msgid[0], &recvMsg, sizeof(recvMsg), 0);
        }
        
        while(message2.size() > 0) {
	        pthread_mutex_lock(&lock_x2);
            Message recvMsg=message2.front();
            
            cout << recvMsg.msgBuf.buf << endl;
	        message2.pop();
	        pthread_mutex_unlock(&lock_x2);
	        recvMsg.mtype=2;
	        
	        msgsnd(msgid[1], &recvMsg, sizeof(recvMsg), 0);
	    }
	    
	    while(message3.size() > 0) {
	        pthread_mutex_lock(&lock_x3);
            Message recvMsg=message3.front();
            cout << recvMsg.msgBuf.buf << endl;
	        message3.pop();
	        pthread_mutex_unlock(&lock_x3);
	        recvMsg.mtype=2;
	        
	        msgsnd(msgid[2], &recvMsg, sizeof(recvMsg), 0);
	    }
	    
	    sleep(1);
    }
    
    cout<<"server: quitting..."<<endl;
    for(int i=0; i<MAX_CLIENTS; ++i) {
        Message sendMsg;
        sendMsg.mtype=2;
        sprintf(sendMsg.msgBuf.buf, "%s", "Quit");
	    msgsnd(msgid[i], &sendMsg, sizeof(sendMsg), 0);
    }

    for(int j=0; j<MAX_CLIENTS; ++j) {
        pthread_join(tid[j], NULL);
    }
    
    for(auto k = 0; k < MAX_CLIENTS; ++k){
        msgctl(msgid[k], IPC_RMID, NULL); 
    }
}

// 1. Between sockets, pipes, fifos, and messages, which is your favorite and why?
// My favourite is messages, because it requires less effort to create and is equally efficient
// 2. Which is your least favorite and why?
// My least favourite is sockets, because it requires too much repeating codeto  create and use.

