#ifndef RAYFLEX_CORE_APP_HPP
#define RAYFLEX_CORE_APP_HPP

#include "./rfCursor.hpp"
#include "./rfRenderer.hpp"
#include "./rfSaveManager.hpp"
#include "./rfAssetManager.hpp"
#include "./rfMusicManager.hpp"

#include <RenderTexture.hpp>
#include <AudioDevice.hpp>
#include <Window.hpp>
#include <Shader.hpp>

#include <unordered_map>
#include <utility>
#include <memory>
#include <thread>
#include <atomic>

namespace rf { namespace core {

    class App;

    /**
     * @brief The State class serves as the base class for different states within the application.
     */
    class State
    {
      protected:
        App *app;  ///< Pointer to the App instance associated with this state.

      private:
        friend class App;
        /**
         * @brief Sets the App instance for this state.
         * @param app Pointer to the App instance.
         */
        void SetApp(App* app)
        {
            this->app = app;
        }

      public:
        virtual ~State() = default;

      public:
        /**
         * @brief Called when entering this state.
         */
        virtual void Enter()
        { }

        /**
         * @brief Called when exiting this state.
         */
        virtual void Exit()
        { }

        /**
         * @brief Updates the state logic.
         * @param dt Delta time since the last frame.
         */
        virtual void Update(float dt)
        { }

        /**
         * @brief Draws the state content.
         */
        virtual void Draw(const Renderer& target)
        { }
    };

    /**
     * @brief The LoadingState class, derived from State, is used for performing asynchronous operations
     *        using core::App::Loading().
     */
    class LoadingState : public State
    {
      public:
        virtual ~LoadingState() = default;

      public:
        /**
         * @brief The main task to be executed in a separate thread during the state's execution.
         */
        virtual void Task() = 0;

        /**
         * @brief Optional task executed after the main Task on the main thread.
         */
        virtual void PostTask()
        { }
    };

    /**
     * @brief The App class represents the main application framework.
     */
    class App
    {
      public:
        raylib::Window window;                      ///< Window instance for the application.
        raylib::AudioDevice audio;                  ///< AudioDevice instance for the application.

      public:
        AssetManager assetManager;                  ///< Basic generic asset manager.
        MusicManager musicManager;                  ///< State independent music and playlist manager.
        std::unique_ptr<SaveManager> saveManager;   ///< Basic generic save manager (optionnal).

      private:
        std::unordered_map<std::string, std::unique_ptr<State>> states;                 ///< Map of all states.
        std::pair<const std::string, std::unique_ptr<State>> *nextState = nullptr;      ///< Next state of the App.
        std::pair<const std::string, std::unique_ptr<State>> *currentState = nullptr;   ///< Current state of the App.

      private:
        Cursor cursor;                                      ///< Custom mouse cursor.
        Renderer renderer;                                  ///< Renderer instance for the application.
        Renderer rendererTransition;                        ///< Renderer used for rendering the next state during transition.
        raylib::Shader *shaderMain = nullptr;               ///< Main shader applied to all rendering, including transitions.
        raylib::Shader *shaderTransition = nullptr;         ///< Transition shader for custom transition effects.

      private:
        int locTransitionProgress = 0;                      ///< Uniform location for the progress of the custom transition shader.
        int locTransitionNextTexture = 0;                   ///< Uniform location for the texture of the next state during custom transition.
        float valTransitionProgress = 0.0f;                 ///< Internal value of the transition progress (float from 0 to 1).
        float valTransitionInvDuration = 1.0f;              ///< Inverse of the desired duration in seconds for a custom transition (1/duration).

      private:
        bool running = false;           ///< Flag indicating whether the application is running.
        int retCode = 0;                ///< Return code of the application.

      private:
        void Init(const std::string& title, const Vector2& winSize, const Vector2& targetSize, bool keepAspectRatio, uint32_t flags, bool initAudio);
        void UpdateAndDraw();
        void UpdateAndDrawTransition();

