#pragma once
#include "Message.h"
#include "Session.h"

class Server
{
public:	

	
	void ProcessClient(SOCKET hSock);
	void StartServer();
	void TimeOut();
private:
	map<int, shared_ptr<Session>> clientSessions;
	int maxClientID = MR_USER;
	
};

