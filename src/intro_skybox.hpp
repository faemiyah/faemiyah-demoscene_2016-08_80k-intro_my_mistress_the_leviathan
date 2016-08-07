#ifndef INTRO_SKYBOX_HPP
#define INTRO_SKYBOX_HPP

#include "intro_csg.hpp"

/// Skybox class.
///
/// Presents a fake perspective skybox cube pretending to be environment.
///
/// Order for faces from coordinate axises: left, right, down, up, front, back.
class Skybox
{
  public:
    /// Convenience typedef.
    ///
    /// Takes coordinates in as spherical coordinates.
    typedef vec3 (*SkyboxColoringFunc)(float horiz, float vert, const Skybox &skybox);

  public:
    /// Object count for skybox.
    static const unsigned OBJECT_COUNT = 6;

  private:
    /// Mesh related to the skybox.
    MeshUptr m_mesh;

    /// Objects related to the skybox.
    ObjectUptr m_objects[6];

    /// Textures related to the skybox.
    Texture m_textures[6];

    /// Images related to the skybox.
    ImageRGBUptr m_images[6];

    /// Noise images.
    ImageGrayUptr m_noise_images[9];

    /// Forward color 1.
    vec3 m_color_forward_1;

    /// Forward color 2.
    vec3 m_color_forward_2;

    /// Backward color.
    vec3 m_color_backward;

  public:
    /// Constructor.
    ///
    /// \param fw1 First forward color.
    /// \param fw2 First forward color.
    /// \param bk Backward color.
    Skybox(const vec3 &fw1 = vec3(0.0f, 0.0f, 0.0f), const vec3 &fw2 = vec3(0.0f, 0.0f, 0.0f),
        const vec3 &bk = vec3(0.0f, 0.0f, 0.0f)) :
      m_color_forward_1(fw1),
      m_color_forward_2(fw2),
      m_color_backward(bk) { }

  private:
    /// Color a ceiling.
    ///
    /// \param angle Angle at the middle, Positive is up, negative down.
    /// \param func Coloring function.
    /// \param detail Coloring detail.
    ImageRGBUptr colorCeiling(float angle, SkyboxColoringFunc func, unsigned detail)
    {
      ImageRGBUptr ret(new ImageRGB(detail, detail));
      const float fdetail_mul = 1.0f / (static_cast<float>(detail) * 0.5f);

      for(unsigned ii = 0; (detail > ii); ++ii)
      {
        float fi = (static_cast<float>(ii) + 0.5f) * fdetail_mul - 1.0f;

        for(unsigned jj = 0; (detail > jj); ++jj)
        {
          float fj = (static_cast<float>(jj) + 0.5f) * fdetail_mul - 1.0f;
          float hangle = dnload_atan2f(fi, fj);
          float hyp = dnload_sqrtf((fi * fi) + (fj * fj));
          float vangle = dnload_atanf(hyp);

          if(angle >= 0.0f)
          {
            // If at top, must flip orientation.
            hangle = -hangle;
            vangle = angle - vangle;
          }
          else
          {
            vangle = angle + vangle;
          }

          ret->setPixel(ii, jj, func(hangle, vangle, *this));
        }
      }

      return ret;
    }

    /// Color one wall.
    ///
    /// \param angle Neutral angle.
    /// \param func Coloring function.
    /// \param detail Coloring detail.
    ImageRGBUptr colorWall(float angle, SkyboxColoringFunc func, unsigned detail)
    {
      ImageRGBUptr ret(new ImageRGB(detail, detail));
      const float fdetail_mul = 1.0f / (static_cast<float>(detail) * 0.5f);

      for(unsigned ii = 0; (detail > ii); ++ii)
      {
        float fi = (static_cast<float>(ii) + 0.5f) * fdetail_mul - 1.0f;
        float hangle = dnload_atanf(fi) + angle;
        float hyp = dnload_sqrtf((fi * fi) + 1);

        for(unsigned jj = 0; (detail > jj); ++jj)
        {
          float fj = (static_cast<float>(jj) + 0.5f) * fdetail_mul - 1.0f;
          float vangle = dnload_atanf(fj / hyp);

          ret->setPixel(ii, jj, func(hangle, vangle, *this));
        }
      }

      return ret;
    }

    /// Free noise images.
    void freeNoise()
    {
      for(unsigned ii = 0; (9 > ii); ++ii)
      {
        m_noise_images[ii].reset();
      }
    }

    /// Generate noise images.
    void generateNoise()
    {
      bsd_srand(5);

      for(unsigned ii = 0, aa = 8, bb = 4; (9 > ii); ++ii, aa = aa * 9 / 5, bb = bb * 9 / 5)
      {
        m_noise_images[ii] = new ImageGray(aa, bb);
        m_noise_images[ii]->noise();
      }
    }

