cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0.22621.0 CACHE STRING "" FORCE)

project(E4COnline CXX)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:.932")

# ###############################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
# ###############################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()

message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

# ###############################################################################
# Global configuration types
# ###############################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

# ###############################################################################
# Global compiler options
# ###############################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

# ###############################################################################
# Global linker options
# ###############################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

# ###############################################################################
# Nuget packages function stub.
# ###############################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
        "Package \"${PACKAGE}\" with version \"${VERSION}\" "
        "for target \"${TARGET}\" is ignored!")
endfunction()

# ###############################################################################
# Common utils
# ###############################################################################
include(CMake/Utils.cmake)

# ###############################################################################
# Additional Global Settings(add specific info there)
# ###############################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

# ###############################################################################
# Use solution folders feature
# ###############################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ###############################################################################
# Sub-projects
# ###############################################################################

# ###############################################################################
# Target
# ###############################################################################
set(PROJECT_NAME Game)

set(External__DirectXTex
    "External/DirectX/WICTextureLoader12.cpp"
    "External/DirectX/WICTextureLoader12.h"
)
source_group("External\\DirectXTex" FILES ${External__DirectXTex})

set(External__imgui-docking
    "External/imgui-docking/imconfig.h"
    "External/imgui-docking/imgTimer.cpp"
    "External/imgui-docking/imgTimer.h"
    "External/imgui-docking/imgui.cpp"
    "External/imgui-docking/imgui.h"
    "External/imgui-docking/imgui_demo.cpp"
    "External/imgui-docking/imgui_draw.cpp"
    "External/imgui-docking/imgui_impl_dx11.cpp"
    "External/imgui-docking/imgui_impl_dx11.h"
    "External/imgui-docking/imgui_impl_dx12.cpp"
    "External/imgui-docking/imgui_impl_dx12.h"
    "External/imgui-docking/imgui_impl_win32.cpp"
    "External/imgui-docking/imgui_impl_win32.h"
    "External/imgui-docking/imgui_internal.h"
    "External/imgui-docking/imgui_tables.cpp"
    "External/imgui-docking/imgui_widgets.cpp"
    "External/imgui-docking/imstb_rectpack.h"
    "External/imgui-docking/imstb_textedit.h"
    "External/imgui-docking/imstb_truetype.h"
    "External/imgui-docking/profiler.cpp"
    "External/imgui-docking/profiler.h"
)
source_group("External\\imgui-docking" FILES ${External__imgui-docking})

#####################################################################
# SOURCE_GROUPS_START
#####################################################################

set(Source
    "Source/Framework.cpp"
    "Source/Framework.h"
    "Source/GameData.cpp"
    "Source/GameData.h"
    "Source/Main.cpp"
    "Source/Manager.h"
    "Source/PlayerCharacterData.cpp"
    "Source/PlayerCharacterData.h"
    "Source/PlayerCharacterPattern.cpp"
    "Source/PlayerCharacterPattern.h"
)
source_group("Source" FILES ${Source})

set(Source__GameObject
    "Source/GameObject/GameObject.cpp"
    "Source/GameObject/GameObject.h"
    "Source/GameObject/ModelObject.cpp"
    "Source/GameObject/ModelObject.h"
    "Source/GameObject/ObjectManager.h"
)
source_group("Source\\GameObject" FILES ${Source__GameObject})

set(Source__Map
    "Source/Map/CrossRoom1.cpp"
    "Source/Map/CrossRoom1.h"
    "Source/Map/DeadEndRoom.cpp"
    "Source/Map/DeadEndRoom.h"
    "Source/Map/DungeonData.cpp"
    "Source/Map/DungeonData.h"
    "Source/Map/EndRoom1.cpp"
    "Source/Map/EndRoom1.h"
    "Source/Map/Map001.cpp"
    "Source/Map/Map001.h"
    "Source/Map/Map001BossRoomTeleport.cpp"
    "Source/Map/Map001BossRoomTeleport.h"
    "Source/Map/Map001Key.cpp"
    "Source/Map/Map001Key.h"
    "Source/Map/MapTile.cpp"
    "Source/Map/MapTile.h"
    "Source/Map/MapTileManager.cpp"
    "Source/Map/MapTileManager.h"
    "Source/Map/Passage1.cpp"
    "Source/Map/Passage1.h"
    "Source/Map/RoomBase.cpp"
    "Source/Map/RoomBase.h"
    "Source/Map/SimpleRoom1.cpp"
    "Source/Map/SimpleRoom1.h"
)
source_group("Source\\Map" FILES ${Source__Map})

set(Source__Network
    "Source/Network/OnlineController.cpp"
    "Source/Network/OnlineController.h"
)
source_group("Source\\Network" FILES ${Source__Network})

set(Source__Scene
    "Source/Scene/Scene.cpp"
    "Source/Scene/Scene.h"
    "Source/Scene/SceneDungeon.cpp"
    "Source/Scene/SceneDungeon.h"
    "Source/Scene/SceneGame.cpp"
    "Source/Scene/SceneGame.h"
    "Source/Scene/SceneGameState.cpp"
    "Source/Scene/SceneGameState.h"
    "Source/Scene/SceneLoading.cpp"
    "Source/Scene/SceneManager.cpp"
    "Source/Scene/SceneManager.h"
    "Source/Scene/SceneTest.cpp"
    "Source/Scene/SceneTest.h"
    "Source/Scene/SceneTitle.cpp"
    "Source/Scene/SceneTitle.h"
    "Source/Scene/SceneTitleState.cpp"
    "Source/Scene/SceneTitleState.h"
)
source_group("Source\\Scene" FILES ${Source__Scene})

set(Source__GameObject__Character
    "Source/GameObject/Character/Character.cpp"
    "Source/GameObject/Character/Character.h"
)
source_group("Source\\GameObject\\Character" FILES ${Source__GameObject__Character})

set(Source__GameObject__Projectile
    "Source/GameObject/Projectile/Arrow.cpp"
    "Source/GameObject/Projectile/Arrow.h"
    "Source/GameObject/Projectile/Grenade.cpp"
    "Source/GameObject/Projectile/Grenade.h"
    "Source/GameObject/Projectile/Projectile.cpp"
    "Source/GameObject/Projectile/Projectile.h"
    "Source/GameObject/Projectile/ProjectileManager.h"
    "Source/GameObject/Projectile/TwoHandedAxe.cpp"
    "Source/GameObject/Projectile/TwoHandedAxe.h"
)
source_group("Source\\GameObject\\Projectile" FILES ${Source__GameObject__Projectile})

set(Source__GameObject__Props
    "Source/GameObject/Props/Teleporter.cpp"
    "Source/GameObject/Props/Teleporter.h"
)
source_group("Source\\GameObject\\Props" FILES ${Source__GameObject__Props})

