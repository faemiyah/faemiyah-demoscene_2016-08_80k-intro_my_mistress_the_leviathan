#ifndef INTRO_FLOATING_ISLAND_HPP
#define INTRO_FLOATING_ISLAND_HPP

#include "intro_csg.hpp"

/// Generate floating island.
///
/// We in Zendikar? Nah, we just don't have fill rate for a heightmap.
///
/// The two meshes are created in the same space.
class FloatingIsland
{
  private:
    /// Lower mesh.
    MeshUptr m_mesh_lower;

    /// Upper mesh.
    MeshUptr m_mesh_upper;

    /// Height.
    float m_height;

    /// Radius.
    float m_radius;

    /// Vertical detail.
    unsigned m_vdetail;

    /// Hole coefficient.
    float m_hole_coefficient;

    /// Pit coefficient.
    float m_pit_coefficient;

  public:
    /// Constructor.
    ///
    /// \param height Height.
    /// \param width Width.
    /// \param vdetail Vertical detail.
    /// \param buf Geometry buffer.
    /// \param unsigned seed Random seed.
    FloatingIsland(float height, float radius, unsigned vdetail, GeometryBuffer &buf,
        unsigned seed, float hole_coefficient = 0.3f, float pit_coefficient = 0.2f) :
      m_height(height),
      m_radius(radius),
      m_vdetail(vdetail),
      m_hole_coefficient(hole_coefficient),
      m_pit_coefficient(pit_coefficient)
    {
      construct(buf, seed);
    }

