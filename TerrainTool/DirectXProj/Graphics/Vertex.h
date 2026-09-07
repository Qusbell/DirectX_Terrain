#pragma once
#include "SimpleMath.h"

struct Vertex
{
    DirectX::SimpleMath::Vector3 pos;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 uv;
};

struct UIVertex
{
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector2 uv;
};