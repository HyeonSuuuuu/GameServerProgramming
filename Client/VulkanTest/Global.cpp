#include "pch.h"
#include "Global.h"
#include "../../Common/Protocol.h"


// atomic한 moveflag
uint8_t g_moveFlag{};

std::unordered_map<uint8_t, PlayerInfo> g_clients;
std::unordered_map<uint8_t, lve::Entity> g_idToEntity;

uint8_t g_myId{};
