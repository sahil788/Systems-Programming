
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>

using namespace std;

char procDir[] = "/proc";

int main(void)
{
    vector<string> directories;
    DIR *dp;
    struct dirent *dirp;
    int numProcesses;
    int begin, end;
    bool isParent;
    dp=opendir(procDir);
    if(dp==NULL) {
        cout << "Could not open the directory " << procDir << endl;
        return 1;
    }

    while(dirp=readdir(dp)) {
        if(isdigit(dirp->d_name[0])) {
            directories.push_back(dirp->d_name);
        }
    }

    closedir(dp);
    numProcesses = directories.size();

	
	 isParent=true;
    for(int i=0; i<4 && isParent; i++)
	{
 begin = numProcesses*i/4;
    end = numProcesses*(i+1)/4-1;
	pid_t childId = fork();
    	    
    
        if(childId==0) {

            isParent = false;
            cout<<"child #" <<i+1<<" pid:"<<getpid()<<endl;
            
 for(int z=begin; z<=end; ++z) 
	{
	string filename = "/proc/" + directories.at(z) + "/status";
	string name;
	string pid;
	string vmrss;
	ifstream infile(filename.c_str());
	if(infile.is_open()) {
            string data;
            while(!infile.eof()) {
                infile >> data;
                if(data=="Name:") {
                    infile >> name;
                } else if(data=="Pid:") {
                    infile >> pid;
                } else if(data=="VmRSS:") {
                    infile >> vmrss;
                    int VmRss = atoi(vmrss.c_str());
                    if(VmRss>=10000) {
                        cout << "Name: " << name << " Pid: " << pid << " VmRss: " << vmrss << endl;
                    }
                }
            }
        }
        
	infile.close();
    }
    

    return 0;
             
	}
 }
	
	if(isParent)
	{

	  return 0;
	}
 }
  //What order do these children execute in and why?
  they execute in the following order that is 4,3,2,1
  because the order in which exit handler executes is in reverse order of registration.
	
    
   