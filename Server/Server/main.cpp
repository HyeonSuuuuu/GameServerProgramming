#include "pch.h"
#include "Session.h"
#include "Global.h"



int main()
{
	// random init
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 7);

	WORD wVersionRequested;
	WSADATA wsaData;
	int result;

	wVersionRequested = MAKEWORD(2, 2);
	result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0)
	{
		error_display(L"WSAStartup()", WSAGetLastError());
		return 1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		std::print("Winsock.dll의 사용 가능한 버전을 찾을 수 없습니다");
		WSACleanup();
		return 1;
	}

	SOCKET listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (listenSock == INVALID_SOCKET)
	{
		error_display(L"listenSock = WSASocket()", WSAGetLastError());
	}
	
	SOCKADDR_IN serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	result = bind(listenSock, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		error_display(L"bind failed", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR)
	{
		error_display(L"listen failed", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	HANDLE h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // IOCP 생성
	CreateIoCompletionPort((HANDLE)listenSock, h_iocp, -1, 0); // lstenSocket 등록
	
	std::println("listen...");

	SOCKET clientSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	EXP_OVER acceptOver(IO_ACCEPT);
	if (AcceptEx(listenSock, clientSock, &acceptOver.m_buff, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &acceptOver.m_over) == false)
	{
		int errCode = WSAGetLastError();
		if (errCode != ERROR_IO_PENDING)
		{
			error_display(L"AcceptEx failed", errCode);
			return 1;
		}
	}


	bool isRunning = true;
	uint8_t playerIndex = 0;
	while (isRunning)
	{
		DWORD numBytes;
		ULONG_PTR key;
		LPOVERLAPPED over;
		GetQueuedCompletionStatus(h_iocp, &numBytes, &key, &over, INFINITE);
		if (over == nullptr)
		{
			error_display(L"GQCS Error: ", WSAGetLastError());
			if (key == -1)
			{
				exit(-1);
			}
			std::cout << "client[" << key << "] Disconnected.\n";
			g_clients[key].IsConnected() = false;
			for (auto& [id, client] : g_clients)
				if (client.IsConnected())
					client.SendRemovePlayer(key);
			continue;
		}
		EXP_OVER* expOver = reinterpret_cast<EXP_OVER*>(over);
		switch (expOver->m_iotype)
		{
		case IO_ACCEPT:
			break;
		case IO_RECV:
			break;
		case IO_SEND:
			break;
		default:
			std::println("Unknown IO type");
			exit(-1);
			break;
		}
	}
	closesocket(listenSock);
	WSACleanup();
	return 0;
}