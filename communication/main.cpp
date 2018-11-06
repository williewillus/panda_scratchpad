#include <unistd.h>
#include <string>
#include <iostream>
#include "ClientSide.h"

using std::string;
using std::cout;
using std::endl;
//using communication::ClientSocket;
//using communication::QemuCommand;
//using communication::SockError;
using namespace communication;

int main(int argc, char** argv) {

	ClientSocket* vm = NULL;
	vm = new ClientSocket("192.168.122.1", 4444);
	
	//init socket
	if (vm->Init() < 0) {
		int err_no = errno;
		cout << "Error starting socket" << endl;
		delete vm;
		return -1;
	}
	cout << "Connected to socket" << endl;

	QemuCommand command = cListPlugins;
	if (vm->SendCommand( command ) != eNone ) {
		int err_no = errno;
		cout << "Error sending message" << endl;
		delete vm;
		return -1;
	}
	sleep(1);
  	SockMessage* msg;
	vm->ReceiveReply(msg);

	delete vm;
	return 0;


}
