#ifndef VERBATIM_EDGE_HPP
#define VERBATIM_EDGE_HPP

#include "verbatim_edge_vertex.hpp"
#include "verbatim_element.hpp"

/// Edge ready to be added to 
class Edge : public Element
{
  private:
    /// Vertex data.
    EdgeVertex m_data[4];

  public:
    /// Constructor.
    ///
    /// \param v1 First vertex.
    /// \param v2 Second vertex.
    /// \param n1 Left normal.
    /// \param n2 Right normal.
    /// \param elem Element settings.
    Edge(const vec3 &v1, const vec3 &v2, const vec3 &n1, const vec3 &n2, const Element &elem) :
      Element(elem)
    {
      m_data[0] = EdgeVertex(v1, n1);
      m_data[1] = EdgeVertex(v1, n2);
      m_data[2] = EdgeVertex(v2, n1);
      m_data[3] = EdgeVertex(v2, n2);
    }

  public:
    /// Accessor.
    ///
    /// \param idx Index.
    /// \return Edge vertex at index.
    const EdgeVertex& getVertex(unsigned idx) const
    {
      return m_data[idx];
    }

  public:
    /// Comparison function.
    ///
    /// \param lhs Right-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return negative if lhs < rhs, positive if lhs > rhs, 0 otherwise.
    static int qsort_cmp_edge(const void *lhs, const void *rhs)
    {
      const Edge *aa = static_cast<const Edge*>(lhs);
      const Edge *bb = static_cast<const Edge*>(rhs);

      // Vertices do not matter.
      return qsort_cmp_element(*aa, *bb);
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[" << m_data[0] << ", " << m_data[1] << ", " << m_data[2] << ", " << m_data[3] << "]";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const Edge &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