  private:
    /// Construct content.
    ///
    /// \param buf Geometry buffer.
    /// \param seed Random seed.
    void construct(GeometryBuffer &buf, unsigned seed)
    {
      static const float SHAPE_FAR = 0.67f;
      static const float SHAPE_DEPTH = 0.1366f; // May seem silly but is obligatory.
      float fdetail_mul = 1.0f / static_cast<float>(m_vdetail);
      float ystep = fdetail_mul * m_height;

      {
        LogicalMesh msh;
        msh.setPaintColor(0.9f, 0.85f, 0.82f);

        for(unsigned ii = 0; (m_vdetail > ii); ++ii)
        {
          float h1 = -static_cast<float>(ii + 0) * ystep;
          float h2 = -static_cast<float>(ii + 1) * ystep;
          float r1 = static_cast<float>(ii + 0) * fdetail_mul;
          float r2 = static_cast<float>(ii + 1) * fdetail_mul;
          r1 = (1.0f - (r1 * r1)) * m_radius;
          r2 = (1.0f - (r2 * r2)) * m_radius;

          // Last cycle.
          if(m_vdetail == ii + 1)
          {
            unsigned base = msh.getLogicalVertexCount();

            msh.addVertex(0.0f, -m_height, 0.0f);

            msh.addFace(base, base - 4, base - 3);
            msh.addFace(base, base - 3, base - 2);
            msh.addFace(base, base - 2, base - 1);
            msh.addFace(base, base - 1, base - 4);
          }
          else
          {
            mesh_generate_polygon_ring(msh,
                h1, r1, m_vdetail - ii + 3, seed + ii,
                h2, r2, m_vdetail - ii - 1 + 3, seed + ii + 1,
                false);
          }
        }

        // Main mesh done.
        m_mesh_lower = msh.insert(buf, seed + m_vdetail + 3);
      }

      // Bottom mesh.
      {
        float s_far = SHAPE_FAR * m_radius;
        float s_near = m_hole_coefficient * m_radius;
        float yshape = ystep * 2.0f;
        float ynear = yshape - (SHAPE_DEPTH * ystep);

        LogicalMesh msh;
        msh.setPaintColor(0.89f, 0.85f, 0.83f);

        // First cycle (upwards).
        mesh_generate_polygon_ring(msh,
            0.0f, m_radius, m_vdetail + 3, seed,
            ystep, (1.0f - fdetail_mul) * m_radius, m_vdetail - 1 + 3, seed + m_vdetail + 1,
            true);

        {
          // Now the last vdetail + 2 vertices make the top 'plane'.
          unsigned cross_base = msh.getLogicalVertexCount();
          unsigned plane_base = cross_base - m_vdetail - 2;

          // Cross shape. After this, last 8 vertices make the top 'cap'.
          msh.addVertex(s_far, yshape, s_near);
          msh.addVertex(s_near, yshape, s_far);
          msh.addVertex(-s_near, yshape, s_far);
          msh.addVertex(-s_far, yshape, s_near);
          msh.addVertex(-s_far, yshape, -s_near);
          msh.addVertex(-s_near, yshape, -s_far);
          msh.addVertex(s_near, yshape, -s_far);
          msh.addVertex(s_far, yshape, -s_near);

          unsigned ii = cross_base;
          const vec3 &cmp = msh.getLogicalVertex(ii).getPosition();
          unsigned start = msh.findClosestVertex(cmp, plane_base, plane_base + m_vdetail + 1);

          // Back out to find first vertex that is closest to ii.
          for(;;)
          {
            unsigned prev = start - 1;
            if(prev < plane_base)
            {
              prev = cross_base - 1;
            }
            const vec3 &prev_pos = msh.getLogicalVertex(prev).getPosition();

            if(msh.findClosestVertex(prev_pos, cross_base, cross_base + 7) != ii)
            {
              break;
            }
            start = prev;
          }

          unsigned jj = start;
          for(;;)
          {
            unsigned next = jj + 1;
            if(next >= cross_base)
            {
              next = plane_base;
            }
            const vec3 &next_pos = msh.getLogicalVertex(next).getPosition();

            if(msh.findClosestVertex(next_pos, cross_base, cross_base + 7) == ii)
            {
              msh.setPaintColor(0.87f, 0.85f, 0.84f);
              msh.addFace(jj, ii, next);
              jj = next;
            }
            else
            {
              ++ii;
              if(ii == cross_base + 8)
              {
                msh.setPaintColor(0.9f, 0.9f, 0.9f);
                msh.addFace(ii - 1, cross_base, jj);
                msh.addFace(jj, cross_base, next);
                break;
              }
              msh.addFace(ii - 1, ii, jj);
            }
          }
        }

        {
          unsigned cross_base = msh.getLogicalVertexCount();

          // Cross shape in cap.
          msh.addVertex(s_far, yshape, -s_near);
          msh.addVertex(s_near, yshape, -s_near);
          msh.addVertex(s_near, yshape, -s_far);
          msh.addVertex(-s_near, yshape, -s_far);
          msh.addVertex(-s_near, yshape, -s_near);
          msh.addVertex(-s_far, yshape, -s_near);
          msh.addVertex(-s_far, yshape, s_near);
          msh.addVertex(-s_near, yshape, s_near);
          msh.addVertex(-s_near, yshape, s_far);
          msh.addVertex(s_near, yshape, s_far);
          msh.addVertex(s_near, yshape, s_near);
          msh.addVertex(s_far, yshape, s_near);

          // Indentation.
          msh.addVertex(s_near, ynear, -s_near);
          msh.addVertex(-s_near, ynear, -s_near);
          msh.addVertex(-s_near, ynear, s_near);
          msh.addVertex(s_near, ynear, s_near);

          msh.setPaintColor(0.9f, 0.9f, 0.9f);

          // Fill cap.
          msh.addFace(cross_base + 12, cross_base + 0, cross_base + 1);
          msh.addFace(cross_base + 0, cross_base + 2, cross_base + 1);
          msh.addFace(cross_base + 12, cross_base + 1, cross_base + 2);
          msh.addFace(cross_base + 12, cross_base + 2, cross_base + 3, cross_base + 13);
          msh.addFace(cross_base + 13, cross_base + 3, cross_base + 4);
          msh.addFace(cross_base + 4, cross_base + 3, cross_base + 5);
          msh.addFace(cross_base + 13, cross_base + 4, cross_base + 5);
          msh.addFace(cross_base + 13, cross_base + 5, cross_base + 6, cross_base + 14);
          msh.addFace(cross_base + 14, cross_base + 6, cross_base + 7);
          msh.addFace(cross_base + 6, cross_base + 8, cross_base + 7);
          msh.addFace(cross_base + 14, cross_base + 7, cross_base + 8);
          msh.addFace(cross_base + 14, cross_base + 8, cross_base + 9, cross_base + 15);
          msh.addFace(cross_base + 15, cross_base + 9, cross_base + 10);
          msh.addFace(cross_base + 10, cross_base + 9, cross_base + 11);
          msh.addFace(cross_base + 15, cross_base + 10, cross_base + 11);
          msh.addFace(cross_base + 15, cross_base + 11, cross_base + 0, cross_base + 12);

          mesh_generate_box(msh, vec3(0.0f, ynear - (m_pit_coefficient * m_height * 0.5f), 0.0f),
              s_near * 2.0f, m_pit_coefficient * m_height, s_near * 2.0f, CSG_NO_TOP & CSG_INVERSE);
        }

        m_mesh_upper = msh.insert(buf, seed + m_vdetail + 2);
      }
    }

  public:
    /// Accessor.
    ///
    /// \return Lower mesh.
    const Mesh& getMeshLower() const
    {
      return *m_mesh_lower;
    }

    /// Accessor.
    ///
    /// \return Upper mesh.
    const Mesh& getMeshUpper() const
    {
      return *m_mesh_upper;
    }
};

/// Convenience typedef.
typedef uptr<FloatingIsland> FloatingIslandUptr;

#endif
