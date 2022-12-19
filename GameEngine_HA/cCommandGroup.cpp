#include "cCommandScheduler.h"

cCommandGroup::cCommandGroup()
{

}

void cCommandGroup::AddParallelCommand(cCommand* pParallelCommand)
{
	vecParallelCommands.push_back(pParallelCommand);
}

void cCommandGroup::AtStart(void)
{
	return;
}

void cCommandGroup::AtEnd(void)
{
	return;
}

// * Run the current serial command (if any)
// * Run ALL the parallel commands (if any)
void cCommandGroup::Update(double deltaTime)
{
	for (cCommand* command : vecParallelCommands)
	{
		command->Update(deltaTime);
	}
	for (cCommand* command : vecSerialCommands)
	{
		command->Update(deltaTime);
	}
	return;
}


// Returns true only when ALL the serial commands are done and ALL the parallel commands are done
bool cCommandGroup::isDone(void)
{
	return true;
}