set(Source__GameObject__Character__Enemy
    "Source/GameObject/Character/Enemy/Chest.cpp"
    "Source/GameObject/Character/Enemy/Chest.h"
    "Source/GameObject/Character/Enemy/ChestState.cpp"
    "Source/GameObject/Character/Enemy/ChestState.h"
    "Source/GameObject/Character/Enemy/Enemy.cpp"
    "Source/GameObject/Character/Enemy/Enemy.h"
    "Source/GameObject/Character/Enemy/EnemyManager.cpp"
    "Source/GameObject/Character/Enemy/EnemyManager.h"
    "Source/GameObject/Character/Enemy/EnemyState.cpp"
    "Source/GameObject/Character/Enemy/EnemyState.h"
    "Source/GameObject/Character/Enemy/SkeletonMinion.cpp"
    "Source/GameObject/Character/Enemy/SkeletonMinion.h"
    "Source/GameObject/Character/Enemy/SkeletonMinionState.cpp"
    "Source/GameObject/Character/Enemy/SkeletonMinionState.h"
)
source_group("Source\\GameObject\\Character\\Enemy" FILES ${Source__GameObject__Character__Enemy})

set(Source__GameObject__Character__Player
    "Source/GameObject/Character/Player/Barbarian.cpp"
    "Source/GameObject/Character/Player/Barbarian.h"
    "Source/GameObject/Character/Player/BarbarianState.cpp"
    "Source/GameObject/Character/Player/BarbarianState.h"
    "Source/GameObject/Character/Player/Knight.cpp"
    "Source/GameObject/Character/Player/Knight.h"
    "Source/GameObject/Character/Player/KnightState.cpp"
    "Source/GameObject/Character/Player/KnightState.h"
    "Source/GameObject/Character/Player/Mage.cpp"
    "Source/GameObject/Character/Player/Mage.h"
    "Source/GameObject/Character/Player/NonPlayerCharacter.cpp"
    "Source/GameObject/Character/Player/NonPlayerCharacter.h"
    "Source/GameObject/Character/Player/Player.cpp"
    "Source/GameObject/Character/Player/Player.h"
    "Source/GameObject/Character/Player/PlayerCharacter.cpp"
    "Source/GameObject/Character/Player/PlayerCharacter.h"
    "Source/GameObject/Character/Player/PlayerCharacterManager.cpp"
    "Source/GameObject/Character/Player/PlayerCharacterManager.h"
    "Source/GameObject/Character/Player/PlayerManager.cpp"
    "Source/GameObject/Character/Player/PlayerManager.h"
    "Source/GameObject/Character/Player/PlayerState.cpp"
    "Source/GameObject/Character/Player/PlayerState.h"
    "Source/GameObject/Character/Player/Rogue.cpp"
    "Source/GameObject/Character/Player/Rogue.h"
    "Source/GameObject/Character/Player/RogueState.cpp"
    "Source/GameObject/Character/Player/RogueState.h"
)
source_group("Source\\GameObject\\Character\\Player" FILES ${Source__GameObject__Character__Player})

set(Source__GameObject__Character__Player__State
    "Source/GameObject/Character/Player/State/NonPlayerCharacterState.cpp"
    "Source/GameObject/Character/Player/State/NonPlayerCharacterState.h"
    "Source/GameObject/Character/Player/State/PlayerCharacterState.cpp"
    "Source/GameObject/Character/Player/State/PlayerCharacterState.h"
    "Source/GameObject/Character/Player/State/PlayerCharacterSwordState.cpp"
    "Source/GameObject/Character/Player/State/PlayerCharacterSwordState.h"
)
source_group("Source\\GameObject\\Character\\Player\\State" FILES ${Source__GameObject__Character__Player__State})

set(Source__Network__TCPCommand
    "Source/Network/TCPCommand/TCPChat.cpp"
    "Source/Network/TCPCommand/TCPChat.h"
    "Source/Network/TCPCommand/TCPClientData.cpp"
    "Source/Network/TCPCommand/TCPClientData.h"
    "Source/Network/TCPCommand/TCPCommand.cpp"
    "Source/Network/TCPCommand/TCPCommand.h"
    "Source/Network/TCPCommand/TCPLogin.cpp"
    "Source/Network/TCPCommand/TCPLogin.h"
    "Source/Network/TCPCommand/TCPMatching.cpp"
    "Source/Network/TCPCommand/TCPMatching.h"
    "Source/Network/TCPCommand/TCPRoom.cpp"
    "Source/Network/TCPCommand/TCPRoom.h"
    "Source/Network/TCPCommand/TCPToken.cpp"
    "Source/Network/TCPCommand/TCPToken.h"
)
source_group("Source\\Network\\TCPCommand" FILES ${Source__Network__TCPCommand})

set(Source__Network__UDPCommand
    "Source/Network/UDPCommand/UDPCommand.cpp"
    "Source/Network/UDPCommand/UDPCommand.h"
    "Source/Network/UDPCommand/UDPSync.cpp"
    "Source/Network/UDPCommand/UDPSync.h"
)
source_group("Source\\Network\\UDPCommand" FILES ${Source__Network__UDPCommand})

set(Source__Scene__Stage
    "Source/Scene/Stage/Stage.cpp"
    "Source/Scene/Stage/Stage.h"
    "Source/Scene/Stage/StageManager.cpp"
    "Source/Scene/Stage/StageManager.h"
    "Source/Scene/Stage/TestingStage.cpp"
    "Source/Scene/Stage/TestingStage.h"
)
source_group("Source\\Scene\\Stage" FILES ${Source__Scene__Stage})

set(Source__Scene__GameLoop__SceneCharacter
    "Source/Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.cpp"
    "Source/Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"
    "Source/Scene/GameLoop/SceneCharacter/SceneCharacter_E4CState.cpp"
    "Source/Scene/GameLoop/SceneCharacter/SceneCharacter_E4CState.h"
)
source_group("Source\\Scene\\GameLoop\\SceneCharacter" FILES ${Source__Scene__GameLoop__SceneCharacter})

set(Source__Scene__GameLoop__SceneGame
    "Source/Scene/GameLoop/SceneGame/SceneGame_E4C.cpp"
    "Source/Scene/GameLoop/SceneGame/SceneGame_E4C.h"
    "Source/Scene/GameLoop/SceneGame/SceneGame_E4CState.cpp"
    "Source/Scene/GameLoop/SceneGame/SceneGame_E4CState.h"
)
source_group("Source\\Scene\\GameLoop\\SceneGame" FILES ${Source__Scene__GameLoop__SceneGame})

