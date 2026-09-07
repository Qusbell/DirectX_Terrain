#pragma once
#include "Component.h"

// Forward-declare Graphics so we don't have to include Graphics.h
class CoreGraphicsManager;

// An abstract interface for any component that can be rendered.
// This allows the Scene to initialize all renderers without knowing their concrete types.
class IRenderer : public Component
{
public:
    using Component::Component; // Inherit Component's constructor
    virtual ~IRenderer() = default;
};
