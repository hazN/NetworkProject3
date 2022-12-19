#include "globalThings.h"


// The variable is HERE.
cLightManager* g_pTheLightManager = NULL;
std::vector< cMeshObject* > g_pMeshObjects;
int currentLight = 0;
int currentModel = 0;
cVAOManager* pVAOManager = new cVAOManager;
int theEditMode = eEditMode::MOVING_CAMERA;
int gameMode = eGameMode::Click;
bool isTyping = false;
PhysicsSystem g_physics_system;
PhysicsEngine g_engine;
GLFWwindow* window;
int numHitTargets = 0;
bool menuMode = true;
glm::vec3 angles[3] = {glm::vec3(0.651306f, -0.335724f, -0.680554f),
						glm::vec3(0.355136f, -0.425400f, -0.832417f),
						glm::vec3(-0.165933f, -0.375744f, -0.911748f) };
float rotateSpeed = 0.001f;
int followBeholder = 0;
bool attack = false;
Player* players[4];
UserInput g_UserInput;