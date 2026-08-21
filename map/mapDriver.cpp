

#include <iostream>
#include "map.h"
#include <filesystem>
#include <cstdlib>
#include <limits>

/// @brief Checks if the filename is of correct type .map
/// @param filename filename to check extension of
/// @return bool if filename is of type .map
bool validFileType(const string &filename)
{

    filesystem::path filepath(filename);

    return filepath.extension() == ".map";
}
/// @brief Shows second menu to add filepaths to be loaded later on
/// @return vector of all filepaths as strings
vector<string> addFilenames()
{
    vector<string> filenames;
    string filename;

    do
    {
        cout << "Enter the full filepath (q to exit): ";
        cin >> filename;
        if (filename != "q")
        {
            filenames.push_back(filename);
        }
    } while (filename != "q");

    return filenames;
}
/// @brief Gets the console input from user and checks its validity
/// @param minChoice minimum integer value allowed for choice
/// @param maxChoice maximum integer value allowed for choice
/// @return parsed choice as int
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
    // load all necessary objects and variables.
    MapLoader *loader = new MapLoader();
    vector<Map *> maps;
    vector<string> filenames;
    int choice;
    Map *map = nullptr;
    string mapName;
    bool mapFound = false;

    // go through menu and choices
    do
    {

        cout << endl
             << "MENU:" << endl;
        cout << "1.Enter Map Filenames" << endl;
        cout << "2.Load Maps" << endl;
        cout << "3.Show Map Info" << endl;
        cout << "4.Exit" << endl;

        choice = readMenuChoice(1, 4);

        switch (choice)
        {
        case 1:
            // add files
            filenames = addFilenames();
            break;

        case 2:
            // attempt to load all stored maps
            for (string filename : filenames)
            {

                if (validFileType(filename))
                {
                    map = loader->loadMap(filename);
                    // only add to vector if valid map
                    if (map)
                    {
                        if (map->validate())
                        {
                            cout << map->getMapName() << " map loaded sucessfully!" << endl;
                            maps.push_back(map);
                        }
                        else
                        {
                            // delete map pointer as it is not valid
                            cout << map->getMapName() << " map not loaded because it is invalid!" << endl;
                            delete (map);
                        }
                    }
                    else
                    {
                        cout << filename << " not loaded sucessfully!" << endl;
                    }

                    map = nullptr;
                }
                else
                {
                    cout << "Invalid file type! Needs to be a .map" << endl;
                }
            }
            break;

        case 3:
            // show map if entered map exists
            cout << "Enter map name to show: ";
            if (!std::cin)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            cin >> mapName;
            mapFound = false;
            for (Map *m : maps)
            {
                if (m->getMapName() == mapName)
                {
                    cout << *m << endl; // use overloaded << operator to print map info
                    mapFound = true;
                    break; // only show the first map with the same name
                }
            }
            if (!mapFound)
            {
                cout << mapName << " doesn't exist!" << endl;
            }
            break;

        case 4:
            // end program
            cout << endl
                 << "bye! :)" << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
            break;
        }
    } while (choice != 4);

    // delete all map pointers in vector
    for (Map *m : maps)
    {
        delete m;
    }
    maps.clear();

    // remove loader pointer (no longer needed)
    delete (loader);
    loader = nullptr;

    return 0;
}
