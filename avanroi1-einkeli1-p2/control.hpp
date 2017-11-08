#include <stdio.h>
#include <fcntl.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <thread> // allows for threads!
#include <pthread.h>
#include <mutex>
#include <time.h>

#ifndef PORTS_H
#define PORTS_H

struct PORTS {
  int controlPort;
  int dataPort;
  std::string hostName;
};
#endif
std::vector<std::string> split(char delim,std::string s);
int executeCommand(std::map<std::string,PORTS> mapPorts,std::string cmd);
int sendPayload(std::string payload,std::map<std::string,PORTS> mapPorts,std::string dest,std::string dest2, int controlSocket, int id);
