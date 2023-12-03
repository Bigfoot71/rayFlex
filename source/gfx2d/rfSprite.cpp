#include "gfx2d/rfSprite.hpp"
#include <memory>

using namespace rf;

// LOADING METHODS //

void gfx2d::Sprite::Load(const std::string& imPath, int cols, int rows, float speed)
{
    Load(imPath, cols, rows, { 0, 0, static_cast<float>(texture->width), static_cast<float>(texture->height) }, speed);
}

void gfx2d::Sprite::Load(const std::string& imPath, int cols, int rows, Rectangle texSource, float speed)
{
    unloadTexture = true;

    Image tmpImage = LoadImage(imPath.c_str());

    if (texSource.x != 0
     || texSource.y != 0
     || texSource.width != tmpImage.width
     || texSource.height != tmpImage.height)
    {
        ImageCrop(&tmpImage, texSource);
        texSource.x = 0, texSource.y = 0;
    }

    texture = new raylib::Texture2D(tmpImage);
    UnloadImage(tmpImage);

    Load(texture, cols, rows, texSource, speed);
}

void gfx2d::Sprite::Load(Image image, int cols, int rows, Rectangle texSource, float speed)
{
    bool imageCroped = false;

    if (texSource.x != 0
        || texSource.y != 0
        || texSource.width != image.width
        || texSource.height != image.height)
    {
        imageCroped = true;
        image = ImageCopy(image);
        ImageCrop(&image, texSource);
        texSource.x = 0, texSource.y = 0;
    }

    texture = new raylib::Texture2D(image);
    if (imageCroped) UnloadImage(image);

    Load(texture, cols, rows, texSource, speed);
}

void gfx2d::Sprite::Load(raylib::Texture2D* texture, int cols, int rows, const Rectangle& texSource, float speed)
{
    this->texture = texture;
    this->texSource = texSource;
    this->cols = cols, this->rows = rows;
    frameSize = raylib::Vector2(texSource.width / cols, texSource.height / rows);
    frameCenter = frameSize * 0.5f, frameNum = cols * rows;

    NewAnimation("main", 0, frameNum - 1, speed, true);
    NewInstance("main");
}


// ANIMATION MANAGEMENT //

gfx2d::Sprite::Animation* gfx2d::Sprite::NewAnimation(const std::string& keyAnimation, uint16_t startFrame, uint16_t endFrame, float speed, bool loop)
{
    return (animations[keyAnimation] = std::make_unique<Animation>(
        speed, static_cast<uint16_t>((endFrame - startFrame) + 1),
        startFrame, endFrame, loop)).get();
}

void gfx2d::Sprite::SetAnimation(const std::string& keyAnimation, const std::string& keyInstance)
{
#ifdef DEBUG
    if (animations.find(keyAnimation) != animations.end())
    {
        Instance* instance = GetInstance(keyInstance);
        instance->animation = GetAnimation(keyAnimation);
        instance->currentFrame = instance->animTime = 0;
    }
    else
    {
        TraceLog(LOG_ERROR, "Animation key [%s] not found", keyAnimation.c_str());
    }
#else
    Instance* instance = GetInstance(keyInstance);
    instance->animation = GetAnimation(keyAnimation);
    instance->currentFrame = instance->animTime = 0;
#endif
}

gfx2d::Sprite::Animation* gfx2d::Sprite::GetAnimation(const std::string& keyAnimation)
{
#ifdef DEBUG
    const auto itAnimation = animations.find(keyAnimation);

    if (itAnimation == animations.end())
    {
        TraceLog(LOG_ERROR, "Animation key [%s] not found", keyAnimation.c_str());
        return animations.find("main")->second.get();
    }

    return itAnimation->second.get();
#else
    return animations[keyAnimation].get();
#endif
}

const gfx2d::Sprite::Animation* gfx2d::Sprite::GetAnimation(const std::string& keyAnimation) const
{
#ifdef DEBUG
    const auto itAnimation = animations.find(keyAnimation);

    if (itAnimation == animations.end())
    {
        TraceLog(LOG_ERROR, "Animation key [%s] not found", keyAnimation.c_str());
        return animations.find("main")->second.get();
    }

    return itAnimation->second.get();
#else
    return animations.find(keyAnimation)->second.get();
#endif
}

