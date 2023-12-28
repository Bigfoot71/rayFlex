#ifndef RAYFLEX_CORE_MUSIC_MANAGER_HPP
#define RAYFLEX_CORE_MUSIC_MANAGER_HPP

#include <raylib-cpp.hpp>
#include <unordered_map>
#include <memory>
#include <string>

/**
 * @brief A module for managing music in Raylib.
 *
 * TODO: Code written in an hour, very unintuitive, just functional, to be reviewed...
 */
namespace rf { namespace core {

    /**
     * @brief The MusicManager class handles the management of music in Raylib.
     */
    class MusicManager
    {
      private:
        using Playlist = std::vector<std::pair<const std::string, std::unique_ptr<raylib::Music>>*>;

      private:
        std::unordered_map<std::string, std::unique_ptr<raylib::Music>> musics;         ///< Map of music objects by their names. 
        std::unordered_map<std::string, Playlist> playlists;                            ///< Map of playlists containing multiple music tracks.

      private:
        std::pair<const std::string, std::unique_ptr<raylib::Music>> *currentMusic;     ///< Pointer to the current music being played.
        std::pair<const std::string, Playlist> *currentPlaylist;                        ///< Pointer to the current playlist being used.
        int currentMusicIndex;                                                          ///< Index of the current music track in the playlist.

      private:
        float volume, pitch, pan;                                                       ///< Audio properties: volume, pitch, pan.
        bool looping, randomize;                                                        ///< Flags for looping and randomizing music playback.
        bool onPlaying;                                                                 ///< Flag indicating whether music is currently playing.

      public:
        /**
         * @brief Default constructor for the MusicManager class.
         */
        MusicManager()
        : currentMusic(nullptr)
        , currentPlaylist(nullptr)
        , currentMusicIndex(-1)
        , volume(1.0f)
        , pitch(1.0f)
        , pan(0.5f)
        , looping(false)
        , randomize(false)
        , onPlaying(false)
        { }

        /**
         * @brief Loads a music file from the file system.
         *
         * @param name The unique name to associate with the loaded music.
         * @param filePath The path to the music file.
         */
        void Load(const std::string& name, const std::string& filePath)
        {
            musics.emplace(name, std::make_unique<raylib::Music>(filePath));
        }

        /**
         * @brief Loads a music file from raw data.
         *
         * @param name The unique name to associate with the loaded music.
         * @param fileType The type of the music file.
         * @param data The vector containing raw data of the music file.
         */
        void Load(const std::string& name, const std::string& fileType, const std::vector<uint8_t>& data)
        {
            Load(name, fileType, data.data(), data.size());
        }

        /**
         * @brief Loads a music file from raw data.
         *
         * @param name The unique name to associate with the loaded music.
         * @param fileType The type of the music file.
         * @param data The pointer to the raw data of the music file.
         * @param dataSize The size of the raw data.
         */
        void Load(const std::string& name, const std::string& fileType, const uint8_t* data, int dataSize)
        {
            musics.emplace(name, std::make_unique<raylib::Music>(LoadMusicStreamFromMemory(fileType.c_str(), data, dataSize)));
        }

        /**
         * @brief Associates an existing music object with a unique name in the playlist.
         *
         * This function allows associating an existing music object with a unique name in the playlist.
         *
         * @param name The unique name to associate with the loaded music.
         * @param music The existing music struct to be associated with the given name.
         */
        void Load(const std::string& name, const Music& music)
        {
            musics.emplace(name, std::make_unique<raylib::Music>(music));
        }

        /**
         * @brief Creates a new playlist with the given name and a list of music tracks.
         *
         * @param name The unique name to associate with the new playlist.
         * @param list The list of music track names to include in the playlist.
         *
         * @return True if the playlist is successfully created, false otherwise.
         */
        bool NewPlaylist(const std::string& name, const std::initializer_list<std::string>& list)
        {
            if (list.size() == 0) return false;

            // Try to insert the new playlist into the playlists map
            auto it = playlists.emplace(name, Playlist{});
            if (!it.second) return false;

            auto &playlist = it.first->second;
            playlist.reserve(list.size());

            // Populate the playlist with music tracks from the provided list
            for (const auto& music : list)
            {
                auto itMusic = musics.find(music);

                // Check if the music track is loaded
                if (itMusic == musics.end())
                {
                    ::TraceLog(LOG_WARNING, "MusicManager::NewPlaylist() -> Music [%s] was not found in the list of loaded music", music.c_str());
                    continue;
                }

                playlist.push_back(&(*itMusic));
            }

            return true;
        }