set(Source__Scene__GameLoop__SceneTitle
    "Source/Scene/GameLoop/SceneTitle/SceneTitle_E4C.cpp"
    "Source/Scene/GameLoop/SceneTitle/SceneTitle_E4C.h"
    "Source/Scene/GameLoop/SceneTitle/SceneTitle_E4CState.cpp"
    "Source/Scene/GameLoop/SceneTitle/SceneTitle_E4CState.h"
)
source_group("Source\\Scene\\GameLoop\\SceneTitle" FILES ${Source__Scene__GameLoop__SceneTitle})

set(Source__Scene__GameLoop__SceneGame__Stage
    "Source/Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.cpp"
    "Source/Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.h"
    "Source/Scene/GameLoop/SceneGame/Stage/StageOpenWorld_E4C.cpp"
    "Source/Scene/GameLoop/SceneGame/Stage/StageOpenWorld_E4C.h"
)
source_group("Source\\Scene\\GameLoop\\SceneGame\\Stage" FILES ${Source__Scene__GameLoop__SceneGame__Stage})

set(Source__UI__Widget
    "Source/UI/Widget/WidgetBossHp.cpp"
    "Source/UI/Widget/WidgetBossHp.h"
    "Source/UI/Widget/WidgetButton.cpp"
    "Source/UI/Widget/WidgetButton.h"
    "Source/UI/Widget/WidgetButtonImage.cpp"
    "Source/UI/Widget/WidgetButtonImage.h"
    "Source/UI/Widget/WidgetButtonText.cpp"
    "Source/UI/Widget/WidgetButtonText.h"
    "Source/UI/Widget/WidgetCrosshair.cpp"
    "Source/UI/Widget/WidgetCrosshair.h"
    "Source/UI/Widget/WidgetDragFloat.cpp"
    "Source/UI/Widget/WidgetDragFloat.h"
    "Source/UI/Widget/WidgetEnemiesHp.cpp"
    "Source/UI/Widget/WidgetEnemiesHp.h"
    "Source/UI/Widget/WidgetGem.cpp"
    "Source/UI/Widget/WidgetGem.h"
    "Source/UI/Widget/WidgetImage.cpp"
    "Source/UI/Widget/WidgetImage.h"
    "Source/UI/Widget/WidgetInputBool.cpp"
    "Source/UI/Widget/WidgetInputBool.h"
    "Source/UI/Widget/WidgetLogin.cpp"
    "Source/UI/Widget/WidgetLogin.h"
    "Source/UI/Widget/WidgetMatching.cpp"
    "Source/UI/Widget/WidgetMatching.h"
    "Source/UI/Widget/WidgetMenu.cpp"
    "Source/UI/Widget/WidgetMenu.h"
    "Source/UI/Widget/WidgetPlayerHP.cpp"
    "Source/UI/Widget/WidgetPlayerHP.h"
    "Source/UI/Widget/WidgetRoomOption.cpp"
    "Source/UI/Widget/WidgetRoomOption.h"
    "Source/UI/Widget/WidgetSettingWindow.cpp"
    "Source/UI/Widget/WidgetSettingWindow.h"
    "Source/UI/Widget/WidgetSkillTimer.cpp"
    "Source/UI/Widget/WidgetSkillTimer.h"
    "Source/UI/Widget/WidgetText.cpp"
    "Source/UI/Widget/WidgetText.h"
    "Source/UI/Widget/WidgetUpDown.cpp"
    "Source/UI/Widget/WidgetUpDown.h"
)
source_group("Source\\UI\\Widget" FILES ${Source__UI__Widget})

set(TAKOEngine__AI
    "TAKOEngine/AI/BaseState.h"
    "TAKOEngine/AI/StateMachine.h"
)
source_group("TAKOEngine\\AI" FILES ${TAKOEngine__AI})

set(TAKOEngine__Effects
    "TAKOEngine/Effects/Effect.cpp"
    "TAKOEngine/Effects/Effect.h"
    "TAKOEngine/Effects/EffectManager.cpp"
    "TAKOEngine/Effects/EffectManager.h"
)
source_group("TAKOEngine\\Effects" FILES ${TAKOEngine__Effects})

set(TAKOEngine__GUI
    "TAKOEngine/GUI/Dialog.cpp"
    "TAKOEngine/GUI/Dialog.h"
    "TAKOEngine/GUI/UIManager.cpp"
    "TAKOEngine/GUI/UIManager.h"
    "TAKOEngine/GUI/Widget.h"
)
source_group("TAKOEngine\\GUI" FILES ${TAKOEngine__GUI})

set(TAKOEngine__Network
    "TAKOEngine/Network/ClientNetworkController.cpp"
    "TAKOEngine/Network/HostNetworkController.cpp"
    "TAKOEngine/Network/HttpRequest.cpp"
    "TAKOEngine/Network/HttpRequest.h"
    "TAKOEngine/Network/Network.cpp"
    "TAKOEngine/Network/Network.h"
    "TAKOEngine/Network/NetworkController.h"
    "TAKOEngine/Network/Socket.cpp"
    "TAKOEngine/Network/Socket.h"
    "TAKOEngine/Network/TCPSocket.cpp"
    "TAKOEngine/Network/TCPSocket.h"
    "TAKOEngine/Network/UDPSocket.cpp"
    "TAKOEngine/Network/UDPSocket.h"
    "TAKOEngine/Network/url.cpp"
    "TAKOEngine/Network/url.h"
    "TAKOEngine/Network/WinSock2Wrapper.h"
)
source_group("TAKOEngine\\Network" FILES ${TAKOEngine__Network})

set(TAKOEngine__Physics
    "TAKOEngine/Physics/BoundingBoxCollider.cpp"
    "TAKOEngine/Physics/BoundingBoxCollider.h"
    "TAKOEngine/Physics/CapsuleCollider.cpp"
    "TAKOEngine/Physics/CapsuleCollider.h"
    "TAKOEngine/Physics/Collider.cpp"
    "TAKOEngine/Physics/Collider.h"
    "TAKOEngine/Physics/Collision.cpp"
    "TAKOEngine/Physics/Collision.h"
    "TAKOEngine/Physics/CollisionDataManager.cpp"
    "TAKOEngine/Physics/CollisionDataManager.h"
    "TAKOEngine/Physics/CollisionManager.cpp"
    "TAKOEngine/Physics/CollisionManager.h"
    "TAKOEngine/Physics/Liner8TreeManager.h"
    "TAKOEngine/Physics/MapCollider.cpp"
    "TAKOEngine/Physics/MapCollider.h"
    "TAKOEngine/Physics/MapCollisionManager.cpp"
    "TAKOEngine/Physics/MapCollisionManager.h"
    "TAKOEngine/Physics/MapQuadtree.cpp"
    "TAKOEngine/Physics/MapQuadtree.h"
    "TAKOEngine/Physics/ModelCollider.cpp"
    "TAKOEngine/Physics/ModelCollider.h"
    "TAKOEngine/Physics/OctreeNode.cpp"
    "TAKOEngine/Physics/OctreeNode.h"
    "TAKOEngine/Physics/PlayerCollisionObject.cpp"
    "TAKOEngine/Physics/PlayerCollisionObject.h"
    "TAKOEngine/Physics/QuadtreeNode.cpp"
    "TAKOEngine/Physics/QuadtreeNode.h"
    "TAKOEngine/Physics/RigidBody.h"
    "TAKOEngine/Physics/SphereCollider.cpp"
    "TAKOEngine/Physics/SphereCollider.h"
    "TAKOEngine/Physics/UnrotatedBoxCollider.cpp"
    "TAKOEngine/Physics/UnrotatedBoxCollider.h"
)
source_group("TAKOEngine\\Physics" FILES ${TAKOEngine__Physics})

