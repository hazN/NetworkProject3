#include "BeholderAttack.h"

#include "globalThings.h"		// for getRandom()

BeholderAttack::BeholderAttack(cMeshObject* Beholder[3], int room)
{
	this->m_Beholder[0] = Beholder[0];
	this->m_Beholder[1] = Beholder[1];
	this->m_Beholder[2] = Beholder[2];

	this->m_CurrentState = eStates::STEP_1_MOVE;

	this->room = room;

	this->destination = glm::vec3(0.f, 0.f, -32.5f);
}

void BeholderAttack::AtStart(void)
{
	this->m_CurrentState = eStates::STEP_1_MOVE;
	this->m_EngineAngle = 0.0f;
	this->m_EngineThrust = 0.0f;
}

void BeholderAttack::AtEnd(void)
{
	return;
}
void BeholderAttack::RotateLights(int i)
{
	if (i == 1)
		i = 7;
	if (i == 2)
		i = 10;
	if (i == 3)
		i = 13;
	glm::mat4 matRotation = glm::mat4(m_Beholder[room - 1]->qRotation);
	glm::vec4 eulerDir = glm::vec4(angles[0], 1.f);
	eulerDir = matRotation * eulerDir;
	g_pTheLightManager->vecTheLights[i].direction = eulerDir;
	matRotation = glm::mat4(m_Beholder[room - 1]->qRotation);
	eulerDir = glm::vec4(angles[1], 1.f);
	eulerDir = matRotation * eulerDir;
	g_pTheLightManager->vecTheLights[(i + 1)].direction = eulerDir;
	matRotation = glm::mat4(m_Beholder[room - 1]->qRotation);
	eulerDir = glm::vec4(angles[2], 1.f);
	eulerDir = matRotation * eulerDir;
	g_pTheLightManager->vecTheLights[(i + 2)].direction = eulerDir;
}
bool BeholderAttack::Update(double deltaTime)
{
	switch (this->m_CurrentState)
	{
	case STEP_1_MOVE:
	{
		// ROOM1
		if (room == 1)
		{
			destination = glm::vec3(12.5f, 3.f, -32.5f); //TOP RIGHT CORNER
		} // ROOM 2
		else if (room == 2)
		{
			destination = glm::vec3(-12.5f, 3.f, -62.5f); //TOP RIGHT CORNER
		} // ROOM 3
		else if (room == 3)
		{
			destination = glm::vec3(-37.5f, 3.f, -32.5f); //TOP RIGHT CORNER
		}
		glm::vec3 direction = glm::normalize(m_Beholder[room - 1]->position - destination);
		direction *= deltaTime;
		m_Beholder[room - 1]->position -= direction;
		if (glm::length(m_Beholder[room - 1]->position - destination) < 1.f)
		{
			m_Beholder[room - 1]->position = destination;
			this->m_CurrentState = STEP_2_TURN;
		}
	}
	break;
	case STEP_2_TURN:
	{
		if (room != 2)
		{
			this->m_CurrentState = STEP_3_MOVE;
			break;
		}
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		this->m_EngineAngle -= AdjustAngleDelta / 10;
		m_Beholder[room - 1]->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		RotateLights(room);
		if (this->m_EngineAngle <= -3.1f)
		{
			// Yes, so go to the next state
			m_Beholder[room - 1]->setRotationFromEuler(glm::vec3(0.0f, 9.43f, 0.0f));
			RotateLights(room);
			this->m_CurrentState = STEP_3_MOVE;
		}
	}
	break;
	case STEP_3_MOVE:
		// ROOM1
		if (room == 1)
		{
			destination = glm::vec3(12.5f, 3.f, -47.5f); // BOTTOM RIGHT CORNER
		} // ROOM 2
		else if (room == 2)
		{
			destination = glm::vec3(-12.5f, 3.f, -47.5f); // BOTTOM RIGHT CORNER
		} // ROOM 3
		else if (room == 3)
		{
			destination = glm::vec3(-37.5, 3.f, -47.5f); // BOTTOM RIGHT CORNER
		}
		glm::vec3 direction = glm::normalize(m_Beholder[room - 1]->position - destination);
		direction *= deltaTime;
		m_Beholder[room - 1]->position -= direction;
		if (glm::length(m_Beholder[room - 1]->position - destination) < 1.f)
		{
			m_Beholder[room - 1]->position = destination;
			this->m_CurrentState = STEP_4_TURN;
		}
		break;
	case STEP_4_TURN:
	{
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		if (room == 2)
		{
			this->m_CurrentState = STEP_5_MOVE;
			break;
		}
		if (room == 1)
			this->m_EngineAngle += AdjustAngleDelta / 10;
		if (room == 3)
			this->m_EngineAngle -= AdjustAngleDelta / 10;
		m_Beholder[room - 1]->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		RotateLights(room);
		if (room == 1)
		{
			if (this->m_EngineAngle >= 1.55f)
			{
				// Yes, so go to the next state
				m_Beholder[room - 1]->setRotationFromEuler(glm::vec3(0.0f, 1.57f, 0.0f));
				RotateLights(room);
				this->m_CurrentState = STEP_5_MOVE;
			}
		}
		if (room == 3)
		{
			if (this->m_EngineAngle <= -1.55f)
			{
				// Yes, so go to the next state
				m_Beholder[room - 1]->setRotationFromEuler(glm::vec3(0.0f, -1.57f, 0.0f));
				RotateLights(room);
				this->m_CurrentState = STEP_5_MOVE;
			}
		}
	}
	break;
	case STEP_5_MOVE:
	{
		if (room == 2)
		{
			if (glm::length(m_Beholder[0]->position - m_Beholder[1]->position) < 9.f || glm::length(m_Beholder[2]->position - m_Beholder[1]->position) < 9.f)
			{
				this->m_CurrentState = STEP_6_TURN;
			}
			break;
		}
		else
		{
			if (room == 1)
			{
				destination = glm::vec3(-6.5f, 3.f, -47.5f); // BOTTOM LEFT CORNER
			}
			if (room == 3)
			{
				destination = glm::vec3(-17.5f, 3.f, -47.5f); // BOTTOM LEFT CORNER
			}
			glm::vec3 direction = glm::normalize(m_Beholder[room - 1]->position - destination);
			direction *= deltaTime;
			m_Beholder[room - 1]->position -= direction;
			if (glm::length(m_Beholder[room - 1]->position - destination) < 1.f)
			{
				m_Beholder[room - 1]->position = destination;
				this->m_CurrentState = ALL_DONE;
			}
		}
	}
	break;
	case STEP_6_TURN:
	{
		float scale = m_Beholder[1]->scaleXYZ.x;
		const float ROTATION_SPEED = 1.1f;	// 2 degrees per second
		float AdjustAngleDelta = ROTATION_SPEED * deltaTime;
		this->m_EngineAngle -= AdjustAngleDelta;
		m_Beholder[1]->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		m_Beholder[1]->SetUniformScale(scale -= 0.001f);
		m_Beholder[2]->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		m_Beholder[2]->SetUniformScale(scale -= 0.001f);
		m_Beholder[0]->setRotationFromEuler(glm::vec3(0.0f, this->m_EngineAngle, 0.0f));
		m_Beholder[0]->SetUniformScale(scale -= 0.001f);
		RotateLights(1);
		RotateLights(2);
		RotateLights(3);
		RotateLights(room);
		if (this->m_EngineAngle <= -50.f || scale <= 0.f)
		{
			// Yes, so go to the next state
			m_Beholder[1]->bIsVisible = false;
			m_Beholder[2]->bIsVisible = false;
			m_Beholder[0]->bIsVisible = false;
			for (int i = 7; i < 16; i++)
			{
				g_pTheLightManager->vecTheLights[i].param2.x = 0;
			}
			RotateLights(room);
			this->m_CurrentState = STEP_7_MOVE;
		}
	}
	break;
	case STEP_7_MOVE:
	{
		this->m_CurrentState = ALL_DONE;
	}
	break;
	case ALL_DONE:

		return false;
		break;
	};//switch (this->m_CurrentState)

	// Uprate worked (i.e. it's still doing whatever it is that it's supposed to be doing...)
	return true;
}

bool BeholderAttack::isComplete(void)
{
	if (this->m_CurrentState == ALL_DONE)
	{
		return true;
	}
	return false;
}