#ifndef INTRO_AQUEDUCT_HPP
#define INTRO_AQUEDUCT_HPP

#include "intro_maze.hpp"

/// Aqueduct part class.
///
/// Extends mesh.
class Aqueduct
{
  private:
    /// Aqueduct type.
    static const unsigned AQUEDUCT_NONE = 0;
    /// Aqueduct type.
    static const unsigned AQUEDUCT_PILLAR_NONE = 1;
    /// Aqueduct type.
    static const unsigned AQUEDUCT_PILLAR_FORWARD = 2;
    /// Aqueduct type.
    static const unsigned AQUEDUCT_PILLAR_BACKWARD = 3;
    /// Aqueduct type.
    static const unsigned AQUEDUCT_PILLAR_BOTH = 4;

  public:
    /// Aqueduct type count.
    static const unsigned AQUEDUCT_COUNT = 5;

  private:
    /// Mesh reference.
    MeshUptr m_mesh;

    /// Left type.
    unsigned m_left;

    /// Right type.
    unsigned m_right;

  public:
    /// Constructor.
    ///
    /// \param left Left type.
    /// \param right Right type.
    /// \param seed Texturing random seed.
    Aqueduct(unsigned left, unsigned right, GeometryBuffer &buf, unsigned seed) :
      m_left(left),
      m_right(right)
    {
      construct(buf, seed);
    }