set(TAKOEngine__Rendering
    "TAKOEngine/Rendering/Constant.h"
    "TAKOEngine/Rendering/ConstantBuffer.h"
    "TAKOEngine/Rendering/Cylinder.cpp"
    "TAKOEngine/Rendering/Cylinder.h"
    "TAKOEngine/Rendering/DebugRenderer.cpp"
    "TAKOEngine/Rendering/DebugRenderer.h"
    "TAKOEngine/Rendering/DeferredRendering.cpp"
    "TAKOEngine/Rendering/DeferredRendering.h"
    "TAKOEngine/Rendering/Descriptor.cpp"
    "TAKOEngine/Rendering/Descriptor.h"
    "TAKOEngine/Rendering/FrameBuffer.cpp"
    "TAKOEngine/Rendering/FrameBuffer.h"
    "TAKOEngine/Rendering/FrameBufferDX12.cpp"
    "TAKOEngine/Rendering/FrameBufferDX12.h"
    "TAKOEngine/Rendering/FrameBufferManager.cpp"
    "TAKOEngine/Rendering/FrameBufferManager.h"
    "TAKOEngine/Rendering/FrameBufferTexture.cpp"
    "TAKOEngine/Rendering/FrameBufferTexture.h"
    "TAKOEngine/Rendering/FrustumCulling.cpp"
    "TAKOEngine/Rendering/FrustumCulling.h"
    "TAKOEngine/Rendering/Gizmos.cpp"
    "TAKOEngine/Rendering/Gizmos.h"
    "TAKOEngine/Rendering/GpuResourceUtils.cpp"
    "TAKOEngine/Rendering/GpuResourceUtils.h"
    "TAKOEngine/Rendering/Graphics.cpp"
    "TAKOEngine/Rendering/Graphics.h"
    "TAKOEngine/Rendering/Light.cpp"
    "TAKOEngine/Rendering/Light.h"
    "TAKOEngine/Rendering/LightManager.cpp"
    "TAKOEngine/Rendering/LightManager.h"
    "TAKOEngine/Rendering/LineRenderer.cpp"
    "TAKOEngine/Rendering/LineRenderer.h"
    "TAKOEngine/Rendering/Misc.h"
    "TAKOEngine/Rendering/MyRender.h"
    "TAKOEngine/Rendering/ParticleRenderer.cpp"
    "TAKOEngine/Rendering/ParticleRenderer.h"
    "TAKOEngine/Rendering/PipelineState.cpp"
    "TAKOEngine/Rendering/PipelineState.h"
    "TAKOEngine/Rendering/Plane.cpp"
    "TAKOEngine/Rendering/Plane.h"
    "TAKOEngine/Rendering/PostEffect.cpp"
    "TAKOEngine/Rendering/PostEffect.h"
    "TAKOEngine/Rendering/PostprocessingRenderer.cpp"
    "TAKOEngine/Rendering/PostprocessingRenderer.h"
    "TAKOEngine/Rendering/RenderContext.h"
    "TAKOEngine/Rendering/RenderState.cpp"
    "TAKOEngine/Rendering/RenderState.h"
    "TAKOEngine/Rendering/ResourceManager.cpp"
    "TAKOEngine/Rendering/ResourceManager.h"
    "TAKOEngine/Rendering/ShadowMapRender.cpp"
    "TAKOEngine/Rendering/ShadowMapRender.h"
    "TAKOEngine/Rendering/Sprite.cpp"
    "TAKOEngine/Rendering/Sprite.h"
    "TAKOEngine/Rendering/SpriteDX12.cpp"
    "TAKOEngine/Rendering/SpriteDX12.h"
    "TAKOEngine/Rendering/TextSprite.cpp"
    "TAKOEngine/Rendering/TextSprite.h"
)
source_group("TAKOEngine\\Rendering" FILES ${TAKOEngine__Rendering})

set(TAKOEngine__Runtime
    "TAKOEngine/Runtime/tentacle_lib.cpp"
    "TAKOEngine/Runtime/tentacle_lib.h"
)
source_group("TAKOEngine\\Runtime" FILES ${TAKOEngine__Runtime})

set(TAKOEngine__Sound
    "TAKOEngine/Sound/Audio.cpp"
    "TAKOEngine/Sound/Audio.h"
    "TAKOEngine/Sound/Sound.cpp"
    "TAKOEngine/Sound/Sound.h"
)
source_group("TAKOEngine\\Sound" FILES ${TAKOEngine__Sound})

set(TAKOEngine__Tool
    "TAKOEngine/Tool/AssimpImporter.cpp"
    "TAKOEngine/Tool/AssimpImporter.h"
    "TAKOEngine/Tool/Console.cpp"
    "TAKOEngine/Tool/Console.h"
    "TAKOEngine/Tool/Encode.cpp"
    "TAKOEngine/Tool/Encode.h"
    "TAKOEngine/Tool/GLTFImporter.cpp"
    "TAKOEngine/Tool/GLTFImporter.h"
    "TAKOEngine/Tool/ImGuiRenderer.cpp"
    "TAKOEngine/Tool/ImGuiRenderer.h"
    "TAKOEngine/Tool/Logger.cpp"
    "TAKOEngine/Tool/Logger.h"
    "TAKOEngine/Tool/Mathf.cpp"
    "TAKOEngine/Tool/Mathf.h"
    "TAKOEngine/Tool/Singleton.h"
    "TAKOEngine/Tool/Timer.cpp"
    "TAKOEngine/Tool/Timer.h"
    "TAKOEngine/Tool/TransformUtils.cpp"
    "TAKOEngine/Tool/TransformUtils.h"
    "TAKOEngine/Tool/U8Buffer.cpp"
    "TAKOEngine/Tool/U8Buffer.h"
    "TAKOEngine/Tool/XMFLOAT.h"
)
source_group("TAKOEngine\\Tool" FILES ${TAKOEngine__Tool})

