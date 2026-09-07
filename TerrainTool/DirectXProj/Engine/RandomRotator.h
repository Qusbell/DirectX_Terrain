#pragma once
#include "Component.h"

class RandomRotator : public Component
{
public:
    RandomRotator(GameObject* owner);
    ~RandomRotator() override = default;

    void Update() override;
};
