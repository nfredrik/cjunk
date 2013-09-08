#include <stdio.h>
#include <vector>
//#include <list>

//
// Working version of Observer design pattern!!!
// Try to polish it!!
//



using namespace std;

class Subject;
    
class Observer {
public:
  virtual ~Observer(){};
  virtual void Update(Subject* theChangedSubject) = 0;
protected:
  Observer(){};
};

#if 0
Observer::~Observer() {

}

Observer::Observer() {

}
#endif

//=======================================================================
class Subject {

public:
  virtual ~Subject(){};
  virtual void Attach(Observer*);
  virtual void Detach(Observer*);
  virtual void Notify();

protected:
  Subject(){};
private:
  vector <class Observer*> views; // 3. Coupled only to "interface"
};

   void Subject::Attach (Observer* o) {
     views.push_back(o);
   }
    
   void Subject::Detach (Observer* o) {
     views.push_back(o);    // remove!!!
   }
    
   void Subject::Notify () {
  // 5. Publisher broadcasts
  for (int i = 0; i < views.size(); i++)
    views[i]->Update(this);

   }

//=======================================================================

 class ClockTimer : public Subject {
   public:
   ~ClockTimer(){};   
   ClockTimer(){};   
#if 0
   virtual int GetHour();
   virtual int GetMinute();
   virtual int GetSecond();
#endif
   void Tick();
 };


 void ClockTimer::Tick () {
       // update internal time-keeping state
       // ...
       Notify();
 }


//=======================================================================

  class DigitalClock: public Observer {
   public:
       DigitalClock(ClockTimer*);
       virtual ~DigitalClock();
    
       virtual void Update(Subject*);
           // overrides Observer operation
    
       virtual void Draw();
           // overrides Widget operation;
           // defines how to draw the digital clock
   private:
       ClockTimer* _subject;
   };
    
   DigitalClock::DigitalClock (ClockTimer* s) {
       _subject = s;
       _subject->Attach(this);
   }
    
   DigitalClock::~DigitalClock () {
       _subject->Detach(this);
   }

  void DigitalClock::Update (Subject* theChangedSubject) {
       if (theChangedSubject == _subject) {
           Draw();
	   printf("got an update!!\n");
       }
   }
    
   void DigitalClock::Draw () {
       // get the new values from the subject
   
     //       int hour = _subject->GetHour();
     //       int minute = _subject->GetMinute();
       // etc.
  
       // draw the digital clock
   }


main() {

    ClockTimer* timer = new ClockTimer;
//   AnalogClock* analogClock = new AnalogClock(timer);
   DigitalClock* digitalClock1 = new DigitalClock(timer);
   DigitalClock* digitalClock2 = new DigitalClock(timer);

   timer->Tick();

   timer->Tick();


}


