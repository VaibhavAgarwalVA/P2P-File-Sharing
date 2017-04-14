#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dirent.h> 
#include <list>
#include <string>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#define BUFFER_SIZE 1024
using namespace std;

vector <string> dirfiles,fisfilelist;
#define pb push_back
string a;
int sock, n;
unsigned int length;
struct sockaddr_in server, from;
struct hostent *hp;
char buffer[1024];

void initdir()
{
	DIR *d;
  	struct dirent *ent;
  	
  	cout<<"Mention a directory path default is the home directory\n";
  	dirfiles.clear();
  	cin>>a;
  	const char * c = a.c_str();
  	d = opendir(c);
  	if(d == NULL) d = opendir(".");
	if (d != NULL) 
	{
  
	  	while ((ent = readdir (d)) != NULL)
	  	{
	  		if (ent->d_type == DT_REG) 
	  		{
	       		dirfiles.pb(ent->d_name);
	    		//printf ("%s\n", ent->d_name);
	  		}
	  	}
	  	closedir (d);
   	} 
   	else 
   	{
  		perror ("");
	}

}
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void connect_to_fis()
{
	  sock= socket(AF_INET, SOCK_DGRAM, 0);
  	if (sock < 0) error("socket");
  	server.sin_family = AF_INET;
  	server.sin_addr.s_addr = inet_addr("10.145.133.9");
  	server.sin_port = htons(12000);
  	length=sizeof(struct sockaddr_in);
}
string getfilename()
{
	string temp;
	int no=dirfiles.size();
	for(int i=0;i<no;i++)temp+=dirfiles[i]+":";
	return temp;
}
void gettokens(const string &str,vector<string> &token )
{
	stringstream ss(str);
	string item;
	token.clear();
	char delim=':';
    while (std::getline(ss, item, delim)) 
    {
         token.push_back(item);
    }
}
void printfiles(string a,vector<string> &v)
{
	cout<<a<<endl;
	int no=v.size();
	for (int i = 0; i < no; ++i)
		cout<<" "<<v[i]<<endl;

	cout<<endl;
}
void getfilelist()
{
	string query="UPD";
	connect_to_fis();
	strcpy(buffer,query.c_str());
  	n=sendto(sock,buffer,strlen(buffer),0,(const struct sockaddr *)&server,length);
  	if (n < 0)
  	{
  		perror("Sendto");
  	}
  	else
  	{
  		cout<<"Sucessfuly sent\n";
  	}
 	n = recvfrom(sock,buffer,BUFFER_SIZE,0,(struct sockaddr *)&from, &length);
  	if (n < 0)
  	{
  		error("recvfrom");
  	}
  	else
  	{
  		cout<<"Sucessfuly received\n";
  	}
  	string msg(buffer,n);
  	gettokens(msg,fisfilelist);
  	close(sock);
  	printfiles("The fis file list available for download is ",fisfilelist);
}
void initfiles()
{
	connect_to_fis();
	string f=getfilename();
	f = "ADD" + f;
	strcpy(buffer, f.c_str());
  	n=sendto(sock,buffer,strlen(buffer),0,(const struct sockaddr *)&server,length);
  	if (n < 0)
  	{ 
  		perror("Sendto");
  	}
  	else
  	{
  		cout<<"Sucessfuly sent\n";
  	}
  	n = recvfrom(sock,buffer,BUFFER_SIZE,0,(struct sockaddr *)&from, &length);
  	if (n < 0) 
  	{
  		perror("recvfrom");
 	}
 	else
  	{
  		cout<<"Sucessfuly received\n";
  	}
  	write(1,"Got an ack: ",12);
	write(1,buffer,n);
  	close(sock);
}
int getfileipaddr( string &name)
{

	string fname = "REQ"+name;
	connect_to_fis();
	strcpy(buffer,fname.c_str());
	n=sendto(sock,buffer,strlen(buffer),0,(const struct sockaddr *)&server,length);
	if (n < 0)
	{ 
  		perror("Sendto");
  	}
  	else
  	{
  		cout<<"Sucessfuly sent\n";
  	}
    n = recvfrom(sock,buffer,BUFFER_SIZE,0,(struct sockaddr *)&from, &length);	  
    if (n < 0) 
    { 
  		perror("Sendto");
  	}
  	else
  	{
  		cout<<"Sucessfuly sent\n";
  	}
  	write(1,"Got an ack: ",12);
	write(1,buffer,n);
	close(sock);
	if (buffer[0]=='-') return 0;
	return 1;
}
void send_fname(string fname,int id)
{
  char buffer[BUFFER_SIZE];
  strcpy(buffer,fname.c_str());
  send(id,buffer,strlen(buffer),0);
}

void downloadfile(string ip,string file,string saveas)
{
  socklen_t addr_size;
  sockaddr_in serverAddr;
  int filesocket;
  //file=file.substr(3);
  cout<<ip<<" "<<file<<" "<<saveas<<endl;
  //return ;
  if((filesocket = socket(PF_INET, SOCK_STREAM, 0)) == 0)
  {
  	perror("Socket error");
  	exit(EXIT_FAILURE);
  }
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(12002);
  serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
  // memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  if(connect(filesocket, (struct sockaddr *) &serverAddr, addr_size)<0)
  {
    error("Connect: ");
  }
  send_fname(file,filesocket);
  FILE *f=fopen(saveas.c_str(),"w");
  char buffer[BUFFER_SIZE];
  int t;

  while((t=recv(filesocket, buffer, BUFFER_SIZE, MSG_WAITALL))>40) {
 
    for(int i=0;i<t;i++){
      putc(buffer[i],f);
    }
    cout<<"Receiving"<<endl;

  }

  fclose(f);
  close(filesocket);
  cout << "File saved.\n";
}

int main()
{
	initdir();///initialise the files of the current directory
	initfiles();////send all the file names to the fis
	getfilelist();

	while(1)
	{
		cout<<"Here are the options that you have"<<endl;
		cout<<"1. Update the file list and view it "<<endl;
		cout<<"2. Download a file from FIS server"<<endl;
		cout<<"3. Exit"<<endl;

		int input;
		cin>>input;
		if(input == 1)
		{
			initdir();
			initfiles();
			getfilelist();
		}
		else if(input == 2)
		{
			cout<<"Enter the filename you want to download"<<endl;
			string name;
			cin>>name;
			if(getfileipaddr(name) != 0)
			{
				cout<<"Enter the name you want to save the file as"<<endl;
				string output;
				cin>>output;
				string ipaddr(buffer,n);
				cout<<ipaddr<<" "<<name<<" "<<output<<endl;
				downloadfile(ipaddr,name,output);
			}
			else 
			{
				cout<<"File not found"<<endl;
			}
		}
		else if (input == 3)
		{
			break;
		}
		else 
		{
			cout<<"Incorrect Option"<<endl;
			break;
		}
	}


	return 0;
}
