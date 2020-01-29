/*
This is the interface monitor which acts as a client and communicates with network monitor and recieves commands to perform tasks
it has all the logic to monitor statistics.
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
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

using namespace std;

char socket_path[] = "/tmp/assign1";

int monitorStatistics(string intf);

int main(int argc, char *argv[]) 
{
    struct sockaddr_un addr;
    char buf[100];
    int fd, rc;
    bool isRunning = true;
    string interface = argv[1];

    //Set socket addr into zero position
    memset(&addr, 0, sizeof(addr));

    //Create the socket
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        cout << "intfMonitor: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;
    //Set the socket path to a local socket file
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    //Connect to the local socket
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "intfMonitor: " << strerror(errno) << endl;
        close(fd);
        exit(-1);
    }

    //Read to the local socket 
    while (isRunning) 
    {
       //Send 'Ready to netMonitor
       send(fd, "Ready", 100, 0);

       //Receive 'Monitor' from netMonitor
       read(fd, buf, sizeof(buf));    

       if(strncmp("Monitor", buf, 7) == 0){
          
	  rc = monitorStatistics(interface);
          if(rc == 1){
            send(fd, "Done", 100, 0);
            sleep(1);
          }else{ 
            send(fd, "Link Down", 100, 0);
            sleep(1);
          }
       }else if(strncmp("Shut Down", buf, 9) == 0){
         cout << "Interface Monitor #" << getpid() << " will Shut Down!" << endl;
         isRunning = false;
       }else if(strncmp("set link up",buf,11) == 0){
            int sockfd,ret;
            struct ifreq ifr;

            sockfd = socket(AF_INET, SOCK_DGRAM, 0);

            if (sockfd < 0)
              return -1;

            memset(&ifr, 0, sizeof ifr);

            strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);

            ifr.ifr_flags |= IFF_UP;
            ret = ioctl(sockfd, SIOCSIFFLAGS, &ifr);

	    if(ret<0){
                cout << strerror(errno);
            } else if(ifr.ifr_hwaddr.sa_family!=ARPHRD_ETHER) {
                cout << "not an Ethernet interface" << endl;
            }
       }
       //Reset variable into zero position
       memset(&buf, 0, sizeof(buf)); 
    }

    close(fd);
    return 0;
}

int monitorStatistics(string intf)
{
    string operstate;
    int up_count, down_count;
    int rx_bytes, rx_dropped, rx_errors, rx_packets;
    int tx_bytes, tx_dropped, tx_errors, tx_packets;
    string filename;
    ifstream iffile;
    string intfDir="/sys/class/net/"+intf;

    //operstate
    filename = intfDir+"/operstate";
    iffile.open(filename);
    iffile>>operstate;
    iffile.close();

    //up_count
    filename = intfDir+"/carrier_up_count";
    iffile.open(filename);
    iffile>>up_count;
    iffile.close();

    //down_count
    filename = intfDir+"/carrier_down_count";
    iffile.open(filename);
    iffile>>down_count;
    iffile.close();

    //rx_bytes
    filename = intfDir+"/statistics/rx_bytes";
    iffile.open(filename);
    iffile>>rx_bytes;
    iffile.close();
    
    //rb_bytes
    filename = intfDir+"/statistics/rx_dropped";
    iffile.open(filename);
    iffile>>rx_dropped;
    iffile.close();
    
    //rx_errors
    filename = intfDir+"/statistics/rx_errors";
    iffile.open(filename);
    iffile>>rx_errors;
    iffile.close();
    
    //rx_packets
    filename = intfDir+"/statistics/rx_packets";
    iffile.open(filename);
    iffile>>rx_packets;
    iffile.close();
    
    //tx_bytes
    filename = intfDir+"/statistics/tx_bytes";
    iffile.open(filename);
    iffile>>tx_bytes;
    iffile.close();
    
    //tx_bytes
    filename = intfDir+"/statistics/tx_dropped";
    iffile.open(filename);
    iffile>>tx_dropped;
    iffile.close();
    
    //tx_errors
    filename = intfDir+"/statistics/tx_errors";
    iffile.open(filename);
    iffile>>tx_errors;
    iffile.close();
    
    //tx_packets
    filename = intfDir+"/statistics/tx_packets";
    iffile.open(filename);
    iffile>>tx_packets;
    iffile.close();
    
    //Display inferface directories and statistics        
    cout << "Interface: " << intf; 
    cout << " state: " << operstate;
    cout << " up_count: " << up_count;
    cout << " down_count: " << down_count << endl;

    cout << "rx_bytes: " << rx_bytes;
    cout << " rx_dropped: " << rx_dropped;
    cout << " rx_errors: " << rx_errors;
    cout << " rx_packets: " << rx_packets << endl;

    cout << "tx_bytes: " << rx_bytes;
    cout << " tx_dropped: " << rx_dropped;
    cout << " tx_errors: " << rx_errors;
    cout << " tx_packets: " << rx_packets << endl << endl;
        
    if(operstate == "up"){
         return 1;
    }
         
    return 0;    
}
