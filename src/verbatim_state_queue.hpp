#ifndef VERBATIM_STATE_QUEUE
#define VERBATIM_STATE_QUEUE

#include "verbatim_threading.hpp"
#include "verbatim_state.hpp"

/// State queue.
///
/// Static data only.
class StateQueue
{
  private:
    /// Number of states to store.
    static const unsigned NUM_STATES = 3;

  private:
    /// Mutex.
    Mutex m_mutex;

    /// Cond.
    Cond m_cond;

    /// State array.
    State m_states[NUM_STATES];

    /// Last inserted state.
    const State *m_last_state;

    /// Current ready state extraction point.
    unsigned m_extract;

    /// Current number of ready states.
    unsigned m_num_states;

    /// Terminate flag.
    bool m_terminated;
    
  public:
    /// Constructor.
    StateQueue() :
      m_extract(0),
      m_num_states(0),
      m_terminated(false) { }

  private:
    /// Acquire state queue mutex.
    void acquire()
    {
      m_mutex.acquire();
    }

    /// Signal state queue cond.
    void signal()
    {
      m_cond.signal();
    }

    /// Release state queue mutex.
    void release()
    {
      m_mutex.release();
    }

    /// Wait on this queue.
    void wait()
    {
      m_cond.wait(m_mutex);
    }

  public:
    /// Acquire one empty state from array.
    ///
    /// \return State or NULL if queue is terminated.
    State* acquireEmpty()
    {
      ScopedLock lock(&m_mutex);

      for(;;)
      {
        if(m_terminated)
        {
          return NULL;
        }

        if(NUM_STATES > m_num_states)
        {
          break;
        }

        this->wait();
      }

      return &(m_states[(m_extract + m_num_states) % NUM_STATES]);
    }

    /// Acquire one ready state from array.
    ///
    /// \return State or NULL if queue is terminated.
    State* acquireReady()
    {
      ScopedLock lock(&m_mutex);

      for(;;)
      {
        if(m_terminated)
        {
          return NULL;
        }

        if(0 < m_num_states)
        {
          break;
        }

#if defined(USE_LD)
        std::cout << "WARNING: state queue empty" << std::endl;
#endif
        this->wait();
      }

      return &(m_states[m_extract]);
    }

    /// Get last state added.
    ///
    /// \return Last state added to the queue.
    const State* getLastState() const
    {
      return m_last_state;
    }

    /// Finish an empty state, making it ready.
    ///
    /// The state must be a previously acquired empty state.
    ///
    /// \param op State to finish.
    void finishEmpty(const State &op)
    {
      ScopedLock lock(&m_mutex);

      m_last_state = &op;
      ++m_num_states;

      signal();
    }

    /// Finish the last acquired ready state, making it empty.
    void finishReady()
    {
      ScopedLock lock(&m_mutex);

      m_extract = (m_extract + 1) % NUM_STATES;
      --m_num_states;

      this->signal();
    }

    /// Terminate execution.
    void terminate()
    {
      this->acquire();
      m_terminated = true;
      this->signal();
      this->release();
    }
};

#endif
