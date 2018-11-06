#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>

#include "ClientSide.h"

namespace communication {

using std::string;
using std::cout;
using std::endl;

ClientSocket::ClientSocket(string addr, unsigned int port): sockaddr(addr), sockport(port) {};

//close sonnection on destructor
ClientSocket::~ClientSocket() {
	CloseConnection();
}

int ClientSocket::Init() {
    	struct sockaddr_in server_addr; 
    	
	// Communicate over TCP, IPv4
	if (( sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0) 
    	{	 
        	cout << "Socket creation error" << endl; 
        	return -1; 
    	} 
   
    	memset(&server_addr, '0', sizeof(server_addr)); 
   	server_addr.sin_family = AF_INET; 
    	server_addr.sin_port = htons(sockport); 

      
    	// Convert IPv4 addresses from string to address struct in af family 
    	if ( inet_pton( AF_INET, sockaddr.c_str(), &server_addr.sin_addr ) <=0 )  
    	{ 
        	cout << "Invalid address" << endl; 
        	return -1; 
    	} 
   
	// Now that we've populated address and port, let's connect to the Qemu
	// socket listening on port *sockport*
    	if ( connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr) ) < 0 ) 
    	{ 
        	cout << "Connection Failed" << endl;; 
        	return -1; 
    	} 
	
	return 0;
}

SockError ClientSocket::SendCommand( QemuCommand cmd ) {
	SockError err = eNone;
	
	// First check if the client is connected
	if ( sockfd < 0 ) {
		cout << "Client not connected" << endl;
		err = eNotConnected;
		return err;
	}
	
	// Create the message packet
	SockMessage msg;
	msg.q_cmd = cmd;
	
	//Send the actual message now
	char* list = "load_plugin writetracker";
	if ( write( sockfd, list, strlen(list)*sizeof(char)) < 0) {
		err = eOther;
		return err;
	}
	cout << "Length of write"  << strlen(list) << endl;
	/*int bytes_written = 0;
  	do {
    		int res = send(socket, (char*) &d + bytes_written,
        	sizeof(d) - bytes_written, 0);
    		if (res < 0) {
      			return -1;
    		}
    		bytes_written += res;
	} while (bytes_written < sizeof(d));
	*/
	return err;
}

SockError ClientSocket::ReceiveReply(SockMessage *msg) {
	SockError err = eNone;
	int valread;
	int len = 0;
	ioctl(sockfd, FIONREAD, &len);
	cout << "Expected length of reply = " << len  << endl;
	//char *buffer = (char*)malloc ((len+1)* sizeof(char));
	char buffer[len];
	int bytes_read = 0;
	do {
		valread = recv( sockfd , buffer + bytes_read, len - bytes_read, 0);
    		if (valread <= 0){
			err = eOther;
			return err;
		}
		bytes_read += valread;
	}while (bytes_read < len);

	cout  << "Read data = " << buffer << endl;
	return err;
}

void ClientSocket::CloseConnection() {
	close(sockfd);
	sockfd = -1;
}

} //namespace communication
