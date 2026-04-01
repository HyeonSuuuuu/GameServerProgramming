#include "pch.h"
#include "Session.h"
#include "NetDefine.h"
#include"Global.h"


EXP_OVER::EXP_OVER(uint8_t client_id, int x, int y)
	: m_id{ client_id }
{
	m_movePkt.id = static_cast<uint8_t>(client_id);
	m_movePkt.x = x;
	m_movePkt.y = y;
}

Session::Session(uint8_t id, SOCKET s)
	: m_id{ id }
	, m_socket{ s }
{
	m_recvWsaBuf.len = SC_MOVE_PKT_SIZE;
	m_recvWsaBuf.buf = reinterpret_cast<char*>(&m_recvMovePkt);
}

Session::~Session()
{
	closesocket(m_socket);
}


void Session::DoRecv()
{
	DWORD recvFlag{};
	memset(&m_recvOver, 0, sizeof(m_recvOver));
	m_recvOver.hEvent = reinterpret_cast<HANDLE>(m_id);

	int32_t result = WSARecv(m_socket, &m_recvWsaBuf, 1, 0, &recvFlag, &m_recvOver, RecvCallback);
	if (result == SOCKET_ERROR)
	{
		int32_t err = WSAGetLastError();
		if (err != ERROR_IO_PENDING)
		{
			error_display(L"recv 실패", err);
			return;
		}
	}
}

void Session::DoSend(uint8_t sender_id, int x, int y)
{
	EXP_OVER* over = new EXP_OVER(sender_id, x, y);
	int32_t result = WSASend(m_socket, over->m_wsaBuf, 1, 0, 0, &over->m_over, SendCallback);
	if (result == SOCKET_ERROR)
	{
		int32_t err = WSAGetLastError();
		if (err != ERROR_IO_PENDING)
		{
			error_display(L"데이터 전송 실패", err);
			delete over;
			return;
		}
	}

	std::println("Client({}): Send({}, {}, {})", m_id, sender_id, x, y);

}

void Session::MoveUpdate()
{
	// 계산 a:1 d:2 w:4 s:8
	if (m_recvMovePkt.moveFlag & 1) x -= 1;
	if (m_recvMovePkt.moveFlag & 2) x += 1;
	if (m_recvMovePkt.moveFlag & 4) y -= 1;
	if (m_recvMovePkt.moveFlag & 8) y += 1;

	if (x < 0)
		x = 0;
	if (x > 7)
		x = 7;
	if (y < 0)
		y = 0;
	if (y > 7)
		y = 7;
}

void RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flags)
{
	uint8_t clientId = static_cast<uint8_t>(reinterpret_cast<uint64_t>(over->hEvent));
	// 연결 끊김
	if (numBytes <= 0)
	{
		std::println("연결 끊김({})", clientId);
		g_clients.erase(clientId);
		// 끊겼다는걸 broadcast
		for (auto& [id, session] : g_clients)
		{
			session.DoSend(clientId, -1, -1);
		}
		return;
	}

	auto& client = g_clients[clientId];
	
	client.MoveUpdate();

	// broadcasting
	for (auto& [id, session] : g_clients)
	{
		session.DoSend(clientId, client.x, client.y);
	}


	client.DoRecv();
}

void SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flags)
{

	EXP_OVER* expOver = reinterpret_cast<EXP_OVER*>(over);
	delete expOver;
}
