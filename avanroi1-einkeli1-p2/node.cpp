#include "node.hpp"

using namespace std;

//TO MAKE UDP SOCKET socket(AF_INET, SOCK_DGRAM, 0))

int main(int argc,char * argv[])
{
  Node me = init(argv);
 
  int controlSock = socket(AF_INET, SOCK_DGRAM,0);//possibly make controlSock a member of Node
  struct sockaddr_in sa2;
  sa2.sin_family = AF_INET;
  sa2.sin_port = htons(me.controlPort);//binds to port specified in node struct
  sa2.sin_addr.s_addr = htonl(INADDR_ANY);//allows communicaiton from any IP
  if(bind(dataSock, (struct sockaddr*) &sa2, sizeof(sa2)) == -1){
	//abort, implement later
  } 
  //need a fork right here for control thread

  int dataSock = socket(AF_INET, SOCK_DGRAM,0);//possibly make dataSock a member of Node
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(me.dataPort);//binds to port specified in node struct
  sa.sin_addr.s_addr = htonl(INADDR_ANY);//allows communication from any IP
  if(bind(dataSock, (struct sockaddr*) &sa, sizeof(sa)) == -1){
	//abort, implement later
  }
 
  while(1){
	sockaddr *contRecv, *dataRecv;
	socklen_t *contLen, *dataLen;
	void *contBuf, *dataBuf;
	recvfrom(controlSock, contBuf, 100/*prob needs to be changed*/,0, contRecv, contLen);
	recvfrom(dataSock, dataBuf, 100/*prob needs to be changed*/,0, dataRecv, dataLen);
	//need mechanism for retrieving command line args
	
  }

  return 1;
}

Node init(char * argv[])
{
  string line;
  vector<string> configs;
  ifstream configfile (argv[2]);
  Node itsAme;
  bool isMade = false;
  int i = 0;
  PORTS temp;
  int tempPort;
  if(configfile.is_open())
    {
      while(getline(configfile,line))
	{
	  configs = (split(',',line));
	  i=0;
	  tempPort = stoi(configs[0]);
	  temp.controlPort = stoi(configs[2]);
	  temp.dataPort = stoi(configs[3]);
	  itsAme.mapPorts.insert(pair<int,PORTS>(tempPort,temp));
	  if(stoi(configs[0])==stoi(argv[1]))
	    {
	      itsAme.id = stoi(configs[0]);
	      itsAme.hostName=configs[1];
	      itsAme.controlPort=stoi(configs[2]);
	      itsAme.dataPort=stoi(configs[3]);
	      while(4+i<configs.size())
		{
		  //cout<<configs[4+i];
		  itsAme.neighboors.push_back(stoi(configs[4+i]));
		  i++;
		}
	      DV myself;
	      myself.dest=itsAme.id;
	      myself.cost=0;
	      myself.nextHop=-1;//always through controlport
	      itsAme.DVT.push_back(myself);
	      isMade=true;
	    }
	}
      if(isMade == false)
	{
	  cerr<<"NODE ID "<<stoi(argv[1])<<" is not available";
	}
      i = 0;
      for(;i<itsAme.neighboors.size();++i)
	{
	  DV neighbr;
	  neighbr.dest=itsAme.neighboors[i];
	  neighbr.cost=1;
	  neighbr.nextHop=itsAme.neighboors[i];
	  itsAme.DVT.push_back(neighbr);
	}
    }
  else
    {
      cerr<<"FILE NOT AVAILABLE"<<argv[2];
    }
  return itsAme;
}

vector<string> split(char delim,string s)
{
  int index = 0;
  int startIndex = 0;
  vector<string> hold;
  while (index < s.length())
    {
      if(s[index]==delim)
	{
	  hold.push_back(s.substr(startIndex,index-startIndex));
	  startIndex=index;
	  startIndex++;
	}
      index++;
    }
  hold.push_back(s.substr(startIndex,index));
  return hold;
}

int sendtext(int sd, char *msg)
{

}
