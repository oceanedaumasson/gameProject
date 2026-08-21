#include "CommandProcessing.h"
#include "../GameEngine/GameEngine.h"

int readMenuChoice(int minChoice, int maxChoice)
{
    int choice;
    while (true)
    {
        cout << "Enter choice: ";
        if (cin >> choice)
        {
            if (choice >= minChoice && choice <= maxChoice)
            {
                return choice;
            }
            cout << "Please enter a number between " << minChoice << " and " << maxChoice << ".\n";
        }
        else
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        cout << "Error: Invalid number of arguments. Usage: ./CommandProcessorDriver - file [filename] or ./CommandProcessorDriver - console" << endl;
        return 1;
    }

    CommandProcessor *commandProcessor;

    if (argv[1] == string("-console"))
    {
        commandProcessor = new CommandProcessor();
    }
    else if (argc == 3 && argv[1] == string("-file"))
    {
        string filename = argv[2];
        commandProcessor = new FileCommandProcessorAdapter(filename);
    }
    else
    {
        cout << "Error: Invalid arguments. Usage: ./CommandProcessorDriver - file [filename] or ./CommandProcessorDriver - console" << endl;
        return 1;
    }

    int choice;

    do
    {
        cout << "\n\n1. Load and run next command" << endl;
        cout << "2. Print current game state" << endl;
        cout << "3. Print command history" << endl;
        cout << "4. Exit" << endl;
        choice = readMenuChoice(1, 4);

        switch (choice)
        {
        case 1:
        {
            commandProcessor->getCommand();
            break;
        }
        case 2:
            cout << commandProcessor->getCurrentState() << endl;
            break;
        case 3:
            cout << *commandProcessor << endl;
            break;
        case 4:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }

    } while (choice != 4);

    delete commandProcessor;
    return 0;
}