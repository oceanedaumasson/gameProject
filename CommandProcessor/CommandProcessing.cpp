#include "CommandProcessing.h"

// Command class implementation

/// @brief Default constructor of Command class
Command::Command()
{
    commandName = nullptr;
    commandEffect = nullptr;
}
/// @brief contructor of command class that set the commandName
/// @param commandName string name of command
Command::Command(string commandName)
{
    this->commandName = new string(commandName);
    commandEffect = nullptr;
}
/// @brief constructor of command class that set the commandName and commandEffect
/// @param commandName string name of command
/// @param commandEffect string effect of command
Command::Command(string commandName, string commandEffect)
{
    this->commandName = new string(commandName);
    this->commandEffect = new string(commandEffect);
}
/// @brief destructor of command class that deallocate the memory of commandName and commandEffect
Command::~Command()
{
    delete (commandName);
    commandName = nullptr;

    delete (commandEffect);
    commandEffect = nullptr;
}
/// @brief copy constructor of command class
/// @param command command to copy from
Command::Command(const Command &command)
{
    // dont delete the existing commandName and commandEffect
    if (command.commandName != nullptr)
    {
        commandName = new string(*command.commandName);
    }
    else
    {
        commandName = nullptr;
    }

    if (command.commandEffect != nullptr)
    {
        commandEffect = new string(*command.commandEffect);
    }
    else
    {
        commandEffect = nullptr;
    }
}
/// @brief assignment operator of command class
/// @param command command to assign from
/// @return reference to the assigned command
Command &Command::operator=(const Command &command)
{
    if (this != &command)
    {
        // delete the existing commandName and commandEffect
        delete (commandName);
        commandName = nullptr;

        delete (commandEffect);
        commandEffect = nullptr;

        // copy the new commandName and commandEffect
        if (command.commandName != nullptr)
        {
            commandName = new string(*command.commandName);
        }
        else
        {
            commandName = nullptr;
        }

        if (command.commandEffect != nullptr)
        {
            commandEffect = new string(*command.commandEffect);
        }
        else
        {
            commandEffect = nullptr;
        }
    }
    return *this;
}
/// @brief stream insertion operator of command class that print the commandName and commandEffect
/// @param os stream to insert to
/// @param command to print
/// @return stream after insertion
ostream &operator<<(ostream &os, const Command &command)
{
    os << "Command Name: " << (command.commandName != nullptr ? *command.commandName : "null");
    os << ", Command Effect: " << (command.commandEffect != nullptr ? *command.commandEffect : "null") << endl;
    return os;
}
/// @brief getter for commandName
/// @return string name of command
string Command::getCommandName() const
{
    return (commandName != nullptr) ? *commandName : "";
}
/// @brief getter for commandEffect
/// @return string effect of command
string Command::getCommandEffect() const
{
    return (commandEffect != nullptr) ? *commandEffect : "";
}
/// @brief setter for commandName
/// @param commandName string name of command
void Command::setCommandName(string commandName)
{
    // delete the existing commandName
    delete (this->commandName);
    this->commandName = nullptr;

    // set the new commandName
    this->commandName = new string(commandName);
}
/// @brief setter for commandEffect
/// @param effect string effect of command
void Command::saveEffect(string effect)
{
    // delete the existing commandEffect
    delete (commandEffect);
    commandEffect = nullptr;

    // set the new commandEffect
    commandEffect = new string(effect);

    Notify(this);
}
/// @brief gives the commands effect as a string
/// @return string of command effect
string Command::stringToLog() const
{
    return "Command's effect: " + (commandEffect ? *commandEffect : "null");
}

// CommandProcessor class implementation

