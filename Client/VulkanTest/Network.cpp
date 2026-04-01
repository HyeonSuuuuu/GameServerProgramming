#include "pch.h"
#include "Network.hpp"
#include "Global.h"
#include "lve_ecs.hpp"
#include "lve_utils.hpp"
#include "lve_components.hpp"

#include "../../../Common/Protocol.h"


void Networker::Run(lve::Coordinator& coordinator)
{
	if (g_moveFlag != 0)
	{
		CS_MovePkt movePkt;
		movePkt.moveFlag = g_moveFlag;

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(&movePkt);
		wsaBuf.len = CS_MOVE_PKT_SIZE;

		DWORD sentSize{};
		int result = WSASend(m_socket, &wsaBuf, 1, &sentSize, 0, nullptr, nullptr);
		if (result == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				error_display(L"데이터 전송 실패", err);
				exit(1);
			}
		}
		g_moveFlag = 0;
	}

	//Recv
	WSABUF recvWsaBuf;
	SC_MovePkt recvMovePkt;
	recvWsaBuf.buf = reinterpret_cast<char*>(&recvMovePkt);
	recvWsaBuf.len = SC_MOVE_PKT_SIZE;
	DWORD recvSize{};
	DWORD recvFlag{};
	int result = WSARecv(m_socket, &recvWsaBuf, 1, &recvSize, &recvFlag, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
		{
			return;
		}
		error_display(L"recv 실패", err);
		exit(1);
	}
	if (recvSize > 0)
	{
		std::println("데이터 받음id:{}: x:{} y:{}",recvMovePkt.id, recvMovePkt.x, recvMovePkt.y);
	}
	else
	{
		std::println("서버의 연결이 종료되었습니다.");
		exit(1);
	}

	// 처리
	auto id = recvMovePkt.id;
	g_myId = id;
	if (g_clients.contains(id) == false)
	{ // 새로운 클라이언트
		g_clients.try_emplace(id, id, recvMovePkt.x, recvMovePkt.y);
		m_enterQueue.push(id);
	}
	else
	{
		if (recvMovePkt.x == -1 && recvMovePkt.y == -1)
		{ 
			// 클라이언트 퇴장
			g_clients.erase(id);
			m_leaveQueue.push(id);
			return;
		}
		g_clients[id].x = recvMovePkt.x;
		g_clients[id].y = recvMovePkt.y;
	}
}

Networker::~Networker()
{
	WSACleanup();
}

bool Networker::Init()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int result;

	wVersionRequested = MAKEWORD(2, 2);
	result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0)
	{
		error_display(L"WSAStartup()", WSAGetLastError());
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		std::print("Winsock.dll의 사용 가능한 버전을 찾을 수 없습니다");
		return false;
	}
	return true;
}

bool Networker::Connect(const char* ip, uint16_t port)
{
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	if (m_socket == INVALID_SOCKET)
	{
		error_display(L"listenSock = WSASocket()", WSAGetLastError());
	}
	
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);

	int32_t result = inet_pton(AF_INET, ip, &serverAddr.sin_addr);
	if (result == 0)
	{
		std::println("잘못된 IP 주소 형식입니다: {}", ip);
		return false;
	}
	else if (result == -1)
	{
		error_display(L"inet_pton()", WSAGetLastError());
		return false;
	}

	result = WSAConnect(m_socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr), nullptr, nullptr, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		error_display(L"서버 연결 실패", WSAGetLastError());
		return false;
	}


	// 시작위치 받기
	WSABUF recvWsaBuf;
	SC_MovePkt recvMovePkt;
	recvWsaBuf.buf = reinterpret_cast<char*>(&recvMovePkt);
	recvWsaBuf.len = SC_MOVE_PKT_SIZE;
	DWORD recvSize{};
	DWORD recvFlag{};
	result = WSARecv(m_socket, &recvWsaBuf, 1, &recvSize, &recvFlag, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		error_display(L"recv 실패", WSAGetLastError());
		return false;
	}
	if (recvSize > 0)
	{
		std::println("데이터 받음id:{}: x:{} y:{}", recvMovePkt.id, recvMovePkt.x, recvMovePkt.y);
	}
	else
	{
		std::println("서버의 연결이 종료되었습니다.");
		return false;
	}
	// 처리
	auto id = recvMovePkt.id;
	g_myId = id;
	if (g_clients.contains(id) == false)
	{
		g_clients.try_emplace(id, id, recvMovePkt.x, recvMovePkt.y);
	}
	else
	{
		g_clients[id].x = recvMovePkt.x;
		g_clients[id].y = recvMovePkt.y;
	}

	// 논블로킹 전환
	unsigned long mode = 1; // 1: 논블로킹, 0: 블로킹
	if (ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR) {
		error_display(L"ioctlsocket 실패", WSAGetLastError());
		return false;
	}
	return true;
}

