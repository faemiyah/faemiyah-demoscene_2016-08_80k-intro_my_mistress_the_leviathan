#ifndef VERBATIM_INDEX_RUN_HPP
#define VERBATIM_INDEX_RUN_HPP

/// Represents a run of indices that can be drawn.
class IndexRun
{
  private:
    /// Base offset.
    unsigned m_base;

    /// Count.
    GLsizei m_count;

    /// Count.
    GLsizei m_count_full;

  public:
    /// Constructor.
    ///
    /// \param base Base.
    /// \param count Count.
    /// \param count_full Count of all (also non-real) faces.
    IndexRun(unsigned base, unsigned count, unsigned count_full) :
      m_base(base),
      m_count(static_cast<GLsizei>(count)),
      m_count_full(static_cast<GLsizei>(count_full)) { }

  public:
    /// Draw elements in this index run.
    ///
    /// \param full Draw all triangles or just real ones?
    void drawTriangles(bool full) const
    {
      //std::cout << "drawing " << (full ? "FULL" : "SOME") << " triangles: " <<
      //  (full ? m_count_full : m_count) << std::endl;
      dnload_glDrawElements(GL_TRIANGLES, full ? m_count_full : m_count, GL_UNSIGNED_SHORT,
          static_cast<uint16_t*>(NULL) + m_base);
    }

    /// Accessor.
    ///
    /// \return Index base.
    unsigned getBase() const
    {
      return m_base;
    }

    /// Accessor.
    ///
    /// \return Index count.
    unsigned getCount() const
    {
      return static_cast<unsigned>(m_count);
    }

#if defined(USE_LD)
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "IndexRun(" << m_base << ", " << m_count << ")";
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const IndexRun &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
