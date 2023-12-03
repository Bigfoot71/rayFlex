#include <rayflex.hpp>
#include "common.hpp"

using namespace rf;

class Game : public core::State, net::ClientInterface<GameMsg>
{
  private:
	std::unordered_map<uint32_t, PlayerDescription> mapObjects;
	PlayerDescription descPlayer;
	uint32_t playerID = 0;

    core::RandomGenerator gen;
    float waitingTimeAnim = 0.0f;
	bool waitingForConnection = true;

  public:

    /* CLIENT PART */

    void GetPackets()
    {
		if (!IsConnected()) return;

        while (!Incoming().empty())
        {
            auto packet = *(Incoming().pop_front());

            switch (packet.header.id)
            {
                // Après la connexion, nous devrions recevoir cette reponse d'acceptation
                // Si nous la recevons nous lui envoyons donc notre entité (joueur)
                case(GameMsg::Client_Accepted):
                {
                    TraceLog(LOG_INFO, "Server accepted client!");
                    net::Packet<GameMsg> response;
                    response.header.id = GameMsg::Client_RegisterWithServer;
                    descPlayer.position = gen.RandomVec2({0,0}, app->renderer.GetSize());
                    for (int i = 0; i < 7; i++) descPlayer.name[i] = gen.RandomChar();
                    descPlayer.name[7] = '\0';
                    response << descPlayer;
                    Send(response);
                    break;
                }

                // Après que nous ayaons envoyé notre entité,
                // le serveur nous renvoie un ID unique
                case(GameMsg::Client_AssignID):
                {
                    packet >> playerID;
                    TraceLog(LOG_INFO, "Assigned Client ID! [ID %i]", playerID);
                    break;
                }

                // Ici nous gérons tous les nouveaux joueurs entrant
                // Notemment nous même juste après GameMsg::AssignID
                case(GameMsg::Game_AddPlayer):
                {
                    PlayerDescription desc;
                    packet >> desc;
                    mapObjects.insert_or_assign(desc.uniqueID, desc);
                    if (desc.uniqueID == playerID) waitingForConnection = false;    // Si le nouveau joueur s'agit de nous, nous n'avons plus à attendre
                    break;
                }

                // Ici nous gérons les deconnexions
                case(GameMsg::Game_RemovePlayer):
                {
                    uint32_t removalID = 0;
                    packet >> removalID;
                    mapObjects.erase(removalID);
                    break;
                }

                // Ici nous gérons les mise à jour
                case(GameMsg::Game_UpdatePlayer):
                {
                    PlayerDescription desc;
                    packet >> desc;
                    mapObjects.insert_or_assign(desc.uniqueID, desc);
                    break;
                }

                default: break;
            }
        }
    }

    void SendPacket()
    {
        // Send player description
        net::Packet<GameMsg> ownPacket;
        ownPacket.header.id = GameMsg::Game_UpdatePlayer;
        ownPacket << mapObjects[playerID];
        Send(ownPacket);
    }

    /* STATE PART */

    void Enter() override
    {
        if (!Connect("127.0.0.1", 60000))
        {
            TraceLog(LOG_ERROR, "Unable to connect to server");
            app->Finish();
        }
    }

    void Exit() override
    {
        Disconnect();
    }

    void Update(float dt) override
    {
        GetPackets();

        if (waitingForConnection)
        {
            waitingTimeAnim += dt;
            return;
        }

		// Control of Player Object
		mapObjects[playerID].velocity = raylib::Vector2(
            static_cast<float>(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)),
            static_cast<float>(IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP))
        ).Normalize();

		// Update objects locally
		for (auto& object : mapObjects)
		{
			raylib::Vector2 potentialPosition = object.second.position + object.second.velocity * object.second.speed * dt;
			object.second.position = potentialPosition;
		}

        // send our packets
        SendPacket();
    }

    void Draw() override
    {
		if (waitingForConnection)
		{
            static const char text[] = "Waiting to connect...";
            int len = (sizeof(text) - 1) - (3 - (static_cast<int>(2.5f*waitingTimeAnim)%4));
			DrawText(TextSubtext(text, 0, len),
                10, 10, 24, WHITE);
			return;
		}

		for (const auto& object : mapObjects)
		{
            const auto &desc = object.second;

			// Draw entity
			DrawCircleV(desc.position, desc.fRadius, RED);

            // Draw name
            DrawText(desc.name,
                desc.position.x - desc.fRadius,
                desc.position.y - desc.fRadius,
                24, WHITE);
		}
    }
};

int main()
{
    core::App app("NET - Client", 800, 600);
    app.AddState<Game>("game");
    return app.Run("game");
}
