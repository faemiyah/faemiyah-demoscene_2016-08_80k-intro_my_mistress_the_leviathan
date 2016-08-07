#ifndef INTRO_MOELLI_HPP
#define INTRO_MOELLI_HPP

#include "intro_sprite.hpp"

/// 'Mölli' eye thingy.
class MoelliSprite : public Sprite<ImageRGBA>
{
  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param seed Random seed.
    MoelliSprite(GeometryBuffer &buf, unsigned seed) :
      Sprite(buf, 2.5f)
    {
      m_image = generate_image(64, 64, seed);
    }

  private:
    /// Generate image for moelli eye thing.
    ///
    /// \param width Width.
    /// \param height Height.
    /// \param seed Random seed.
    static ImageRGBAUptr generate_image(unsigned width, unsigned height, unsigned seed)
    {
      uptr<ImageRGBA> ret(new ImageRGBA(width, height));
      ImageGrayUptr noise[6];

      for(unsigned ii = 0; (ii < 6); ++ii)
      {
        noise[ii] = new ImageGray((ii + 1) * 64 / 5, (ii + 1) * 31 / 5);
        noise[ii]->noise();
      }

      vec2 center = vec2(static_cast<float>(width), static_cast<float>(height)) * 0.5f;

      bsd_srand(seed);

      for(unsigned ii = 0; (width > ii); ++ii)
      {
        float fi = static_cast<float>(ii) / static_cast<float>(width);

        for(unsigned jj = 0; (height > jj); ++jj)
        {
          float fj = static_cast<float>(jj) / static_cast<float>(height);
          vec2 curr(static_cast<float>(ii), static_cast<float>(jj));
          vec2 diff = curr - center;
          float fstr = std::max(1.0f - (length(diff) / (static_cast<float>(width)) * 2.0f), 0.0f);

          float sum = noise[0]->sampleLinear(fi, fj) * 1.0f +
              noise[1]->sampleLinear(fi, fj) * 0.9f +
              noise[2]->sampleLinear(fi, fj) * 0.8f +
              noise[3]->sampleNearest(fi, fj) * 0.7f +
              noise[4]->sampleNearest(fi, fj) * 0.6f +
              noise[5]->sampleNearest(fi, fj) * 0.5f;

          float alpha = std::min(linear_step(0.0f, 0.5f, fstr), linear_step_down(0.8f, 0.9f, fstr)) * sum;
          float ring_val = std::min(linear_step(0.1f, 0.4f, fstr), linear_step_down(0.45f, 0.6f, fstr));
          vec3 col = mix(vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.6f, 0.0f), ring_val);

          ret->setPixel(ii, jj, col * alpha, alpha);
        }
      }

      ret->normalize(0, 0.0f);
      ret->normalize(1, 0.0f);
      ret->normalize(2, 0.0f);
      ret->normalize(3, 0.0f);
      return ret;
    }
};

/// Convenience typedef.
typedef uptr<MoelliSprite> MoelliSpriteUptr;

/// 'Mölli' part.
class MoelliPart
{
  private:
    /// Mesh of this part.
    MeshUptr m_mesh;

    /// Object for this part.
    ObjectUptr m_object;

    /// Movement direction for this part.
    vec3 m_direction;

  public:
    /// Constructor.
    ///
    /// \param sx X direction.
    /// \param sy Y direction.
    /// \param sz Z direction.
    /// \param inner Inner distance.
    /// \param outer Outer distance.
    /// \param upper Upper distance.
    /// \param buf Buffer to insert to.
    /// \param tex Texture to use.
    MoelliPart(float sx, float sy, float sz, float inner, float outer, float upper, GeometryBuffer &buf,
        unsigned seed, Texture* tex = NULL) :
      m_direction(sx, sy, sz)
    {
      m_mesh = construct(inner, outer, upper, buf, seed);

      m_object = new Object(m_mesh->getBlock(0), mat4::identity(), tex);
    }

  private:
    /// Construct the part.
    ///
    /// \param inner Inner distance.
    /// \param outer Outer distance.
    /// \param upper Upper distance.
    /// \param buf Buffer to insert to.
    /// \param seed Random seed.
    MeshUptr construct(float inner, float outer, float upper, GeometryBuffer &buf, unsigned seed)
    {
      static const float ELONGATION_HORIZONTAL = 0.8f;

      LogicalMesh msh;

      msh.setPaintColor(1.0f, 0.9f, 0.8f);
      msh.setShadowGeneration(true);

      msh.addVertex(inner * m_direction[0], 0.0f, 0.0f);
      msh.addVertex(0.0f, inner * m_direction[1], 0.0f);
      msh.addVertex(0.0f, 0.0f, inner * m_direction[2]);

      msh.addVertex(outer * m_direction[0] * ELONGATION_HORIZONTAL, 0.0f, 0.0f);
      msh.addVertex(0.0f, upper * m_direction[1], 0.0f);
      msh.addVertex(0.0f, 0.0f, outer * m_direction[2] * ELONGATION_HORIZONTAL);

      msh.addFace(0, 2, 1);
      msh.addFace(2, 5, 4, 1U);
      msh.addFace(0, 1, 4, 3U);
      msh.addFace(0, 3, 5, 2U);
      msh.addFace(5, 3, 4);

      unsigned negatives = 0;
      for(unsigned ii = 0; (3 > ii); ++ii)
      {
        if(0.0f > m_direction[ii])
        {
          ++negatives;
        }
      }
      if(negatives % 2)
      {
        msh.flipLastFaces(5);
      }

      return msh.insert(buf, seed);
    }

