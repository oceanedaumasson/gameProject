#include "GameEngine.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{

	cout << "--------------------------------------------" << endl;
	cout << "     Game Engine Driver - Startup Phase     " << endl;
	cout << "--------------------------------------------" << endl;

	GameEngine *engine = new GameEngine();
	engine->startupPhase();

	cout << "\n-- Startup Phase completed successfully --" << endl;

	if (engine->getCurrentState()->getName() == "Assign Reinforcement")
	{
		cout << "\nEngine is now in the PLAY PHASE." << endl;
	}
	else
	{
		cout << "Engine failed to transition to the PLAY PHASE." << endl;
	}

	cout << "\nPress ENTER to close the terminal window..." << endl;

	cin.get();

	// Memory cleanup
	delete engine;
	engine = nullptr;

	return 0;
}