#ifndef INTRO_SPRITE_HPP
#define INTRO_SPRITE_HPP

/// 3D 'sprite' that is always turned towards viewer.
class SpriteBase
{
  private:
    /// Sprite generated.
    MeshUptr m_mesh;

    /// Object.
    ObjectUptr m_object;

  protected:
    /// Texture for sprite.
    Texture m_texture;

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param size Billboard size.
    /// \param elongation Elongation level.
    SpriteBase(GeometryBuffer &buf, float size, float elongation)
    {
      construct(buf, size, elongation);
      m_object = new Object(m_mesh->getBlock(0), mat4::identity(), &m_texture);
    }

  private:
    /// Construction.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param size Billboard size.
    /// \param elongation Elongation level.
    void construct(GeometryBuffer &buf, float size, float elongation)
    {
      LogicalMesh msh;

      msh.addVertex(-size * 0.5f, -size * 0.5f, 0.0f);
      msh.addVertex(size * 0.5f, -size * 0.5f, 0.0f);
      msh.addVertex(size * 0.5f, size * 0.5f * elongation, 0.0f);
      msh.addVertex(-size * 0.5f, size * 0.5f * elongation, 0.0f);

      // Faces in inverse order since this will be looking at camera.
      msh.addFace(1, 0, 3, 2U);

      m_mesh = msh.insert(buf, 0);
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

/// Sprite base extension for image type.
///
/// Templated with image type.
template<typename T> class Sprite : public SpriteBase
{
  protected:
    /// Image for sprite.
    uptr<T> m_image;

  public:
    /// Constructor.
    ///
    /// \param buf Geometry buffer to insert to.
    /// \param size Billboard size.
    /// \param elongation Elongation level.
    Sprite(GeometryBuffer &buf, float size = 1.0f, float elongation = 1.0f) :
      SpriteBase(buf, size, elongation) { }

  public:
    /// Update this to GPU.
    void update()
    {
      m_texture.update(*m_image, BILINEAR, true);
      m_image.reset();
    }
};

#endif
