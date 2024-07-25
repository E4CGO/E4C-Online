cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0.22621.0 CACHE STRING "" FORCE)

project(E4COnline CXX)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)

set (CMAKE_CXX_STANDARD 20)

set (VS_SHADER_MODEL 5_0)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
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

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME Game)

################################################################################
# Source groups
################################################################################
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

set(Source
    "Source/Framework.cpp"
    "Source/Framework.h"
    "Source/GameData.cpp"
    "Source/GameData.h"
    "Source/Main.cpp"
    "Source/Manager.h"
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

set(Source__GameObject__Character
    "Source/GameObject/Character/Character.cpp"
    "Source/GameObject/Character/Character.h"
)
source_group("Source\\GameObject\\Character" FILES ${Source__GameObject__Character})

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
    "Source/GameObject/Character/Player/Player.cpp"
    "Source/GameObject/Character/Player/Player.h"
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

set(Source__Map
    "Source/Map/Map001.cpp"
    "Source/Map/Map001.h"
    "Source/Map/Map001Key.cpp"
    "Source/Map/Map001Key.h"
    "Source/Map/MapTile.cpp"
    "Source/Map/MapTile.h"
    "Source/Map/MapTileManager.cpp"
    "Source/Map/MapTileManager.h"
    "Source/Map/Map001BossRoomTeleport.cpp"
    "Source/Map/Map001BossRoomTeleport.h"
)
source_group("Source\\Map" FILES ${Source__Map})

set(Source__Scene
    "Source/Scene/Scene.cpp"
    "Source/Scene/Scene.h"
    "Source/Scene/SceneGame.cpp"
    "Source/Scene/SceneGame.h"
    "Source/Scene/SceneGameState.cpp"
    "Source/Scene/SceneGameState.h"
    "Source/Scene/SceneLoading.cpp"
    "Source/Scene/SceneManager.cpp"
    "Source/Scene/SceneManager.h"
    "Source/Scene/SceneTitle.cpp"
    "Source/Scene/SceneTitle.h"
    "Source/Scene/SceneTitleState.cpp"
    "Source/Scene/SceneTitleState.h"
)
source_group("Source\\Scene" FILES ${Source__Scene})

set(Source__Scene__Stage
    "Source/Scene/Stage/StageManager.cpp"
    "Source/Scene/Stage/StageManager.h"
    "Source/Scene/Stage/Stage.cpp"
    "Source/Scene/Stage/Stage.h"
    "Source/Scene/Stage/TestingStage.cpp"
    "Source/Scene/Stage/TestingStage.h"
)
source_group("Source\\Scene\\Stage" FILES ${Source__Scene__Stage})

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
)
source_group("Source\\UI\\Widget" FILES ${Source__UI__Widget})

set(TAKOEngine__AI
    "TAKOEngine/AI/BaseState.h"
    "TAKOEngine/AI/StateMachine.cpp"
    "TAKOEngine/AI/StateMachine.h"
)
source_group("TAKOEngine\\AI" FILES ${TAKOEngine__AI})

set(TAKOEngine__Editor__Camera
    "TAKOEngine/Editor/Camera/Camera.cpp"
    "TAKOEngine/Editor/Camera/Camera.h"
    "TAKOEngine/Editor/Camera/CameraController.cpp"
    "TAKOEngine/Editor/Camera/CameraController.h"
    "TAKOEngine/Editor/Camera/FreeCameraController.cpp"
    "TAKOEngine/Editor/Camera/FreeCameraController.h"
    "TAKOEngine/Editor/Camera/ThridPersonCameraController.cpp"
    "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
)
source_group("TAKOEngine\\Editor\\Camera" FILES ${TAKOEngine__Editor__Camera})

set(TAKOEngine__Editor__Input
    "TAKOEngine/Editor/Input/InputMap.h"
    "TAKOEngine/Editor/Input/Input.cpp"
    "TAKOEngine/Editor/Input/Input.h"
)
source_group("TAKOEngine\\Editor\\Input" FILES ${TAKOEngine__Editor__Input})

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
    "TAKOEngine/Network/NetworkController.h"
    "TAKOEngine/Network/url.cpp"
    "TAKOEngine/Network/url.h"
    "TAKOEngine/Network/HttpRequest.cpp"
    "TAKOEngine/Network/HttpRequest.h"
    "TAKOEngine/Network/Network.cpp"
    "TAKOEngine/Network/Network.h"
    "TAKOEngine/Network/Socket.cpp"
    "TAKOEngine/Network/Socket.h"
    "TAKOEngine/Network/TCPSocket.cpp"
    "TAKOEngine/Network/TCPSocket.h"
    "TAKOEngine/Network/UDPSocket.cpp"
    "TAKOEngine/Network/UDPSocket.h"
)
source_group("TAKOEngine\\Network" FILES ${TAKOEngine__Network})

