#include "pch.h"
#include "chess_app.hpp"
#include "Network.hpp"
#include "../../Common/Protocol.h"

int main()
{
	Networker networker;
	std::string serverIP;

	if (!networker.Init())
	{
		std::println ("network 초기화 실패");
		return 1;
	}

	std::print("접속할 서버 IP를 입력해주세요: ");
	std::getline(std::cin, serverIP);
	if (serverIP.empty()) serverIP = "127.0.0.1";

	if (!networker.Connect(serverIP.c_str(), SERVER_PORT))
	{
		std::println("connect 실패");
		return 1;
	}
	networker.Start();
	std::println("네트워크 연결 성공");


	try {
		lve::ChessApp app{};
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}