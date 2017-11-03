#include "node.hpp"

using namespace std;

int main(int argc, char * argv[])
{
  string line;
  vector<string> configs;
  ifstream configfile (argv[2]);
  Node us;
  bool isMade = false;
  int i = 0;
  if(configfile.is_open())
    {
      while(getline(configfile,line))
	{
	  configs = (split(',',line));
	  i=0;
	  if(stoi(configs[0])==stoi(argv[1]))
	    {
	      us.id = stoi(configs[0]);
	      us.hostName=configs[1];
	      us.controlPort=stoi(configs[2]);
	      us.dataPort=stoi(configs[3]);
	      while(4+i<configs.size())
		{
		  //cout<<configs[4+i];
		  us.neighboors.push_back(stoi(configs[4+i]));
		  i++;
		}
	      DV myself;
	      myself.node=us.id;
	      myself.cost=0;
	      myself.port=us.controlPort;//always through controlport
	      us.DVT.push_back(myself);
	      isMade=true;
	    }
	}
      if(isMade == false)
	{
	  cerr<<"NODE ID "<<stoi(argv[1])<<" is not available";
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

int sendtext(int sd, char *msg)
{

}
