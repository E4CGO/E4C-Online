#pragma once

#include "TAKOEngine/Rendering/ModelDX12.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/AI/StateMachine.h"

#include <unordered_set>
// �e�X�g�p

#include "GameObject/ModelObject.h"
#include "Scene/Scene.h"

class SceneTitle : public Scene
{
public:
	SceneTitle() = default;
	~SceneTitle() = default;

	// ������
	void Initialize() override;
	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;
	// �`�揈��
	void Render() override;

	void RenderDX12() override;

	StateMachine<SceneTitle>* GetStateMachine() { return stateMachine.get(); }
private:
	// �V�[��GUI�`��
	void DrawSceneGUI();
public:
	enum STATE
	{
		TITLE,
		LOGIN,
		INPUT,
		SETTING,
		LOGIN_CHECK,
	};
private:
	std::unique_ptr<StateMachine<SceneTitle>> stateMachine;

	std::unique_ptr<myRenderer::shadow::ShadowMapRender> shadowMapRenderer = std::make_unique<myRenderer::shadow::ShadowMapRender>();

	Camera& camera = Camera::Instance();
	std::unique_ptr<FreeCameraController> cameraController;

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// �}�X�N
		// Setting UI
		"Data/Sprites/bar_ready.png",
		"Data/Sprites/button_frame.png",
		"Data/Sprites/button_ready_on.png",
		"Data/Sprites/button2_ready_on.png",
		"Data/Sprites/button_agree.png",
		"Data/Sprites/button3_ready.png",
		"Data/Sprites/button_cancel.png",
		"Data/Sprites/gear.png",
		"Data/Sprites/barmid_ready.png",
		"Data/Sprites/gem.png",
	};
	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;

	// �w�i�p���f��
	std::unique_ptr<ModelObject> map;
	std::unique_ptr<ModelObject> knight;
	std::unique_ptr<ModelObject> rouge;
	std::unique_ptr<ModelObject> barbarian;

	// �e�X�g�p
	std::unique_ptr<ModelDX12> test;
};
