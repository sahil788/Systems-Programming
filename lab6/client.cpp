#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

char socket_path[] = "/tmp/lab6";

int main(int argc, char *argv[]) 
{
    struct sockaddr_un addr;
    char buf[100], buf2[100];
    int fd,rc;
    int len, ret;
    bool isRunning = true;

    cout << "client(" <<getpid() << ")" << endl;

    memset(&addr, 0, sizeof(addr));

    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        cout << "client: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);


    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "client: " << strerror(errno) << endl;
        close(fd);
        exit(-1);
    }

 
    while (isRunning && (rc=read(fd,buf,sizeof(buf))) > 0) 
    {
     

       if(strncmp("quit", buf, 4) == 0) {
	    cout << "This client is quitting!" << endl;
            send(fd, "quit", 4, 0);
	    isRunning = false;
	    exit(-1);
       }
       else if(strncmp("sleep",buf, 4) == 0){
	    cout << "This client is going to sleep for 5 seconds" << endl;
            sleep(5); 
	    send(fd, "done", 4, 0);
         
       }
       else if(strncmp("pid",buf, 3) == 0){
            cout << "A request for the client's pid has been received"<< endl;
       	    send(fd, "pid", 3, 0);
               
       }else{
            send(fd, buf, 100, 0);
       }
    }

    close(fd);
    return 0;
}


