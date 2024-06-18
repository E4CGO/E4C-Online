#pragma once

#include "EventScene.h"
#include "PlayableCharacter.h"

class EventOpening : public EventScene
{
public:
	EventOpening(PlayableCharacter* mario, PlayableCharacter* luig);

	void Update(float elapsedTime);
private:
	PlayableCharacter* mario;
	PlayableCharacter* luig;
};
