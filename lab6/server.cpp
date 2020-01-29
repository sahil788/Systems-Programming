#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;

char socket_path[] = "/tmp/lab6";

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    char buf[100], buf2[100];
    int fd,cl,rc;
    int len, ret;
    bool isRunning = true;

    cout << "server("<<getpid()<<")" << endl;
    cout << "Must enter command: pid, sleep or quit" << endl;

    memset(&addr, 0, sizeof(addr));
 
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        cout << "server: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
    unlink(socket_path);


    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(fd);
        exit(-1);
    }


    cout << "Waiting for the client..." << endl;
    if (listen(fd, 5) == -1) {
        cout << "server: " << strerror(errno) << endl;
        unlink(socket_path);
        close(fd);
        exit(-1);
    }


    cout << "client connected to the server" << endl;
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
        cout << "server: " << strerror(errno) << endl;
        unlink(socket_path);
        close(fd);
        exit(-1);
    }


    cout << "server: accept()" << endl;
    while(isRunning && (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) 
    {

 	
        if (write(cl, buf, rc) != rc) {
            if (rc > 0) {
                fprintf(stderr,"partial write: "); 
            }else {
                cout << "server: " << strerror(errno) << endl;
                close(fd);
                exit(-1);
            }
        }

        memset(&buf2, 0, sizeof(buf2));
        
        recv(cl, &buf2, sizeof(buf2), 0);

if(strncmp("quit", buf2, 4) == 0) {
	    cout << "The server requests the client to quit" << endl;
	    isRunning = false;
	    exit(-1);
	}else if(strncmp("done", buf2, 4) == 0){
            cout << "The server requests the client to sleep" << endl;
        }else if(strncmp("pid", buf2, 3) == 0){
            cout << "The server requests the client's pid" << endl;
	    cout << " This client has pid " << getpid()+1 << endl;
	}else{
           isRunning = false;
           exit(-1);
        }
    }

    unlink(socket_path);
    close(fd);
    close(cl);
    return 0;
}