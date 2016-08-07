#ifndef VERBATIM_EDGE_VERTEX_HPP
#define VERBATIM_EDGE_VERTEX_HPP

#include "verbatim_ivec4.hpp"

/// Edge vertex - vertex for edges.
class EdgeVertex
{
  public:
    /// Position offset.
    static const ptrdiff_t POSITION_OFFSET = 0;

    /// Normal offset.
    static const ptrdiff_t NORMAL_OFFSET = sizeof(vec3);

  private:
    /// Position.
    vec3 m_position;

    /// Normal.
    ivec4 m_normal;

  public:
    /// Empty constructor.
    EdgeVertex() { }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param nor Normal.
    EdgeVertex(const vec3 &pos, const vec3 &nor) :
      m_position(pos),
      m_normal(nor) { }

    /// Constructor.
    ///
    /// \param pos Position.
    /// \param nor Normal.
    EdgeVertex(const vec3 &pos, const ivec4 &nor) :
      m_position(pos),
      m_normal(nor) { }

  public:
    /// Accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
      return m_position;
    }

    /// Accessor.
    ///
    /// \return Normal.
    const ivec4& getNormal() const
    {
      return m_normal;
    }

    /// Tell if this edge vertex matches given parameters.
    ///
    /// \param pos Position.
    /// \param nor Normal.
    bool matches(const vec3 &pos, const ivec4 &nor) const
    {
      return ((m_position == pos) && (m_normal == nor));
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[" << m_position << ", " << m_normal << "]";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const EdgeVertex &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
