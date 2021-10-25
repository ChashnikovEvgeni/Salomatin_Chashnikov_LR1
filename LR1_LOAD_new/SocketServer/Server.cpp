#include "pch.h"
#include "Server.h"
using namespace std;

void Server::TimeOut() {

	while (true)
	{
		for (auto i = clientSessions.begin(); i != clientSessions.end();) {
			if (clientSessions.find(i->first) != clientSessions.end()) {
				if (double(clock() - i->second->time) > 100000) {
					cout << "Client " << i->first << " has been disconnected" << endl;
					i = clientSessions.erase(i);

				}
				else
					i++;
			}
		}
		Sleep(1000);
	}
}


void Server::ProcessClient(SOCKET hSock)
{
	CSocket s;
	s.Attach(hSock);

	Message m;
	int code = m.receive(s);
	try {

		switch (code)
		{
		case INFO:
		{
			auto iSessionFrom = clientSessions.find(m.header.from);
			if (iSessionFrom != clientSessions.end())
			{
				string a = "";
				for (auto& [id, session] : clientSessions)
				{
					if (id != m.header.from)
						a = a + to_string(id);
					a.push_back(32);
				}

				Message M1(m.header.from, m.header.from, MT_DATA, a);
				iSessionFrom->second->add(M1);
				cout << m.header.from << ": clients information has been sent " << endl;
			}
			break;
		}
		case MT_INIT:
		{

			auto session = make_shared<Session>(++maxClientID, m.data, clock());
			clientSessions[session->id] = session;
			Message::send(s, session->id, MR_BROKER, MT_INIT);
			cout << session->id << ": client has been registred " << endl;
			break;
		}
		case MT_EXIT:
		{
			clientSessions.erase(m.header.from);
			cout << "Client " << m.header.from << " has been disconnected \n";
			Message::send(s, m.header.from, MR_BROKER, MT_CONFIRM);
			break;
		}
		case MT_GETDATA:
		{
			auto iSession = clientSessions.find(m.header.from);
			if (iSession != clientSessions.end())
			{
				iSession->second->send(s);
				iSession->second->time = clock();
			}
			break;
		}
		default:
		{
			auto iSessionFrom = clientSessions.find(m.header.from);
			if (iSessionFrom != clientSessions.end())
			{
				auto iSessionTo = clientSessions.find(m.header.to);
				if (iSessionTo != clientSessions.end())
				{
					iSessionTo->second->add(m);
					cout << m.header.from << ": Client sent a message to" << m.header.to << endl;
				}
				else if (m.header.to == MR_ALL)
				{
					for (auto& [id, session] : clientSessions)
					{
						if (id != m.header.from)
							session->add(m);
						cout << m.header.from << ": Client sent a message to everyone " << endl;
					}
				}
				else {
					Message M1(m.header.from, m.header.from, MT_DATA, "The recipient with the specified ID was not found in the system");
					iSessionFrom->second->add(M1);
					cout << m.header.from << ": Error in Id entry" << endl;
				}
				clientSessions[m.header.from]->time = clock();
			}
			break;
		}
		}
	}
	catch (runtime_error err)
	{
		cout << "Runtime_error:" << err.what() << endl;
	};
}

void Server::StartServer()
{
	AfxSocketInit();
	CSocket ServerSocket;
	ServerSocket.Create(12345);

	thread tt(&Server::TimeOut,this);
	tt.detach();
	while (true)
	{
		if (!ServerSocket.Listen())
			break;
		CSocket s;
		ServerSocket.Accept(s);
		thread t(&Server::ProcessClient,this, s.Detach());
		t.detach();
	}
}
