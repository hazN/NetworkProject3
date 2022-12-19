#include "FollowBeholder.h"

#include <thread>

#include "globalThings.h"		// for getRandom()
extern glm::vec3 g_cameraEye;// = glm::vec3(0.0, 0.0, -25.0f);
extern glm::vec3 g_cameraTarget;// = glm::vec3(0.0f, 0.0f, 0.0f);

FollowBeholder::FollowBeholder(cMeshObject* Beholder[3], int room)
{
	this->m_Beholder[0] = Beholder[0];
	this->m_Beholder[1] = Beholder[1];
	this->m_Beholder[2] = Beholder[2];

	this->m_CurrentState = eStates::STEP_1_RISE;

	this->room = room - 2;

	this->destination = glm::vec3(0.f, 0.f, -32.5f);
}

void FollowBeholder::AtStart(void)
{
	this->m_CurrentState = eStates::STEP_1_RISE;
	this->room = followBeholder - 2;
	this->m_EngineAngle = 0.0f;
	this->m_EngineThrust = 0.0f;
}

void FollowBeholder::AtEnd(void)
{
	return;
}

bool FollowBeholder::Update(double deltaTime)
{
	switch (this->m_CurrentState)
	{
	case STEP_1_RISE:
	{
		// RISE
		destination = glm::vec3(g_cameraEye.x, 30.f, g_cameraEye.z); //TOP RIGHT CORNER
		glm::vec3 direction = glm::normalize(g_cameraEye - destination);
		//else direction = destination;
		direction *= deltaTime;
		g_cameraEye -= direction * 10.f;
		if (glm::length(g_cameraEye - destination) < 1.f)
		{
			room++;
			if (room >= 3)
				room = 0;
			this->m_CurrentState = STEP_2_POINT;
		}
	}
	break;
	case STEP_2_POINT:
	{
		destination = m_Beholder[room]->position;
		glm::vec3 direction = glm::normalize(g_cameraTarget - destination);
		direction *= deltaTime;
		g_cameraTarget -= direction * 5.f;
		if (glm::length(g_cameraTarget - destination) < 1.f)
		{
			this->m_CurrentState = STEP_3_FLY;
		}
	}
	break;
	case STEP_3_FLY:
	{
		destination = m_Beholder[room]->position + glm::vec3(5, 10.f, 5); //TOP RIGHT CORNER
		glm::vec3 direction = glm::normalize(g_cameraEye - destination);
		direction *= deltaTime;
		g_cameraEye -= direction * 5.f;
		g_cameraTarget = m_Beholder[room]->position;
		if (glm::length(g_cameraEye - destination) < 1.f)
		{
			this->m_CurrentState = STEP_4_FOLLOW;
		}
	}
	break;
	case STEP_4_FOLLOW:
	{
		destination = m_Beholder[room]->position + glm::vec3(5, 10.f, 5); //TOP RIGHT CORNER
		glm::vec3 direction = glm::normalize(g_cameraEye - destination);
		direction *= deltaTime;
		g_cameraEye -= direction;
		g_cameraTarget = m_Beholder[room]->position;
		if (glm::length(g_cameraEye - destination) < 1.f)
		{
			if (room != followBeholder - 1)
			{
				this->m_CurrentState = ALL_DONE;
				break;
			}
			else this->m_CurrentState = STEP_4_FOLLOW;
		}
	}
	break;
	case ALL_DONE:

		return false;
		break;
	};//switch (this->m_CurrentState)

	// Uprate worked (i.e. it's still doing whatever it is that it's supposed to be doing...)
	return true;
}

bool FollowBeholder::isComplete(void)
{
	if (this->m_CurrentState == ALL_DONE)
	{
		return true;
	}
	return false;
}