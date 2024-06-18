#include "LuiG.h"
#include "UnrotatedBoxCollider.h"

LuiG::LuiG() : PlayableCharacter("Data/Model/Lui.G/Lui-G.fbx", 1.0f)
{
	collider = std::make_unique<UnrotatedBoxCollider>();
	collider->SetScale({ 75.0f, 160.0f, 75.0f });
	colliderOffset = { 0, 80.0f, 0 };
	SetShader(ModelShaderId::Toon);
}