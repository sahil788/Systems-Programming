/*
This is the network Monitor that is the server which communicates with the clients and commands the interface monitor 
to monitor to monitor the ststictics

*/


#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <sys/ioctl.h>
#include <cstring>
#include <signal.h>

using namespace std;

//Global variable
char socket_path[] = "/tmp/assign1";
bool condition = false;
int fd,cl,rc;
bool isRunning = true;
bool isParent = true;

//If netMonitor hit Ctrl-C, it will shut down every monitor excluding network 
//and all interface monitor
void sighandler(int signum){

    send(cl, "Shut Down", 100, 0);
    isRunning = false;
    isParent = false;
    sleep(1);
    exit(-1);
}

int main(int argc, char *argv[]) 
{
    struct sockaddr_un addr;
    char buf[100], buf2[100];
    int len, ret;
    //bool isRunning = true;
    //bool isParent = true;
    vector<string> intfName;
    int intfNum;

    //This will cal function sighandler if the use ctrl-c
    signal(SIGINT, sighandler);

    //Ask user for number of interface monitor
    cout << "How many interfaces do you want to monitor: ";
    cin >> intfNum;

    //Ask user for interface name, and push them into vector variable
    for(int a = 0; a < intfNum; ++a){
      string intfTemp;
      cout << "Enter interface " << to_string(a+1) << ": ";
      cin >> intfTemp;
      intfName.push_back(intfTemp);
    }
  
   //Begin of the interface monitor
   for(int b = 0; b < intfNum; ++b){
      cout << "Starting the monitor for the interface " << intfName.at(b) << endl;
      string prgm = "./intfMonitor " + intfName.at(b) + "&";
      system(prgm.c_str());
   }

    //Set socket addr into zero position
    memset(&addr, 0, sizeof(addr));

    //Create the socket
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        cout << "server: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;

    //Set the socket path to a local socket file
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
    unlink(socket_path);

    //Bind the socket to this local socket file
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(fd);
        exit(-1);
    }

    //Listen for a client to connect to this local socket file
    if (listen(fd, 5) == -1) {
        cout << "server: " << strerror(errno) << endl;
        unlink(socket_path);
        close(fd);
        exit(-1);
    }

    //Write and Read to the local socket 
    while(isRunning && isParent) //&& (signal(SIGINT, sighandler) == SIG_ERR))
    {
        //Accept the client's connection to this local socket file        
        if((cl = accept(fd, NULL, NULL)) == -1) {
           cout << "server: " << strerror(errno) << endl;
           unlink(socket_path);
           close(fd);
           exit(-1);
        }

        //Create child using fork
        pid_t child =fork();

 
        if(child==0){ 
	    isParent=false;
        }else{         
	    isParent=true;
	}

        while(isRunning && !isParent)
        {    
           //Receive ready from interface monitor
           recv(cl, &buf, sizeof(buf), 0);
           
           if(strncmp("Ready", buf, 5) == 0){
             send(cl, "Monitor", 100, 0); 
           }else if(strncmp("done", buf, 4) == 0){
             sleep(1);
           }else if(strncmp("link down", buf, 9) == 0){
             send(cl, "set link up", 100, 0);
             sleep(1);
           }         
	}
    }     
           
    //close the socket
    unlink(socket_path);
    cout << "cccllooossseee at netMonitor.cpp" << endl;
    close(fd);
    close(cl);
    return 0;
}