set(TAKOEngine__Physics
    "TAKOEngine/Physics/BoundingBoxCollider.cpp"
    "TAKOEngine/Physics/BoundingBoxCollider.h"
    "TAKOEngine/Physics/Collider.cpp"
    "TAKOEngine/Physics/Collider.h"
    "TAKOEngine/Physics/Collision.cpp"
    "TAKOEngine/Physics/Collision.h"
    "TAKOEngine/Physics/CollisionDataManager.cpp"
    "TAKOEngine/Physics/CollisionDataManager.h"
    "TAKOEngine/Physics/MapCollisionManager.cpp"
    "TAKOEngine/Physics/MapCollisionManager.h"
    "TAKOEngine/Physics/ModelCollider.cpp"
    "TAKOEngine/Physics/ModelCollider.h"
    "TAKOEngine/Physics/PlayerCollisionObject.cpp"
    "TAKOEngine/Physics/PlayerCollisionObject.h"
    "TAKOEngine/Physics/SphereCollider.cpp"
    "TAKOEngine/Physics/SphereCollider.h"
    "TAKOEngine/Physics/UnrotatedBoxCollider.cpp"
    "TAKOEngine/Physics/UnrotatedBoxCollider.h"
)
source_group("TAKOEngine\\Physics" FILES ${TAKOEngine__Physics})

set(TAKOEngine__Rendering
    "TAKOEngine/Rendering/ConstantBuffer.h"
    "TAKOEngine/Rendering/DebugRenderer.cpp"
    "TAKOEngine/Rendering/DebugRenderer.h"
    "TAKOEngine/Rendering/Descriptor.cpp"
    "TAKOEngine/Rendering/Descriptor.h"
    "TAKOEngine/Rendering/FrameBuffer.cpp"
    "TAKOEngine/Rendering/FrameBuffer.h"
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
    "TAKOEngine/Rendering/Model.cpp"
    "TAKOEngine/Rendering/Model.h"
    "TAKOEngine/Rendering/ModelDX12.cpp"
    "TAKOEngine/Rendering/ModelDX12.h"
    "TAKOEngine/Rendering/ModelResource.cpp"
    "TAKOEngine/Rendering/ModelResource.h"
    "TAKOEngine/Rendering/PostEffect.cpp"
    "TAKOEngine/Rendering/PostEffect.h"
    "TAKOEngine/Rendering/PostprocessingRenderer.cpp"
    "TAKOEngine/Rendering/PostprocessingRenderer.h"
    "TAKOEngine/Rendering/RenderContext.h"
    "TAKOEngine/Rendering/RenderState.cpp"
    "TAKOEngine/Rendering/RenderState.h"
    "TAKOEngine/Rendering/ResourceManager.cpp"
    "TAKOEngine/Rendering/ResourceManager.h"
    "TAKOEngine/Rendering/Sprite.cpp"
    "TAKOEngine/Rendering/Sprite.h"
    "TAKOEngine/Rendering/TextSprite.cpp"
    "TAKOEngine/Rendering/TextSprite.h"
    "TAKOEngine/Rendering/MyRender.h"
    "TAKOEngine/Rendering/ShadowMapRender.cpp"
    "TAKOEngine/Rendering/ShadowMapRender.h"
    "TAKOEngine/Rendering/FrustumCulling.cpp"
    "TAKOEngine/Rendering/FrustumCulling.h"

)
source_group("TAKOEngine\\Rendering" FILES ${TAKOEngine__Rendering})

