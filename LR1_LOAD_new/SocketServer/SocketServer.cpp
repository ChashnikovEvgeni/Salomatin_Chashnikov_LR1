// SocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketServer.h"
#include "Message.h"
#include "Session.h"
#include "Server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




CWinApp theApp;

int main()
{
	setlocale(LC_ALL, "Russian");
	cout <<"Server has started" << endl;
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
			Server FirstServer;
			FirstServer.StartServer();
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