  private:
    /// Construct the mesh.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param seed Texturing random seed.
    void construct(GeometryBuffer &buf, unsigned seed)
    {
      //static const float AQUEDUCT_WIDTH = 10.0f;
      static const float PILLAR_WIDTH = 5.0f;
      static const float PILLAR_HEIGHT = 9.0f;
      static const float PILLAR_LENGTH = 3.0f;
      static const float ARC_LENGTH = 8.0f;
      static const float ARC_HEIGHT = 0.9f;
      static const float SIDE_ARC_LENGTH = 2.0f;
      static const float SIDE_ARC_HEIGHT = 0.92f;
      static const float FLOOR_HEIGHT = 0.2f;
      static const float COLUMN_HEIGHT = 6.0f;
      static const float EXTENT = (PILLAR_LENGTH * 0.5f) + ARC_LENGTH;
      static const float EXTENT_PART = 0.2f;
      static const float EXTENT_SMALL = EXTENT_PART * (EXTENT * 2.0f);
      //static const unsigned PILLAR_DETAIL = 9; Pillar detail now generally defined
      static const unsigned ARC_DETAIL = 6;
      static const unsigned SIDE_ARC_DETAIL = 3;
      static const unsigned CYLINDER_DETAIL = 7;
      static const unsigned FLOOR_DETAIL = 5;
      static const vec3 BASE_COLOR(0.8f, 0.7f, 0.65f);
      static const vec3 MED_COLOR(0.83f, 0.8f, 0.79f);
      static const vec3 LIGHT_COLOR(0.85f, 0.85f, 0.82f);

      LogicalMesh msh;
      msh.setPaintColor(BASE_COLOR);
      msh.setPreserveInvisible(true);

      // Pillar parts get longer as they go down so we can keep the illusion.
      {
        static const float PILLAR_PART_INCREASE = 1.0f;
        float pillar_offset = 0.0f;
        float pillar_pos = 0.0f;

        for(unsigned ii = 0; (PILLAR_DETAIL > ii); ++ii)
        {
          CsgFlag flags;
          if(0 == ii)
          {
            flags.disable(CsgFlag::BACK, CsgFlag::FRONT);
            if(hasLeftPillar())
            {
              flags.disable(CsgFlag::LEFT);
            }
            if(hasRightPillar())
            {
              flags.disable(CsgFlag::RIGHT);
            }
          }
          else
          {
            flags.disable(CsgFlag::TOP);
          }
          if(PILLAR_DETAIL - 1 != ii)
          {
            flags.disable(CsgFlag::BOTTOM);
          }

          mesh_generate_box(msh, vec3(0.0f, -PILLAR_HEIGHT * (static_cast<float>(ii) + 0.5f) + pillar_pos, 0.0f),
              PILLAR_WIDTH, PILLAR_HEIGHT + pillar_offset, PILLAR_LENGTH, flags);
          msh.advanceBlockId();

          pillar_pos -= pillar_offset * 0.5f;
          pillar_offset += PILLAR_PART_INCREASE;
          pillar_pos -= pillar_offset * 0.5f;
        }
      }

      msh.setPaintColor(BASE_COLOR);

      mesh_generate_arc(msh, vec3(0.0f, -PILLAR_HEIGHT, PILLAR_LENGTH * 0.5f),
          vec3(0.0f, -PILLAR_HEIGHT, PILLAR_LENGTH * 0.5f + ARC_LENGTH), vec3(0.0f, 1.0f, 0.0f),
          PILLAR_HEIGHT, PILLAR_WIDTH, ARC_HEIGHT, ARC_DETAIL, CSG_NO_BACK, 2);
      msh.advanceBlockId();

      mesh_generate_arc(msh, vec3(0.0f, -PILLAR_HEIGHT, -PILLAR_LENGTH * 0.5f),
          vec3(0.0f, -PILLAR_HEIGHT, -PILLAR_LENGTH * 0.5f - ARC_LENGTH), vec3(0.0f, 1.0f, 0.0f),
          PILLAR_HEIGHT, PILLAR_WIDTH, ARC_HEIGHT, ARC_DETAIL, CSG_NO_BACK, 2);
      msh.advanceBlockId();

      {
        const float fw = EXTENT;
        const float bk = -EXTENT;
        const float len = (fw - bk) / static_cast<float>(FLOOR_DETAIL);

        msh.setPaintColor(LIGHT_COLOR);

        bsd_srand(seed);

        for(float ii = bk; (fw > ii); ii += len)
        {
          float x_offset = frand(-FLOOR_HEIGHT, FLOOR_HEIGHT);
          mesh_generate_box(msh, vec3(0.0f + x_offset, FLOOR_HEIGHT * 0.5f, ii + len * 0.5f),
              PILLAR_WIDTH - 3.0f * FLOOR_HEIGHT, FLOOR_HEIGHT, len, CSG_NO_BOTTOM);
          msh.advanceBlockId();
        }
      }

      if(hasLeftPillar())
      {
        mesh_generate_arc(msh, vec3(-PILLAR_WIDTH * 0.5f, -PILLAR_HEIGHT, 0.0f),
            vec3(-PILLAR_WIDTH * 0.5f - SIDE_ARC_LENGTH, -PILLAR_HEIGHT, 0.0f), vec3(0.0f, 1.0f, 0.0f),
            PILLAR_HEIGHT, PILLAR_LENGTH, SIDE_ARC_HEIGHT, SIDE_ARC_DETAIL, CSG_NO_BACK);
        msh.advanceBlockId();

        const vec3 p1((-PILLAR_WIDTH - SIDE_ARC_LENGTH) * 0.5f, 0.0f, 0.0f);
        const vec3 p2((-PILLAR_WIDTH - SIDE_ARC_LENGTH) * 0.5f, COLUMN_HEIGHT, 0.0f);
        const float pr = SIDE_ARC_LENGTH * 0.5f - FLOOR_HEIGHT;

        msh.setPaintColor(BASE_COLOR);
        mesh_generate_pillar(msh, p1, p2, pr, CYLINDER_DETAIL, 0, CSG_NO_FRONT);
        msh.advanceBlockId();

        if(hasLeftExtent())
        {
          const vec3 p3 = p2 + vec3(0.0f, pr * 0.5f, 0.0f);

          msh.setPaintColor(MED_COLOR);

          mesh_generate_box(msh, p3, pr, pr, EXTENT_SMALL,
              (hasLeftExtentForward() ? CSG_NO_BACK : CSG_ALL) &
              (hasLeftExtentBackward() ? CSG_NO_FRONT : CSG_ALL));
          msh.advanceBlockId();

          if(hasLeftExtentForward())
          {
            for(float ii = EXTENT_SMALL; ((EXTENT - 0.49f * EXTENT_SMALL) > ii); ii += EXTENT_SMALL)
            {
              mesh_generate_box(msh, p3 + vec3(0.0f, 0.0f, ii), pr, pr, EXTENT_SMALL,
                  CSG_NO_FRONT & CSG_NO_BACK);
              msh.advanceBlockId();
            }
          }
          if(hasLeftExtentBackward())
          {
            for(float ii = EXTENT_SMALL; ((EXTENT - 0.49f * EXTENT_SMALL) > ii); ii += EXTENT_SMALL)
            {
              mesh_generate_box(msh, p3 - vec3(0.0f, 0.0f, ii), pr, pr, EXTENT_SMALL,
                  CSG_NO_FRONT & CSG_NO_BACK);
              msh.advanceBlockId();
            }
          }
        }
      }

      if(hasRightPillar())
      {
        mesh_generate_arc(msh, vec3(PILLAR_WIDTH * 0.5f, -PILLAR_HEIGHT, 0.0f),
            vec3(PILLAR_WIDTH * 0.5f + SIDE_ARC_LENGTH, -PILLAR_HEIGHT, 0.0f), vec3(0.0f, 1.0f, 0.0f),
            PILLAR_HEIGHT, PILLAR_LENGTH, SIDE_ARC_HEIGHT, SIDE_ARC_DETAIL, CSG_NO_BACK);
        msh.advanceBlockId();

        const vec3 p1((PILLAR_WIDTH + SIDE_ARC_LENGTH) * 0.5f, 0.0f, 0.0f);
        const vec3 p2((PILLAR_WIDTH + SIDE_ARC_LENGTH) * 0.5f, COLUMN_HEIGHT, 0.0f);
        const float pr = SIDE_ARC_LENGTH * 0.5f - FLOOR_HEIGHT;

        msh.setPaintColor(BASE_COLOR);
        mesh_generate_pillar(msh, p1, p2, pr, CYLINDER_DETAIL, 0, CSG_NO_FRONT);
        msh.advanceBlockId();

        if(hasRightExtent())
        {
          const vec3 p3 = p2 + vec3(0.0f, pr * 0.5f, 0.0f);

          msh.setPaintColor(MED_COLOR);

          mesh_generate_box(msh, p3, pr, pr, EXTENT_SMALL,
              (hasRightExtentForward() ? CSG_NO_BACK : CSG_ALL) &
              (hasRightExtentBackward() ? CSG_NO_FRONT : CSG_ALL));
          msh.advanceBlockId();

          if(hasRightExtentForward())
          {
            for(float ii = EXTENT_SMALL; ((EXTENT - 0.49f * EXTENT_SMALL) > ii); ii += EXTENT_SMALL)
            {
              mesh_generate_box(msh, p3 + vec3(0.0f, 0.0f, ii), pr, pr, EXTENT_SMALL,
                  CSG_NO_FRONT & CSG_NO_BACK);
              msh.advanceBlockId();
            }
          }
          if(hasRightExtentBackward())
          {
            for(float ii = EXTENT_SMALL; ((EXTENT - 0.49f * EXTENT_SMALL) > ii); ii += EXTENT_SMALL)
            {
              mesh_generate_box(msh, p3 - vec3(0.0f, 0.0f, ii), pr, pr, EXTENT_SMALL,
                  CSG_NO_FRONT & CSG_NO_BACK);
              msh.advanceBlockId();
            }
          }
        }
      }

      // Generate the actual mesh.
      m_mesh = msh.insert(buf, seed);
    }

