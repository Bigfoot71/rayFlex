#ifndef RAYSX_NETWORK_SECURITY_HPP
#define RAYSX_NETWORK_SECURITY_HPP

#include "./rfPacket.hpp"

#include <stdexcept>
#include <sodium.h>
#include <cstdint>
#include <random>
#include <chrono>
#include <vector>

namespace rf { namespace net {

    // Size of encryption keys
    constexpr uint8_t CryptoKeySize = 32;

    /**
     * @brief Initializes the LibSodium library.
     * If LibSodium is already initialized, this function does nothing.
     */
    void InitSodium();

    /**
     * @brief A handshake handler for rf::net::Connection.
     * Manages handshakes to ensure pairs can communicate correctly,
     * challenges each new connection with a random challenge, and
     * exchanges public keys for generating encryption/decryption keys locally.
     */
    class HandshakeHandler
    {
      private:
        static constexpr uint8_t ChallengeSize = 32;

      private:
        struct Handshake
        {
            uint8_t publicKey[CryptoKeySize];               ///< Public key of the sending pair
            uint8_t challenge[ChallengeSize];               ///< Randomly generated challenge for the client to solve
        };

      private:
        /**
         * @brief Reversibly scrambles the data in blocks of 8 bytes.
         * @param data The data to be scrambled.
         */
        void Scramble(uint8_t data[ChallengeSize]) const;   ///< Reversible mixing function for handshake challenge

      protected:
        uint8_t handshakeOut[sizeof(Handshake)];            ///< Handshake packet to be sent
        uint8_t handshakeIn[sizeof(Handshake)];             ///< Received handshake packet
        uint8_t handshakeCheck[ChallengeSize];              ///< Expected result by the server

        /* SERVER-SIDE Operations */

        /**
         * @brief Prepares a handshake packet for a client.
         * @param serverPublicKey The public key of the server.
         */
        void PrepareHandshakeForClient(const uint8_t serverPublicKey[CryptoKeySize]);

        /**
         * @brief Checks if the client's handshake is valid.
         * @param clientPublicKey The public key of the client.
         * @return True if the client's handshake is valid, false otherwise.
         */
        bool IsClientHandshakeValid(uint8_t clientPublicKey[CryptoKeySize]) const;

        /* CLIENT-SIDE Operations */

        /**
         * @brief Resolves the server's handshake.
         * @param serverPublicKey The public key of the server.
         * @param clientPublicKey The public key of the client.
         */
        void ResolveServerHandshake(uint8_t serverPublicKey[CryptoKeySize], const uint8_t clientPublicKey[CryptoKeySize]);
    };

    /**
     * @brief A utility class for generating pairs of random keys
     * for use with CryptoHandler to generate encryption/decryption keys.
     * (asymmetric encryption)
     */
    class KeyPair
    {
      private:
        uint8_t publicKey[CryptoKeySize];
        uint8_t privateKey[CryptoKeySize];

      public:
        KeyPair() { crypto_kx_keypair(publicKey, privateKey); }
        inline const uint8_t* Public() const noexcept { return publicKey; }
        inline const uint8_t* Private() const noexcept { return privateKey; }
    };

    /**
     * @brief Used by Network::Connection for asymmetric encryption of communications.
     */
    class CryptoHandler
    {
      private:
        uint8_t decryptKey[CryptoKeySize];
        uint8_t encryptKey[CryptoKeySize];

      public:
        CryptoHandler() : decryptKey{}, encryptKey{} { }

        /**
         * @brief Constructor for CryptoHandler.
         * @param ownKeys The key pair of the local entity.
         * @param targetPublicKey The public key of the target entity.
         * @param isServer Flag indicating if the local entity is a server.
         */
        CryptoHandler(const KeyPair& ownKeys, const uint8_t* targetPublicKey, bool isServer)
        {
            auto genKeys = (isServer ? crypto_kx_server_session_keys : crypto_kx_client_session_keys);
            if (genKeys(decryptKey, encryptKey, ownKeys.Public(), ownKeys.Private(), targetPublicKey) != 0)
            {
                throw std::runtime_error("Suspicious target public key"); // TODO: Handled this type of case
            }
        }

        /**
         * @brief Encrypts the packet using asymmetric encryption.
         * @tparam T_PacketID Type of packet identifier.
         * @param packet The packet to be encrypted.
         * @return True if encryption is successful, false otherwise.
         */
        template <typename T_PacketID>
        bool Encrypt(Packet<T_PacketID>& packet) const
        {
            // If the packet does not contain a body, there is no need to encrypt it
            if (packet.header.size == 0) return true;

            // Generate a unique nonce for each packet
            randombytes_buf(packet.header.nonce, crypto_secretbox_NONCEBYTES);

            // Encrypt the packet body (returns false if failure)
            uint8_t encrypted[packet.body.size() + crypto_secretbox_MACBYTES];
            if (crypto_secretbox_easy(encrypted, packet.body.data(), packet.body.size(), packet.header.nonce, encryptKey) != 0)
            {
                return false;
            }

            // Replaced the body of the packet with its encrypted version
            packet.replace(encrypted, sizeof(encrypted));

            return true;
        }

