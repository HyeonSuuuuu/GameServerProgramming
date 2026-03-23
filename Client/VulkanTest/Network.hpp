// 큐를 통해 통신
// 스레드 -> 네트워크 연결

class Networker
{
public:
	Networker() = default;
	~Networker();
	bool Init();
	bool Connect(const char* ip, uint16_t port);
	void Start();
	void Stop();

private:
	void Run();

	SOCKET m_socket = INVALID_SOCKET;
	std::atomic<bool> m_isRunning = false;
	std::thread m_thread ;
};