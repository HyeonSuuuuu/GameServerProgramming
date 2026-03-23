#include <iostream>
#include <ws2tcpip.h>
#include <print>
#pragma comment(lib, "ws2_32.lib")

#include "../../Common/Protocol.h"



struct Pawn
{
	int8_t x; int8_t y;
};

Pawn pawn{ 4, 0 };

int main()
{
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

	SOCKET listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
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
	std::print("listen...");

	int32_t addrLen = sizeof(serverAddr);
	SOCKET clientSock = WSAAccept(listenSock, reinterpret_cast<SOCKADDR*>(&serverAddr), &addrLen, nullptr, 0);
	std::println("클라이언트 연결 성공");

	// 초기위치 send
	SC_MovePkt sendMovePkt{ pawn.x, pawn.y };

	WSABUF sendWsaBuf;
	sendWsaBuf.buf = reinterpret_cast<char*>(&sendMovePkt);
	sendWsaBuf.len = SC_MOVE_PKT_SIZE;

	DWORD sentSize{};
	result = WSASend(clientSock, &sendWsaBuf, 1, &sentSize, 0, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		error_display(L"데이터 전송 실패", WSAGetLastError());
	}

	while (1)
	{
		// 입력값 Recv
		WSABUF wsaBuf;
		CS_MovePkt movePkt;
		wsaBuf.buf = reinterpret_cast<char*>(&movePkt);
		wsaBuf.len = CS_MOVE_PKT_SIZE;
		DWORD recvSize{};
		DWORD recvFlag{};
		result = WSARecv(clientSock, &wsaBuf, 1, &recvSize, &recvFlag, nullptr, nullptr);
		if (result == SOCKET_ERROR)
		{
			error_display(L"recv 실패", WSAGetLastError());
			break;
		}
		if (recvSize > 0)
		{
			std::println("데이터 받음: {}", movePkt.moveFlag);
		}
		else
		{
			std::println("클라이언트의 연결이 종료되었습니다.");
			break;
		}
		// 계산 a:1 d:2 w:4 s:8
		if (movePkt.moveFlag & 1) pawn.x -= 1;
		if (movePkt.moveFlag & 2) pawn.x += 1;
		if (movePkt.moveFlag & 4) pawn.y -= 1;
		if (movePkt.moveFlag & 8) pawn.y += 1;

		if (pawn.x < 0)
			pawn.x = 0;
		if (pawn.x > 7)
			pawn.x = 7;
		if (pawn.y < 0)
			pawn.y = 0;
		if (pawn.y > 7)
			pawn.y = 7;

		// Send
		SC_MovePkt sendMovePkt{ pawn.x, pawn.y };

		WSABUF sendWsaBuf;
		sendWsaBuf.buf = reinterpret_cast<char*>(&sendMovePkt);
		sendWsaBuf.len = SC_MOVE_PKT_SIZE;

		DWORD sentSize{};
		result = WSASend(clientSock, &sendWsaBuf, 1, &sentSize, 0, nullptr, nullptr);
		if (result == SOCKET_ERROR)
		{
			error_display(L"데이터 전송 실패", WSAGetLastError());
		}
	}



	closesocket(clientSock);
	closesocket(listenSock);
	WSACleanup();
	return 0;
}