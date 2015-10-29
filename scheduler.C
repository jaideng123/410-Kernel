#include "scheduler.H"
   Scheduler::Scheduler(){
    current = 0;
    return;
   }
   /* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed here as well. */

    void Scheduler::yield(){
      current = thread_queue.Dequeue();
      Thread::dispatch_to(current);
    }
   /* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'threads.h' to
      do the context switch. */

    void Scheduler::resume(Thread * _thread){
      thread_queue.Enqueue(_thread);
    }
   /* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */

    void Scheduler::add(Thread * _thread){
      thread_queue.Enqueue(_thread);
    }
   /* Make the given thread runnable by the scheduler. This function is called
	  typically after thread creation. Depending on the
      implementation, this may not entail more than simply adding the 
      thread to the ready queue (see scheduler_resume). */

    void Scheduler::terminate(Thread * _thread){
      if(_thread == current){
        yield();
      }
      else{
        Queue temp;
        //filter queue by ThreadId
        while(!thread_queue.IsEmpty()){
          Thread * t = thread_queue.Dequeue();
          if(t->ThreadId() != _thread->ThreadId())
            temp.Enqueue(t);
        }
        while(!temp.IsEmpty()){
          Thread * t = temp.Dequeue();
          thread_queue.Enqueue(t);
        }
      }
    }
   /* Remove the given thread from the scheduler in preparation for destruction
      of the thread. */