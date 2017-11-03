#include "node.hpp"

using namespace std;

//TO MAKE UDP SOCKET socket(AF_INET, SOCK_DGRAM, 0))

int main(int argc,char * argv[])
{
  Node me = init(argv);
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