set(TAKOEngine__Editor__Camera
    "TAKOEngine/Editor/Camera/Camera.cpp"
    "TAKOEngine/Editor/Camera/Camera.h"
    "TAKOEngine/Editor/Camera/CameraController.cpp"
    "TAKOEngine/Editor/Camera/CameraController.h"
    "TAKOEngine/Editor/Camera/CameraManager.cpp"
    "TAKOEngine/Editor/Camera/CameraManager.h"
    "TAKOEngine/Editor/Camera/FreeCameraController.cpp"
    "TAKOEngine/Editor/Camera/FreeCameraController.h"
    "TAKOEngine/Editor/Camera/ThridPersonCameraController.cpp"
    "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
)
source_group("TAKOEngine\\Editor\\Camera" FILES ${TAKOEngine__Editor__Camera})

set(TAKOEngine__Editor__Input
    "TAKOEngine/Editor/Input/Input.cpp"
    "TAKOEngine/Editor/Input/Input.h"
    "TAKOEngine/Editor/Input/InputMap.h"
)
source_group("TAKOEngine\\Editor\\Input" FILES ${TAKOEngine__Editor__Input})

set(TAKOEngine__Rendering__Model
    "TAKOEngine/Rendering/Model/GLTFModelDX11.cpp"
    "TAKOEngine/Rendering/Model/GLTFModelDX11.h"
    "TAKOEngine/Rendering/Model/Model.h"
    "TAKOEngine/Rendering/Model/ModelDX11.cpp"
    "TAKOEngine/Rendering/Model/ModelDX11.h"
    "TAKOEngine/Rendering/Model/ModelDX12.cpp"
    "TAKOEngine/Rendering/Model/ModelDX12.h"
    "TAKOEngine/Rendering/Model/ModelResource.cpp"
    "TAKOEngine/Rendering/Model/ModelResource.h"
)
source_group("TAKOEngine\\Rendering\\Model" FILES ${TAKOEngine__Rendering__Model})

set(TAKOEngine__Rendering__Shaders
    "TAKOEngine/Rendering/Shaders/ColorGradingShader.cpp"
    "TAKOEngine/Rendering/Shaders/ColorGradingShader.h"
    "TAKOEngine/Rendering/Shaders/CylinderShader.cpp"
    "TAKOEngine/Rendering/Shaders/CylinderShader.h"
    "TAKOEngine/Rendering/Shaders/DeferredLightingShader.cpp"
    "TAKOEngine/Rendering/Shaders/DeferredLightingShader.h"
    "TAKOEngine/Rendering/Shaders/FinalpassShader.cpp"
    "TAKOEngine/Rendering/Shaders/FinalpassShader.h"
    "TAKOEngine/Rendering/Shaders/GaussianBlurShader.cpp"
    "TAKOEngine/Rendering/Shaders/GaussianBlurShader.h"
    "TAKOEngine/Rendering/Shaders/LambertShader.cpp"
    "TAKOEngine/Rendering/Shaders/LambertShader.h"
    "TAKOEngine/Rendering/Shaders/LuminanceExtractionShader.cpp"
    "TAKOEngine/Rendering/Shaders/LuminanceExtractionShader.h"
    "TAKOEngine/Rendering/Shaders/MaskShader.cpp"
    "TAKOEngine/Rendering/Shaders/MaskShader.h"
    "TAKOEngine/Rendering/Shaders/ModelShader.cpp"
    "TAKOEngine/Rendering/Shaders/ModelShader.h"
    "TAKOEngine/Rendering/Shaders/ModelShaderDX12.cpp"
    "TAKOEngine/Rendering/Shaders/ModelShaderDX12.h"
    "TAKOEngine/Rendering/Shaders/ParticleShader.cpp"
    "TAKOEngine/Rendering/Shaders/ParticleShader.h"
    "TAKOEngine/Rendering/Shaders/PhongShader.cpp"
    "TAKOEngine/Rendering/Shaders/PhongShader.h"
    "TAKOEngine/Rendering/Shaders/PlaneShader.cpp"
    "TAKOEngine/Rendering/Shaders/PlaneShader.h"
    "TAKOEngine/Rendering/Shaders/Shader.h"
    "TAKOEngine/Rendering/Shaders/ShadowMapShader.cpp"
    "TAKOEngine/Rendering/Shaders/ShadowMapShader.h"
    "TAKOEngine/Rendering/Shaders/SkydomeShader.cpp"
    "TAKOEngine/Rendering/Shaders/SkydomeShader.h"
    "TAKOEngine/Rendering/Shaders/SpriteShader.cpp"
    "TAKOEngine/Rendering/Shaders/SpriteShader.h"
    "TAKOEngine/Rendering/Shaders/SpriteShaderDX12.cpp"
    "TAKOEngine/Rendering/Shaders/SpriteShaderDX12.h"
    "TAKOEngine/Rendering/Shaders/ToonShader.cpp"
    "TAKOEngine/Rendering/Shaders/ToonShader.h"
    "TAKOEngine/Rendering/Shaders/UVScrollShader.cpp"
    "TAKOEngine/Rendering/Shaders/UVScrollShader.h"
)
source_group("TAKOEngine\\Rendering\\Shaders" FILES ${TAKOEngine__Rendering__Shaders})

set(TAKOEngine__Rendering__Texture
    "TAKOEngine/Rendering/Texture/ramp.png"
    "TAKOEngine/Rendering/Texture/ramp3.png"
    "TAKOEngine/Rendering/Texture/RampTexture-1.jpg"
)
source_group("TAKOEngine\\Rendering\\Texture" FILES ${TAKOEngine__Rendering__Texture})

