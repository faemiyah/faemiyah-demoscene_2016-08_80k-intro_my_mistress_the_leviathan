#ifndef INTRO_HAAMU_HPP
#define INTRO_HAAMU_HPP

#include "intro_sprite.hpp"
#include "model_ghost.hpp"

// If defined, use simle haamu shape.
#undef HAAMU_SHAPE_SIMPLE

/// Haamu model scale.
static const float HAAMU_MODEL_SCALE = 0.00009f;

/// One haamu sprite.
class HaamuSprite : public Sprite<ImageLA>
{
  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param seed Random seed.
    HaamuSprite(GeometryBuffer &buf, unsigned seed) :
      Sprite(buf, 0.8f, 1.4f)
    {
      m_image = generate_image(48, 64, seed);
    }

  private:
    /// Generate image for ghost.
    ///
    /// \param width Width.
    /// \param height Height.
    /// \param seed Random seed.
    static ImageLAUptr generate_image(unsigned width, unsigned height, unsigned seed)
    {
      static const float ELONGATION_X = 0.5f;
      static const float ELONGATION_Y = 0.33f;
      static const float UP_MUL = (1.0f - ELONGATION_Y);
      uptr<ImageLA> ret(new ImageLA(width, height));
      ImageGrayUptr noise[6];

      for(unsigned ii = 0; (ii < 6); ++ii)
      {
        noise[ii] = new ImageGray((ii + 1) * 64 / 5, (ii + 1) * 31 / 5);
        noise[ii]->noise();
      }

      vec2 center(static_cast<float>(width) * ELONGATION_X, static_cast<float>(height) * ELONGATION_Y);

      bsd_srand(seed);

      for(unsigned ii = 0; (width > ii); ++ii)
      {
        float fi = static_cast<float>(ii) / static_cast<float>(width);

        for(unsigned jj = 0; (height > jj); ++jj)
        {
          float fj = static_cast<float>(jj) / static_cast<float>(height);
          vec2 curr(static_cast<float>(ii), static_cast<float>(jj));
          vec2 diff = curr - center;

          if(diff[1] > 0.0f)
          {
            diff[1] *= UP_MUL;
          }
          else
          {
            diff[1] /= UP_MUL;
          }
          float fstr = std::max(1.0f - (length(diff) / (static_cast<float>(width) * ELONGATION_X)), 0.0f);

          float sum = noise[0]->sampleLinear(fi, fj) * 1.0f +
              noise[1]->sampleLinear(fi, fj) * 0.9f +
              noise[2]->sampleLinear(fi, fj) * 0.8f +
              noise[3]->sampleNearest(fi, fj) * 0.7f +
              noise[4]->sampleNearest(fi, fj) * 0.6f +
              noise[5]->sampleNearest(fi, fj) * 0.5f;

          float alpha = fstr * sum;

          ret->setPixel(ii, jj, alpha, alpha);
        }
      }

      ret->normalize(0, 0.0f);
      ret->normalize(1, 0.0f);
      return ret;
    }
};

/// Convenience typedef.
typedef uptr<HaamuSprite> HaamuSpriteUptr;

/// One haamu shape.
class HaamuShape
{
  private:
    /// Shape generated.
    MeshUptr m_mesh;

    /// Object.
    ObjectUptr m_object;

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param seed Random seed.
    HaamuShape(GeometryBuffer &buf, unsigned seed)
    {
      construct(buf, seed);
      m_object = new Object(m_mesh->getBlock(0), mat4::identity());
    }

