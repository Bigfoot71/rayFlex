#include <rayflex.hpp>
#include "common.hpp"

using namespace rf;

class GameServer : public net::ServerInterface<GameMsg>
{
  public:
    std::unordered_map<uint32_t, PlayerDescription> mapPlayerRoster;
    std::vector<uint32_t> garbageIDs;

    GameServer(uint16_t port) : net::ServerInterface<GameMsg>(port) { }

  protected:
    bool OnClientConnect(std::shared_ptr<net::Connection<GameMsg>> client) override
    {
        return true; // Ici nous acceptons tout le monde
    }

    void OnClientValidated(std::shared_ptr<net::Connection<GameMsg>> client) override
    {
        // Le client a réussi le contrôle de validation, alors
        // on lui envoi un message l'informant qu'il peut continuer à communiquer
        net::Packet<GameMsg> packet(GameMsg::Client_Accepted);
        client->Send(packet);
    }

    void OnClientDisconnect(std::shared_ptr<net::Connection<GameMsg>> client) override
    {
        if (client)
        {
            // Nous retirons le client du serveur, uniquement si il a déjà été ajouté
            if (mapPlayerRoster.find(client->GetID()) != mapPlayerRoster.end())
            {
                auto& pd = mapPlayerRoster[client->GetID()];
                std::cout << "SERVER: Ungraceful removal [ID " << std::to_string(pd.uniqueID) << "]" << std::endl;
                mapPlayerRoster.erase(client->GetID());
                garbageIDs.push_back(client->GetID());
            }
        }

    }

    void OnReceivePacket(std::shared_ptr<net::Connection<GameMsg>> client, net::Packet<GameMsg>& packet) override
    {
        // Tout d'abbord, si un client s'est déconécté nous avons
        // gardé son ID, nous envoyé donc son ancien ID à tous les
        // les clients pour leur signalé qu'il faut le retiré
        if (!garbageIDs.empty())
        {
            for (auto pid : garbageIDs)
            {
                std::cout << "Removing " << pid << std::endl;
                net::Packet<GameMsg> packet(GameMsg::Game_RemovePlayer, pid);
                SendPacketToAll(packet);
            }
            garbageIDs.clear();
        }

        /* Gestion des paquet reçus */

        switch (packet.header.id)
        {
            case GameMsg::Client_RegisterWithServer:
            {
                PlayerDescription desc;
                packet >> desc;
                desc.uniqueID = client->GetID();
                mapPlayerRoster.insert_or_assign(desc.uniqueID, desc);

                // Send unique ID to new player
                net::Packet<GameMsg> p1(GameMsg::Client_AssignID, desc.uniqueID);
                SendPacket(client, p1);

                // Send new player description to all
                net::Packet<GameMsg> p2(GameMsg::Game_AddPlayer, desc);
                SendPacketToAll(p2);

                // Send all players descriptions to new player
                for (const auto& player : mapPlayerRoster)
                {
                    net::Packet<GameMsg> p(GameMsg::Game_AddPlayer, player.second);
                    SendPacket(client, p);
                }

                break;
            }

            case GameMsg::Client_UnregisterWithServer:
            {
                break;
            }

            case GameMsg::Game_UpdatePlayer:
            {
                // Un client a envoyé la mise à jour de sont entité (joueur)
                // On envoie donc la mise à jour à tout le monde, sauf au client entrant.
                SendPacketToAll(packet, client);
                break;
            }

            default: break;
        }
    }
};

int main()
{
    GameServer server(60000);
    server.Start();

    for (;;)
    {
        server.Update(-1, true);
    }

    return 0;
}
