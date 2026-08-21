#pragma once

#include <fstream>
#include <list>
#include <string>

using namespace std;

class Observer;
class ILoggable;

class Subject
{
public:
  Subject();
  virtual ~Subject();

  Subject(const Subject &otherSubject);
  Subject &operator=(const Subject &otherSubject);
  friend ostream &operator<<(ostream &os, const Subject &subject);

  virtual void Attach(Observer *o);

  virtual void AttachAll(Subject *other);

  virtual void Detach(Observer *o);

  virtual void Notify(ILoggable *loggable);

private:
  std::list<Observer *> *observers;
};

class ILoggable
{
public:
  ILoggable();
  ILoggable(const ILoggable &otherSubject);
  ILoggable &operator=(const ILoggable &otherSubject);
  friend ostream &operator<<(ostream &os, const ILoggable &subject);

  virtual ~ILoggable() = default;

  virtual std::string stringToLog() const = 0;
};

class Observer
{

public:
  Observer();
  Observer(const Observer &otherSubject);
  Observer &operator=(const Observer &otherSubject);
  friend ostream &operator<<(ostream &os, const Observer &subject);

  virtual ~Observer() = default;
  virtual void Update(ILoggable *loggable) = 0;
};

class LogObserver : public Observer
{
public:
  LogObserver();
  ~LogObserver();

  LogObserver(const LogObserver &otherSubject);
  LogObserver &operator=(const LogObserver &otherSubject);
  friend ostream &operator<<(ostream &os, const LogObserver &subject);

  void Update(ILoggable *loggable) override;

private:
  std::ofstream *logFile;
};
