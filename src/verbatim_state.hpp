#ifndef VERBATIM_STATE_HPP
#define VERBATIM_STATE_HPP

#include "verbatim_frame_buffer.hpp"
#include "verbatim_object_database.hpp"
#include "verbatim_object_reference.hpp"

/// State object.
///
/// State contains information of all objects to render. It can be generated in parallel while actual rendering
/// is being done by the main thread.
class State
{
  private:
    /// Minimum storage 'index'.
    static const unsigned STORAGE_BASE = static_cast<unsigned>('A');

    /// Size of random  storage.
    static const unsigned STORAGE_SIZE = static_cast<unsigned>('Z') + 1 - STORAGE_BASE;

    /// Convenience typedef.
    typedef seq<ObjectReference> ObjectReferenceSeq;

  private:
    /// Object references.
    seq<ObjectReferenceSeq> m_objects;

    /// Animation states.
    seq<AnimationState> m_animation_states;

    /// Projection.
    mat4 m_projection;

    /// Camera transform (viewified).
    mat4 m_camera;

    /// Screen space transform.
    mat4 m_screen_transform;

    /// Light space transform.
    mat4 m_light_transform;

    /// Light direction.
    vec3 m_light_dir;

    /// Current position (viewified camera does not provide this).
    vec3 m_position;

    /// Current frame id.
    int m_frame;

    /// Current animation state.
    unsigned m_current_animation_state;

    /// Current frame id, increases even if frame did not.
    unsigned m_frame_count;

    /// Floating point storage, for random variables.
    float m_storage_float[STORAGE_SIZE];

    /// Boolean storage, for random variables.
    bool m_storage_bool[STORAGE_SIZE];

  private:
    /// Get pass at index.
    ///
    /// Will create pass if not available.
    ///
    /// \param idx Pass index.
    /// \return Reference to pass.
    ObjectReferenceSeq& getPass(unsigned idx)
    {
      unsigned req_pass_count = idx + 1;

      if(m_objects.size() <= req_pass_count)
      {
        m_objects.resize(req_pass_count);
      }

      return m_objects[idx];
    }

  public:
    /// Add a reference to render an object.
    ///
    /// \param object Object to queue for rendering.
    /// \param transform Object transformation.
    /// \param pass Render pass id.
    /// \param optimistic Can we render in an optimistic manner? (default: true)
    void addObject(const Object &object, const mat4 &transform, unsigned pass = 0, bool optimistic = true,
        const AnimationState *state = NULL)
    {
      ObjectGroup *grp = object.getGroup();      

      ObjectReference &vv = getPass(pass).emplace_back(object, m_screen_transform, m_light_transform,
          transform, state);

      if(grp)
      {
        if(optimistic)
        {
          grp->addObjectReference(m_frame_count, vv);
        }
        else
        {
          vv.setOptimistic(false);
        }
      }
      else
      {
        vv.setOptimistic(true);
      }
    }
    /// Add a reference to render an object (wrapper).
    ///
    /// \param object Object to queue for rendering.
    /// \param program Program to use for rendering.
    /// \param pass Render pass id.
    void addObject(const Object &object, unsigned pass = 0)
    {
      addObject(object, object.unpackTransform(m_frame), pass);
    }
    /// Add a reference to render an object (wrapper).
    ///
    /// \param object Object to queue for rendering.
    /// \param state Animation state to use.
    /// \param transform Object transformation.
    /// \param pass Render pass id.
    void addObject(const Object &object, const mat4 &transform, const AnimationState &state, unsigned pass = 0)
    {
      addObject(object, transform, pass, true, &state);
    }
    /// Add a reference to render an object (wrapper).
    ///
    /// \param object Object to queue for rendering.
    /// \param state Animation state to use.
    /// \param pass Render pass id.
    void addObject(const Object &object, const AnimationState &state, unsigned pass = 0)
    {
      addObject(object, object.unpackTransform(m_frame), state, pass);
    }

    /// Add object without accounting for projection.
    ///
    /// \param object Object to queue for rendering.
    /// \param transform Object transformation.
    /// \param pass Render pass id.
    void addObjectScreenspace(const Object &object, const mat4 &transform, unsigned pass)
    {
      getPass(pass).emplace_back(object, transform, mat4::identity(), mat4::identity());
    }

    /// Add everything from an object database.
    ///
    /// \param db Database.
    /// \param pass Render pass id.
    void addObjectDatabase(const ObjectDatabase &db, unsigned pass = 0)
    {
      for(unsigned ii = 0; (ii < db.getObjectCount()); ++ii)
      {
        const Object& obj = db.getObject(ii);

        addObject(obj, pass);
      }
    }

