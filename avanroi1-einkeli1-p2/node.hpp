#include <stdio.h>
#include <fcntl.h>
#include <string>
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

#ifndef NODE_H
#define NODE_H

struct Node {
  int id;
  std::vector<int> neighboors;
  int dataPort;
  int controlPort;
  std::string hostName;
};
#endif

int sendtext(int sd, char *msg);
std::vector<std::string> split(char delim,std::string s);