Rectangle gfx2d::Sprite::GetAnimationFrameRec(uint16_t frameIndex, const Sprite::Animation * const animation) const
{
    frameIndex %= animation->count;

    return {
        texSource.x + ((animation->start + frameIndex) % cols * frameSize.x),
        texSource.y + ((animation->start + frameIndex) / cols * frameSize.y),
        frameSize.x, frameSize.y
    };
}

Rectangle gfx2d::Sprite::GetAnimationFrameRec(uint16_t frameIndex, const std::string& keyAnimation) const
{
    return GetAnimationFrameRec(frameIndex, GetAnimation(keyAnimation));
}

void gfx2d::Sprite::SetAnimationSpeed(float time, const std::string& keyAnimation)
{
    GetAnimation(keyAnimation)->speed = time;
}

void gfx2d::Sprite::SetAnimationLoop(bool loop, const std::string& keyAnimation)
{
    GetAnimation(keyAnimation)->loop = loop;
}

gfx2d::Sprite::MapAnimations::const_iterator gfx2d::Sprite::GetBeginAnimations() const
{
    return animations.cbegin();
}

gfx2d::Sprite::MapAnimations::const_iterator gfx2d::Sprite::GetEndAnimations() const
{
    return animations.cend();
}


// INSTANCE MANAGEMENT //

gfx2d::Sprite::Instance* gfx2d::Sprite::NewInstance(const std::string& keyInstance, Animation* animation)
{
    return (instances[keyInstance] = std::make_unique<Instance>(Rectangle{
        texSource.x, texSource.y, frameSize.x, frameSize.y
        }, animation, 0.0f, 0)).get();
}

void gfx2d::Sprite::RemoveInstance(const std::string& keyInstance)
{
#ifdef DEBUG
    if (keyInstance == "main")
    {
        TraceLog(LOG_WARNING, "Attempt to delete instance [\"main\"]. Attempt cancelled.");
        return;
    }

    auto itInstance = instances.find(keyInstance);

    if (itInstance != instances.end())
    {
        itInstance = instances.erase(itInstance);
        TraceLog(LOG_INFO, "Instance [\"%s\"] deleted successfully.", keyInstance.c_str());
    }
    else
    {
        TraceLog(LOG_WARNING, "Attempt to delete instance [\"main\"] which does not exist. Attempt cancelled.");
    }
#else
    auto itInstance = instances.find(keyInstance);
    itInstance = instances.erase(itInstance);
#endif
}

void gfx2d::Sprite::ClearInstances()
{
    auto itInstance = instances.begin();

    while (itInstance != instances.end())
    {
        if (itInstance->first != "main")
        {
            itInstance = instances.erase(itInstance);
        }
        else
        {
            ++itInstance;
        }
    }
}

gfx2d::Sprite::Instance* gfx2d::Sprite::GetInstance(const std::string& keyInstance)
{
#ifdef DEBUG
    const auto itInstance = instances.find(keyInstance);

    if (itInstance == instances.end())
    {
        TraceLog(LOG_ERROR, "Instance key [%s] not found", keyInstance.c_str());
        return instances.find("main")->second.get();
    }

    return itInstance->second.get();
#else
    return instances[keyInstance].get();
#endif
}

const gfx2d::Sprite::Instance* gfx2d::Sprite::GetInstance(const std::string& keyInstance) const
{
#ifdef DEBUG
    const auto itInstance = instances.find(keyInstance);

    if (itInstance == instances.end())
    {
        TraceLog(LOG_ERROR, "Instance key [%s] not found", keyInstance.c_str());
        return instances.find("main")->second.get();
    }

    return itInstance->second.get();
#else
    return instances.find(keyInstance)->second.get();
#endif
}

void gfx2d::Sprite::GotoFrame(uint16_t position, const std::string& keyInstance)
{
    Instance* instance = GetInstance(keyInstance);
    Animation* animation = instance->animation;

    instance->animTime = 0;
    instance->currentFrame = position % animation->count;
    instance->frameRec = GetAnimationFrameRec(instance->currentFrame, animation);
}

bool gfx2d::Sprite::IsCurrentFrameEqual(uint16_t position, const std::string& keyInstance) const
{
    return GetInstance(keyInstance)->currentFrame == position;
}

