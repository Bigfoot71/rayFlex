#include "core/rfApp.hpp"

#ifdef PLATFORM_WEB
#   include <emscripten/emscripten.h>
#endif

using namespace rf;

/* PRIVATE */

void core::App::Init(const std::string& title, const Vector2& winSize, const Vector2& targetSize, bool keepAspectRatio, uint32_t flags, bool initAudio)
{
    window.Init(winSize.x, winSize.y, title, flags);
    if (initAudio) audio.Init();

    renderer.Load(targetSize, keepAspectRatio);
    rendererTransition.Load(targetSize, keepAspectRatio);
}

void core::App::UpdateAndDraw()
{
    currentState->second->Update(GetFrameTime());

    renderer.BeginMode().Clear();
        currentState->second->Draw();
    renderer.EndMode();

    window.BeginDrawing();
        if (shaderMain) shaderMain->BeginMode();
            renderer.Draw();
        if (shaderMain) shaderMain->EndMode();
    window.EndDrawing();
}

void core::App::UpdateAndDrawTransition()
{
    const float dt = GetFrameTime();
    valTransitionProgress += valTransitionInvDuration * dt;

    // Update states
    currentState->second->Update(dt);
    nextState->second->Update(dt);

    // Render previous state
    renderer.BeginMode().Clear();
        currentState->second->Draw();
    renderer.EndMode();

    // Render next state
    rendererTransition.BeginMode().Clear();
        nextState->second->Draw();
    rendererTransition.EndMode();

    // Re-render both states if main shader is defined
    if (shaderMain != nullptr)
    {
        shaderMain->BeginMode();

            renderer.BeginMode().Clear();
                renderer.Draw();
            renderer.EndMode();

            rendererTransition.BeginMode().Clear();
                rendererTransition.Draw();
            rendererTransition.EndMode();

        shaderMain->EndMode();
    }

    // Transition rendering
    window.BeginDrawing();

        if (shaderTransition != nullptr)
        {
            shaderTransition->BeginMode();

                shaderTransition->SetValue(locTransitionNextTexture, rendererTransition.GetTexture());
                shaderTransition->SetValue(locTransitionProgress, &valTransitionProgress, SHADER_UNIFORM_FLOAT);

                renderer.Draw();

            shaderTransition->EndMode();
        }
        else
        {
            if (valTransitionProgress < 0.5f)
            {
                renderer.Draw();
                DrawRectangle(0, 0, window.GetWidth(), window.GetHeight(), {
                    0, 0, 0, static_cast<uint8_t>(2.f * valTransitionProgress * 255) });
            }
            else
            {
                rendererTransition.Draw();
                DrawRectangle(0, 0, window.GetWidth(), window.GetHeight(), {
                    0, 0, 0, static_cast<uint8_t>((2.f - 2.f * valTransitionProgress) * 255) });
            }
        }

    window.EndDrawing();

    // Handle end transition
    if (valTransitionProgress >= 1.0f)
    {
        valTransitionProgress = 0.0f;
        currentState->second->Exit();
        currentState = std::exchange(nextState, nullptr);
    }
}

/* PUBLIC */

core::App::App(const std::string& title, const Vector2& winSize, const Vector2& targetSize, bool keepAspectRatio, uint32_t flags, bool initAudio) : audio(true)
{
    Init(title, winSize, targetSize, keepAspectRatio, flags, initAudio);
}

core::App::App(const std::string& title, int width, int height, uint32_t flags, bool initAudio) : audio(true)
{
    Init(title, { static_cast<float>(width), static_cast<float>(height) }, { static_cast<float>(width), static_cast<float>(height) }, true, flags, initAudio);
}

void core::App::SetState(const std::string& stateName)
{
    if (stateName != currentState->first)
    {
        currentState->second->Exit();
        currentState = &(*states.find(stateName));
        currentState->second->Enter();
    }
}

void core::App::Transition(const std::string& stateName, float duration, raylib::Shader* shader)
{
    if (nextState != nullptr) return;

    if (shader != nullptr && shader != shaderTransition)
    {
        shaderTransition = shader;
        locTransitionProgress = shader->GetLocation("progress");
        locTransitionNextTexture = shader->locs[SHADER_LOC_VERTEX_TEXCOORD01];
    }

    valTransitionInvDuration = 1.0f / duration;
    nextState = &(*states.find(stateName));
    nextState->second->Enter();
}

int core::App::Run(const std::string& firstState, uint32_t targetFPS)
{
    currentState = &(*states.find(firstState));
    currentState->second->Enter();

    running = true;

#   ifdef PLATFORM_WEB
        emscripten_set_main_loop_arg(
            UpdateAndDrowLoopCallback, this,
            targetFPS, 1);
#   else
        window.SetTargetFPS(targetFPS);
        while (running && !window.ShouldClose())
        {
            if (window.IsResized())
            {
                rendererTransition.Update();
                renderer.Update();
            }
            if (nextState == nullptr) UpdateAndDraw();
            else UpdateAndDrawTransition();
        }
#   endif

    if (currentState != nullptr) currentState->second->Exit();
    if (nextState != nullptr) nextState->second->Exit();

    return retCode;
}

raylib::Vector2 core::App::GetMousePosition() const
{
    return renderer.ScreenToRenderer(::GetMousePosition());
}

bool core::App::IsAspectRatioKept() const
{
    return renderer.IsRatioKept();
}

raylib::Vector2 core::App::GetResolution() const
{
    return renderer.GetSize();
}

void core::App::KeepAspectRatio(bool enabled)
{
    rendererTransition.KeepRatio(enabled);
    renderer.KeepRatio(enabled);
}

void core::App::SetResolution(const Vector2& size, bool fitWindowSize)
{
    rendererTransition.Resize(size);
    renderer.Resize(size);

    if (fitWindowSize)
    {
        window.SetSize(size);
    }
}
