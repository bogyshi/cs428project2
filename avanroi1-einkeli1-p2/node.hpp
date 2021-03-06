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

#ifndef DV_H
#define DV_H

struct DV {
  int dest;
  int nextHop;
  int cost;
};
#endif

#ifndef NODE_H
#define NODE_H

struct Node {
  Node(char* argv[]);
  int id;
  int dataPort;
  int controlPort;
  std::string hostName;
  std::vector<DV> DVT;
  std::vector<int> neighbors;
  std::map<int,PORTS> mapPorts;
};
#endif


void sendText(Node* me, std::vector<std::string> message, int socket);
void parseControlPacket(Node * me, std::string info);
void sendControlPacket(Node * me, int socket);
void handleDataPacket(Node me, char * payload);
std::string alterOrReadTable(int code, std::string changer, Node * me);
void waitforUpdates(Node* me);
void waitforData(Node* me);
std::vector<std::string> split(char delim,std::string s);
