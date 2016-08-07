#ifndef VERBATIM_MUTEX_HPP
#define VERBATIM_MUTEX_HPP

#include "verbatim_realloc.hpp"

/// Mutex class.
class Mutex
{
  private:
    /// Actual mutex.
    SDL_mutex *m_mutex;

  private:
    /// Deleted copy constructor.
    Mutex(const Mutex&) = delete;
    /// Deleted assignment.
    Mutex& operator=(const Mutex&) = delete;

  public:
    /// Constructor.
    Mutex() :
      m_mutex(dnload_SDL_CreateMutex()) { }

    /// Destructor.
    ~Mutex()
    {
      dnload_SDL_DestroyMutex(m_mutex);
    }

  public:
    /// Lock.
    void acquire()
    {
      dnload_SDL_LockMutex(m_mutex);
    }

    /// Accessor.
    ///
    /// \return Inner mutex.
    SDL_mutex* getInnerMutex()
    {
      return m_mutex;
    }

    /// Unlock.
    void release()
    {
      dnload_SDL_UnlockMutex(m_mutex);
    }
};

/// Cond.
class Cond
{
  private:
    /// Actual cond.
    SDL_cond *m_cond;

  private:
    /// Deleted copy constructor.
    Cond(const Cond&) = delete;
    /// Deleted assignment.
    Cond& operator=(const Cond&) = delete;

  public:
    /// Constructor.
    Cond()
      : m_cond(dnload_SDL_CreateCond()) { }

    /// Destructor.
    ~Cond()
    {
      dnload_SDL_DestroyCond(m_cond);
    }

  public:
    /// Signal the cond.
    void signal()
    {
      dnload_SDL_CondSignal(m_cond);
    }

    /// Wait on cond.
    ///
    /// \param mutex Mutex (already locked).
    void wait(Mutex *mutex)
    {
      dnload_SDL_CondWait(m_cond, mutex->getInnerMutex());
    }

    /// Wait on cond wrapper.
    ///
    /// \param mutex Mutex (already locked).
    void wait(Mutex &mutex)
    {
      this->wait(&mutex);
    }
};

/// Scoped lock.
class ScopedLock
{
  private:
    /// Mutex that was locked.
    Mutex *m_mutex;

  private:
    /// Deleted copy constructor.
    ScopedLock(const ScopedLock&) = delete;
    /// Deleted assignment.
    ScopedLock& operator=(const ScopedLock&) = delete;

  public:
    /// Constructor.
    ///
    /// Implicitly locks the mutex.
    ///
    /// \param op Mutex to lock.
    ScopedLock(Mutex *op)
      : m_mutex(op)
    {
      op->acquire();
    }

    /// Destructor.
    ///
    /// Implicitly unlocks the mutex.
    ~ScopedLock()
    {
      m_mutex->release();
    }
};

/// Thread class.
class Thread
{
  private:
    /// Actual thread.
    SDL_Thread* m_thread;

  private:
    /// Deleted copy constructor.
    Thread(const Thread&) = delete;
    /// Deleted assignment.
    Thread& operator=(const Thread&) = delete;

  public:
    /// Constructor.
    ///
    /// \param func Function to start.
    /// \param data Data to pass to the thread.
    Thread(int (*func)(void*), void *data)
      : m_thread(dnload_SDL_CreateThread(func, NULL, data)) { }

    /// Destructor.
    ///
    /// Will implicitly join the thread.
    ~Thread()
    {
      this->join();
    }

  private:
    /// Join the thread.
    void join()
    {
      dnload_SDL_WaitThread(m_thread, NULL);
    }
};

#endif