        /**
         * @brief Sets the current playlist for playback.
         *
         * @param name The name of the playlist to set. If empty, the current playlist is unset.
         *
         * @return True if the playlist is set successfully, false if the playlist is not found.
         */
        bool SetPlaylist(const std::string& name = "")
        {
            // If no name is provided, unset the current playlist
            if (name.empty())
            {
                currentPlaylist = nullptr;
                currentMusicIndex = -1;
                return true;
            }

            // Try to find the playlist with the given name
            auto it = playlists.find(name);
            if (it == playlists.end()) return false;

            // Set the current playlist to the found playlist
            currentPlaylist = &(*it);
            return true;
        }

        /**
         * @brief Sets whether the playlist should be played in random order.
         *
         * @param value If true, the playlist will be played in random order; if false, it will be played in order.
         */
        void RandomizePlaylist(bool value)
        {
            randomize = value;
        }

        /**
         * @brief Gets the music associated with the specified name from the playlist.
         *
         * This function searches for the music with the given name in the playlist.
         *
         * @param name The name of the music to retrieve from the playlist.
         * @return An iterator pointing to the music in the playlist if found; otherwise, musics.end().
         */
        auto Get(const std::string& name)
        {
            return musics.find(name);
        }

        /**
         * @brief Gets the current volume level.
         *
         * @return The current volume level.
         */
        float GetVolume() const
        {
            return volume;
        }

        /**
         * @brief Gets the current pitch level.
         *
         * @return The current pitch level.
         */
        float GetPitch() const
        {
            return pitch;
        }

        /**
         * @brief Gets the current pan level.
         *
         * @return The current pan level.
         */
        float GetPan() const
        {
            return pan;
        }

        /**
         * @brief Sets the volume level for the currently playing music.
         *
         * @param value The new volume level to set.
         */
        void SetVolume(float value)
        {
            volume = value;

            // If there is currently playing music, update its volume
            if (currentMusic != nullptr)
            {
                currentMusic->second->SetVolume(value);
            }
        }

        /**
         * @brief Sets the pitch level for the currently playing music.
         *
         * @param value The new pitch level to set.
         */
        void SetPitch(float value)
        {
            pitch = value;

            // If there is currently playing music, update its pitch
            if (currentMusic != nullptr)
            {
                currentMusic->second->SetPitch(value);
            }
        }

        /**
         * @brief Sets the pan level for the currently playing music.
         *
         * @param value The new pan level to set.
         */
        void SetPan(float value)
        {
            pan = value;

            // If there is currently playing music, update its pan
            if (currentMusic != nullptr)
            {
                currentMusic->second->SetPan(value);
            }
        }

        /**
         * @brief Sets whether the currently playing music should loop.
         *
         * @param value If true, the currently playing music will loop; if false, it will play once.
         */
        void SetLooping(bool value)
        {
            looping = value;
        }

        /**
         * @brief Checks if the currently playing music is set to loop.
         *
         * @return True if looping is enabled, false otherwise.
         */
        bool IsLooping() const
        {
            return looping;
        }

        /**
         * @brief Checks if music is currently playing.
         *
         * @return True if music is playing, false otherwise.
         */
        bool IsPlaying() const
        {
            return currentMusic != nullptr && onPlaying;
        }

        /**
         * @brief Plays the specified music track or the next track in the current playlist.
         *
         * If no name is provided, and a playlist is set, it will play the next track in the playlist.
         * If a name is provided, it will play the specified music track.
         *
         * @param name The name of the music track to play. If empty, it plays the next track in the playlist.
         *
         * @return True if the music is played successfully, false if the music or playlist is not found.
         */
        bool Play(const std::string& name = "")
        {
            // If no name is provided, and a playlist is set, play the next track in the playlist
            if (name.empty())
            {
                if (currentPlaylist == nullptr) return false;

                // Determine the index of the next track based on whether randomization is enabled
                currentMusicIndex = randomize ? GetRandomValue(0, currentPlaylist->second.size() - 1) : 0;

                // Set the current music to the next track in the playlist
                currentMusic = currentPlaylist->second[currentMusicIndex];
            }
            else
            {
                // If a name is provided, play the specified music track
                auto it = musics.find(name);
                if (it == musics.end()) return false;

                currentMusic = &(*it);

                // Clear current playlist information
                currentPlaylist = nullptr;
                currentMusicIndex = -1;
            }

            // Set audio properties and play the music
            currentMusic->second->SetVolume(volume);
            currentMusic->second->SetPitch(pitch);
            currentMusic->second->SetPan(pan);
            currentMusic->second->Play();
            onPlaying = true;

            return true;
        }

        /**
         * @brief Pauses the currently playing music.
         *
         * @return True if the music is paused successfully, false if no music is playing.
         */
        bool Pause()
        {
            if (currentMusic != nullptr)
            {
                currentMusic->second->Pause();
                onPlaying = false;
                return true;
            }
            return false;
        }

        /**
         * @brief Resumes playback of the paused music.
         *
         * @return True if the music is successfully resumed, false if no music is paused.
         */
        bool Resume()
        {
            if (currentMusic != nullptr)
            {
                currentMusic->second->Resume();
                onPlaying = true;
                return true;
            }
            return false;
        }

