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
	std::println("listen...");

	SOCKADDR_IN clientAddr;
	int32_t addrLen = sizeof(clientAddr);
	bool isRunning = true;
	uint8_t clientCnt = 0;
	while (isRunning)
	{
		SOCKET clientSock = WSAAccept(listenSock, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen, nullptr, 0);
		std::println("클라이언트 연결 성공({})", clientCnt);

		auto [it, result] = g_clients.try_emplace(static_cast<uint8_t>(clientCnt), clientCnt, clientSock);
		auto& client = it->second;
		if (result == true)
		{
			client.x = dis(gen);
			client.y = dis(gen);
			// 초기위치 broadcast
			for (auto& [id, session] : g_clients)
			{
				session.DoSend(clientCnt, client.x, client.y);

				// 다른 사람 위치도 보내주기
				if (id == clientCnt)
				{
					for (auto& [otherId, otherSession] : g_clients)
					{
						if (otherId == clientCnt)
							continue;
						client.DoSend(otherId, otherSession.x, otherSession.y);
					}
				}
			}
		}

		client.DoRecv();
		clientCnt++;
	}

	closesocket(listenSock);
	WSACleanup();
	return 0;
}