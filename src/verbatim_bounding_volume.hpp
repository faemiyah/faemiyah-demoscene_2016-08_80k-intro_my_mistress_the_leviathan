#ifndef VERBATIM_BOUNDING_VOLUME
#define VERBATIM_BOUNDING_VOLUME

#include "verbatim_geometry_buffer.hpp"
#include "verbatim_index_block.hpp"
#include "verbatim_mat4.hpp"

/// Bounding volume class.
///
/// To be absolutely specific, more the limits of an object's extent.
class BoundingVolume
{
  private:
    /// Minimum x value.
    float m_x_min;

    /// Maximum x value.
    float m_x_max;

    /// Minimum y value.
    float m_y_min;

    /// Maximum y value.
    float m_y_max;

    /// Minimum z value.
    float m_z_min;

    /// Maximum z value.
    float m_z_max;

    /// Center point.
    vec3 m_center;

  private:
    /// Deleted copy constructor.
    BoundingVolume(const BoundingVolume&) = delete;
    /// Deleted assignment.
    BoundingVolume& operator=(const BoundingVolume&) = delete;

  public:
    /// \brief Constructor.
    ///
    /// \param msh Mesh to construct from.
    /// \param transform Transform to use.
    BoundingVolume(const IndexBlock &block, const mat4 &transform) :
      m_x_min(FLT_MAX),
      m_x_max(-FLT_MAX),
      m_y_min(FLT_MAX),
      m_y_max(-FLT_MAX),
      m_z_min(FLT_MAX),
      m_z_max(-FLT_MAX)
    {
      for(unsigned ii = 0, ee = block.getIndexCount(); (ii != ee); ++ii)
      {
        unsigned vertex_index = block.getIndex(ii);
        vec3 transformed_vertex = transform * block.getVertex(vertex_index).getPosition();

        m_x_min = std::min(m_x_min, transformed_vertex[0]);
        m_x_max = std::max(m_x_max, transformed_vertex[0]);
        m_y_min = std::min(m_y_min, transformed_vertex[1]);
        m_y_max = std::max(m_y_max, transformed_vertex[1]);
        m_z_min = std::min(m_z_min, transformed_vertex[2]);
        m_z_max = std::max(m_z_max, transformed_vertex[2]);
      }
      m_center = (vec3(m_x_max, m_y_max, m_z_max) + vec3(m_x_min, m_y_min, m_z_min)) * 0.5f;
    }

  public:
    /// Tell if this bounding volume conflicts with another.
    ///
    /// Does not check conflicts on Y space.
    ///
    /// \param op Other bounding volume.
    bool conflictsXZ(const BoundingVolume &op) const
    {
      return !((op.getMinX() > m_x_max) ||
          (op.getMaxX() < m_x_min) ||
          (op.getMinZ() > m_z_max) ||
          (op.getMaxZ() < m_z_min));
    }

    /// Accessor.
    ///
    /// \return Comparison value.
    const vec3& getCenter() const
    {
      return m_center;
    }

    /// Accessor.
    ///
    /// \return Maximum X value.
    float getMaxX() const
    {
      return m_x_max;
    }

    /// Accessor.
    ///
    /// \return Maximum Y value.
    float getMaxY() const
    {
      return m_y_max;
    }

    /// Accessor.
    ///
    /// \return Maximum Z value.
    float getMaxZ() const
    {
      return m_z_max;
    }

    /// Accessor.
    ///
    /// \return Minimum X value.
    float getMinX() const
    {
      return m_x_min;
    }

    /// Accessor.
    ///
    /// \return Minimum Y value.
    float getMinY() const
    {
      return m_y_min;
    }

    /// Accessor.
    ///
    /// \return Minimum X value.
    float getMinZ() const
    {
      return m_z_min;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " << m_x_min << " ; " << m_x_max << " ] [ " << m_y_min << " ; " << m_y_max <<
        " ] [ " << m_z_min << " ; " << m_z_max << " ]";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
std::ostream& operator<<(std::ostream &lhs, const BoundingVolume &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