        /**
         * @brief Decrypts the packet using asymmetric encryption.
         * @tparam T_PacketID Type of packet identifier.
         * @param packet The packet to be decrypted.
         * @return True if decryption is successful, false otherwise.
         */
        template <typename T_PacketID>
        bool Decrypt(Packet<T_PacketID>& packet) const
        {
            // If the packet does not contain a body, no need to decrypt
            if (packet.header.size == 0) return true;

            // Decrypt the body of the packet (returns false if unsuccessful)
            uint8_t decrypted[packet.body.size() - crypto_secretbox_MACBYTES];
            if (crypto_secretbox_open_easy(decrypted, packet.body.data(), packet.body.size(), packet.header.nonce, decryptKey) != 0)
            {
                return false;
            }

            // Replace the body of the packet with its decrypted version
            packet.replace(decrypted, sizeof(decrypted));

            // Fill the nonce with zeros (so we can recognize if it is encrypted or not)
            std::fill(packet.header.nonce, packet.header.nonce + crypto_secretbox_NONCEBYTES, 0);

            return true;
        }
    };

    /**
     * @brief Manages encryption and decryption of packets for Network::Connection.
     */
    class EncryptionHandler
    {
    private:
        uint8_t key[crypto_hash_sha256_BYTES];  // Encryption/decryption key

    public:
        /**
         * @brief Constructor for EncryptionHandler.
         * @param password The password used to derive the encryption key.
         */
        EncryptionHandler(const std::string& password)
        {
            if (crypto_hash_sha256(key, reinterpret_cast<const uint8_t*>(password.data()), password.size()) != 0)
            {
                throw std::runtime_error("SHA-256 hashing failed");
            }
        }

        /**
         * @brief Checks if the packet is encrypted.
         * @tparam T_PacketID Type of packet identifier.
         * @param packet The packet to check for encryption.
         * @return True if the packet is encrypted, false otherwise.
         */
        template <typename T_PacketID>
        bool IsEncrypted(const Packet<T_PacketID>& packet)
        {
            auto nonce = reinterpret_cast<const uint64_t*>(packet.header.nonce);
            return (nonce[0] | nonce[1] | nonce[2]) != 0;
        }

        /**
         * @brief Encrypts the packet using symmetric encryption.
         * @tparam T_PacketID Type of packet identifier.
         * @param packet The packet to be encrypted.
         * @return True if encryption is successful, false otherwise.
         */
        template <typename T_PacketID>
        bool Encrypt(Packet<T_PacketID>& packet)
        {
            // If the packet does not contain a body or is already encrypted
            if (packet.header.size == 0 || IsEncrypted(packet))
            {
                return true;
            }

            // Generate a unique nonce for each packet
            randombytes_buf(packet.header.nonce, crypto_secretbox_NONCEBYTES);

            // Encrypt the body of the packet (returns false if unsuccessful)
            uint8_t encrypted[packet.body.size() + crypto_secretbox_MACBYTES];
            if (crypto_secretbox_easy(encrypted, packet.body.data(), packet.body.size(), packet.header.nonce, key) != 0)
            {
                return false;
            }

            // Replace the body of the packet with its encrypted version
            packet.replace(encrypted, sizeof(encrypted));

            return true;
        }

        /**
         * @brief Decrypts the packet using symmetric encryption.
         * @tparam T_PacketID Type of packet identifier.
         * @param packet The packet to be decrypted.
         * @return True if decryption is successful, false otherwise.
         */
        template <typename T_PacketID>
        bool Decrypt(Packet<T_PacketID>& packet)
        {
            // If the packet does not contain a body or is not encrypted
            if (packet.header.size == 0 || !IsEncrypted(packet))
            {
                return true;
            }

            // Decrypt the body of the packet (returns false if unsuccessful)
            uint8_t decrypted[packet.body.size() - crypto_secretbox_MACBYTES];
            if (crypto_secretbox_open_easy(decrypted, packet.body.data(), packet.body.size(), packet.header.nonce, key) != 0)
            {
                return false;
            }

            // Replace the body of the packet with its decrypted version
            packet.replace(decrypted, sizeof(decrypted));

            // Fill the nonce with zeros (so we can recognize if it is encrypted or not)
            std::fill(packet.header.nonce, packet.header.nonce + crypto_secretbox_NONCEBYTES, 0);

            return true;
        }
    };

}}

#endif