set(TAKOEngine__Rendering__HLSL
    "TAKOEngine/Rendering/HLSL/ColorGrading.hlsli"
    "TAKOEngine/Rendering/HLSL/Debug.hlsli"
    "TAKOEngine/Rendering/HLSL/DefaultModel.hlsli"
    "TAKOEngine/Rendering/HLSL/FilterFunctions.hlsli"
    "TAKOEngine/Rendering/HLSL/Finalpass.hlsli"
    "TAKOEngine/Rendering/HLSL/FullScreenQuad.hlsli"
    "TAKOEngine/Rendering/HLSL/GaussianBlur.hlsli"
    "TAKOEngine/Rendering/HLSL/Gizmos.hlsli"
    "TAKOEngine/Rendering/HLSL/Lambert.hlsli"
    "TAKOEngine/Rendering/HLSL/Light.hlsli"
    "TAKOEngine/Rendering/HLSL/Line.hlsli"
    "TAKOEngine/Rendering/HLSL/LuminanceExtraction.hlsli"
    "TAKOEngine/Rendering/HLSL/Mask.hlsli"
    "TAKOEngine/Rendering/HLSL/Phong.hlsli"
    "TAKOEngine/Rendering/HLSL/PostEffect.hlsli"
    "TAKOEngine/Rendering/HLSL/Skinning.hlsli"
    "TAKOEngine/Rendering/HLSL/Skydome.hlsli"
    "TAKOEngine/Rendering/HLSL/Sprite.hlsli"
    "TAKOEngine/Rendering/HLSL/Toon.hlsli"
    "TAKOEngine/Rendering/HLSL/UVScroll.hlsli"
    "TAKOEngine/Rendering/HLSL/ShadowmapFunctions.hlsli"
    "TAKOEngine/Rendering/HLSL/ShadowMap.hlsli"

)
source_group("TAKOEngine\\Rendering\\HLSL" FILES ${TAKOEngine__Rendering__HLSL})

set(TAKOEngine__Rendering__Shaders
    "TAKOEngine/Rendering/Shaders/Shader.cpp"
    "TAKOEngine/Rendering/Shaders/Shader.h"
    "TAKOEngine/Rendering/Shaders/ColorGradingShader.cpp"
    "TAKOEngine/Rendering/Shaders/ColorGradingShader.h"
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
    "TAKOEngine/Rendering/Shaders/PhongShader.cpp"
    "TAKOEngine/Rendering/Shaders/PhongShader.h"
    "TAKOEngine/Rendering/Shaders/SkydomeShader.cpp"
    "TAKOEngine/Rendering/Shaders/SkydomeShader.h"
    "TAKOEngine/Rendering/Shaders/SpriteShader.cpp"
    "TAKOEngine/Rendering/Shaders/SpriteShader.h"
    "TAKOEngine/Rendering/Shaders/ToonShader.cpp"
    "TAKOEngine/Rendering/Shaders/ToonShader.h"
    "TAKOEngine/Rendering/Shaders/UVScrollShader.cpp"
    "TAKOEngine/Rendering/Shaders/UVScrollShader.h"
    "TAKOEngine/Rendering/Shaders/ShadowMapShader.cpp"
    "TAKOEngine/Rendering/Shaders/ShadowMapShader.h"
)
source_group("TAKOEngine\\Rendering\\Shaders" FILES ${TAKOEngine__Rendering__Shaders})

set(TAKOEngine__Runtime
    "TAKOEngine/Runtime/tentacle_lib.cpp"
    "TAKOEngine/Runtime/tentacle_lib.h"
)
source_group("TAKOEngine\\Runtime" FILES ${TAKOEngine__Runtime})

set(TAKOEngine__Sound
    "TAKOEngine/Sound/Audio.cpp"
    "TAKOEngine/Sound/Audio.h"
)
source_group("TAKOEngine\\Sound" FILES ${TAKOEngine__Sound})

set(TAKOEngine__Tool
    "TAKOEngine/Tool/ImGuiRenderer.cpp"
    "TAKOEngine/Tool/ImGuiRenderer.h"
    "TAKOEngine/Tool/Console.cpp"
    "TAKOEngine/Tool/Console.h"
    "TAKOEngine/Tool/Encode.cpp"
    "TAKOEngine/Tool/Encode.h"
    "TAKOEngine/Tool/Logger.cpp"
    "TAKOEngine/Tool/Logger.h"
    "TAKOEngine/Tool/Mathf.cpp"
    "TAKOEngine/Tool/Mathf.h"
    "TAKOEngine/Tool/Singleton.h"
    "TAKOEngine/Tool/Timer.cpp"
    "TAKOEngine/Tool/Timer.h"
    "TAKOEngine/Tool/TransformUtils.cpp"
    "TAKOEngine/Tool/TransformUtils.h"
    "TAKOEngine/Tool/XMFLOAT.h"
    "TAKOEngine/Tool/AssimpImporter.cpp"
    "TAKOEngine/Tool/AssimpImporter.h"
)
source_group("TAKOEngine\\Tool" FILES ${TAKOEngine__Tool})

