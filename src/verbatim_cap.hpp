#ifndef VERBATIM_CAP_HPP
#define VERBATIM_CAP_HPP

#include "verbatim_element.hpp"
#include "verbatim_ivec4.hpp"
#include "verbatim_vec3.hpp"

/// Shadow volume cap face.
class Cap : public Element
{
  private:
    /// Corners.
    vec3 m_corner[3];

    /// Normal.
    ivec4 m_normal;

  public:
    /// Constructor.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param c3 Third corner.
    /// \param nor Face normal.
    /// \param elem Element settings.
    Cap(const vec3 &c1, const vec3 &c2, const vec3 &c3, const vec3 &nor, const Element &elem) :
      Element(elem),
      m_normal(nor)
    {
      m_corner[0] = c1;
      m_corner[1] = c2;
      m_corner[2] = c3;
    }

  public:
    /// Accessor.
    ///
    /// \param idx Index to access.
    /// \return Corner position.
    const vec3& getCorner(unsigned idx) const
    {
      return m_corner[idx];
    }

    /// Accessor.
    ///
    /// \return Normal.
    const ivec4& getNormal() const
    {
      return m_normal;
    }

  public:
    /// Comparison function.
    ///
    /// \param lhs Right-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return negative if lhs < rhs, positive if lhs > rhs, 0 otherwise.
    static int qsort_cmp_cap(const void *lhs, const void *rhs)
    {
      const Cap *aa = static_cast<const Cap*>(lhs);
      const Cap *bb = static_cast<const Cap*>(rhs);

      // Vertices do not matter.
      return qsort_cmp_element(*aa, *bb);
    }
};

#endif