set(TAKOEngine__Rendering__HLSL__Common
    "TAKOEngine/Rendering/HLSL/Common/bidirectional_reflectance_distribution_function.hlsli"
    "TAKOEngine/Rendering/HLSL/Common/image_based_lighting.hlsli"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Common" FILES ${TAKOEngine__Rendering__HLSL__Common})

set(TAKOEngine__Rendering__HLSL__Debug
    "TAKOEngine/Rendering/HLSL/Debug/Debug.hlsli"
    "TAKOEngine/Rendering/HLSL/Debug/DebugPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Debug/DebugVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Debug/Gizmos.hlsli"
    "TAKOEngine/Rendering/HLSL/Debug/GizmosPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Debug/GizmosVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Debug/Line.hlsli"
    "TAKOEngine/Rendering/HLSL/Debug/LinePS.hlsl"
    "TAKOEngine/Rendering/HLSL/Debug/LineVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Debug" FILES ${TAKOEngine__Rendering__HLSL__Debug})

set(TAKOEngine__Rendering__HLSL__Lighting
    "TAKOEngine/Rendering/HLSL/Lighting/Deferred.hlsli"
    "TAKOEngine/Rendering/HLSL/Lighting/DeferredPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Lighting/DeferredVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Lighting/Light.hlsli"
    "TAKOEngine/Rendering/HLSL/Lighting/ShadowMap.hlsli"
    "TAKOEngine/Rendering/HLSL/Lighting/ShadowmapFunctions.hlsli"
    "TAKOEngine/Rendering/HLSL/Lighting/ShadowMapVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Lighting" FILES ${TAKOEngine__Rendering__HLSL__Lighting})

set(TAKOEngine__Rendering__HLSL__Model
    "TAKOEngine/Rendering/HLSL/Model/Constants.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/DefaultModel.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/DefaultModelPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/DefaultModelVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/Lambert.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/LambertDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/LambertDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/LambertDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/LambertInstancingVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/LambertPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/LambertVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/Phong.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/PhongDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/PhongDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/PhongDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/PhongInstancingVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/PhongPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/PhongVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/Skinning.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/SkinningCS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/Toon.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/ToonDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/Model/ToonDX12GS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/ToonDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/ToonDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/ToonInstancingVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/ToonPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Model/ToonVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Model" FILES ${TAKOEngine__Rendering__HLSL__Model})

set(TAKOEngine__Rendering__HLSL__Particle
    "TAKOEngine/Rendering/HLSL/Particle/Particle.hlsli"
    "TAKOEngine/Rendering/HLSL/Particle/ParticleCS.hlsl"
    "TAKOEngine/Rendering/HLSL/Particle/ParticleGS.hlsl"
    "TAKOEngine/Rendering/HLSL/Particle/ParticlePS.hlsl"
    "TAKOEngine/Rendering/HLSL/Particle/ParticleVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Particle" FILES ${TAKOEngine__Rendering__HLSL__Particle})

set(TAKOEngine__Rendering__HLSL__PostEffect
    "TAKOEngine/Rendering/HLSL/PostEffect/BloomPS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/ColorGrading.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingPS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/FilterFunctions.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/Finalpass.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/FinalpassDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/FinalpassDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/FinalpassDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/FinalpassPS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/FullScreenQuad.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/FullScreenQuadVS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlur.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurPS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtraction.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionPS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/Mask.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/MaskPS.hlsl"
    "TAKOEngine/Rendering/HLSL/PostEffect/PostEffect.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect/ShaderData.hlsli"
)
source_group("TAKOEngine\\Rendering\\HLSL\\PostEffect" FILES ${TAKOEngine__Rendering__HLSL__PostEffect})

set(TAKOEngine__Rendering__HLSL__Primitives
    "TAKOEngine/Rendering/HLSL/Primitives/Billboard.hlsli"
    "TAKOEngine/Rendering/HLSL/Primitives/BillboardGS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/BillboardPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/BillboardVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/Fireball.hlsli"
    "TAKOEngine/Rendering/HLSL/Primitives/FireballPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/FireballVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/Plane.hlsli"
    "TAKOEngine/Rendering/HLSL/Primitives/PlanePS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/PlaneVS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/Portal.hlsli"
    "TAKOEngine/Rendering/HLSL/Primitives/PortalPS.hlsl"
    "TAKOEngine/Rendering/HLSL/Primitives/PortalVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Primitives" FILES ${TAKOEngine__Rendering__HLSL__Primitives})

set(TAKOEngine__Rendering__HLSL__Skydome
    "TAKOEngine/Rendering/HLSL/Skydome/SkyBoxDX12.hlsli"
    "TAKOEngine/Rendering/HLSL/Skydome/SkyBoxDX12PS.hlsl"
    "TAKOEngine/Rendering/HLSL/Skydome/SkyBoxDX12VS.hlsl"
    "TAKOEngine/Rendering/HLSL/Skydome/Skydome.hlsli"
    "TAKOEngine/Rendering/HLSL/Skydome/SkydomePS.hlsl"
    "TAKOEngine/Rendering/HLSL/Skydome/SkydomeVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Skydome" FILES ${TAKOEngine__Rendering__HLSL__Skydome})

set(TAKOEngine__Rendering__HLSL__Sprite
    "TAKOEngine/Rendering/HLSL/Sprite/Sprite.hlsli"
    "TAKOEngine/Rendering/HLSL/Sprite/SpritePS.hlsl"
    "TAKOEngine/Rendering/HLSL/Sprite/SpriteVS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\Sprite" FILES ${TAKOEngine__Rendering__HLSL__Sprite})

set(TAKOEngine__Rendering__HLSL__UVScroll
    "TAKOEngine/Rendering/HLSL/UVScroll/UVScroll.hlsli"
    "TAKOEngine/Rendering/HLSL/UVScroll/UVScrollPS.hlsl"
)
source_group("TAKOEngine\\Rendering\\HLSL\\UVScroll" FILES ${TAKOEngine__Rendering__HLSL__UVScroll})

set(ALL_FILES
    ${no_group_source_files}
    ${External__DirectXTex}
    ${External__imgui-docking}
    ${Source}
    ${TAKOEngine}
    ${Source__GameObject}
    ${Source__Map}
    ${Source__Network}
    ${Source__Scene}
    ${Source__UI}
    ${Source__GameObject__Character}
    ${Source__GameObject__Projectile}
    ${Source__GameObject__Props}
    ${Source__GameObject__Character__Enemy}
    ${Source__GameObject__Character__Player}
    ${Source__GameObject__Character__Player__State}
    ${Source__Network__TCPCommand}
    ${Source__Network__UDPCommand}
    ${Source__Scene__GameLoop}
    ${Source__Scene__Stage}
    ${Source__Scene__GameLoop__SceneCharacter}
    ${Source__Scene__GameLoop__SceneGame}
    ${Source__Scene__GameLoop__SceneTitle}
    ${Source__Scene__GameLoop__SceneGame__Stage}
    ${Source__UI__Widget}
    ${TAKOEngine__AI}
    ${TAKOEngine__Editor}
    ${TAKOEngine__Effects}
    ${TAKOEngine__GUI}
    ${TAKOEngine__Network}
    ${TAKOEngine__Physics}
    ${TAKOEngine__Rendering}
    ${TAKOEngine__Runtime}
    ${TAKOEngine__Sound}
    ${TAKOEngine__Tool}
    ${TAKOEngine__Editor__Camera}
    ${TAKOEngine__Editor__Input}
    ${TAKOEngine__Rendering__HLSL}
    ${TAKOEngine__Rendering__Model}
    ${TAKOEngine__Rendering__Shaders}
    ${TAKOEngine__Rendering__Texture}
    ${TAKOEngine__Rendering__HLSL__Common}
    ${TAKOEngine__Rendering__HLSL__Debug}
    ${TAKOEngine__Rendering__HLSL__Lighting}
    ${TAKOEngine__Rendering__HLSL__Model}
    ${TAKOEngine__Rendering__HLSL__Particle}
    ${TAKOEngine__Rendering__HLSL__PostEffect}
    ${TAKOEngine__Rendering__HLSL__Primitives}
    ${TAKOEngine__Rendering__HLSL__Skydome}
    ${TAKOEngine__Rendering__HLSL__Sprite}
    ${TAKOEngine__Rendering__HLSL__UVScroll}
)

#####################################################################
# SOURCE_GROUPS_END
#####################################################################

# ###############################################################################
# SHADERS_START
# ###############################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

function(set_hlsl_properties target source_file shader_model shader_type entry_point)
    set_source_files_properties(${source_file} PROPERTIES
        VS_SHADER_MODEL ${shader_model} # Set the shader model
        VS_SHADER_TYPE ${shader_type} # Set the shader type
        VS_SHADER_ENTRYPOINT ${entry_point} # Set the entry point function name
        VS_SHADER_OBJECT_FILE_NAME "$(OutDir)Data/Shader/%(Filename).cso"
    )
endfunction()

#####################################################################
# SHADERS_FILES_START
#####################################################################

#List of HLSL shaders
set(SHADER_NAMES
    "DebugPS.cso"
    "DebugVS.cso"
    "GizmosPS.cso"
    "GizmosVS.cso"
    "LinePS.cso"
    "LineVS.cso"
    "DeferredPS.cso"
    "DeferredVS.cso"
    "ShadowMapVS.cso"
    "DefaultModelPS.cso"
    "DefaultModelVS.cso"
    "LambertDX12PS.cso"
    "LambertDX12VS.cso"
    "LambertInstancingVS.cso"
    "LambertPS.cso"
    "LambertVS.cso"
    "PhongDX12PS.cso"
    "PhongDX12VS.cso"
    "PhongInstancingVS.cso"
    "PhongPS.cso"
    "PhongVS.cso"
    "SkinningCS.cso"
    "ToonDX12GS.cso"
    "ToonDX12PS.cso"
    "ToonDX12VS.cso"
    "ToonInstancingVS.cso"
    "ToonPS.cso"
    "ToonVS.cso"
    "ParticleCS.cso"
    "ParticleGS.cso"
    "ParticlePS.cso"
    "ParticleVS.cso"
    "BloomPS.cso"
    "ColorGradingDX12PS.cso"
    "ColorGradingDX12VS.cso"
    "ColorGradingPS.cso"
    "FinalpassDX12PS.cso"
    "FinalpassDX12VS.cso"
    "FinalpassPS.cso"
    "FullScreenQuadVS.cso"
    "GaussianBlurDX12PS.cso"
    "GaussianBlurDX12VS.cso"
    "GaussianBlurPS.cso"
    "LuminanceExtractionDX12PS.cso"
    "LuminanceExtractionDX12VS.cso"
    "LuminanceExtractionPS.cso"
    "MaskPS.cso"
    "BillboardGS.cso"
    "BillboardPS.cso"
    "BillboardVS.cso"
    "FireballPS.cso"
    "FireballVS.cso"
    "PlanePS.cso"
    "PlaneVS.cso"
    "PortalPS.cso"
    "PortalVS.cso"
    "SkyBoxDX12PS.cso"
    "SkyBoxDX12VS.cso"
    "SkydomePS.cso"
    "SkydomeVS.cso"
    "SpritePS.cso"
    "SpriteVS.cso"
    "UVScrollPS.cso"
)

# List of shader settings
set(HLSL_SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Debug/DebugPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Debug/DebugVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Debug/GizmosPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Debug/GizmosVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Debug/LinePS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Debug/LineVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Lighting/DeferredPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Lighting/DeferredVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Lighting/ShadowMapVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/DefaultModelPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/DefaultModelVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/LambertDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/LambertDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/LambertInstancingVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/LambertPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/LambertVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/PhongDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/PhongDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/PhongInstancingVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/PhongPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/PhongVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/SkinningCS.hlsl,5.0,Compute,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/ToonDX12GS.hlsl,5.0,Geometry,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/ToonDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/ToonDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/ToonInstancingVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/ToonPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Model/ToonVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Particle/ParticleCS.hlsl,5.0,Compute,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Particle/ParticleGS.hlsl,5.0,Geometry,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Particle/ParticlePS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Particle/ParticleVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/BloomPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/ColorGradingPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/FinalpassDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/FinalpassDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/FinalpassPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/FullScreenQuadVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/GaussianBlurPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/LuminanceExtractionPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/PostEffect/MaskPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/BillboardGS.hlsl,5.0,Geometry,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/BillboardPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/BillboardVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/FireballPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/FireballVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/PlanePS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/PlaneVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/PortalPS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Primitives/PortalVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Skydome/SkyBoxDX12PS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Skydome/SkyBoxDX12VS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Skydome/SkydomePS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Skydome/SkydomeVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Sprite/SpritePS.hlsl,5.0,Pixel,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/Sprite/SpriteVS.hlsl,5.0,Vertex,main"
    "${CMAKE_CURRENT_SOURCE_DIR}/TAKOEngine/Rendering/HLSL/UVScroll/UVScrollPS.hlsl,5.0,Pixel,main"
)

#####################################################################
# SHADERS_FILES_END
#####################################################################

# Apply HLSL properties to each shader file
foreach(shader_properties ${HLSL_SHADERS})
    string(REPLACE "," ";" MY_LIST ${shader_properties})

    list(GET MY_LIST 0 shader_file)
    list(GET MY_LIST 1 shader_model)
    list(GET MY_LIST 2 shader_type)
    list(GET MY_LIST 3 entry_point)

    set_hlsl_properties(${PROJECT_NAME} ${shader_file} ${shader_model} ${shader_type} ${entry_point})
endforeach()

# ###############################################################################
# SHADERS_END
# ###############################################################################

# ###############################################################################
# Target
# ###############################################################################
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)

