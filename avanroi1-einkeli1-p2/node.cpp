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
int MAXSIZE=500;
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
  if(configfile.is_open())
    {
      while(getline(configfile,line))
	{
	  configs = (split(',',line));
	  i=0;
	  temp.controlPort = stoi(configs[2]);
	  temp.dataPort = stoi(configs[3]);
	  temp.hostName = configs[1];
	  this->mapPorts.insert(pair<int,PORTS>(stoi(configs[0]),temp));
	  if(stoi(configs[0])==stoi(argv[1]))
	    {
	      this->id = stoi(configs[0]);
	      this->hostName=configs[1];
	      this->controlPort=stoi(configs[2]);
	      this->dataPort=stoi(configs[3]);
	      while(4+i<configs.size())
		{
		  //cout<<configs[4+i];
		  this->neighbors.push_back(stoi(configs[4+i]));
		  i++;
		}
	      DV myself;
	      myself.dest=this->id;
	      myself.cost=0;
	      myself.nextHop=-1;//always through controlport
	      this->DVT.push_back(myself);
	      isMade=true;
	    }
	}
      if(isMade == false)
	{
	  cerr<<"NODE ID "<<stoi(argv[1])<<" is not available";
	}
      i = 0;
      for(;i<this->neighbors.size();++i)
	{
	  DV neighbr;
	  neighbr.dest=this->neighbors[i];
	  neighbr.cost=1;
	  neighbr.nextHop=this->neighbors[i];
	  this->DVT.push_back(neighbr);
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

void waitforUpdates(Node* me)
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
  struct timeval tmv;
  
  int status2;
  sockaddr_in contRecv;
  socklen_t contLen = sizeof(contRecv);
  memset(&contRecv,0,contLen);
  char controlBuf[MAXSIZE];
  string payload;
  memset(&controlBuf,0,MAXSIZE);
  time_t start=time(0);
  time_t end;
  while(1)
    {
      tmv.tv_sec = 2;
      tmv.tv_usec = 1;
      status2 = select(controlSocket+1,&contSet,NULL,NULL,&tmv);
      
      if(FD_ISSET(controlSocket,&contSet))
	{
	  cerr<<"received data";
	  if(recvfrom(controlSocket, controlBuf, MAXSIZE/*prob needs to be changed*/,0, (sockaddr *)&contRecv, &contLen)<0)
	    {
	      perror("Something wrong happened while receiving packet");
	    }
	  payload=controlBuf;
	  parseControlPacket(me, payload);
	  //cerr<<payload;
	  //we have something to send via control!
	}
      end=time(0);
      if(difftime(end,start)*1000>2)
	{
	  cerr<<("nothing to see here");
	  sendControlPacket(me,controlSocket);
	  start=end;
	  //we have nothing to recv via control, so lets send our distance vector!!
	}

      //select(...)
      FD_ZERO(&contSet);
      FD_SET(controlSocket,&contSet);
    }
}

void waitforData(Node* me)
{
  fd_set dataSet;
  FD_ZERO(&dataSet);

  int dataSock = socket(AF_INET, SOCK_DGRAM,0);//possibly make dataSock a member of Node
  struct sockaddr_in sa;
  hostent * host = gethostbyname(me->hostName.c_str());
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
  while(1){
    FD_SET(dataSock,&dataSet);
    status = pselect(dataSock+1,&dataSet,NULL,NULL,&t,NULL);
    if(FD_ISSET(dataSock,&dataSet))
      {
	recvfrom(dataSock, dataBuf, 100/*prob needs to be changed*/,0, dataRecv, dataLen);
	//we have something to send via data!
      }
    else
      {
	printf("thisisdata");
	//we have nothing to send via data!
      }
    
  }
}

int sendtext(int sd, char *msg)
{
  return 1;
}

void parseControlPacket(Node *me, string info)
{
  const char type = info[0];
  string temp= info.substr(1,info.length());
  vector<string> toBeAdded;
  vector<string> toBeRemoved;
  vector<string> toSend;
  //cerr<<info;
  switch(type)
    {
    case '0': //DVTable received
      temp = temp.substr(1,temp.length());
      alterOrReadTable(1,temp,me);
      //alterOrReadTable()
      break;
    case '1':
      toBeAdded = split(',',temp);//2 arguments, source, us,and destination
      break;
    case '2':
      toBeRemoved = split(',',temp); //2 arguments, source, us,and destination
      //alterOrReadTable()
      break;
    case '3':
      toSend = split(',',temp);//2 arguments, source, us,and destination
      //sharedInt = stoi(configs[1]);//notifies data thread we want to send a packet to destation
      break;
    }
}
/**
Control Packet:
char id = 0 1 2 3 (0=DVT,1=removeLink,2=addLink,3=generatePacket)
*newline
...=data

 **/
void sendControlPacket(Node * me,int socket)//only packet we ever send is our DVT
{
  string ourDVT = alterOrReadTable(0,"",me);//this may be blocked waiting for lock
  struct sockaddr_in addr;//maybe this should be a pointer
  socklen_t szaddr = sizeof(addr); //IS THIS RIGHT?
  memset(&addr,0,szaddr);
  hostent * host;
  
  int cntrlPrt;
  string hostnm;
  /**sa2.sin_family = AF_INET;
  sa2.sin_port = htons(me.controlPort);//binds to port specified in node struct
  memcpy(&sa2.sin_addr,host->h_addr,host->h_length);*/
  for (int x : me->neighbors)
    {
      hostnm = me->mapPorts[x].hostName;
      cntrlPrt = htons(me->mapPorts[x].controlPort);
      addr.sin_family = AF_INET;
      addr.sin_port=cntrlPrt;
      host = gethostbyname(hostnm.c_str());
      memcpy(&addr.sin_addr,host->h_addr,host->h_length);
      if(sendto(socket,ourDVT.c_str(), ourDVT.length(), 0, (struct sockaddr *)&addr, szaddr)<0)
	{
	  perror("somehow it didnt send");
	}
    }
  
}
void handleDataPacket(Node * me, char * payload)
{

}
string alterOrReadTable(int code, string changer, Node * me)
{
  //mtx.lock();
  string resultString;
  vector<string> entries;
  vector<string> entry;
  int dest;
  int nextHop;
  int cost;
  int i;
  int sz=(me->DVT).size();
  bool found;
  int incoming;
  switch(code)
    {
    case 0: //string will look like 0|d,h,c|d2,h2,c2|...|dn,hn,cn
      i=0;
      sz=(me->DVT).size();
      resultString.append(to_string(0));
      for (DV x : me->DVT)
	{
	  if(i==0)
	    {
	      resultString.append("|");
	      resultString.append(to_string(x.dest));
	      resultString.append(",");
	      resultString.append(to_string(x.nextHop));
	      resultString.append(",");
	      resultString.append(to_string(x.cost));
	      resultString.append("|");
	    }
	  else if(i<sz-1)
	    {
	      resultString.append(to_string(x.dest));
	      resultString.append(",");
	      resultString.append(to_string(x.nextHop));
	      resultString.append(",");
	      resultString.append(to_string(x.cost));
	      resultString.append("|");
	    }
	  else
	    {
	      resultString.append(to_string(x.dest));
	      resultString.append(",");
	      resultString.append(to_string(x.nextHop));
	      resultString.append(",");
	      resultString.append(to_string(x.cost));
	    }
	  i+=1;
	}
      // iterate through entries, convert to string, append to return str
      break;
    case 1:
      cerr<<"\n"<<changer<<"\n";
      entries=split('|',changer);
      found=false;
      for (string x : entries)
	{
	  entry = split(',',x);
	  if(stoi(entry[1])==-1)
	    {
	      incoming=stoi(entry[0]);
	      break;
	    }
	}
      for (string x : entries)
	{
	  entry = split(',',x);
	  dest=stoi(entry[0]);
	  nextHop=stoi(entry[1]);
	  cost=stoi(entry[2]);
	  found=false;
	  i=0;
	  for (;i<sz;i++)
	    {
	      if((me->DVT[i]).dest==dest)//the incoming vector has a way to get to the same dest
		{
		  if(cost<((me->DVT[i]).cost-1))//cheaper cost
		    {
		      (me->DVT[i]).cost=cost+1;
		      (me->DVT[i]).nextHop=incoming;
		    }
		  else if(me->DVT[i].nextHop==incoming)//if table entry uses incoming node, and node has updated cost for the same destination as that entry
		    {
		      me->DVT[i].cost=cost+1;
		    }
		  found=true;
		}

	    }
	  if(found==false)//the current entry from the new table has dest unkown to old table
	    {
	      DV temp;
	      temp.dest = dest;
	      temp.nextHop = incoming;
	      temp.cost = cost+1;
	      me->DVT.push_back(temp);
	    }
	}
      //iterate through incoming changer string containing DVT, do DVT algo, update table by changing the cost / next hop. and so on wont return anythin
      break;
    case 2:
      //iterate through DVT, look for a match for the second argument in changer (split on delim ,) and remove from neighboor vector (if its there) wont return anything
      break;
    case 3:
      //same as 2, but we are adding, so add to neighboor vector and update DVT wont return anything
      break;
    }
  //mtx.unlock();
  return resultString;
}
/**
   code = 0 1 (0=read table (returns string representing table)1=updateTable DVT, 2 = remove particular connection, 3 = add particular connection)
 */

