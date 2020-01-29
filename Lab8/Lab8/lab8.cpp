#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h> 
#include <unistd.h> 

using namespace std;

int len = 32;

int main(int argc, char* argv[])
{
   char argument1[len];
   char argument2[len];
   char buf[len], *temp1, *temp2;
   char argum1[3][len], argum2[3][len];
   int ret, count = 0, len1 = 0, len2 = 0;
   int pipe[2];
   bool space = false;

   ret = pipe(pipe);

   if(ret == -1){
      perror("Error: pipe()");
      exit(1);
   }
     
   strcpy(argument1, argv[1]);
   strcpy(argument2, argv[2]);

   temp1 = strtok(argument1, " ");
   while(temp1 != NULL){
      strcpy(argum1[len1], temp1);
      temp1 = strtok(NULL, " ");
      ++len1;
   }

   temp2 = strtok(argument2, " ");
   while(temp2 != NULL){
      strcpy(argum2[len2], temp2);
      temp2 = strtok(NULL, " ");
      ++len2;
   }

   if(len1 > 3 || len2 > 3){
     len1 = len2 = 3;
   }

   cout << len1 << "  " << len2 << endl;
   for(int i = 0; i < len1; i++)
   {  
      write(pipe[1], argum1[i], len);
      cout << "Argument #1 (string #" << i+1 << "): " << argum1[i] << endl;
      count++;
   }
   cout << endl;

   for(int i = 0; i < len2; i++)
   {
      write(pipe[1], argum2[i], len);
      cout << "Argument #2 (string #" << i+1 << "): " << argum2[i] << endl;
      count++;
   }
   cout << endl;

   cout << "Argument 1 & 2: ";
   for(int i = 0; i < count; i++){
      read(pipe[0], buf, len);
      cout << buf << " ";
   }
   cout << endl;

   return 0;
}

// the way that dup() works is that is creates a copy of a file descriptor and uses the lowest numnered unused file descriptor for the new descriptor
// if it is created they both can be used interchangebly and they both refer to the same file descriptor and share the same file offset


// dup2() is similar to dup() but is uses the file descriptor specified by the user rather than taking the lowest numbered fd


//exclp() function is used to replace the currebt process image with a new process image specified by the file and is constricted from a regular filw and if it is succesfully created no return is made to the call