      private:
#     ifdef PLATFORM_WEB
        // emscripten needs a plain old C function for the main loop
        // however a 'global' function would not have had access to private members
        // then this static member function trick seems to be the simplest way
        static void UpdateAndDrawLoopCallback(void* arg)
        {
            auto app = static_cast<core::App*>(arg);
            if (app->nextState == nullptr) app->UpdateAndDraw();
            else app->UpdateAndDrawTransition();
            app->musicManager.Update();
        }
#     endif

      public:
        /**
         * @brief Constructs an App instance with specified parameters.
         * @param title The title of the application window.
         * @param winSize The size of the window.
         * @param targetSize The target size for rendering.
         * @param keepAspectRatio Whether to keep the aspect ratio when resizing the window.
         * @param flags Additional flags for window creation.
         * @param initAudio Whether to initialize the audio subsystem.
         */
        App(const std::string& title, const Vector2& winSize, const Vector2& targetSize, bool keepAspectRatio = true, uint32_t flags = 0, bool initAudio = true);

        /**
         * @brief Constructs an App instance with specified parameters.
         * @param title The title of the application window.
         * @param width The width of the window.
         * @param height The height of the window.
         * @param flags Additional flags for window creation.
         * @param initAudio Whether to initialize the audio subsystem.
         */
        App(const std::string& title, int width, int height, uint32_t flags = 0, bool initAudio = true);

        /**
         * @brief Initializes the SaveManager with the specified parameters.
         * @tparam _Ts The type of the object to be saved.
         * @param origin The object to be saved.
         * @param version The version of the SaveManager (default is 1).
         * @param directory The directory for saving/loading files (default is empty).
         */
        template <typename _Ts>
        void InitSaveManager(const _Ts& origin, int version = 1, const std::string& directory = "")
        {
            saveManager = std::make_unique<SaveManager>(origin, version, directory);
        }

        /**
         * @brief Sets the main shader for rendering.
         * @param shader Pointer to the main shader.
         */
        void SetMainShader(raylib::Shader* shader)
        {
            shaderMain = shader;
        }

        /**
         * @brief Initiates a loading screen and executes a loading task.
         * @tparam _Tls The type of LoadingState to be used.
         */
        template<typename _Tls> void Loading();

        /**
         * @brief Adds a state of type _Ts to the App.
         * @tparam _Ts The type of state to be added.
         * @param name The name of the state.
         */
        template<typename _Ts>
        void AddState(const std::string& name)
        {
            states[name] = std::make_unique<_Ts>();
            states.at(name)->SetApp(this);
        }

        /**
         * @brief Sets the current state of the App.
         * @param stateName The name of the state to set.
         */
        void SetState(const std::string& stateName);

        /**
         * @brief Initiates a transition to a new state.
         * @param stateName The name of the state to transition to.
         * @param duration The duration of the transition in seconds (default is 1.0f).
         * @param shader The shader to use for the transition (default is nullptr).
         */
        void Transition(const std::string& stateName, float duration = 1.0f, raylib::Shader* shader = nullptr);

        /**
         * @brief Runs the application with the specified initial state.
         * @param firstState The name of the first state to run.
         * @param targetFPS The target frames per second for the application.
         * @return The return code of the application.
         */
        int Run(const std::string& firstState, uint32_t targetFPS = 60);

        /**
         * @brief Finishes the application with the specified return code.
         * @param ret The return code to set (default is 0).
         */
        void Finish(int ret = 0) { running = false, retCode = ret; }

        /**
         * @brief Checks if the application is currently in transition between two states.
         * @return true if in transition, false otherwise.
         */
        bool OnTransition() const
        {
            return nextState != nullptr;
        }

        /**
         * @brief Retrieves the core::Renderer currently in use for the given state.
         * @param state The state for which to retrieve the Renderer.
         * @return A reference to the Renderer in use.
         */
        const Renderer& GetRenderer(const State* state) const;

        /**
         * @brief Gets the current mouse position.
         * @return The current mouse position as a raylib::Vector2.
         */
        raylib::Vector2 GetMousePosition() const;

        /**
         * @brief Checks whether the aspect ratio is being kept when resizing the window.
         * @return True if the aspect ratio is being kept, false otherwise.
         */
        bool IsAspectRatioKept() const;