bool gfx2d::Sprite::IsCurrentFrameBefore(uint16_t position, const std::string& keyInstance) const
{
    return GetInstance(keyInstance)->currentFrame < position;
}

bool gfx2d::Sprite::IsCurrentFrameAfter(uint16_t position, const std::string& keyInstance) const
{
    return GetInstance(keyInstance)->currentFrame > position;
}

bool gfx2d::Sprite::IsAnimFinished(const std::string& keyInstance) const
{
    const Instance* instance = GetInstance(keyInstance);
    const Animation* animation = instance->animation;
    const bool isLastFrame = (instance->currentFrame == animation->count - 1);
    const bool isTimeExceeded = (instance->animTime >= animation->speed);
    return (isLastFrame && isTimeExceeded);
}

gfx2d::Sprite::MapInstances::const_iterator gfx2d::Sprite::GetBeginInstances() const
{
    return instances.cbegin();
}

gfx2d::Sprite::MapInstances::const_iterator gfx2d::Sprite::GetEndInstances() const
{
    return instances.cend();
}


// INSTANCE - UPDATE AND DRAW //

void gfx2d::Sprite::Update(float dt, Instance* instance)
{
    Animation* animation = instance->animation;

    const int8_t direction = (dt > 0) ? 1 : -1; // Direction de l'animation (1 pour l'avant, -1 pour l'arrière)

    if (animation->loop
        || (direction > 0 && instance->currentFrame < animation->count - 1)
        || (direction < 0 && instance->currentFrame > 0)                 )
    {
        if ((instance->animTime += dt) * direction >= animation->speed) // Animation (jusqu'à la première ou la dernière frame selon la direction)
        {
            instance->currentFrame = (instance->currentFrame + direction + animation->count) % animation->count;
            instance->animTime = 0;
        }
    }
    else if (instance->animTime < animation->speed) // Première ou dernière (selon la direction) frame atteinte en non-loop, incrémentation du timer jusqu'à sa fin (pour l'utilisation de `IsAnimFinished`)
    {
        instance->animTime += dt;
    }

    instance->frameRec.x = texSource.x + ((animation->start + instance->currentFrame) % cols * frameSize.x);
    instance->frameRec.y = texSource.y + ((animation->start + instance->currentFrame) / cols * frameSize.y);
}

void gfx2d::Sprite::UpdateAll(float dt)
{
    for (auto & itInstance : instances)
    {
        Update(dt, itInstance.first);
    }
}

void gfx2d::Sprite::Draw(Vector2 pos, Color color, const Instance * const instance) const
{
    texture->Draw(instance->frameRec, { pos.x, pos.y, frameSize.x, frameSize.y }, frameCenter, 0.0f, color);
}

void gfx2d::Sprite::Draw(Vector2 pos, float scale, float rotation, Vector2 uvOrigin, Color color, const Instance * const instance) const
{
    Rectangle srcFrameRect = instance->frameRec;
    const raylib::Vector2 scaledSize = frameSize * std::fabs(scale);

    if (scale < 0)
    {
        srcFrameRect.x += srcFrameRect.width, srcFrameRect.width *= -1;
        srcFrameRect.y += srcFrameRect.height, srcFrameRect.height *= -1;
    }

    texture->Draw(srcFrameRect, {
        pos.x, pos.y, scaledSize.x, scaledSize.y
    }, scaledSize * uvOrigin, rotation, color);
}

void gfx2d::Sprite::Draw(Vector2 pos, float sx, float sy, float rotation, Vector2 uvOrigin, Color color, const Instance * const instance) const
{
    Rectangle srcFrameRect = instance->frameRec;

    const raylib::Vector2 scaledSize(
        frameSize.x * std::fabs(sx),
        frameSize.y * std::fabs(sy));

    if (sx < 0) srcFrameRect.width *= -1;
    if (sy < 0) srcFrameRect.height *= -1;

    texture->Draw(srcFrameRect, {
        pos.x, pos.y, scaledSize.x, scaledSize.y
    }, scaledSize * uvOrigin, rotation, color);
}

void gfx2d::Sprite::Draw(Rectangle dest, Vector2 origin, float rot, Color color, const Instance * const instance) const
{
    texture->Draw(instance->frameRec, dest, origin, rot, color);
}
