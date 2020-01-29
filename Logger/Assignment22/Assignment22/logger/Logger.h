#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
using namespace std;
typedef enum {
                        DEBUG,
                        WARNING,
                        ERROR,
                        CRITICAL
                    } LOG_LEVEL;


  
         /* const char *prog;
          LOG_LEVEL level;
          const char *func;
          int line;
          const char *message;*/
      
    
      

int  InitializeLog();
void SetLogLevel(LOG_LEVEL level);
void Log(LOG_LEVEL level, const char *file, const char *func, int line, const char *message);
void ExitLog();


#endif