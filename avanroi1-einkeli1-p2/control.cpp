#include "control.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  ifstream configfile (argv[1]);
  bool isMade = false;
  vector<string> configs;
  map<string,PORTS> mapPorts;
  string line;
  PORTS temp;
  if(configfile.is_open())
    {
      while(getline(configfile,line))
	{
	  configs = (split(',',line));
	  temp.controlPort = stoi(configs[2]);
	  temp.dataPort = stoi(configs[3]);
	  temp.hostName = configs[1];
	  mapPorts.insert(pair<string,PORTS>((configs[0]),temp));
	}

    }
  
  //setting up socket
  int controlSocket = socket(AF_INET, SOCK_DGRAM,0);
  struct sockaddr_in sa2;
  hostent * host = gethostbyname(argv[2]);
  sa2.sin_family = AF_INET;
  sa2.sin_port = htons(stoi(argv[3]));//binds to port specified in node struct
  memcpy(&sa2.sin_addr,host->h_addr,host->h_length);

  if(bind(controlSocket, (struct sockaddr*) &sa2, sizeof(sa2)) == -1){
    perror("Error binding  controlport");
    exit(-1);
  } 
  
  line = "";
  getline(cin,line);
  vector<string> input;
  string payload="";
  bool fail=false;
  while(line.compare("quit")!=0)
    {
      input = split(' ',line);
      payload="";
      if(input[0].compare("generate-packet")==0){
	payload+=("3|");
      }
      else if(input[0].compare("create-link")==0){
	payload+=("1|");
      }
      else if(input[0].compare("remove-link")==0){
	payload+=("2|");
      }
      else{
	cerr<<"Incorrect packet formation, please try again"<<"\n";
	fail=true;
      }
     
      if((fail==true) || sendPayload(payload,mapPorts,(input[1]),input[2],controlSocket,stoi(payload.substr(0,1)))==-1)
	{
	  cerr<<"Error sending Payload!"<<"\n";
	  fail=false;
	}
      getline(cin,line);     
    }
  return 1;
}

int sendPayload(string payload,map<string,PORTS> mapPorts,string dest,string dest2,int controlSocket, int id)
{
  struct sockaddr_in addr;//maybe this should be a pointer
  socklen_t szaddr = sizeof(addr); //IS THIS RIGHT?
  memset(&addr,0,szaddr);
  hostent * host;
  string pl1;
  string pl2;
  if(id==1||id==2)
    {
      pl1 = payload+dest;
      pl1+=",";
      pl1+=dest2;
      pl2 = payload+dest2;
      pl2+=",";
      pl2+=dest;
      cerr<<pl1<<"\n"<<pl2<<"\n";
      int cntrlPrt1;
      string hostnm1;
      hostnm1 = mapPorts[dest].hostName;
      cntrlPrt1 = htons(mapPorts[dest].controlPort);
      addr.sin_family = AF_INET;
      addr.sin_port=cntrlPrt1;
      cerr<<hostnm1;
      host = gethostbyname(hostnm1.c_str());
      memcpy(&addr.sin_addr,host->h_addr,host->h_length);
      if(sendto(controlSocket,pl1.c_str(), pl1.length(), 0, (struct sockaddr *)&addr, szaddr)<0)
	{
	  perror("somehow it didnt send");
	}
      memset(&addr,0,szaddr);
      int cntrlPrt2;
      string hostnm2;
      hostnm2 = mapPorts[dest2].hostName;
      cntrlPrt2 = htons(mapPorts[dest2].controlPort);
      addr.sin_family = AF_INET;
      addr.sin_port=cntrlPrt2;
      cerr<<hostnm2;
      host = gethostbyname(hostnm2.c_str());
      memcpy(&addr.sin_addr,host->h_addr,host->h_length);
      if(sendto(controlSocket,pl2.c_str(), pl2.length(), 0, (struct sockaddr *)&addr, szaddr)<0)
	{
	  perror("somehow it didnt send");
	}
    }
  else if(id == 3){
      pl1 = payload+dest;
      pl1+=",";
      pl1+=dest2;
      int cntrlPrt1;
      string hostnm1;
      hostnm1 = mapPorts[dest].hostName;
      cntrlPrt1 = htons(mapPorts[dest].controlPort);
      addr.sin_family = AF_INET;
      addr.sin_port=cntrlPrt1;
      cerr<<hostnm1;
      host = gethostbyname(hostnm1.c_str());
      memcpy(&addr.sin_addr,host->h_addr,host->h_length);
      if(sendto(controlSocket,pl1.c_str(), pl1.length(), 0, (struct sockaddr *)&addr, szaddr)<0)
	{
	  perror("somehow it didnt send");
	}
  }
  return 1;
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
