#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#define BUFFER_SIZE 1024
using namespace std;
char buffer[BUFFER_SIZE];
int dostuff(const string & fname,int j)
{
  FILE *f=fopen(fname.c_str(),"rb");
  struct stat st;          
  stat(fname.c_str(), &st);   
  int result;
  while(!feof(f))
  {
    result=fread (buffer,1,BUFFER_SIZE,f);
    int sb = send(j,buffer,result,0);
    // cout<<"Aboout to go in sleep"<<endl;
    // sleep(5);
  }
  fclose(f);
  cout << "Closing the given file\n" << endl;
  return result;
}
int main()
{
  
  int peersock, srvsock, j, result, result1, len;
  sockaddr_in addr;

  if((srvsock = socket(PF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket error");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_storage serverStorage;
  socklen_t addr_size = sizeof(serverStorage);

  addr.sin_family = AF_INET;
  addr.sin_port = htons(12002);
  addr.sin_addr.s_addr = INADDR_ANY;
  fd_set readset, tempset;
  
  timeval tv;
  
  int maxfd, flags;
  

  memset(addr.sin_zero, '\0', sizeof addr.sin_zero);
  int iSetOption = 1;
  if(setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption)) == -1)
  {
    perror("set socket error");
    exit(EXIT_FAILURE);
  }
  if(bind(srvsock, (struct sockaddr *) &addr, sizeof(addr)) <0 )
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if(listen(srvsock,5)!=0)
  {
      printf("Error\n");
  }
  else
  {
      printf("== Listening\n");
  }

  FD_ZERO(&readset);
  FD_SET(srvsock, &readset);
  maxfd = srvsock;

  do 
  {
    memcpy(&tempset, &readset, sizeof(tempset));
    tv.tv_sec = 1000;
    tv.tv_usec = 0;
    result = select(maxfd + 1, &tempset, NULL, NULL, &tv);

    if (result < 0 && errno != EINTR) 
    {
      printf("Error in select(): %s\n", strerror(errno));
    }
    else if (result == 0) 
    {
       printf("select() timed out!\n");
    }
    
    else if (result > 0) 
    {
      if (FD_ISSET(srvsock, &tempset) != 0) 
      {
        len = sizeof(addr);
        peersock = accept(srvsock, (struct sockaddr *) &serverStorage, &addr_size);
        if (peersock < 0) 
        {
          printf("Error in accept(): %s\n", strerror(errno));
        }
        else
        {
          FD_SET(peersock, &readset);
          if(maxfd < peersock)
          {
            maxfd = peersock;
          }
          else
          {
            maxfd = maxfd;
          } 
        }
          FD_CLR(srvsock, &tempset);
      }
      
      int size; 
      for (j=0; j<maxfd+1; j++) 
      {
        if (FD_ISSET(j, &tempset)) 
        {
          result = recv(j, buffer, BUFFER_SIZE, 0);
          string fname(buffer,result);

          if(result > 0) 
          {
            int id=fork();
            if(id == 0)
            {
              buffer[result] = 0;
              printf("Opening: %s\n", buffer);
             
              result1=dostuff(fname,j);
              if (result1 < 0 && errno != EINTR)
                   break;
              
              cout << "File sent: "<< endl;
              close(j);
              FD_CLR(j, &readset);
              exit(0);
            }
              close(j);
              FD_CLR(j, &readset);
          }
          else 
          {
             printf("Error in recv(): %s\n", strerror(errno));
          }
        }      
      }      
    }     
  } while (1);
}