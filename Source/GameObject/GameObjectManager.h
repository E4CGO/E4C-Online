#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/GameObject.h"

class GameObjectManager : public ObjectManager<GameObject>, public Singleton<GameObjectManager>
{
	friend class Singleton<GameObjectManager>;
protected:
	GameObjectManager() = default;
	~GameObjectManager() = default;
};