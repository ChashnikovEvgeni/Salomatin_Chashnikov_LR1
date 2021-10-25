// SocketClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketClient.h"
#include "../SocketServer/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





void ProcessMessages()
{
	while (true)
	{
		Message m;
		try {
			m = Message::exchange(MR_BROKER, MT_GETDATA);
		}
		catch (runtime_error err)
		{
			cout << "Runtime_error:" << err.what() << endl;
		};
		switch (m.header.type)
		{
		case MT_DATA:
			
			cout << m.data << endl;
			break;
		default:
			Sleep(100);
			break;
		}
	}
}



void Client()
{
	AfxSocketInit();
	thread t(ProcessMessages);
	t.detach();
	
	Message m = Message::exchange(MR_BROKER, MT_INIT);

	while (true)
	{
		Sleep(500);
		cout<<"Send message"<<'\n'<<
			"1.Send All."<< '\n'<<
			"2.Send only one client "<< '\n' <<
			"3.Exit" << endl;
		int p;
		cin >> p;
		switch (p) {
		case 1: 
		{
			cout << "Write your message:" << endl;
			string s;
			cin.ignore();
			getline(cin, s);
			Message::send(MR_ALL, MT_DATA, s); 
			break; 
		}
		case 2: 
		{
			cout << "Write users ID:" << endl;
			Message::send(MR_ME,INFO,"");
			int v;
			cin >> v;
			cout << "Write your message:" << endl;
			string s;
			cin.ignore();
			getline(cin, s);
			Message::send(v, MT_DATA, s); 

			break;
		}
		case 3:
			cout << "Application is closing" << endl;
			Message::send(MR_ME, MT_EXIT);
			Sleep(1000);
			
			exit(0);
		}
	}
}

CWinApp theApp;


int main()
{

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
			try {
				Client();
			}
			catch (runtime_error err)
			{
				cout << "Runtime_error:" << err.what() << endl;
			};
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
