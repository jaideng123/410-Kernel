/* 
    Author: Joshua Capehart
			Based on code by:
			R. Bettati
            Department of Computer Science
            Texas A&M University
			
			A thread scheduler.

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

/*--------------------------------------------------------------------------*/
/* DEFINES 
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* INCLUDES 
/*--------------------------------------------------------------------------*/

#include "machine.H"
#include "thread.H"

/*--------------------------------------------------------------------------*/
/* QUEUE
/*--------------------------------------------------------------------------*/
// Based on code from https://gist.github.com/mycodeschool/7331785
#define MAX_SIZE 101  //maximum size of the array that will store Queue. 
// Creating a class named Queue.
class Queue
{
private:
   Thread * A[MAX_SIZE];
   int front, rear; 
public:
   // Constructor - set front and rear as -1. 
   // We are assuming that for an empty Queue, both front and rear will be -1.
   Queue()
   {
      front = -1; 
      rear = -1;
   }

   // To check wheter Queue is empty or not
   bool IsEmpty()
   {
      return (front == -1 && rear == -1); 
   }

   // To check whether Queue is full or not
   bool IsFull()
   {
      return (rear+1)%MAX_SIZE == front ? true : false;
   }

   // Inserts an element in queue at rear end
   int Enqueue(Thread * x)
   {
      if(IsFull())
      {
         return 1;
      }
      if (IsEmpty())
      { 
         front = rear = 0; 
      }
      else
      {
          rear = (rear+1)%MAX_SIZE;
      }
      A[rear] = x;
      return 0;
   }

   // Removes a thread from front end. 
   Thread * Dequeue()
   {
      if(IsEmpty())
      {
         return 0;
      }
      else if(front == rear ) 
      {
         Thread * t = A[front];
         rear = front = -1;
         return t;
      }
      else
      {
         Thread * t = A[front];
         front = (front+1)%MAX_SIZE;
         return t;
      }
      return 0;
   }
};

/*--------------------------------------------------------------------------*/
/* SCHEDULER
/*--------------------------------------------------------------------------*/

class Scheduler {
   Queue thread_queue;
   Thread * current;
   
public:

   Scheduler();
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed here as well. */

   virtual void yield();
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'threads.h' to
      do the context switch. */

   virtual void resume(Thread * _thread);
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */

   virtual void add(Thread * _thread);
   /* Make the given thread runnable by the scheduler. This function is called
	  typically after thread creation. Depending on the
      implementation, this may not entail more than simply adding the 
      thread to the ready queue (see scheduler_resume). */

   virtual void terminate(Thread * _thread);
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. */
};
	
	

#endif