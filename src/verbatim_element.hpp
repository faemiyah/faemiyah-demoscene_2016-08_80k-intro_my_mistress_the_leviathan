#ifndef VERBATIM_ELEMENT_HPP
#define VERBATIM_ELEMENT_HPP

#include "verbatim_realloc.hpp"

/// Singular construction element.
class Element
{
  private:
    /// Block ID this element belongs to.
    unsigned m_block_id;

    /// Is this element real (always drawn) or is it only drawn when the object is drawn individually.
    bool m_exists;

  public:
    /// Constructor.
    ///
    /// \param block_id Object id.
    /// \param exists Existence.
    Element(unsigned block_id, bool exists) :
      m_block_id(block_id),
      m_exists(exists) { }

  public:
    /// Accessor.
    ///
    /// \return Block ID.
    unsigned getBlockId() const
    {
      return m_block_id;
    }

    /// Tell if this element is real (drawn always)?
    ///
    /// \return True if yes, false if no.
    bool isReal() const
    {
      return m_exists;
    }

  public:
    /// Comparison (for qsort).
    ///
    /// \param lhs Right-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return negative if lhs < rhs, positive if lhs > rhs, 0 otherwise.
    static int qsort_cmp_element(const Element &lhs, const Element &rhs)
    {
      if(lhs.getBlockId() < rhs.getBlockId())
      {
        return -1;
      }
      if(lhs.getBlockId() > rhs.getBlockId())
      {
        return 1;
      }
      if(lhs.isReal() != rhs.isReal())
      {
        return lhs.isReal() ? -1 : 1;
      }
      return 0;
    }
};

#endif