    /// Add objects from an object database, constructing at limits.
    ///
    /// \param db Database.
    /// \param pos Position to render from.
    /// \param fw Collect objects that have bounding box lying in area this much forward.
    /// \param bk Collect objects that have bounding box lying in area this much backward.
    /// \param interp Fade-in interpolation range (<=0 to disable).
    /// \param pass Render pass id.
    void addObjectDatabaseConstructing(const ObjectDatabase &db, const vec3 &pos, float fw, float bk,
        float fade = 0.0f, unsigned pass = 0)
    {
      unsigned mid = db.find(pos[2]);

#if defined(USE_LD)
      if((fade >= fw) || (fade >= bk))
      {
        std::ostringstream sstr;
        sstr << "fade-in range " << fade << " must be smaller than advance ranges " << vec2(bk, fw);
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      // Forward.
      {
        float limit = pos[2] + fw;

        for(unsigned ii = mid; (ii < db.getObjectCount()); ++ii)
        {
          const Object &obj = db.getObject(ii);
          float cmp = obj.getBoundingVolume().getMinZ();

          if(cmp <= limit)
          {
            float diff = limit - cmp;
            if(diff <= fade)
            {
              mat4 aa = obj.unpackTransform(m_frame);
              mat4 bb = randomOffsetTransform(aa, obj.getCenter(), ii, diff / fade);

              addObject(obj, bb, pass, false);
            }
            else
            {
              addObject(obj, pass);
            }
          }
          else if(cmp - fade > limit)
          {
            break;
          }
        }
      }

      if(mid > 0)
      {
        float limit = pos[2] - bk;
        unsigned ii = mid - 1;

        for(;;)
        {
          const Object &obj = db.getObject(ii);
          float cmp = obj.getBoundingVolume().getMaxZ();

          if(cmp >= limit)
          {
            float diff = cmp - limit;
            if(diff <= fade)
            {
              //std::cout << "cmp " << cmp << " versus limit " << limit << ", diff: " << diff << std::endl <<
              //  obj.getBoundingVolume() << std::endl;
              mat4 aa = obj.unpackTransform(m_frame);
              mat4 bb = randomOffsetTransform(aa, obj.getCenter(), ii, diff / fade);

              addObject(obj, bb, pass, false);
            }
            else
            {
              addObject(obj, pass);
            }
          }
          else if(cmp + fade < limit)
          {
            break;
          }

          if(0 >= ii)
          {
            break;
          }
          --ii;
        }
      }
    }

    /// Add everything from an object database, swaying positions.
    ///
    /// \param db Database.
    /// \param amplitude Swaying amplitudes.
    /// \param phase Current phase.
    /// \param block_size Block size, objects in same block are considered to be in same phase.
    /// \param pass Render pass id.
    void addObjectDatabaseSwaying(const ObjectDatabase &db, const vec3 &amplitude, float phase,
        unsigned block_size, unsigned pass = 0)
    {
      for(unsigned ii = 0; (ii < db.getObjectCount()); ++ii)
      {
        const Object& obj = db.getObject(ii);

        bsd_srand(ii / block_size);

        float px = frand(-amplitude[0], amplitude[0]);
        float py = frand(-amplitude[1], amplitude[1]);
        float pz = frand(-amplitude[2], amplitude[2]);
        float ax = frand(static_cast<float>(M_PI));
        float ay = frand(static_cast<float>(M_PI));
        float az = frand(static_cast<float>(M_PI));
       
        px *= dnload_sinf(ax + phase * (brand() ? -1.0f : 1.0f));
        py *= dnload_sinf(ay + phase * (brand() ? -1.0f : 1.0f));
        pz *= dnload_sinf(az + phase * (brand() ? -1.0f : 1.0f));

        mat4 tr = obj.unpackTransform(m_frame);

        tr[12] += px;
        tr[13] += py;
        tr[14] += pz;

        addObject(obj, tr, pass);
      }
    }

    /// Draw geometry for this state to screen space.
    ///
    /// \param prg Program to use.
    /// \param pass Which pass to draw.
    void drawGeometry(const Program &prg, unsigned pass = 0) const
    {
      if(m_objects.size() > pass)
      {
        for(const ObjectReference &vv : m_objects[pass])
        {
          vv.drawGeometry(prg);
        }
      }
    }

    /// Draw shadow edges for this state.
    ///
    /// \param prg Program to use.
    /// \param pass Which pass to draw.
    void drawShadowEdges(const Program &prg, unsigned pass = 0) const
    {
      if(m_objects.size() > pass)
      {
        for(const ObjectReference &vv : m_objects[pass])
        {
          vv.drawShadowEdges(prg);
        }
      }
    }

    /// Draw shadow caps for this state.
    ///
    /// \param prg Program to use.
    /// \param pass Which pass to draw.
    void drawShadowCaps(const Program &prg, unsigned pass = 0) const
    {
      if(m_objects.size() > pass)
      {
        for(const ObjectReference &vv : m_objects[pass])
        {
          vv.drawShadowCaps(prg);
        }
      }
    }

    /// Accessor.
    ///
    /// \return Camera matrix.
    const mat4& getCameraMatrix() const
    {
      return m_camera;
    }

    /// Accessor.
    ///
    /// \return Frame number.
    int getFrame() const
    {
      return m_frame;
    }
    /// Set state frame.
    ///
    /// Although the frame id can be set as-is, frame count will always grow.
    ///
    /// \param prev Previous frame.
    /// \param frame Next frame number.
    void setFrame(const State &prev, int frame)
    {
      m_frame = frame;
      m_frame_count = prev.m_frame_count + 1;
    }
    /// Set state to initial frame.
    void setFrameInitial()
    {
      m_frame = 0;
      m_frame_count = 0;
    }
    /// Accessor.
    ///
    /// \return Total frame count.
    unsigned getFrameCount() const
    {
      return m_frame_count;
    }

    /// Accessor.
    ///
    /// \return Light direction vector.
    const vec3& getLightDirection() const
    {
      return m_light_dir;
    }
    /// Accessor.
    ///
    /// \return Light space transformation.
    const mat4& getLightTransform() const
    {
      return m_light_transform;
    }
    /// Set light.
    ///
    /// \param dir Light direction.
    /// \param eye Light focus point.
    /// \param view_width Width of viewing area at eye point.
    /// \param xfov Field of view at eye point.
    /// \param znear Near distance (from eye point).
    /// \param zfar Far distance (from eye point).
    /// \param width View width (default: 1).
    /// \param height View height (default: 1).
    void setLight(const vec3 &dir, const vec3 &eye, float view_width, float xfov, float znear, float zfar,
        unsigned width = 1, unsigned height = 1)
    {
      float dist = (view_width * 0.5f) / dnload_tanf(xfov * 0.5f);
      vec3 unit_dir = normalize(dir);
      vec3 pos = eye - (unit_dir * dist);
      mat4 cam = mat4::lookat(pos, eye);
      mat4 proj = mat4::projection(xfov, width, height, dist - znear, dist + zfar);
      m_light_transform = proj * viewify(cam);
      m_light_dir = -unit_dir;
    }
    /// Set light wrapper.
    ///
    /// Will take projection aspect ratio from shadow map dimensions.
    ///
    /// \param dir Light direction.
    /// \param eye Light focus point.
    /// \param view_width Width of viewing area at eye point.
    /// \param xfov Field of view at eye point.
    /// \param znear Near distance (from eye point).
    /// \param zfar Far distance (from eye point).
    /// \param fbo Shadow map framebuffer.
    void setLight(const vec3 &dir, const vec3 &eye, float view_width, float xfov, float znear, float zfar,
        const FrameBuffer &fbo)
    {
      setLight(dir, eye, view_width, xfov, znear, zfar, fbo.getWidth(), fbo.getHeight());
    }

    /// Accessor.
    ///
    /// \return Current position.
    const vec3& getPosition() const
    {
      return m_position;
    }
    /// Setter.
    ///
    /// \return New position.
    void setPosition(const vec3& op)
    {
      m_position = op;
    }

    /// Accessor.
    ///
    /// \return Projection matrix.
    const mat4& getProjectionMatrix() const
    {
      return m_projection;
    }

    /// Accessor.
    ///
    /// \return Screen space transformation.
    mat4 getScreenTransform() const
    {
      return m_projection * m_camera;
    }

    /// Tell if a pass has any objects.
    ///
    /// \param op Pass index.
    bool hasPass(unsigned op)
    {
      return ((m_objects.size() > op) && !m_objects[op].empty());
    }

    /// Initialize state for generation.
    ///
    /// \param projection Projection matrix.
    /// \param camera Camera transformation.
    void initialize(const mat4 &projection, const mat4 &camera)
    {
      for(ObjectReferenceSeq &vv : m_objects)
      {
        vv.clear();
      }

      // Return animation states from front again.
      m_current_animation_state = 0;

      m_projection = projection;
      m_camera = viewify(camera);
      m_screen_transform = m_projection * m_camera;
    }

    /// Get a fresh animation state.
    ///
    /// Data contained in the returned animation state is undefined, but not invalid.
    ///
    /// \return Animation state.
    AnimationState& newAnimationState()
    {
      unsigned ret = m_current_animation_state++;

      if(m_animation_states.size() <= ret)
      {
        m_animation_states.emplace_back();
      }

      return m_animation_states[ret];
    }

    /// Retrieve an earlier stored boolean value.
    ///
    /// \param name Name of bool value.
    /// \return Bool value, results undetermined if note stored.
    bool retrieveBool(char name) const
    {
      unsigned idx = static_cast<unsigned>(name) - STORAGE_BASE;
#if defined(USE_LD)
      if(idx >= STORAGE_SIZE)
      {
        std::ostringstream sstr;
        sstr << "trying to retrieve bool value " << idx;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return m_storage_bool[idx];
    }
    /// Store a boolean value.
    ///
    /// \param name Name (one letter) of the value.
    /// \param value Value to store.
    void storeBool(char name, bool value)
    {
      unsigned idx = static_cast<unsigned>(name) - STORAGE_BASE;
#if defined(USE_LD)
      if(idx >= STORAGE_SIZE)
      {
        std::ostringstream sstr;
        sstr << "trying to store boolean value " << idx << ": " << value;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_storage_bool[idx] = value;
    }

    /// Retrieve an earlier stored float value.
    ///
    /// \param name Name (one letter) of the value.
    /// \return Float value, results undetermined if not stored.
    float retrieveFloat(char name) const
    {
      unsigned idx = static_cast<unsigned>(name) - STORAGE_BASE;
#if defined(USE_LD)
      if(idx >= STORAGE_SIZE)
      {
        std::ostringstream sstr;
        sstr << "trying to retrieve float value " << idx;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return m_storage_float[idx];
    }
    /// Retrieve an earlier stored value wrapper.
    ///
    /// \param name1 First name.
    /// \param name2 Second name.
    /// \return Retrieved vector.
    vec2 retrieveFloat(char name1, char name2) const
    {
      return vec2(retrieveFloat(name1),
          retrieveFloat(name2));
    }
    /// Retrieve an earlier stored value wrapper.
    ///
    /// \param name1 First name.
    /// \param name2 Second name.
    /// \param name3 Third name.
    /// \return Retrieved vector.
    vec3 retrieveFloat(char name1, char name2, char name3) const
    {
      return vec3(retrieveFloat(name1),
          retrieveFloat(name2),
          retrieveFloat(name3));
    }
    /// Store a floating point value.
    ///
    /// \param name Name (one letter) of the value.
    /// \param value Value to store.
    void storeFloat(char name, float value)
    {
      unsigned idx = static_cast<unsigned>(name) - STORAGE_BASE;
#if defined(USE_LD)
      if(idx >= STORAGE_SIZE)
      {
        std::ostringstream sstr;
        sstr << "trying to store float value " << idx << ": " << value;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_storage_float[idx] = value;
    }
    /// Store a floating point value wrapper.
    ///
    /// \param name1 First name.
    /// \param name2 Second name.
    /// \param name3 Third name.
    /// \param value Vector to store.
    void storeFloat(char name1, char name2, char name3, const vec3 &value)
    {
      storeFloat(name1, value[0]);
      storeFloat(name2, value[1]);
      storeFloat(name3, value[2]);
    }

  private:
    /// Random offset for given transform.
    ///
    /// \param transform Tranform to interpolate from.
    /// \param center Object center.
    /// \param seed Seed for random numbers.
    /// \param interp Interpolation value.
    /// \return Transform interpolated towards random offset.
    static mat4 randomOffsetTransform(const mat4 &transform, const vec3& center, unsigned seed, float interp)
    {
      static const float HIGH_DIFF = 12.0f;
      static const float MID_DIFF = 8.0f;
      static const float LOW_DIFF = 4.0f;
      static const float MIN_SCALE = 0.01f;
      mat4 ret(MIN_SCALE, 0.0f, 0.0f, 0.0f,
          0.0f, MIN_SCALE, 0.0f, 0.0f,
          0.0f, 0.0f, MIN_SCALE, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f);

      bsd_srand(seed);

      float diff_x = frand(MID_DIFF, HIGH_DIFF);
      float diff_y = frand(LOW_DIFF, HIGH_DIFF);
      float diff_z = frand(LOW_DIFF, HIGH_DIFF);

      ret[12] = randomOffsetCoord(center[0], diff_x);
      ret[13] = randomOffsetCoord(center[1], diff_y);
      ret[14] = center[2] + (brand(-1.0f, 1.0f) * diff_z);

      return mix(ret, transform, interp);
    }

    /// Randomize a fade-in direction based on coordinate itself.
    ///
    /// \param coord Coordinate value.
    /// \param diff Difference value.
    /// \return Coordinate value coming from correct direction.
    static float randomOffsetCoord(float coord, float diff)
    {
      if(coord < 0.0f)
      {
        return coord - diff;
      }
      else if(coord > 0.0f)
      {
        return coord + diff;
      }
      return brand(-1.0f, 1.0f) * diff;
    }
};

#endif