/// @brief commandProcessor constructor
CommandProcessor::CommandProcessor()
{
    gameEngine = new GameEngine();
    commandList = new vector<Command *>();
}
CommandProcessor::CommandProcessor(GameEngine *gameEngine)
{
    this->gameEngine = gameEngine;
    commandList = new vector<Command *>();
}
/// @brief commandProcessor destructor that deallocate the memory of commandList and its commands
CommandProcessor::~CommandProcessor()
{

    // delete all commands in the commandList
    for (Command *command : *commandList)
    {
        delete (command);
    }
    delete (commandList);
    commandList = nullptr;
}
/// @brief copy constructor of commandProcessor class that create a deep copy of the commandList of other
/// @param other commandProcessor to copy from
CommandProcessor::CommandProcessor(const CommandProcessor &other)
{
    gameEngine = new GameEngine(*other.gameEngine);
    // dont delete the commandList from other
    commandList = new vector<Command *>();
    for (Command *command : *other.commandList)
    {
        // creates new pointer to a new command object with the same values as the command in other
        // deep copy of commandList
        commandList->push_back(new Command(*command));
    }
}
/// @brief assignment operator of commandProcessor class that create a deep copy of the commandList of other and delete the existing commandList and its commands
/// @param other commandProcessor to copy from
/// @return reference to the current commandProcessor
CommandProcessor &CommandProcessor::operator=(const CommandProcessor &other)
{
    if (this != &other)
    {
        // delete the existing commandList and its commands
        for (Command *command : *commandList)
        {
            delete (command);
        }
        delete (commandList);
        commandList = nullptr;

        gameEngine = new GameEngine(*other.gameEngine);

        // copy the new commandList and its commands
        commandList = new vector<Command *>();
        for (Command *command : *other.commandList)
        {
            // creates new pointer to a new command object with the same values as the command in other
            // deep copy of commandList
            commandList->push_back(new Command(*command));
        }
    }
    return *this;
}
/// @brief stream insertion operator of commandProcessor class that print the commandList
/// @param os stream to insert to
/// @param commandProcessor to print
/// @return stream after insertion
ostream &operator<<(ostream &os, const CommandProcessor &commandProcessor)
{
    string state = commandProcessor.gameEngine->getCurrentState()->getName();
    os << "Current State: " << state << endl;
    os << "Command List:" << endl;
    for (Command *command : *commandProcessor.commandList)
    {
        os << "\t" << *command << endl;
    }
    return os;
}
/// @brief getCommand method of commandProcessor class that call the readCommand method to get a command from the user
void CommandProcessor::getCommand()
{
    readCommand();
}
/// @brief saveCommand method of commandProcessor class that create a new command with the given commandName and add it to the commandList
/// @param commandName string name of the command to save
void CommandProcessor::saveCommand(string commandName)
{
    // create a new command with the given commandName and add it to the commandList

    Command *command = new Command(commandName);
    if (validate(command))
    {

        commandList->push_back(command);

        Notify(this);
        AttachAll(command);

        stringstream ss = stringstream(commandName);
        string commandNameFirstSegment;
        ss >> commandNameFirstSegment;
        command->saveEffect("Command '" + commandName + "' executed. New state: " + getCurrentState());
        cout << "Command executed: " << commandName << endl;
    }
    else
    {
        cout << "Error: Invalid command '" << commandName << "' for the current state of the game." << endl;
        cout << "Ignoring command." << endl;
        delete (command);
    }
}
/// @brief readCommand method of commandProcessor class that read a command from the user and save it to the commandList
void CommandProcessor::readCommand()
{
    string commandName;
    cout << "Enter a command: ";
    if (cin.peek() == '\n')
    {
        cin.ignore();
    }
    getline(cin, commandName);
    saveCommand(commandName);
}
/// @brief method to check if command name is valid
/// @param commandName to check
/// @return bool true if command name is valid, false otherwise
bool CommandProcessor::validateCommandName(string commandName)
{
    stringstream ss(commandName);
    string word;
    ss >> word; // get the first word of the commandName

    if (find(begin(validCommands), end(validCommands), word) != end(validCommands))
    {
        if (word == "loadmap" || word == "addplayer")
        {
            // check if the commandName has a second word (argument)
            string argument;
            if (!(ss >> argument))
            {
                cout << "Error: Command '" << word << "' requires an argument." << endl;
                return false;
            }
        }

        return validateCommandState(word);
    }

    return false;
}
/// @brief method to check if command name is valid for the given state
/// @param commandName to check
/// @param state current state of the game
/// @return bool true if command name is valid for the given state, false otherwise
bool CommandProcessor::validateCommandState(string commandName)
{
    string state = gameEngine->getCurrentState()->getName();
    // check if the commandName is valid for the given state
    if (state == "Start")
    {
        return commandName == "loadmap";
    }
    else if (state == "Map Loaded")
    {
        return commandName == "validatemap" || commandName == "loadmap";
    }
    else if (state == "Map Validated")
    {
        return commandName == "addplayer";
    }
    else if (state == "Players Added")
    {
        return commandName == "gamestart" || commandName == "addplayer";
    }
    else if (state == "Win")
    {
        return commandName == "replay" || commandName == "quit";
    }

    return false;
}
/// @brief validate method of commandProcessor class that check if the given command is valid for the current state of the game
/// @param command to validate
/// @param gameEngine with current state
/// @return bool true if the command is valid for the current state of the game, false otherwise
bool CommandProcessor::validate(Command *command)
{
    // check if the commandName of the given command is in the commandList

    string commandName = command->getCommandName();

    return validateCommandName(commandName);
}
/// @brief getLatestCommand method of commandProcessor class that returns the latest command in the commandList
/// @return pointer to the latest command in the commandList, or nullptr if the commandList is empty
Command *CommandProcessor::getLatestCommand()
{
    if (commandList->empty())
    {
        return nullptr;
    }
    return commandList->back();
}

