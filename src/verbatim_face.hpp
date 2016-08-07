#ifndef VERBATIM_FACE_HPP
#define VERBATIM_FACE_HPP

#include "verbatim_element.hpp"
#include "verbatim_ivec4.hpp"

/// Face class.
///
/// Upon rendering, all faces are triangles.
class Face : public Element
{
  private:
    /// Internal data.
    unsigned m_index[3];

    /// Normal.
    ivec4 m_normal;

    /// Does this face exist always or only when alone?
    bool m_exists;

  public:
    /// Constructor.
    ///
    /// \param c1 First corner.
    /// \param c2 Second corner.
    /// \param c3 Third corner.
    /// \param nor Normal.
    /// \param elem Element settings.
    Face(unsigned c1, unsigned c2, unsigned c3, const vec3 &nor, const Element &elem) :
      Element(elem),
      m_normal(nor)
    {
      m_index[0] = c1;
      m_index[1] = c2;
      m_index[2] = c3;

      rotate();
    }

  private:
    /// Rotate indices thus that the smallest one is first.
    void rotate()
    {
      if(m_index[1] < m_index[0])
      {
        if(m_index[1] < m_index[2])
        {
          rotate2();
        }
        else
        {
          rotate1();
        }
      }
      else if(m_index[2] < m_index[0])
      {
        rotate1();
      }
    }

    /// Rotate indices once.
    void rotate1()
    {
      unsigned cc = m_index[0];
      m_index[0] = m_index[2];
      m_index[2] = m_index[1];
      m_index[1] = cc;
    }

    /// Rotate indices twice.
    void rotate2()
    {
      unsigned cc = m_index[0];
      m_index[0] = m_index[1];
      m_index[1] = m_index[2];
      m_index[2] = cc;
    }

  public:
    /// Get corner index with base.
    ///
    /// \param base Base to add to.
    /// \param idx Index of corner.
    /// \return Corner index (converted).
    uint16_t getConvertedIndex(unsigned base, unsigned idx) const
    {
      return static_cast<uint16_t>(base + m_index[idx]);
    }

    /// Get corner index.
    ///
    /// \param idx Index of corner.
    /// \return Corner index.
    unsigned getIndex(unsigned idx) const
    {
      return m_index[idx];
    }

    /// Accessor.
    ///
    /// \return Normal.
    const ivec4& getNormal() const
    {
      return m_normal;
    }

    /// Test if face is valid.
    ///
    /// Valid faces are not degenerate and their indices fit in 16-bit unsigned integers.
    ///
    /// \return True if yes, false if not.
    bool isValid() const
    {
      return ((m_index[0] != m_index[1]) && (m_index[1] != m_index[2]) && (m_index[2] != m_index[0]) &&
          (0xFFFFU >= m_index[0]) && (0xFFFFU >= m_index[1]) && (0xFFFFU >= m_index[2]));
    }

  public:
    /// Comparison function.
    ///
    /// \param lhs Right-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return negative if lhs < rhs, positive if lhs > rhs, 0 otherwise.
    static int qsort_cmp_face(const void *lhs, const void *rhs)
    {
      const Face *aa = static_cast<const Face*>(lhs);
      const Face *bb = static_cast<const Face*>(rhs);

      // Element comparison takes priority.
      int ret = qsort_cmp_element(*aa, *bb);
      if(ret)
      {
        return ret;
      }

      if(aa->getIndex(0) < bb->getIndex(0))
      {
        return -1;
      }
      if(aa->getIndex(0) > bb->getIndex(0))
      {
        return 1;
      }
      if(aa->getIndex(1) < bb->getIndex(1))
      {
        return -1;
      }
      if(aa->getIndex(1) > bb->getIndex(1))
      {
        return 1;
      }
      if(aa->getIndex(2) < bb->getIndex(2))
      {
        return -1;
      }
      if(aa->getIndex(2) > bb->getIndex(2))
      {
        return 1;
      }
      return 0;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[" << m_index[0] << " ; " << m_index[1] << " ; " << m_index[2] << "]";
    }
#endif
};


#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const Face &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
