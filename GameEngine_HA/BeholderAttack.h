#pragma once

#include "cCommand.h"
#include "cMeshObject.h"


class BeholderAttack : public cCommand
{
private:
	BeholderAttack() {};	// Can't be called because it's private
public:
	BeholderAttack(cMeshObject* Beholder[3], int room);

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
		STEP_1_MOVE,
		STEP_2_TURN,
		STEP_3_MOVE,
		STEP_4_TURN,
		STEP_5_MOVE,
		STEP_6_TURN,
		STEP_7_MOVE,
		STEP_8_TURN,
		ALL_DONE
	};
	int room = -1;
	glm::vec3 destination;
	double m_StateCountDownTimer;
	float m_EngineAngle = 0.0f;
	float m_EngineThrust = 0.0f;
	void RotateLights(int i);
	eStates m_CurrentState;
};
