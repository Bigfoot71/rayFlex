#include "net/rfSecurity.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>

using namespace rf;

/* LibSodium management */

namespace { bool SodiumInitialized = false; }

void net::InitSodium()
{
    if (SodiumInitialized)
    {
        return;
    }

    if (sodium_init() < 0)
    {
        throw std::runtime_error("Sodium initialization failed");
    }

    SodiumInitialized = true;
}

/* HandshakeHandler implementation */

void net::HandshakeHandler::Scramble(uint8_t data[ChallengeSize]) const
{
    // NOTE: This is a reversible mixing function

    // Loop through 8-byte blocks
    for (uint8_t i = 0; i < ChallengeSize; i += 8)
    {
        uint64_t* block = reinterpret_cast<uint64_t*>(data + i);

        uint64_t scrambled = 0;
        constexpr uint64_t mask = 0x00000000000000FF;

        // Mix the bytes in the block
        scrambled |= ((*block & mask) << 56);
        scrambled |= ((*block & (mask << 8)) << 40);
        scrambled |= ((*block & (mask << 16)) << 24);
        scrambled |= ((*block & (mask << 24)) << 8);
        scrambled |= ((*block & (mask << 32)) >> 8);
        scrambled |= ((*block & (mask << 40)) >> 24);
        scrambled |= ((*block & (mask << 48)) >> 40);
        scrambled |= ((*block & (mask << 56)) >> 56);

        *block = scrambled;
    }
}

void net::HandshakeHandler::PrepareHandshakeForClient(const uint8_t serverPublicKey[CryptoKeySize])
{
    auto packet = reinterpret_cast<Handshake*>(handshakeOut);

    // Generate a random challenge for the client
    randombytes(handshakeCheck, sizeof(handshakeCheck));
    std::memcpy(packet->challenge, handshakeCheck, sizeof(handshakeCheck));

    // Mix the random challenge; the client must unscramble it
    Scramble(packet->challenge);

    // Copy the server's public key into the packet
    std::memcpy(packet->publicKey, serverPublicKey, CryptoKeySize);

    // The packet is now ready for transmission!
}

bool net::HandshakeHandler::IsClientHandshakeValid(uint8_t clientPublicKey[CryptoKeySize]) const
{
    auto packet = reinterpret_cast<const Handshake*>(handshakeIn);

    // Check if the challenge sent matches the expected challenge
    for (uint8_t i = 0; i < ChallengeSize; i++)
    {
        if (packet->challenge[i] != handshakeCheck[i])
        {
            return false;
        }
    }

    // Client successfully solved the challenge; retrieve their public key
    std::memcpy(clientPublicKey, packet->publicKey, CryptoKeySize);

    return true;
}

void net::HandshakeHandler::ResolveServerHandshake(uint8_t serverPublicKey[CryptoKeySize], const uint8_t clientPublicKey[CryptoKeySize])
{
    std::memcpy(handshakeOut, handshakeIn, sizeof(Handshake));
    auto packet = reinterpret_cast<Handshake*>(handshakeOut);

    // Unscramble the challenge value
    Scramble(packet->challenge);

    // Copy the server's public key from the packet
    std::memcpy(serverPublicKey, packet->publicKey, CryptoKeySize);

    // Write our own public key into the packet to be sent back
    std::memcpy(packet->publicKey, clientPublicKey, CryptoKeySize);

    // The packet is now ready for transmission!
}
