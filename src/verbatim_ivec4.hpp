#ifndef VERBATIM_IVEC4_HPP
#define VERBATIM_IVEC4_HPP

#include "verbatim_vec2.hpp"
#include "verbatim_vec3.hpp"

/// 4-component integer vector class.
///
/// Practically, represents a normal.
class ivec4
{
  private:
    /// Data.
    int8_t m_data[4];

  public:
    /// Empty constructor.
    ivec4() { }

    /// Constructor.
    ///
    /// \param px X component.
    /// \param py Y component.
    /// \param pz Z component.
    /// \param pw Weight component.
    ivec4(int8_t px, int8_t py, int8_t pz, int8_t pw)
    {
      m_data[0] = px;
      m_data[1] = py;
      m_data[2] = pz;
      m_data[3] = pw;
    }

    /// Constructor.
    ///
    /// \param op1 First input vector.
    /// \param op2 Second input vector.
    ivec4(const vec2 &op1, const vec2 &op2)
    {
      m_data[0] = convert_normal_value(op1[0]);
      m_data[1] = convert_normal_value(op1[1]);
      m_data[2] = convert_normal_value(op2[0]);
      m_data[3] = convert_normal_value(op2[1]);
    }

    /// Constructor.
    ///
    /// \param op Input vector.
    ivec4(const vec3 &op)
    {
      m_data[0] = convert_normal_value(op[0]);
      m_data[1] = convert_normal_value(op[1]);
      m_data[2] = convert_normal_value(op[2]);
      m_data[3] = 0; // Must initialize to enable comparison.
    }

  private:
    /// Convert normal value.
    ///
    /// \param op Floating point input.
    /// \return Signed integer output.
    static int8_t convert_normal_value(float op)
    {
      if(op > 0.0f)
      {
        return static_cast<int8_t>(op * 127.0f + 0.5f);
      }
      return static_cast<int8_t>(op * 128.0f + 0.5f);
    }

  public:
    /// Accessor.
    ///
    /// \return Data.
    const int8_t* getData() const
    {
      return m_data;
    }

    /// Access operator.
    ///
    /// \return Value.
    int8_t& operator[](unsigned idx)
    {
      return m_data[idx];
    }

    /// Const access operator.
    ///
    /// \return Value.
    const int8_t& operator[](unsigned idx) const
    {
      return m_data[idx];
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    bool operator==(const ivec4 &rhs) const
    {
      return ((m_data[0] == rhs.m_data[0]) &&
          (m_data[1] == rhs.m_data[1]) &&
          (m_data[2] == rhs.m_data[2]) &&
          (m_data[3] == rhs.m_data[3]));
    }

  public:
    /// Calculate square error.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Square error.
    friend int sqr_error(const ivec4 &lhs, const ivec4 &rhs)
    {
      int e1 = static_cast<int>(lhs[0]) - static_cast<int>(rhs[0]);
      int e2 = static_cast<int>(lhs[1]) - static_cast<int>(rhs[1]);
      int e3 = static_cast<int>(lhs[2]) - static_cast<int>(rhs[2]);
      int e4 = static_cast<int>(lhs[3]) - static_cast<int>(rhs[3]);
      return (e1 * e1) + (e2 * e2) + (e3 * e3) + (e4 * e4);
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " << static_cast<int>(m_data[0]) << " ; " << static_cast<int>(m_data[1]) <<
        " ; " << static_cast<int>(m_data[2]) << " ; " << static_cast<int>(m_data[3]) << " ]";
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const ivec4 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
