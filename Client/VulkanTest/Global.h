#pragma once
#include "lve_ecs.hpp"
namespace lve { class LveModel; }

struct PlayerInfo
{
	PlayerInfo() = default;
	PlayerInfo(uint8_t id, int8_t x, int8_t y)
		: id{ id }, x{x}, y{y}
	{
	}
	~PlayerInfo()
	{
		
	}
	uint8_t id;
	int8_t x;
	int8_t y;
};

extern uint8_t g_moveFlag;
extern std::unordered_map<uint8_t, PlayerInfo> g_clients;
extern std::unordered_map<uint8_t, lve::Entity> g_idToEntity;
extern uint8_t g_myId;
// PlayerInfo에 Entity 집어넣는 작업중