#ifndef VERBATIM_OBJECT_GROUP_HPP
#define VERBATIM_OBJECT_GROUP_HPP

#include "verbatim_object_reference.hpp"

/// Reference to one mesh.
///
/// The object group algorithm is NOT thread-safe. only one thread can be generating states at a time when
/// using object groups. However, this should not be a problem since the thread drawing should always be
/// slower than the thread generating. If this is not the case, something is wrong already.
class ObjectGroup
{
  private:
    /// Object references.
    seq<ObjectReference*> m_references;

    /// Number of elements in the group.
    const unsigned m_count;

    /// Currently rendered frame.
    unsigned m_frame;

  public:
    /// Constructor.
    ///
    /// \param count Object reference count.
    ObjectGroup(unsigned count) :
      m_references(count),
      m_count(count),
      m_frame(0xFFFFFFFFU) { }

  public:
    /// Add object reference.
    ///
    /// \param frame Frame currently in progress.
    /// \param ref Object reference.
    void addObjectReference(unsigned frame, ObjectReference &ref)
    {
      if(frame != m_frame)
      {
        m_references.clear();
        m_frame = frame;
      }
#if defined(USE_LD)
      // Check for exceeded capacity, should never happen.
      if(m_references.size() >= m_count)
      {
        std::ostringstream sstr;
        sstr << "trying to render more than " << m_references.size() << " objects in a group";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_references.push_back(&ref);

      // If we have added every object, we can check for optimism.
      if(m_references.size() >= m_count)
      {
        for(ObjectReference *vv : m_references)
        {
          vv->setOptimistic(true);
        }
        return;
      }
      // Otherwise stay non-optimistic for now.
      ref.setOptimistic(false);
    }
};

#endif
