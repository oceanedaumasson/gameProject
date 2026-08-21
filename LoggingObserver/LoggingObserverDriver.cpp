#include <iostream>
#include "LoggingObserver.h"
#include "../CommandProcessor/CommandProcessing.h"
#include "../GameEngine/GameEngine.h"
#include "../map/map.h"
#include "../Orders/Orders.h"
#include "../Player/Player.h"
// include your .h file

int main(int argc, char *argv[])
{
    // create a log observer
    LogObserver *logObserver = new LogObserver();

    // create a command processor and attach the log observer to it
    CommandProcessor *commandProcessor = new FileCommandProcessorAdapter("../CommandProcessor/commands.txt");
    commandProcessor->Attach(logObserver);

    // execute some commands and see the log output in the log file

    // get commands from the file and execute them until the "quit" command is reached (which will update the state to "exitprogram")
    while (commandProcessor->getCurrentState() != "exitprogram")
    {
        commandProcessor->getCommand();
    }

    // clean up
    delete commandProcessor;
    commandProcessor = nullptr;

    /*
    TODO: create and attach your classes to log observer,
    run some things to fill the log file
    delete the object
    */

    GameEngine *gameEngine = new GameEngine();

    gameEngine->Attach(logObserver);

    gameEngine->startupPhase();

    delete gameEngine;
    gameEngine = nullptr;

    Player *arwen = new Player("Arwen");
    Player *aragorn = new Player("Aragorn");
    Territory *quebec = new Territory(1, "Quebec", 1);
    Territory *ontario = new Territory(2, "Ontario", 1);
    Territory *alberta = new Territory(3, "Alberta", 1);
    quebec->setArmyCount(10);
    ontario->setArmyCount(7);
    alberta->setArmyCount(5);
    arwen->addTerritory(quebec);
    arwen->addTerritory(ontario);
    quebec->setOwner(arwen);
    ontario->setOwner(arwen);
    aragorn->addTerritory(alberta);
    alberta->setOwner(aragorn);
    arwen->setArmyPool(new int(15));

    Order *deployOrder = new Deploy(arwen, quebec, new int(3));
    deployOrder->Attach(logObserver);
    Order *bombOrder = new Bomb(aragorn, quebec);
    bombOrder->Attach(logObserver);
    Order *airliftOrder = new Airlift(arwen, ontario, quebec, new int(3));
    airliftOrder->Attach(logObserver);
    Order *negotiateOrder = new Negotiate(arwen, aragorn);
    negotiateOrder->Attach(logObserver);

    OrdersList *orders = new OrdersList();
    orders->Attach(logObserver);
    orders->addOrder(deployOrder);
    orders->addOrder(bombOrder);
    orders->addOrder(airliftOrder);
    orders->addOrder(negotiateOrder);

    deployOrder->execute();
    bombOrder->execute();
    airliftOrder->execute();
    negotiateOrder->execute();

    delete orders;

    delete logObserver;
    logObserver = nullptr;
    return 0;
}