set(ALL_FILES
    ${no_group_source_files}
    ${External__DirectXTex}
    ${External__imgui-docking}
    ${Source}
    ${Source__GameObject}
    ${Source__GameObject__Character}
    ${Source__GameObject__Character__Enemy}
    ${Source__GameObject__Character__Player}
    ${Source__GameObject__Projectile}
    ${Source__Map}
    ${Source__Scene}
    ${Source__Scene__Stage}
    ${Source__UI__Widget}
    ${TAKOEngine__AI}
    ${TAKOEngine__Editor__Camera}
    ${TAKOEngine__Editor__Input}
    ${TAKOEngine__Effects}
    ${TAKOEngine__GUI}
    ${TAKOEngine__Network}
    ${TAKOEngine__Physics}
    ${TAKOEngine__Rendering}
    ${TAKOEngine__Rendering__HLSL}
    ${TAKOEngine__Rendering__Shaders}
    ${TAKOEngine__Runtime}
    ${TAKOEngine__Sound}
    ${TAKOEngine__Tool}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG     "Game"
    TARGET_NAME_RELEASE   "Game"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG     "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE   "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE   "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE   "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
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

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/include"
    ">"
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/include"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui-docking;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/stb;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/cereal/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/TeNTaCleLib;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer162a/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/nlohmann;"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/openssl/include"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG;"
        "CMAKE_INTDIR=\"Debug\""
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG;"
        "CMAKE_INTDIR=\"Release\""
    ">"
    "_CONSOLE;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /Oi;
            /Gy
        >
        /permissive-;
        /MP;
        /sdl;
        /W3;
        /Zi;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /MANIFEST;
        /DEBUG;
        /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Post build events
################################################################################

set(SOURCE_DIR_DATA "${CMAKE_SOURCE_DIR}/Data")
set(SOURCE_DIR_DEPENDENCY "${CMAKE_SOURCE_DIR}/Dependencies")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SOURCE_DIR_DATA} 
    ${CMAKE_BINARY_DIR}/Data
    COMMENT "Copying data directory to build folder"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SOURCE_DIR_DEPENDENCY}
    ${CMAKE_BINARY_DIR}
    COMMENT "Copying dependency filest to build folder"
)


if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND if "$(Configuration)" == "Release" ${CMAKE_COMMAND} -E copy_directory
	${SOURCE_DIR_DATA}
    	${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/Data
)
endif()

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND if "$(Configuration)" == "Debug" ${CMAKE_COMMAND} -E copy_directory
	${SOURCE_DIR_DATA}
    	${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/Data
)
endif()

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND if "$(Configuration)" == "Release" ${CMAKE_COMMAND} -E copy_directory
	${SOURCE_DIR_DEPENDENCY}
    	${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/
)
endif()

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND if "$(Configuration)" == "Debug" ${CMAKE_COMMAND} -E copy_directory
	${SOURCE_DIR_DEPENDENCY}
    	${CMAKE_SOURCE_DIR}/bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/
)
endif()





################################################################################
# Dependencies
################################################################################
# Link with other targets.

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:"
        "SDL2;"
        "DirectXTK;"
        "DirectXTex"
    ">"
    "$<$<CONFIG:Release>:"
        "SDL2;"
        "DirectXTK;"
        "DirectXTex"
    ">"
    "kernel32;"
    "user32;"
    "gdi32;"
    "winspool;"
    "shell32;"
    "ole32;"
    "oleaut32;"
    "uuid;"
    "comdlg32;"
    "advapi32"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/nlohmann/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/lib/x64/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/lib/x64;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer162a/lib/vs2019/MD/x64/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/openssl/x64/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtk/native/lib/x64/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtex/native/lib/x64/$<CONFIG>"
    ">"
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/nlohmann;"
        "${CMAKE_CURRENT_SOURCE_DIR}/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/assimp/lib/x64/Release;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/Effekseer162a/lib/vs2019/MD/x64/Release;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/openssl/x64/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtex/native/lib/x64/Release;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX/directxtk/native/lib/x64/Release;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/DirectX;"
        "${CMAKE_CURRENT_SOURCE_DIR}/External/SDL2/lib/x64"
    ">"
)


