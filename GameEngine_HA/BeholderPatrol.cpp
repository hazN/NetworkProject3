#include "BeholderPatrol.h"

#include "globalThings.h"		// for getRandom()

BeholderPatrol::BeholderPatrol(cMeshObject* Beholder, int room)
{
	this->m_Beholder = Beholder;

	this->m_CurrentState = eStates::STEP_1_MOVE;

	this->room = room;

	this->destination = glm::vec3(0.f, 0.f, -32.5f);
}

void BeholderPatrol::AtStart(void)
{
	this->m_CurrentState = eStates::STEP_1_MOVE;
	this->m_EngineAngle = 0.0f;
	this->m_EngineThrust = 0.0f;
}

void BeholderPatrol::AtEnd(void)
{
	this->m_CurrentState = eStates::ALL_DONE;
	return;
}
void BeholderPatrol::RotateLights(int i)
{
	if (i == 1)
		i = 7;
	if (i == 2)
		i = 10;
	if (i == 3)
		i = 13;
		glm::mat4 matRotation = glm::mat4(m_Beholder->qRotation);
		glm::vec4 eulerDir = glm::vec4(angles[0], 1.f);
		eulerDir = matRotation * eulerDir;
		g_pTheLightManager->vecTheLights[i].direction = eulerDir;
		matRotation = glm::mat4(m_Beholder->qRotation);
		eulerDir = glm::vec4(angles[1], 1.f);
		eulerDir = matRotation * eulerDir;
		g_pTheLightManager->vecTheLights[(i + 1)].direction = eulerDir;
		matRotation = glm::mat4(m_Beholder->qRotation);
		eulerDir = glm::vec4(angles[2], 1.f);
		eulerDir = matRotation * eulerDir;
		g_pTheLightManager->vecTheLights[(i + 2)].direction = eulerDir;
}
bool BeholderPatrol::Update(double deltaTime)
{
	switch (this->m_CurrentState)
	{
	case STEP_1_MOVE:
	{
		// ROOM1
		if (room == 1)
		{
			destination = glm::vec3(2.5f, 3.f, -32.5f); //TOP RIGHT CORNER
		} // ROOM 2
		else if (room == 2)
		{
			destination = glm::vec3(-22.5f, 3.f, -82.5f); //TOP RIGHT CORNER
		} // ROOM 3
		else if (room == 3)
		{
			destination = glm::vec3(-47.5f, 3.f, -32.5f); //TOP RIGHT CORNER
		}
		glm::vec3 direction = glm::normalize(m_Beholder->position - destination);
		direction *= deltaTime;
		m_Beholder->position -= direction;
		if (glm::length(m_Beholder->position - destination) < 1.f)
		{
			m_Beholder->position = destination;
			this->m_CurrentState = STEP_2_TURN;
		}
	}
	break;
	case STEP_2_TURN:
	{
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		this->m_EngineAngle -= AdjustAngleDelta / 10;
		m_Beholder->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		RotateLights(room);
		if (this->m_EngineAngle <= -1.55f)
		{
			// Yes, so go to the next state
			m_Beholder->setRotationFromEuler(glm::vec3(0.0f, 4.7f, 0.0f));
			RotateLights(room);
			this->m_CurrentState = STEP_3_MOVE;
		}
	}
	break;
	case STEP_3_MOVE:
		// ROOM1
		if (room == 1)
		{
			destination = glm::vec3(22.5, 3.f, -32.5f); // BOTTOM RIGHT CORNER
		} // ROOM 2
		else if (room == 2)
		{
			destination = glm::vec3(-12.5f, 3.f, -82.5f); // BOTTOM RIGHT CORNER
		} // ROOM 3
		else if (room == 3)
		{
			destination = glm::vec3(-27.5, 3.f, -32.5f); // BOTTOM RIGHT CORNER
		}
		glm::vec3 direction = glm::normalize(m_Beholder->position - destination);
		direction *= deltaTime;
		m_Beholder->position -= direction;
		if (glm::length(m_Beholder->position - destination) < 1.f)
		{
			m_Beholder->position = destination;
			this->m_CurrentState = STEP_4_TURN;
		}
		break;
	case STEP_4_TURN:
	{
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		this->m_EngineAngle -= AdjustAngleDelta / 10;
		m_Beholder->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		RotateLights(room);
		if (this->m_EngineAngle <= -3.1f)
		{
			// Yes, so go to the next state
			m_Beholder->setRotationFromEuler(glm::vec3(0.0f, 9.43f, 0.0f));
			RotateLights(room);
			this->m_CurrentState = STEP_5_MOVE;
		}
	}
	break;
	case STEP_5_MOVE:
	{
		// ROOM1
		if (room == 1)
		{
			destination = glm::vec3(22.5, 3.f, -2.5f); // BOTTOM LEFT CORNER
		} // ROOM 2
		else if (room == 2)
		{
			destination = glm::vec3(-12.5f, 3.f, -62.5f); // BOTTOM LEFT CORNER
		} // ROOM 3
		else if (room == 3)
		{
			destination = glm::vec3(-27.5, 3.f, -2.5f); // BOTTOM LEFT CORNER
		}
		glm::vec3 direction = glm::normalize(m_Beholder->position - destination);
		direction *= deltaTime;
		m_Beholder->position -= direction;
		if (glm::length(m_Beholder->position - destination) < 1.f)
		{
			m_Beholder->position = destination;
			this->m_CurrentState = STEP_6_TURN;
		}
	}
		break;
	case STEP_6_TURN:
	{
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		this->m_EngineAngle -= AdjustAngleDelta / 10;
		m_Beholder->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		RotateLights(room);
		if (this->m_EngineAngle <= -4.65f)
		{
			// Yes, so go to the next state
			m_Beholder->setRotationFromEuler(glm::vec3(0.0f, -4.7f, 0.0f));
			RotateLights(room);
			this->m_CurrentState = STEP_7_MOVE;
		}
	}
	break;
	case STEP_7_MOVE:
	{
		// ROOM1
		if (room == 1)
		{
			destination = glm::vec3(2.5, 3.f, -2.5f); // TOP LEFT CORNER
		} // ROOM 2
		else if (room == 2)
		{
			destination = glm::vec3(-22.5f, 3.f, -62.5f); // TOP LEFT CORNER
		} // ROOM 3
		else if (room == 3)
		{
			destination = glm::vec3(-47.5, 3.f, -2.5f); // TOP LEFT CORNER
		}
		glm::vec3 direction = glm::normalize(m_Beholder->position - destination);
		direction *= deltaTime;
		m_Beholder->position -= direction;
		if (glm::length(m_Beholder->position - destination) < 1.f)
		{
			m_Beholder->position = destination;
			this->m_CurrentState = STEP_8_TURN;
		}
	}
	break;
	case STEP_8_TURN:
	{
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		this->m_EngineAngle -= AdjustAngleDelta / 10;
		m_Beholder->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		RotateLights(room);
		if (this->m_EngineAngle <= -6.2f)
		{
			// Yes, so go to the next state
			m_Beholder->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));
			if (room == 1)
			{
				g_pTheLightManager->vecTheLights[7].direction = glm::vec4(angles[0],1);
				g_pTheLightManager->vecTheLights[8].direction = glm::vec4(angles[1],1);
				g_pTheLightManager->vecTheLights[9].direction = glm::vec4(angles[2],1);
			}
			if (room == 2)
			{
				g_pTheLightManager->vecTheLights[10].direction = glm::vec4(angles[0], 1);
				g_pTheLightManager->vecTheLights[11].direction = glm::vec4(angles[1], 1);
				g_pTheLightManager->vecTheLights[12].direction = glm::vec4(angles[2], 1);
			}
			if (room == 3)
			{
				g_pTheLightManager->vecTheLights[13].direction = glm::vec4(angles[0], 1);
				g_pTheLightManager->vecTheLights[14].direction = glm::vec4(angles[1], 1);
				g_pTheLightManager->vecTheLights[15].direction = glm::vec4(angles[2], 1);
			}
			this->m_CurrentState = ALL_DONE;
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

bool BeholderPatrol::isComplete(void)
{
	if (this->m_CurrentState == ALL_DONE)
	{
		return true;
	}
	return false;
}