        /**
         * @brief Stops the currently playing music.
         *
         * @return True if the music is stopped successfully, false if no music is playing.
         */
        bool Stop()
        {
            if (currentMusic != nullptr)
            {
                currentMusic->second->Stop();
                currentMusicIndex = -1;
                onPlaying = false;
                return true;
            }
            return false;
        }

        /**
         * @brief Rewinds the currently playing music to the beginning.
         *
         * @return True if the music is rewound successfully, false if no music is playing.
         */
        bool Rewind()
        {
            if (currentMusic != nullptr)
            {
                // Stop the music and restart it to simulate rewinding
                currentMusic->second->Stop();
                currentMusic->second->Play();
                return true;
            }
            return false;
        }

        /**
         * @brief Plays the next music track in the current playlist.
         *
         * If the current playlist is set and music is playing, it stops the current track and plays the next one.
         *
         * @return True if the next music track is played successfully, false if no playlist is set.
         */
        bool NextMusic()
        {
            if (currentPlaylist != nullptr)
            {
                // If music is currently playing, stop it
                if (currentMusic != nullptr) currentMusic->second->Stop();

                // Calculate the index of the next music track in the playlist
                currentMusicIndex = (currentMusicIndex + 1) % currentPlaylist->second.size();

                // Set the current music to the next track in the playlist
                currentMusic = currentPlaylist->second[currentMusicIndex];

                return true;
            }
            return false;
        }

        /**
         * @brief Plays the previous music track in the current playlist.
         *
         * If the current playlist is set and music is playing, it stops the current track and plays the previous one.
         *
         * @return True if the previous music track is played successfully, false if no playlist is set.
         */
        bool PreviousMusic()
        {
            if (currentPlaylist != nullptr)
            {
                // If music is currently playing, stop it
                if (currentMusic != nullptr) currentMusic->second->Stop();

                // Calculate the index of the previous music track in the playlist
                currentMusicIndex = (currentMusicIndex + currentPlaylist->second.size() - 1) % currentPlaylist->second.size();

                // Set the current music to the previous track in the playlist
                currentMusic = currentPlaylist->second[currentMusicIndex];

                return true;
            }
            return false;
        }

        /**
         * @brief Plays a random music track in the current playlist.
         *
         * If the current playlist is set, it stops the current track and plays a randomly chosen track from the playlist.
         *
         * @return True if a random music track is played successfully, false if no playlist is set or only one track is in the playlist.
         */
        bool RandomMusic()
        {
            if (currentPlaylist == nullptr || currentPlaylist->second.size() < 2) return false;

            // If music is currently playing, stop it
            if (currentMusic != nullptr)
            {
                currentMusic->second->Stop();
            }

            // If there is only one track in the playlist, set it as the current track
            if (currentPlaylist->second.size() == 1)
            {
                currentMusic = currentPlaylist->second[0];
                currentMusicIndex = 0;
            }

            int nextIndex = -1;

            // Choose a random index for the next track, ensuring it is different from the current index
            do nextIndex = GetRandomValue(0, currentPlaylist->second.size()-1);
            while (nextIndex == currentMusicIndex);

            // Set the current music to the randomly chosen track
            currentMusic = currentPlaylist->second[nextIndex];
            currentMusicIndex = nextIndex;

            return true;
        }

        /**
         * @brief Gets the name of the currently playing music track.
         *
         * @return The name of the currently playing music track, or an empty string if no music is playing.
         */
        std::string CurrentMusic() const
        {
            return IsPlaying() ? currentMusic->first : "";
        }

        /**
         * @brief Gets the name of the currently active playlist.
         *
         * @return The name of the currently active playlist, or an empty string if no music is playing.
         */
        std::string CurrentPlaylist() const
        {
            return IsPlaying() ? currentPlaylist->first : "";
        }

        /**
         * @brief Updates the MusicManager, checking for track completion and handling playback accordingly.
         *
         * If a track is completed and looping is enabled, the track is rewound. If a playlist is active,
         * and randomization is enabled, a random track is played next. Otherwise, the next track in the
         * playlist is played. If none of these conditions are met, playback stops.
         */
        void Update()
        {
            if (currentMusic != nullptr)
            {
                currentMusic->second->Update();

                // Check if the current track has completed
                if (currentMusic->second->GetTimePlayed() >= currentMusic->second->GetTimeLength())
                {
                    // Handle track completion based on looping, randomization, and playlist settings
                    if (looping)
                    {
                        Rewind();
                    }
                    else if (currentPlaylist != nullptr)
                    {
                        if (randomize) RandomMusic();
                        else NextMusic();
                    }
                    else
                    {
                        Stop();
                    }
                }
            }
        }
    };

}}

#endif //RAYFLEX_CORE_MUSIC_MANAGER_HPP