  public:
    /// Construct the skybox.
    ///
    /// \param buf Buffer to insert to.
    /// \param size Size (radius).
    /// \param func Coloring function.
    /// \param detail Coloring detail.
    void construct(GeometryBuffer &buffer, float size, SkyboxColoringFunc func, unsigned detail = 256)
    {
      LogicalMesh msh;

      mesh_generate_box(msh, vec3(0.0f, 0.0f, 0.0f), size, size, size,
          CsgFlag(CsgFlag::INVERSE, CsgFlag::SPLIT));

      m_mesh = msh.insert(buffer);

      for(unsigned ii = 0; (OBJECT_COUNT > ii); ++ii)
      {
        m_objects[ii] = new Object(m_mesh->getBlock(ii), mat4::identity(), &m_textures[ii]);
      }

      generateNoise();

      m_images[0] = colorWall(0.0f, func, detail);
      m_images[1] = colorWall(static_cast<float>(M_PI), func, detail);
      m_images[2] = colorCeiling(-static_cast<float>(M_PI * 0.5), func, detail);
      m_images[3] = colorCeiling(static_cast<float>(M_PI * 0.5), func, detail);
      m_images[4] = colorWall(static_cast<float>(M_PI * 0.5), func, detail);
      m_images[5] = colorWall(static_cast<float>(M_PI * 1.5), func, detail);

      freeNoise();
    }

    /// Accessor.
    ///
    /// \return First forward color.
    const vec3& getColorForward1() const
    {
      return m_color_forward_1;
    }
    /// Setter.
    ///
    /// \param op New first forward color.
    void setColorForward1(const vec3 &op)
    {
      m_color_forward_1 = op;
    }

    /// Accessor.
    ///
    /// \return Second forward color.
    const vec3& getColorForward2() const
    {
      return m_color_forward_2;
    }
    /// Setter.
    ///
    /// \param op New second forward color.
    void setColorForward2(const vec3 &op)
    {
      m_color_forward_2 = op;
    }

    /// Accessor.
    ///
    /// \return Backward color.
    const vec3& getColorBackward() const
    {
      return m_color_backward;
    }
    /// Setter.
    ///
    /// \param op New backward color.
    void setColorBackward(const vec3 &op)
    {
      m_color_backward = op;
    }

    /// Accessor.
    ///
    /// \param idx Index of object to access.'
    const Object& getObject(unsigned idx) const
    {
      return *m_objects[idx];
    }

    /// Update skybox to GPU.
    void update()
    {
      for(unsigned ii = 0; (OBJECT_COUNT > ii); ++ii)
      {
        m_textures[ii].update(*m_images[ii], BILINEAR, true);
        m_images[ii].reset(); // Can free image after update.
      }
    }

  private:
    /// Ger direction from color.
    ///
    /// \param hangle Horizontal angle.
    /// \param vangle Vertical angle.
    static vec3 dir_from_angles(float hangle, float vangle)
    {
      float rx = dnload_cosf(hangle);
      float rz = dnload_sinf(hangle);
      float hyp = dnload_cosf(vangle);
      float ry = dnload_sinf(vangle);

      return normalize(vec3(rx * hyp, ry, rz * hyp));
    }

  public:
    /// Horrori coloring function.
    ///
    /// \param hangle Horizontal angle.
    /// \param vangle Vertical angle.
    static vec3 coloring_func_horrori(float hangle, float vangle, const Skybox &skybox)
    {
      vec3 dir = Skybox::dir_from_angles(hangle, vangle);
      float rh = hangle / static_cast<float>(M_PI * 2.0);
      float rv = vangle / static_cast<float>(M_PI);
      float strength = dot(vec3(-1.0f, 0.2f, 0.0f), dir);
      float sun = std::max(strength, 0.0f);

      float sn = skybox.m_noise_images[0]->sampleLinear(rh, rv) * 1.0f +
        skybox.m_noise_images[1]->sampleLinear(rh, rv) * 0.9f +
        skybox.m_noise_images[2]->sampleNearest(rh, rv) * 0.3f +
        skybox.m_noise_images[3]->sampleNearest(rh, rv) * 0.25f +
        skybox.m_noise_images[4]->sampleNearest(rh, rv) * 0.2f +
        skybox.m_noise_images[5]->sampleNearest(rh, rv) * 0.15f +
        skybox.m_noise_images[6]->sampleNearest(rh, rv) * 0.1f +
        skybox.m_noise_images[7]->sampleNearest(rh, rv) * 0.05f;

      if(strength < 0.0f)
      {
        strength = -(strength * strength);
      }

      float ambient = dot(vec3(-1.0f, 0.1f, 0.0f), dir) * 0.4f + 0.6f;

      float mul = std::abs((vangle - 0.1f) * static_cast<float>(1.0 / M_PI)) * 8.0f * (strength * 0.3f + 0.4f);

      float sum = skybox.m_noise_images[0]->sampleLinear(rh, rv) * mul +
        skybox.m_noise_images[1]->sampleLinear(rh, rv) * mul * 0.9f +
        skybox.m_noise_images[2]->sampleLinear(rh, rv) * mul * 0.8f +
        skybox.m_noise_images[3]->sampleLinear(rh, rv) * mul * 0.7f +
        skybox.m_noise_images[4]->sampleLinear(rh, rv) * mul * 0.6f +
        skybox.m_noise_images[5]->sampleNearest(rh, rv) * mul * 0.5f +
        skybox.m_noise_images[6]->sampleNearest(rh, rv) * mul * 0.4f +
        skybox.m_noise_images[7]->sampleNearest(rh, rv) * mul * 0.3f +
        skybox.m_noise_images[8]->sampleNearest(rh, rv) * mul * 0.2f;

      sum = dnload_sqrtf(sum);

      return vec3(sun * sun * (sn * 0.33f + 0.7f), ambient, sum);
    }