  public:
    /// Accessor.
    ///
    /// \return Direction of the moelli.
    const vec3& getDirection() const
    {
      return m_direction;
    }

    /// Accessor.
    ///
    /// \return Mesh.
    const Mesh& getMesh() const
    {
      return *m_mesh;
    }

    /// Accessor.
    ///
    /// \return Object.
    const Object& getObject() const
    {
      return *m_object;
    }
};

/// Convenience typedef.
typedef uptr<MoelliPart> MoelliPartUptr;

/// The 'mölli' that casts some shadows.
class Moelli
{
  public:
    /// Number of parts. One for each diagonal.
    static const unsigned NUM_PARTS = 8;

    /// Number of eye sprites.
    static const unsigned NUM_SPRITES = 4;

  private:
    /// Parts.
    MoelliPartUptr m_parts[NUM_PARTS];

    /// Sprites.
    MoelliSpriteUptr m_sprites[NUM_SPRITES];

    /// Inner scale.
    float m_inner;

    /// Outer scale.
    float m_outer;

  public:
    /// Constructor.
    ///
    /// \param inner Inner scale.
    /// \param outer Outer scale.
    /// \param buf Buffer to insert to.
    /// \param tex Texture to use.
    Moelli(float inner, float outer, GeometryBuffer &buf, Texture* tex = NULL) :
      m_inner(inner),
      m_outer(outer)
    {
      for(unsigned ii = 0; (NUM_SPRITES > ii); ++ii)
      {
        m_sprites[ii] = new MoelliSprite(buf, ii + 1);
      }
      construct(buf, tex);
    }

  private:
    /// Construct.
    ///
    /// \param buf Buffer to insert to.
    /// \param tex Texture to use.
    void construct(GeometryBuffer &buf, Texture* tex)
    {
      static const float ELONGATION_DOWN = 1.3f;

      m_parts[0] = new MoelliPart(1.0f, 1.0f, 1.0f, m_inner, m_outer, m_outer, buf, 1, tex);
      m_parts[1] = new MoelliPart(-1.0f, 1.0f, 1.0f, m_inner, m_outer, m_outer, buf, 2, tex);
      m_parts[2] = new MoelliPart(-1.0f, 1.0f, -1.0f, m_inner, m_outer, m_outer, buf, 3, tex);
      m_parts[3] = new MoelliPart(1.0f, 1.0f, -1.0f, m_inner, m_outer, m_outer, buf, 4, tex);
      m_parts[4] = new MoelliPart(1.0f, -1.0f, 1.0f, m_inner, m_outer, m_outer * ELONGATION_DOWN, buf, 5, tex);
      m_parts[5] = new MoelliPart(-1.0f, -1.0f, 1.0f, m_inner, m_outer, m_outer * ELONGATION_DOWN, buf, 6, tex);
      m_parts[6] = new MoelliPart(-1.0f, -1.0f, -1.0f, m_inner, m_outer, m_outer * ELONGATION_DOWN, buf, 7, tex);
      m_parts[7] = new MoelliPart(1.0f, -1.0f, -1.0f, m_inner, m_outer, m_outer * ELONGATION_DOWN, buf, 8, tex);
    }

  public:
    /// Accessor.
    ///
    /// \param idx Index of part.
    /// \return Corresponding object.
    const MoelliPart& getPart(unsigned idx) const
    {
#if defined(USE_LD)
      if(NUM_PARTS <= idx)
      {
        std::ostringstream sstr;
        sstr << "trying to access part " << idx << " of " << NUM_PARTS;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return *m_parts[idx];
    }

    /// Accessor.
    ///
    /// \param seed Random seed.
    /// \return Randomized sprite.
    const Object& getSprite(unsigned seed) const
    {
      bsd_srand(seed);

      return m_sprites[urand(NUM_SPRITES)]->getObject();
    }

    /// Update this to GPU.
    void update()
    {
      for(MoelliSpriteUptr &vv : m_sprites)
      {
        vv->update();
      }
    }
};

/// Convenience typedef.
typedef uptr<Moelli> MoelliUptr;

#endif
