#include "LoggingObserver.h"
#include <ctime>
#include <iostream>

using namespace std;

// Subject implementation

/// @brief Subject constructor initializes observer list
Subject::Subject()
{
    observers = new list<Observer *>();
}
/// @brief Subject desctructor deletes observers list
Subject::~Subject()
{
    delete observers;
    observers = nullptr;
}
/// @brief Attaches observer object to a subject
/// @param o observer to attach to subject
void Subject::Attach(Observer *o)
{
    observers->push_back(o);
}
/// @brief attaches all observers of this subject to passed through subject
/// @param other subject to attach observers to
void Subject::AttachAll(Subject *other)
{
    for (Observer *o : *observers)
    {
        other->Attach(o);
    }
}
/// @brief detaches observer from subject
/// @param o observer to detach
void Subject::Detach(Observer *o)
{
    observers->remove(o);
}
/// @brief updates all observers attached to subject
/// @param loggable object to update observers with
void Subject::Notify(ILoggable *loggable)
{
    for (Observer *o : *observers)
    {
        o->Update(loggable);
    }
}
/// @brief copy constructor of subject class
/// @param otherSubject to copy from
Subject::Subject(const Subject &otherSubject)
{
    observers = new list<Observer *>(*otherSubject.observers);
}
/// @brief assignment operator of subject class
/// @param otherSubject to assign from
/// @return new subject
Subject &Subject::operator=(const Subject &otherSubject)
{
    if (this != &otherSubject)
    {
        delete observers;
        observers = new list<Observer *>(*otherSubject.observers);
    }
    return *this;
}
/// @brief stream insertion overload
/// @param os stream to insert into
/// @param subject to print
/// @return stream after insertion
ostream &operator<<(ostream &os, const Subject &subject)
{
    os << "Subject[observers=" << subject.observers->size() << "]";
    return os;
}

// LogObserver implementation

/// @brief LogObserver constructor, opens filestream
LogObserver::LogObserver() : Observer()
{
    time_t now = time(0);
    string timeStr = ctime(&now);
    string filename = timeStr.substr(0, timeStr.length() - 1) + "-gamelog.txt"; // removing \n from timeStr

    logFile = new ofstream();

    logFile->open(filename, ios::app); // open file to append to end of it (this is incase multiple are created in the same second)

    if (!logFile->is_open())
    {
        cout << "Error opening log file: " << filename << endl;
    }
}
/// @brief LogObserver destructor closes filestream
LogObserver::~LogObserver()
{
    logFile->close();
    delete logFile;
    logFile = nullptr;
}
/// @brief writes loggable string to opened file
/// @param loggable object to log
void LogObserver::Update(ILoggable *loggable)
{
    if (logFile->is_open())
    {
        (*logFile) << loggable->stringToLog() << endl;
    }
    else
    {
        cout << "Error: log file is not open." << endl;
    }
}
/// @brief copy constructor of logObserver
/// @param otherLogObserver to copy from
LogObserver::LogObserver(const LogObserver &otherLogObserver)
{
    time_t now = time(0);
    string timeStr = ctime(&now);
    string filename = timeStr.substr(0, timeStr.length() - 1) + "-gamelog.txt"; // removing \n from timeStr

    logFile = new ofstream();

    logFile->open(filename, ios::app); // open file to append to end of it (this is incase multiple are created in the same second)

    if (!logFile->is_open())
    {
        cout << "Error opening log file: " << filename << endl;
    }
}
/// @brief assignment operator for logObserver
/// @param otherLogObserver to assign from
/// @return new logObserver
LogObserver &LogObserver::operator=(const LogObserver &otherLogObserver)
{
    if (this != &otherLogObserver)
    {
        if (logFile->is_open())
            logFile->close();
        delete logFile;
        time_t now = time(0);
        string timeStr = ctime(&now);
        string filename = timeStr.substr(0, timeStr.length() - 1) + "-gamelog.txt"; // removing \n from timeStr

        logFile = new ofstream();

        logFile->open(filename, ios::app); // open file to append to end of it (this is incase multiple are created in the same second)

        if (!logFile->is_open())
        {
            cout << "Error opening log file: " << filename << endl;
        }
    }
    return *this;
}
/// @brief stream insertion overload
/// @param os stream to insert into
/// @param logObserver to print
/// @return stream after insertion
ostream &operator<<(ostream &os, const LogObserver &logObserver)
{
    os << "LogObserver[logFile=" << (logObserver.logFile->is_open() ? "open" : "closed") << "]";
    return os;
}

// loggable implementation (doesnt actually have any but the three methods are mandatory)

ILoggable::ILoggable() {}

ILoggable::ILoggable(const ILoggable &otherLoggable) {}

ILoggable &ILoggable::operator=(const ILoggable &otherLoggable)
{

    return *this;
}

ostream &operator<<(ostream &os, const ILoggable &loggable)
{
    os << "ILoggable[log=" << loggable.stringToLog() << "]";
    return os;
}

// observer implementation also doesnt actually have anything in it

Observer::Observer() {}

Observer::Observer(const Observer &otherObserver) {}

Observer &Observer::operator=(const Observer &otherObserver)
{
    // No owned resources in base; derived classes should handle their own state
    return *this;
}

ostream &operator<<(ostream &os, const Observer &observer)
{
    os << "Observer[]";
    return os;
}