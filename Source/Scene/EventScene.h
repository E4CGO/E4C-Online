#pragma once

#include <vector>
#include "EventAction.h"

class EventScene
{
public:
	EventScene() {}
	~EventScene();

	virtual void Update(float elapsedTime);

	bool IsEnd() { return step >= actions.size(); }
protected:
	int step = 0;
	std::vector<EventAction*> actions;
};
