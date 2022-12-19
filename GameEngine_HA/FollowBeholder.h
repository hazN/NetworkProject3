#pragma once

#include "cCommand.h"
#include "cMeshObject.h"


class FollowBeholder : public cCommand
{
private:
	FollowBeholder() {};	// Can't be called because it's private
public:
	FollowBeholder(cMeshObject* Beholder[3], int room);

	virtual void AtStart(void);
	virtual void AtEnd(void);
	virtual bool Update(double deltaTime);
	virtual bool isComplete(void);

private:
	cMeshObject* m_Beholder[3];

	// Here's the steps:
	// Turn engines straight down
	// Increase thrust to maximum
	// Slowly lift off (go up) for 5 seconds
	// Start turning engines backwards...
	// ...while we start moving forwards
	enum eStates
	{
		STEP_1_RISE,
		STEP_2_POINT,
		STEP_3_FLY,
		STEP_4_FOLLOW,
		ALL_DONE
	};
	int room = -1;
	glm::vec3 destination;
	double m_StateCountDownTimer;
	float m_EngineAngle = 0.0f;
	float m_EngineThrust = 0.0f;
	eStates m_CurrentState;
};
