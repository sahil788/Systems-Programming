#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
using namespace std;
int main (void)
{
system("clear");
	char  *dev = "/dev/cdrom";
	int num;
    int fd = open(dev, O_RDONLY | O_NONBLOCK);
    perror("open");
    

   
    if(fd <0){
        
        cout<<"Failed to open "<<dev<<endl;
        exit(1);
    }
do{
	
	//system("clear");
	cout<<"Choose one from the following options"<<endl;
	cout<<"1. Eject the CD Rom"<<endl;
	cout<<"2. Close the CD Rom Tray"<<endl;
	cout<<"3. Get the tray position"<<endl;
	cout<<"0. Exit\n";

	cin>>num;

switch(num)
	{	
	case 1:{
		if (ioctl(fd,CDROMEJECT,0)<0){
		perror("ioctl");
	}
		
		}
	break;

	case 2:{
		if (ioctl(fd,CDROMCLOSETRAY,0)<0){
		perror("ioctl");
	}
		
		}
 
	
	break;

	case 3:{
	int result;
		result=	ioctl(fd,CDROM_DRIVE_STATUS,0);
		switch(result){
			case 0:
				cout<< "No information"<<endl;
				
				break;
			case 1:
				cout<< "No disk"<<endl;

				break;
			case 2:
				cout<<"Tray open"<<endl;


				break;
			case 3:
				cout<<"Drive not ready"<<endl;

				break;
			case 4:
				cout<<"Disk ok"<<endl;
		
				break;
			case -1:
				cout<<"Error"<<endl;
		
				break;
		
		}

		}
	break;

	case 0:
	cout<< " Goodbye:\n";
 close(fd);
	break;

	default:

	cout<< "not a valid choice"<<endl;
	break;
	}
 cout<<"wait for 2 seconds!!"<<endl;
sleep(2);
	system("clear");
}while(num !=0);


return 0;

}

		