/// @brief getter for current state of the game
/// @return string current state of the game
string CommandProcessor::getCurrentState() const
{
    return this->gameEngine->getCurrentState()->getName();
}
/// @brief gives command name as string to log
/// @return string of latest saved command name
string CommandProcessor::stringToLog() const
{
    Command *latestCommand = commandList->empty() ? nullptr : commandList->back();
    return "Command: " + (latestCommand != nullptr ? latestCommand->getCommandName() : "null");
}

// implementation of fileCommandProcessor class that inherits from commandProcessor class

/// @brief Constructor for FileCommandProcessorAdapter
/// @param filename to read commands from
/// @param gameEngine pointer to the game engine
FileCommandProcessorAdapter::FileCommandProcessorAdapter(string filename) : CommandProcessor()
{
    fileName = new string(filename);
    flr = new FileLineReader(filename);
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(string filename, GameEngine *gameEngine) : CommandProcessor(gameEngine)
{
    fileName = new string(filename);
    flr = new FileLineReader(filename);
}
/// @brief Destructor for FileCommandProcessorAdapter that deallocate the memory of fileName and flr
FileCommandProcessorAdapter::~FileCommandProcessorAdapter()
{
    delete (fileName);
    fileName = nullptr;

    delete (flr);
    flr = nullptr;
}
/// @brief Copy constructor for FileCommandProcessorAdapter that also calls the parent copy constructor to copy the commandList
/// @param other FileCommandProcessorAdapter to copy from
FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other) : CommandProcessor(other)
{
    if (other.fileName != nullptr)
    {
        fileName = new string(*other.fileName);
    }
    else
    {
        fileName = nullptr;
    }

    if (other.flr != nullptr)
    {
        flr = new FileLineReader(*other.flr);
    }
    else
    {
        flr = nullptr;
    }
}
/// @brief assignment operator for FileCommandProcessorAdapter that also calls the parent assignment operator to copy the commandList and delete the existing fileName and flr before copying
/// @param other FileCommandProcessorAdapter to copy from
/// @return FileCommandProcessorAdapter& reference to the assigned object
FileCommandProcessorAdapter &FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter &other)
{
    if (this != &other)
    {
        CommandProcessor::operator=(other);

        delete (fileName);
        fileName = nullptr;

        delete (flr);
        flr = nullptr;

        if (other.fileName != nullptr)
        {
            fileName = new string(*other.fileName);
        }
        else
        {
            fileName = nullptr;
        }

        if (other.flr != nullptr)
        {
            flr = new FileLineReader(*other.flr);
        }
        else
        {
            flr = nullptr;
        }
    }
    return *this;
}
/// @brief stream insertion operator for FileCommandProcessorAdapter that also calls the parent stream insertion operator to print the commandList and print the fileName and flr
/// @param os stream to insert to
/// @param fileCommandProcessor to print
/// @return stream after insertion
ostream &operator<<(ostream &os, const FileCommandProcessorAdapter &fileCommandProcessor)
{
    os << static_cast<const CommandProcessor &>(fileCommandProcessor);
    os << "File Name: " << (fileCommandProcessor.fileName != nullptr ? *fileCommandProcessor.fileName : "null") << endl;
    os << "File Line Reader: ";
    if (fileCommandProcessor.flr != nullptr)
    {
        os << *fileCommandProcessor.flr << endl;
    }
    else
    {
        os << "null" << endl;
    }

    return os;
}
/// @brief override of readCommand method for FileCommandProcessorAdapter that read a command from the file and save it to the commandList
void FileCommandProcessorAdapter::readCommand()
{
    string commandName = flr->readLineFromFile();
    if (!commandName.empty())
    {
        saveCommand(commandName);
    }
}

