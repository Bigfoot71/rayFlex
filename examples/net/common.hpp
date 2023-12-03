#ifndef COMMON_HPP
#define COMMON_HPP

#include "Vector2.hpp"
#include <raylib-cpp.hpp>

static constexpr uint64_t HANDSHAKE_SECRET = 0x53757065724C4F4C;

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
};

struct PlayerDescription
{
	uint32_t uniqueID = 0;
	char name[8];

    float speed = 64.0f;
	float fRadius = 24.0f;

	raylib::Vector2 position;
	raylib::Vector2 velocity;
};

#endif //COMMON_HPP