    /// Tell if left pillar exists.
    ///
    /// \return True if yes, false if no.
    bool hasLeftPillar() const
    {
      return (AQUEDUCT_NONE < m_left);
    }

    /// Tell if left pillar exists.
    ///
    /// \return True if yes, false if no.
    bool hasRightPillar() const
    {
      return (AQUEDUCT_NONE < m_right);
    }

  public:
    /// Accessor.
    ///
    /// \return Pointer to the mesh.
    const Mesh& getMesh() const
    {
      return *m_mesh;
    }

    /// Tell if has left extent.
    ///
    /// \return True if yes, false if no.
    bool hasLeftExtent() const
    {
      return (AQUEDUCT_PILLAR_NONE < m_left);
    }

    /// Tell if has left extent backwards.
    ///
    /// \return True if yes, false if no.
    bool hasLeftExtentBackward() const
    {
      return (AQUEDUCT_PILLAR_BACKWARD == m_left) || (AQUEDUCT_PILLAR_BOTH == m_left);
    }

    /// Tell if has left extent forwards.
    ///
    /// \return True if yes, false if no.
    bool hasLeftExtentForward() const
    {
      return (AQUEDUCT_PILLAR_FORWARD == m_left) || (AQUEDUCT_PILLAR_BOTH == m_left);
    }

    /// Tell if has left extent.
    ///
    /// \return True if yes, false if no.
    bool hasRightExtent() const
    {
      return (AQUEDUCT_PILLAR_NONE < m_right);
    }

    /// Tell if has right extent backwards.
    ///
    /// \return True if yes, false if no.
    bool hasRightExtentBackward() const
    {
      return (AQUEDUCT_PILLAR_BACKWARD == m_right) || (AQUEDUCT_PILLAR_BOTH == m_right);
    }

    /// Tell if has right extent forwards.
    ///
    /// \return True if yes, false if no.
    bool hasRightExtentForward() const
    {
      return (AQUEDUCT_PILLAR_FORWARD == m_right) || (AQUEDUCT_PILLAR_BOTH == m_right);
    }
};

/// Convenience typedef.
typedef uptr<Aqueduct> AqueductUptr;

#endif