// implementation of FileLineReader class

/// @brief constructor for FileLineReader that open the file with the given filename and initialize the fileStream
/// @param filename to open
FileLineReader::FileLineReader(string filename)
{
    this->filename = new string(filename);

    fileStream = new fstream(filename);
    if (!fileStream->is_open())
    {
        cout << "Error: Could not open file '" << filename << "'." << endl;
        delete (fileStream);
        fileStream = nullptr;
    }
}
/// @brief destructor for FileLineReader that close the fileStream and deallocate its memory
FileLineReader::~FileLineReader()
{

    delete (filename);
    filename = nullptr;

    if (fileStream != nullptr)
    {
        fileStream->close();
        delete (fileStream);
        fileStream = nullptr;
    }
}
/// @brief copy constructor for FileLineReader that create a shallow copy of the fileStream pointer (since fstream is not copyable)
/// @param other FileLineReader to copy from
FileLineReader::FileLineReader(const FileLineReader &other)
{

    this->filename = new string(*other.filename);

    this->fileStream = new fstream(*filename);

    if (other.fileStream != nullptr && this->fileStream->is_open())
    {
        fileStream->seekg(other.fileStream->tellg());
    }
}
/// @brief assignment operator for FileLineReader that create a shallow copy of the fileStream pointer (since fstream is not copyable)
/// @param other FileLineReader to copy from
/// @return FileLineReader& reference to the assigned object
FileLineReader &FileLineReader::operator=(const FileLineReader &other)
{
    if (this != &other)
    {

        delete (filename);
        filename = nullptr;

        if (fileStream != nullptr)
        {
            fileStream->close();
            delete (fileStream);
            fileStream = nullptr;
        }

        this->filename = new string(*other.filename);
        this->fileStream = new fstream(*filename);

        if (other.fileStream != nullptr && this->fileStream->is_open())
        {
            fileStream->seekg(other.fileStream->tellg());
        }
    }
    return *this;
}
/// @brief stream insertion operator for FileLineReader
/// @param os stream to insert to
/// @param fileLineReader FileLineReader to print
/// @return stream after insertion
ostream &operator<<(ostream &os, const FileLineReader &fileLineReader)
{
    os << "FileLineReader object for file: " << *fileLineReader.filename << endl;
    return os;
}
/// @brief reads a line from the file and returns it as a string, if the fileStream is not initialized or if there is an error reading from the file, it will print an error message and return an empty string
/// @param fileName to read a line from
/// @return string line read from the file, or an empty string if there is an error
string FileLineReader::readLineFromFile()
{
    if (fileStream == nullptr)
    {
        cout << "Error: File stream is not initialized." << endl;
        return "";
    }

    string line;

    if (fileStream->eof())
    {
        // let user know that there are no more commands left in the file to read
        cout << "End of file reached for file '" << *filename << "'." << endl;
        return "";
    }

    if (getline(*fileStream, line))
    {
        return line;
    }
    else
    {
        cout << "Error: Could not read from file '" << *filename << "'." << endl;
        return "";
    }
}