# ###############################################################################
# Target name
# ###############################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG "Game"
    TARGET_NAME_RELEASE "Game"
)

# ###############################################################################
# Output directory
# ###############################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()

# ###############################################################################
# MSVC runtime library
# ###############################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Debug>:
    MultiThreadedDebugDLL
    >
    $<$<CONFIG:Release>:
    MultiThreadedDLL
    >
    $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

# ###############################################################################
# Include directories
# ###############################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/include"
    ">"
    "$<$<CONFIG:Release>:"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/include"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui-docking"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/stb"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/cereal/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/TeNTaCleLib"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer170e/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer170e/include/Effekseer"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/openssl/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/tinygltf"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/OpenAL/include"
)

# ###############################################################################
# Compile definitions
# ###############################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
    "_DEBUG"
    "CMAKE_INTDIR=\"Debug\""
    ">"
    "$<$<CONFIG:Release>:"
    "NDEBUG"
    "CMAKE_INTDIR=\"Release\""
    ">"
    "_CONSOLE"
    "UNICODE"
    "_UNICODE"
    "WIN32_LEAN_AND_MEAN"
)

# ###############################################################################
# Compile and link options
# ###############################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
        /Oi
        /Gy
        >
        /permissive-
        /MP
        /sdl
        /W3
        /Zi
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
        /Y-
        /wd4819
        /wd4099
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
        /INCREMENTAL
        >
        $<$<CONFIG:Release>:
        /OPT:REF
        /OPT:ICF
        >
        /MANIFEST
        /DEBUG
        /SUBSYSTEM:WINDOWS
        /wd4819
        /wd4099
    )
