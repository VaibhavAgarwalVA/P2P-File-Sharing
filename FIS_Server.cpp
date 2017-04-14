#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "bits/stdc++.h"
#define NEW 1
#define REQ 2
#define UPD 3
#define BUFFER_SIZE 1024

#define PORT 12000

using namespace std;

std::map<string, string> filemap;

void showFileMap() 
{
    cout << "The Mapping between files and IP addresses:\n";
    cout << "Files\tIP addresses\n";
    std::map<string,string>::iterator it;
    for (it=filemap.begin(); it!=filemap.end(); ++it)
        cout << it->first << "\t" << it->second << '\n';
}

void gettokens(const string &str,vector<string> &token )
{
   stringstream ss;
   ss.str(str);
   string item;
   token.clear();
   char delim=':';
    while (std::getline(ss, item, delim)) {
         token.push_back(item);
    }
}

int main () 
{
    showFileMap();

    int sock, length, n;
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024] = {0};


    if((sock=socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
      perror("Socket failed ");
      exit(EXIT_FAILURE);
    }

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(PORT);


    if (bind(sock,(struct sockaddr *)&server, sizeof(server))<0)
    {
      perror("bind failed");
        exit(EXIT_FAILURE);
    } 

    fromlen = sizeof(struct sockaddr_in);

    string type;
    while (1) 
    {
        n = recvfrom(sock, buf, 1024, 0,(struct sockaddr *)&from , &fromlen);
        if(n < -1)
        {
         perror("receive failed");
        }
        char* Datagram_Address = inet_ntoa(from.sin_addr);
        int Datagram_Port = (int) ntohs(from.sin_port);
        printf("Datagram's IP address is: %s\n", Datagram_Address);
        printf("Datagram's port is: %d\n", Datagram_Port);

        string temp_buf(buf, n);
        type = temp_buf.substr(0,3);
        temp_buf = temp_buf.substr(3);
        if (strcmp(type.c_str(),"REQ")==0) 
        {
            cout << "Received request for " << temp_buf << endl;
          if ( filemap.find(temp_buf) == filemap.end() )
          {
              cout << "File "<< temp_buf <<" not found." << endl;
              sprintf (buf, "-");
          }
          else 
          {
              strcpy(buf, filemap[temp_buf].c_str());
          }
        }

        else if (strcmp(type.c_str(),"ADD")==0) 
        {
            // A list of files
            printf("This is file list\n");
            vector<string> tokens;
            gettokens(temp_buf, tokens);
            vector<string>::iterator it;
            for (it = tokens.begin() ; it != tokens.end(); ++it)
            {
                filemap[*it] = inet_ntoa(from.sin_addr);
            }
            strcpy(buf,"Files added. Kaam kr le\n");
            printf("Done file ADD");
            showFileMap();
        }
        else if (strcmp(type.c_str(),"UPD")==0) 
        {
            // A list of files
            printf("File list requested.\n");

            string out;
          map<string,string>::iterator it;
            cout<<"Concatenating files\n";
          for (it = filemap.begin(); it!=filemap.end(); ++it)
              out = out + it->first + ":";
            strcpy(buf,out.c_str());
            cout<<"updating done\n";
        }
        n = sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&from,fromlen);
        if (n  < 0)
        {
         perror("Error in sending");
      }
    }
}