    /// Skybox coloring function.
    ///
    /// \param hangle Horizontal angle.
    /// \param vangle Vertical angle.
    static vec3 coloring_func_normal(float hangle, float vangle, const Skybox &skybox)
    {
      vec3 dir = Skybox::dir_from_angles(hangle, vangle);
      float rh = hangle / static_cast<float>(M_PI * 2.0);
      float rv = vangle / static_cast<float>(M_PI);

      float mul = std::abs(vangle * static_cast<float>(1.0 / M_PI)) * 1.45f;
      float sum = skybox.m_noise_images[0]->sampleLinear(rh, rv) * mul +
        skybox.m_noise_images[1]->sampleLinear(rh, rv) * mul * 0.9f +
        skybox.m_noise_images[2]->sampleLinear(rh, rv) * mul * 0.8f +
        skybox.m_noise_images[3]->sampleLinear(rh, rv) * mul * 0.7f +
        skybox.m_noise_images[4]->sampleLinear(rh, rv) * mul * 0.6f +
        skybox.m_noise_images[5]->sampleNearest(rh, rv) * mul * 0.5f +
        skybox.m_noise_images[6]->sampleNearest(rh, rv) * mul * 0.4f +
        skybox.m_noise_images[7]->sampleNearest(rh, rv) * mul * 0.3f +
        skybox.m_noise_images[8]->sampleNearest(rh, rv) * mul * 0.2f;

#if 0
      if(0.0f > hangle)
      {
        hangle += static_cast<float>(M_PI * 2.0);
      }

      return vec3(hangle / static_cast<float>(M_PI * 2.0), 0.0f, (vangle / static_cast<float>(M_PI * 0.5)) * 0.5f + 0.5f);
#else
      return vec3(dot(vec3(-1.0f, 0.1f, 0.0f), dir) * 0.5f + 0.5f, 0.0f, sum);
#endif
    }

    /// Overcast coloring function.
    ///
    /// \param hangle Horizontal angle.
    /// \param vangle Vertical angle.
    static vec3 coloring_func_overcast(float hangle, float vangle, const Skybox &skybox)
    {
      vec3 dir = Skybox::dir_from_angles(hangle, vangle);
      float rh = hangle / static_cast<float>(M_PI * 2.0);
      float rv = vangle / static_cast<float>(M_PI);
      float strength = dot(vec3(-1.0f, 0.2f, 0.0f), dir) * 0.5f + 0.5f;
      float sun = std::max(strength, 0.0f);

      if(strength < 0.0f)
      {
        strength = -(strength * strength);
      }

      float mul = (static_cast<float>(M_PI) - std::abs(vangle - 0.1f)) * 0.7f;

      float sum = skybox.m_noise_images[0]->sampleLinear(rh, rv) * mul +
        skybox.m_noise_images[1]->sampleLinear(rh, rv) * mul * 0.9f +
        skybox.m_noise_images[2]->sampleLinear(rh, rv) * mul * 0.8f +
        skybox.m_noise_images[3]->sampleLinear(rh, rv) * mul * 0.7f +
        skybox.m_noise_images[4]->sampleLinear(rh, rv) * mul * 0.6f +
        skybox.m_noise_images[5]->sampleNearest(rh, rv) * mul * 0.5f +
        skybox.m_noise_images[6]->sampleNearest(rh, rv) * mul * 0.4f +
        skybox.m_noise_images[7]->sampleNearest(rh, rv) * mul * 0.3f +
        skybox.m_noise_images[8]->sampleNearest(rh, rv) * mul * 0.2f;
      //sum = dnload_sqrtf(sum) * 0.1f;
      
      float ambient = 1.0f - (std::abs(vangle) / static_cast<float>(M_PI * 0.5));

      return vec3(sun * sun * 0.4f, sum * 0.1f, ambient);
    }
};

#endif