endif()

# ###############################################################################
# Post build events
# ###############################################################################

# URL of the ZIP files
set(ZIP_FILE_URLS
    "https://www.dropbox.com/scl/fi/ovb2dkjr3n1ayljdb0ftn/Data.zip?rlkey=2th7h96r3p4qsxn12oqnnp1wv&st=uywpwppu&dl=0"
    "https://www.dropbox.com/scl/fi/nm7h69ap66e62978butip/Dependencies.zip?rlkey=9icgfv9xwc9is1hziwhk5kjjs&st=gv2qvxdg&dl=0"
    "https://www.dropbox.com/scl/fi/1nbbetvdxp4dthu45kayx/External.zip?rlkey=ndtf9xh27fhaiuhnwnn4btf26&st=16hvp0yp&dl=0"
    "https://www.dropbox.com/scl/fi/hvu3y0cztxca9ztosju2j/Docs.zip?rlkey=24lazv9kbo0y86cnihpxff6re&st=8ulu8iw1&dl=0"
)

# Local path where the ZIP file will be downloaded
set(ZIP_FILE_PATHS
    "${CMAKE_SOURCE_DIR}/Data.zip"
    "${CMAKE_SOURCE_DIR}/Dependencies.zip"
    "${CMAKE_SOURCE_DIR}/External.zip"
    "${CMAKE_SOURCE_DIR}/Docs.zip"
)

# Local path where ZIP file will be unarchived
set(DESTINATION_DIRS
    "${CMAKE_SOURCE_DIR}/Data"
    "${CMAKE_SOURCE_DIR}/Dependencies"
    "${CMAKE_SOURCE_DIR}/External"
    "${CMAKE_SOURCE_DIR}/Docs"
)

foreach(i RANGE 0 3)
    list(GET ZIP_FILE_URLS ${i} ZIP_FILE_URL)
    list(GET ZIP_FILE_PATHS ${i} ZIP_FILE_PATH)
    list(GET DESTINATION_DIRS ${i} DESTINATION_DIR)

    # Command to download the ZIP file
    file(DOWNLOAD ${ZIP_FILE_URL} ${ZIP_FILE_PATH}
        SHOW_PROGRESS
        STATUS status)

    list(GET status 0 status_code)
    list(GET status 1 status_string)

    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Error downloading file: ${status_string}")
    endif()

    message(STATUS "File downloaded and verified successfully!")

    file(MAKE_DIRECTORY ${DESTINATION_DIR})

    # Unzip the file
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xvf ${ZIP_FILE_PATH}
        WORKING_DIRECTORY ${DESTINATION_DIR}
        RESULT_VARIABLE result
    )

    if(result)
        message(FATAL_ERROR "Failed to unzip file")
    else()
        message(STATUS "Successfully unzipped file")
    endif()

    file(REMOVE ${ZIP_FILE_PATH})
endforeach()

set(SOURCE_DIR_DATA "${CMAKE_SOURCE_DIR}/Data")
set(SOURCE_DIR_DEPENDENCY "${CMAKE_SOURCE_DIR}/Dependencies")

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${SOURCE_DIR_DATA}
    ${CMAKE_BINARY_DIR}/Data
    COMMENT "Copying data directory to build folder"
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${SOURCE_DIR_DEPENDENCY}
    ${CMAKE_BINARY_DIR}
    COMMENT "Copying dependency filest to build folder"
)

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND if "$(Configuration)" == "Release" ${CMAKE_COMMAND} -E copy_directory_if_different
        ${SOURCE_DIR_DATA}
        ${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/Data
    )
endif()

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND if "$(Configuration)" == "Debug" ${CMAKE_COMMAND} -E copy_directory_if_different
        ${SOURCE_DIR_DATA}
        ${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/Data
    )
endif()

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND if "$(Configuration)" == "Release" ${CMAKE_COMMAND} -E copy_directory_if_different
        ${SOURCE_DIR_DEPENDENCY}
        ${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/
    )
endif()

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND if "$(Configuration)" == "Debug" ${CMAKE_COMMAND} -E copy_directory_if_different
        ${SOURCE_DIR_DEPENDENCY}
        ${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/
    )
endif()

if(MSVC)
    foreach(CurrentShaderFile IN LISTS SHADER_NAMES)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/Data/Shader/${CurrentShaderFile} ${CMAKE_BINARY_DIR}/Data/Shader
            COMMENT "Copying shaders ${CurrentShaderFile}")
    endforeach()
endif()


# ###############################################################################
# Dependencies
# ###############################################################################
# Link with other targets.
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:"
    "SDL2"
    "DirectXTK"
    "DirectXTex"
    "d3d11"
    "d3d12"
    "dxgi"
    "xinput"
    "xaudio2"
    "winmm"
    "zlibstatic"
    "assimp"
    "SDL2"
    "SDL2main"
    "SDL2_mixer"
    "SDL2-static"
    "Effekseerd"
    "EffekseerRendererDX11d"
    "EffekseerRendererDX12d"
    "LLGId"
    "xinput"
    "Crypt32"
    "libssl"
    "libcrypto"
    "OpenAL32"
    ">"
    "$<$<CONFIG:Release>:"
    "SDL2"
    "DirectXTK"
    "DirectXTex"
    "d3d11"
    "d3d12"
    "dxgi"
    "xinput"
    "xaudio2"
    "winmm"
    "zlibstatic"
    "assimp"
    "SDL2"
    "SDL2main"
    "SDL2_mixer"
    "SDL2-static"
    "Effekseer"
    "EffekseerRendererDX11"
    "EffekseerRendererDX12"
    "LLGI"
    "xinput"
    "Crypt32"
    "libssl"
    "libcrypto"
    "OpenAL32"
    ">"
    "kernel32"
    "user32"
    "gdi32"
    "winspool"
    "shell32"
    "ole32"
    "oleaut32"
    "uuid"
    "comdlg32"
    "advapi32"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/nlohmann/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/lib/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/lib/x64"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer170e/lib/vs2022/MD/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/openssl/x64/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtk/native/lib/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtex/native/lib/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/OpenAL/libs/Win64/"
    ">"
    "$<$<CONFIG:Release>:"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/nlohmann"
    "${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/lib/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer170e/lib/vs2022/MD/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/openssl/x64/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtex/native/lib/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtk/native/lib/x64/$<CONFIG>"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/lib/x64"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/OpenAL/libs/Win64/"
    ">"
)