        /**
         * @brief Retrieves the aspect ratio of the render targets.
         * @return The aspect ratio of the render targets.
         */
        float GetAspectRatio() const;

        /**
         * @brief Gets the current resolution of the render targets.
         * @return The resolution as a raylib::Vector2.
         */
        raylib::Vector2 GetResolution() const;

        /**
         * @brief Sets whether to keep the aspect ratio when resizing the window.
         * @param enabled True to keep the aspect ratio, false otherwise.
         */
        void KeepAspectRatio(bool enabled);

        /**
         * @brief Sets the resolution of the render targets.
         * @param size The new resolution as a raylib::Vector2.
         * @param fitWindowSize Whether to fit the window size to the new resolution (default is false).
         */
        void SetResolution(const Vector2& size, bool fitWindowSize = false);

        /**
         * @brief Sets a custom mouse cursor.
         * @param texture A pointer to the raylib::Texture representing the custom cursor.
         * @param origin The origin point of the cursor texture (default is {0, 0}).
         * @param scale The scale factor applied to the cursor texture (default is 1.0f).
         * @param source The source rectangle defining the portion of the texture to use (default is the entire texture).
         * @param color The color tint applied to the cursor texture (default is WHITE).
         *
         * If the texture pointer is nullptr, the default system cursor is shown; otherwise, the system cursor is hidden.
         * The cursor properties, such as origin, scale, source, and color, are set accordingly.
         */
        void SetCursor(raylib::Texture* texture, const Vector2& origin = {}, float scale = 1.0f, const Rectangle& source = {}, const Color& color = WHITE);

        /**
         * @brief Toggles the fullscreen mode by updating rendering targets.
         *
         * This function activates or deactivates the fullscreen mode for the application window.
         * When fullscreen mode is toggled, rendering targets are updated accordingly.
         */
        void ToggleFullscreen();

        /**
         * @brief Toggles borderless windowed fullscreen mode by updating rendering targets.
         *
         * This function activates or deactivates the borderless windowed fullscreen mode for the application window.
         * When borderless fullscreen mode is toggled, rendering targets are updated accordingly.
         */
        void ToggleBorderless();
    };

    template<typename _Tls>
    void App::Loading()
    {
    #ifdef PLATFORM_WEB
        // It doesn't matter what options when linking or where we call this method (before or during the main loop)
        // the thread seems to never launch with emscripten (firefox/chromium). To see again in the future...

        LoadingState *loadingState = new _Tls();
        loadingState->SetApp(this);
        loadingState->Enter();

        loadingState->Task();
        loadingState->PostTask();

        loadingState->Exit();
        delete loadingState;
    #else
        LoadingState *loadingState = new _Tls();
        loadingState->SetApp(this);
        loadingState->Enter();

        bool doPostTask = true;
        float alphaTrans = EPSILON;

        std::atomic<bool> onLoading = true;
        std::thread taskThread([&loadingState, &onLoading]() {
            loadingState->Task(); onLoading = false;
        });

        while (alphaTrans > 0.0f)
        {
            float dt = GetFrameTime();

            musicManager.Update();

            if (!onLoading && doPostTask)
            {
                loadingState->PostTask();
                doPostTask = false;
            }

            if (onLoading && alphaTrans < 1.0f) alphaTrans = std::min(alphaTrans + 4.0f * dt, 1.0f);
            else if (!onLoading) alphaTrans = std::max(alphaTrans - 4.0f * dt, 0.0f);

            loadingState->Update(dt);

            rendererTransition.BeginMode();
                loadingState->Draw(rendererTransition);
            rendererTransition.EndMode();

            window.BeginDrawing().ClearBackground();
                renderer.Draw(); // We redraw the previous state behind the loading screen
                rendererTransition.Draw({ 255, 255, 255, static_cast<uint8_t>(255 * alphaTrans) });
                if (cursor.IsActive()) cursor.Draw(::GetMousePosition());
            window.EndDrawing();
        }

        if (taskThread.joinable())
        {
            taskThread.join();
        }

        loadingState->Exit();
        delete loadingState;
    #endif
    }

}}

#endif //RAYFLEX_CORE_APP_HPP
