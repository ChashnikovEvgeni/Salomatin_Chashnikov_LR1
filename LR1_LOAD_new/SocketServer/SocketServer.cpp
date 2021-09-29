// SocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketServer.h"
#include "Message.h"
#include "Session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void LaunchClient()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	CreateProcess(NULL, (LPSTR)"SocketClient.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	//Аргументы: имя, команда, атрибуты процесса, атрибуты потока,дискриптор параметров наследования,флаги создания,блок конфигурации, имя текущего каталога, вконце инфо - размер в байтах,инфа о процессе
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

int maxClientID = MR_USER;
map<int, shared_ptr<Session>> clientSessions;


void TimeOut() {

	while (true)
	{
		for (auto i = clientSessions.begin(); i != clientSessions.end();) {
			if (clientSessions.find(i->first) != clientSessions.end()) {
				if (double(clock() - i->second->time) > 100000) {
					cout << "Клиент " << i->first << " был отключён" << endl;
					i = clientSessions.erase(i);
					
				}
				else
					i++;
			}
		}
		Sleep(1000);
	}
}


void ProcessClient(SOCKET hSock)
{
	CSocket s;
	s.Attach(hSock);

	Message m;
	int code = m.receive(s);
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
			
			Message M1(m.header.from, m.header.from, MT_DATA,a);
			iSessionFrom->second->add(M1);
			cout << m.header.from << ": информация о клиентах отправлена " << endl;
		}
		break;
	}
	case MT_INIT:
	{
		
		auto session = make_shared<Session>(++maxClientID, m.data, clock());
		clientSessions[session->id] = session;
		Message::send(s, session->id, MR_BROKER, MT_INIT);
		cout << session->id << ": клиент зарегистрирован " << endl;
		break;
	}
	case MT_EXIT:
	{
		clientSessions.erase(m.header.from);
		cout << "Клиент " << m.header.from << " отключился \n";
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
				cout << m.header.from << ": Клиент отправил сообщение " << m.header.to << endl;
			}
			else if(m.header.to == MR_ALL)
			{
				for (auto& [id, session] : clientSessions)
				{
					if (id != m.header.from)
						session->add(m);
					cout << m.header.from << ": Клиент отправил сообщение всем " << endl;
				}
			}
			else {
				Message M1(m.header.from, m.header.from, MT_DATA, "Адресат с предоставленным ID не обнаружен в системе");
				iSessionFrom->second->add(M1);
				cout << m.header.from << ": Ошибка в записи ID " << endl;
			}
			clientSessions[m.header.from]->time=clock();
		}
		break;
	}
	}
}

void Server()
{
	AfxSocketInit();

	CSocket Server;
	Server.Create(12345);

	for (int i = 0; i < 3; ++i)
	{
		LaunchClient();
	}

	thread tt(TimeOut);
	tt.detach();
	while (true)
	{
		if (!Server.Listen()) 
			break;
		CSocket s;
		Server.Accept(s);
		thread t(ProcessClient, s.Detach());
		t.detach();
	}
}

CWinApp theApp;

int main()
{
	setlocale(LC_ALL, "Russian");
	cout <<"Стартуем" << endl;
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: code your application's behavior here.
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			Server();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