  private:
    /// Construction.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param seed Random seed.
    void construct(GeometryBuffer &buf, unsigned seed)
    {
      LogicalMesh msh;

#if defined(HAAMU_SHAPE_SIMPLE)
      static const unsigned HAAMU_DETAIL = 8;
      static const unsigned HAAMU_DETAIL_UP = 4;
      static const float HAAMU_WIDTH = 0.5f;
      static const float HAAMU_HEIGHT_DOWN = 1.0f;
      msh.setPaintColor(1.0f, 1.0f, 1.0f);

      // Copypasta from floating island because I'm lazy.
      float fdetail_mul = 1.0f / static_cast<float>(HAAMU_DETAIL);
      float ystep = HAAMU_HEIGHT_DOWN * fdetail_mul; 

      // Down.
      for(unsigned ii = 0; (HAAMU_DETAIL > ii); ++ii)
      {
        float h1 = -static_cast<float>(ii + 0) * ystep;
        float h2 = -static_cast<float>(ii + 1) * ystep;
        float r1 = static_cast<float>(ii + 0) * fdetail_mul;
        float r2 = static_cast<float>(ii + 1) * fdetail_mul;
        r1 = (1.0f - (r1 * r1)) * HAAMU_WIDTH;
        r2 = (1.0f - (r2 * r2)) * HAAMU_WIDTH;

        // Last cycle.
        if(HAAMU_DETAIL == ii + 1)
        {
          unsigned base = msh.getLogicalVertexCount();

          msh.addVertex(0.0f, h2, 0.0f);

          msh.addFace(base, base - 4, base - 3);
          msh.addFace(base, base - 3, base - 2);
          msh.addFace(base, base - 2, base - 1);
          msh.addFace(base, base - 1, base - 4);
        }
        else
        {
          mesh_generate_polygon_ring(msh,
              h1, r1, HAAMU_DETAIL - ii + 3, seed + ii,
              h2, r2, HAAMU_DETAIL - ii - 1 + 3, seed + ii + 1,
              false);
        }
      }

      // Up.
      for(unsigned ii = 0; (HAAMU_DETAIL_UP > ii); ++ii)
      {
        float h1 = static_cast<float>(ii + 0) * ystep;
        float h2 = static_cast<float>(ii + 1) * ystep;
        float r1 = static_cast<float>(ii + 0) * fdetail_mul;
        float r2 = static_cast<float>(ii + 1) * fdetail_mul;
        r1 = (1.0f - (r1 * r1)) * HAAMU_WIDTH;
        r2 = (1.0f - (r2 * r2)) * HAAMU_WIDTH;

        // Last cycle.
        if(HAAMU_DETAIL_UP == ii + 1)
        {
          unsigned base = msh.getLogicalVertexCount();

          msh.addVertex(0.0f, h2, 0.0f);

          for(unsigned jj = 0; (jj < (HAAMU_DETAIL - ii + 3)); ++jj)
          {
            if(jj == 0)
            {
              msh.addFace(base, base - (HAAMU_DETAIL - ii + 3), base - 1);
            }
            else
            {
              msh.addFace(base, base - jj, base - jj - 1);
            }
          }
        }
        else
        {
          mesh_generate_polygon_ring(msh,
              h1, r1, HAAMU_DETAIL - ii + 3, seed + ii,
              h2, r2, HAAMU_DETAIL - ii - 1 + 3, seed + ii + 1,
              true);
        }
      }

#else
      // Shape.
      msh.initFromData(g_vertices_ghost, g_weights_ghost, g_indices_ghost,
          sizeof(g_vertices_ghost) / sizeof(*g_vertices_ghost),
          sizeof(g_weights_ghost) / sizeof(*g_weights_ghost),
          sizeof(g_indices_ghost) / sizeof(*g_indices_ghost),
          HAAMU_MODEL_SCALE);
#endif

      // If seed specified, mangle texture coordinates.
      if(seed != 0)
      {
        for(unsigned ii = 0; (msh.getLogicalFaceCount() > ii); ++ii)
        {
          LogicalFace &face = msh.getLogicalFace(ii);

          face.setTexcoord(0, vec2(0.0f, 0.0f));
          face.setTexcoord(1, vec2(0.0f, 0.0f));
          face.setTexcoord(2, vec2(0.0f, 0.0f));

          unsigned sidx = urand(8);
          if(3 > sidx)
          {
            face.setTexcoord(sidx, vec2(1.0f, 1.0f));
          }
        }

        m_mesh = msh.insert(buf);
      }
      else
      {
        m_mesh = msh.insert(buf, 1);
      }
    }

  public:
    /// Accessor.
    ///
    /// \return Object.
    const Object& getObject() const
    {
      return *m_object;
    }
};

/// Convenience typedef.
typedef uptr<HaamuShape> HaamuShapeUptr;

/// The haamu figure.
///
/// Collects different random shapes.
class Haamu
{
  public:
    enum AnimationEnum
    {
      /// Animation index.
      ANIMATION_CURSE = 0,

