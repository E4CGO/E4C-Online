#include "EventScene.h"

EventScene::~EventScene()
{
	for (EventAction* action : actions)
	{
		delete action;
	}
}

void EventScene::Update(float elapsedTime)
{
	if (IsEnd()) return;

	EventAction* action = actions.at(step);
	action->Update(elapsedTime);
	if (action->IsNextAction())
	{
		step++;
	}
}