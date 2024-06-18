#include "MariO.h"
#include "UnrotatedBoxCollider.h"

MariO::MariO() : PlayableCharacter("Data/Model/Mari.O/Mari-O.fbx", 1.0f)
{
	collider = std::make_unique<UnrotatedBoxCollider>();
	collider->SetScale({ 75.0f, 150.0f, 75.0f });
	colliderOffset = { 0, 75.0f, 0 };
	SetShader(ModelShaderId::Toon);
}