      /// Animation index.
      ANIMATION_DEAD,

      /// Animation index.
      ANIMATION_TURN,

      /// Animation index.
      ANIMATION_WALK,
  
      /// Animation index.
      ANIMATION_COUNT
    };

  private:
    /// Number of shapes and sprites.
    static const unsigned SHAPE_COUNT = 6;

  private:
    /// Shapes.
    HaamuShapeUptr m_shapes[SHAPE_COUNT];

    /// Sprites.
    HaamuSpriteUptr m_sprites[SHAPE_COUNT];

    /// Armature.
    Armature m_armature;

    /// Animation array.
    Animation m_animation[ANIMATION_COUNT];

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to insert to.
    Haamu(GeometryBuffer &buf)
    {
      for(unsigned ii = 0; (SHAPE_COUNT > ii); ++ii)
      {
        m_shapes[ii] = new HaamuShape(buf, ii); // First shape has no mangled texcoords.
        m_sprites[ii] = new HaamuSprite(buf, ii + 1);
      }

      // Armature.
      m_armature.initFromData(g_bones_ghost, g_armature_ghost,
          sizeof(g_bones_ghost) / sizeof(*g_bones_ghost),
          sizeof(g_armature_ghost) / sizeof(*g_armature_ghost),
          HAAMU_MODEL_SCALE);

      // Animations.
      m_animation[ANIMATION_CURSE].initFromData(g_animation_ghost_curse,
          sizeof(g_bones_ghost) / sizeof(*g_bones_ghost),
          sizeof(g_animation_ghost_curse) / sizeof(*g_animation_ghost_curse),
          HAAMU_MODEL_SCALE);
      m_animation[ANIMATION_DEAD].initFromData(g_animation_ghost_dead,
          sizeof(g_bones_ghost) / sizeof(*g_bones_ghost),
          sizeof(g_animation_ghost_dead) / sizeof(*g_animation_ghost_dead),
          HAAMU_MODEL_SCALE);
      m_animation[ANIMATION_TURN].initFromData(g_animation_ghost_turn,
          sizeof(g_bones_ghost) / sizeof(*g_bones_ghost),
          sizeof(g_animation_ghost_turn) / sizeof(*g_animation_ghost_turn),
          HAAMU_MODEL_SCALE);
      m_animation[ANIMATION_WALK].initFromData(g_animation_ghost_walk,
          sizeof(g_bones_ghost) / sizeof(*g_bones_ghost),
          sizeof(g_animation_ghost_walk) / sizeof(*g_animation_ghost_walk),
          HAAMU_MODEL_SCALE);
    }

  public:
    /// Get random shape object.
    ///
    /// \param seed Random seed for acquiring the object.
    const Object& getShape(unsigned seed) const
    {
      if(seed)
      {
        bsd_srand(seed);
  
        return m_shapes[urand(SHAPE_COUNT - 1) + 1]->getObject();
      }
      return m_shapes[0]->getObject();
    }

    /// Get random sprite object.
    ///
    /// \param seed Random seed for acquiring the object.
    const Object& getSprite(unsigned seed) const
    {
      bsd_srand(seed);

      return m_sprites[urand(SHAPE_COUNT)]->getObject();
    }

    /// Insert into state.
    ///
    /// \param state State to insert to.
    /// \param transform Transformation.
    /// \param aidx Animation to play.
    /// \param atime Animation time.
    /// \param pass Pass to insert to.
    /// \param seed Which randomly seeded haamu to take?
    void insertShape(State &state, const mat4 &transform, AnimationEnum aidx, float atime, unsigned pass = 0,
        unsigned seed = 0)
    {
#if defined(HAAMU_SHAPE_SIMPLE)
      (void)aidx;
      (void)atime;
      state.addObject(getShape(seed), transform, pass);
#else
      AnimationState& anim = state.newAnimationState();

      anim.interpolateFrom(m_armature, m_animation[aidx], atime);

      state.addObject(getShape(seed), transform, anim, pass);
#endif
    }

    /// Update this to GPU.
    void update()
    {
      for(HaamuSpriteUptr &vv : m_sprites)
      {
        vv->update();
      }
    }
};

/// Convenience typedef.
typedef uptr<Haamu> HaamuUptr;

#endif
