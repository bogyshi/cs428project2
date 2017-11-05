#include "node.hpp"

using namespace std;

//TO MAKE UDP SOCKET socket(AF_INET, SOCK_DGRAM, 0))
//NEXT STEPS: create fd_set that hold onto only its own socket
//both data and control threads get a fd_set that just looks at itself and waits for someone to send it something.
//to send our DVR packet to our neighboors, we need a well defined packet structure
//packet tells us 1) what type of information (for control, we could see a DVR, addLink,removLink,generatePacket)
//2) our DVR.
//packet for Data tells us
//8-bit Source node id
//8-bit Destination node id
//8-bit Packet id
//8-bit TTL
//Data
//PRIORITY: get both threads to wait in a loop and look for data coming in from other nodes.
//SECOND PRIORITY: at some interval, we can use timers and take their difference, ex 3 seconds, send our DVR to our neighoboors
//Note: to send data to a udp socket, we only need to specify port and correct IP by using *(gethostbyname(me.hostName.c_str)->h_addr)
int main(int argc,char * argv[])
{
  Node* me = new Node(argv);
    
  thread control(waitforUpdates, me);//tells one thread to wait in the control state`
  thread data(waitforData, me);

  control.join();
  data.join();
  
  delete me; 
  return 1;
}

Node::Node(char * argv[])
{
  string line;
  vector<string> configs;
  ifstream configfile (argv[2]);
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
	  this.mapPorts.insert(pair<int,PORTS>(tempPort,temp));
	  if(stoi(configs[0])==stoi(argv[1]))
	    {
	      this.id = stoi(configs[0]);
	      this.hostName=configs[1];
	      this.controlPort=stoi(configs[2]);
	      this.dataPort=stoi(configs[3]);
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
	      this.DVT.push_back(myself);
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
	  neighbr.dest=this.neighboors[i];
	  neighbr.cost=1;
	  neighbr.nextHop=this.neighboors[i];
	  this.DVT.push_back(neighbr);
	}
    }
  else
    {
      cerr<<"FILE NOT AVAILABLE"<<argv[2];
    }
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

int waitforUpdates(Node* me)
{
  fd_set contSet;

  int controlSocket = socket(AF_INET, SOCK_DGRAM,0);
  struct sockaddr_in sa2;
  hostent * host = gethostbyname(me->hostName.c_str());
  sa2.sin_family = AF_INET;
  sa2.sin_port = htons(me->controlPort);//binds to port specified in node struct
  memcpy(&sa2.sin_addr,host->h_addr,host->h_length);

  if(bind(controlSocket, (struct sockaddr*) &sa2, sizeof(sa2)) == -1){
    perror("Error binding  controlport");
    exit(-1);
  } 

  FD_ZERO(&contSet);
  FD_SET(controlSocket,&contSet);
  struct timespec tmv;
  tmv.tv_sec = 1;
  int status2;
  sockaddr *contRecv;
  socklen_t *contLen;
  void * controlBuf;
  cout<<"before the flood";
  while(1)
    {
      cerr<<"howdy";
      status2 = pselect(controlSocket+1,&contSet,NULL,NULL,&tmv,NULL);
      
      if(FD_ISSET(controlSocket,&contSet))
	{
	  cerr<<"wtf";
	  recvfrom(controlSocket, controlBuf, 100/*prob needs to be changed*/,0, contRecv, contLen);
	  //we have something to send via control!
	}
      else
	{
	  cerr<<("nothing to see here");
	  //we have nothing to send via control, so lets send our distance vector!!
	}

      //select(...)
    }
  return 0;
}

void * waitforData(Node* me)
{
  fd_set dataSet;
  FD_ZERO(&dataSet);

  int dataSock = socket(AF_INET, SOCK_DGRAM,0);//possibly make dataSock a member of Node
  struct sockaddr_in sa;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(me->dataPort);//binds to port specified in node struct
  memcpy(&sa.sin_addr,host->h_addr,host->h_length);
  
  if(bind(dataSock, (struct sockaddr*) &sa, sizeof(sa)) == -1){
    perror("error binding dataport");
    exit(-1);
  }

  struct timespec t;
  t.tv_sec = 2;
  int status;
  sockaddr *dataRecv;
  socklen_t *dataLen;
  void * dataBuf;
  cerr<<"hmmmm";
  while(1){
    FD_SET(dataSock,&dataSet);
    status = pselect(dataSock+1,&dataSet,NULL,NULL,&t,NULL);
    cerr<<"howdy";
    if(FD_ISSET(dataSock,&dataSet))
      {
	recvfrom(dataSock, dataBuf, 100/*prob needs to be changed*/,0, dataRecv, dataLen);
	//we have something to send via data!
      }
    else
      {
	printf("meat");
	//we have nothing to send via data!
      }
    
  }
}

int sendtext(int sd, char *msg)
{

}
