#ifndef INTRO_DIRECTION_HPP
#define INTRO_DIRECTION_HPP

/// Scene enumeration.
enum SceneEnum
{
  /// Main island and filler islands rendered with a stub maze.
  OPENING, 

  /// Main island cap and maze rendered.
  MAZE,

  /// Aqueduct and trip islands rendered.
  AQUEDUCT,

  /// Coliseum and its island rendered.
  COLISEUM_ENTRY_FIRST,

  /// Coliseum and its island rendered.
  COLISEUM_ENTRY_SECOND,

  /// Coliseum only rendered.
  COLISEUM,

  /// Quick credits blurb.
  CREDITS,

  /// Short greets blurb (with hellraiser maze in the background).
  GREETS,

  /// Massive maze rendered.
  HELLRAISER,

  /// No scene set.
  NONE
};

/// Direction.
///
/// Series of signed 16-bit integers in the following format.
/// 0: Scene enum casted into 16-bit int.
/// 1: Scene length.
/// 2: Offset into the spline at scene start.
/// 3: Spline mode: 0 = weighted, 1 = bezier.
/// - Arbitrary number of 4-component spline innput for camera position (x, y, z, timestamp).
/// - 4 consecutive zeros.
/// - Arbitrary number of 4-component spline innput for eye position (x, y, z, timestamp).
/// - 4 consecutive zeros.
///
/// After this, the format repeats unless 4 there is 4 consecutive zeros immediately following.
static const int16_t g_direction[] =
{
  static_cast<int16_t>(SceneEnum::MAZE), 200, 0, 1,
  -10, -25, 13, 300,
  -13, -25, 10, 300,
  0, 0, 0, 0,
  -99, -60, 40, 300,
  -72, -49, 87, 300,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::MAZE), 2020, 0, 1,
  -14, -25, 14, 200, // Start looking at the body.
  -14, -26, 13, 78, // Floor 1 start   88
  0, -26, 16, 61, // through doorway 71
  12, -25, 14, 72, // SE cell          82
  12, -26, 0, 60, // through doorway 
  10, -26, -10, 59, // NE cell         
  0, -26, -10, 30, // through doorway 
  -5, -26, -10, 43, // Floor 1 end NW Cell
  -8, -20, -13, 50, // Floor 2 start NW Cell
  0, -20, -16, 71, // through doorway 
  10, -20, -9, 53, // NE Cell         
  10, -20, 0, 82, // through doorway 
  9, -21, 14, 37, // SE Cell         
  3, -21, 16, 110, // through doorway,
  -14, -14, 12, 85, // Floor 3, SW Cell
  0, -15, 8, 65, // through doorway 
  11, -15, 7, 41, // SE Cell         
  11, -15, 0, 69, // through doorway 
  7, -10, -10, 60, // Floor 4, NE Cell
  9, -10, 0, 59, // through doorway 
  9, -10, 10, 53, // SE Cell         
  0, -10, 10, 83, // through doorway 
  -13, -5, 7, 45, // Floor 5, SW Cell
  -10, -5, 0, 76, // through doorway 
  -9, 0, -12, 71, // Floor 6, NW Cell
  -2, 0, -2, 30, // through doorway 
  3, 0, -2, 70, // NE Cell         
  4, 0, 10, 26, // SE Cell         
  0, 0, 12, 86, // through doorway 
  -12, 7, 7, 70, // Floor 7, SW Cell
  -13, 13, 9, 130, // Rise and look at ghost
  -14, 18, 11, 143, // Rise and look at ghost
  -16, 20, 15, 0, // Rise and look at ghost

  0, 0, 0, 0,

  -87, -69, 64, 200, // Start looking at the body.
  -27, -30, 112, 58, // Turn to floor 1 start 58
  99, -33, 16, 91, // doorway look direction 101
  -25, -24, -81, 122, // SE cell  142
  -87, -29, 16, 89, // NE cell
  5, -36, 98, 43, // Floor 1 end NW Cell
  89, -30, -16, 121, // Floor 2 start NW cell look at doorway
  47, -25, 84, 172, // NE cell
  -80, -28,	-40, 110, // last doorway Floor 2
  88, -25, -13, 150, // Floor 3, SW Cell
  -7, -16, -91, 41, // SE Cell
  -50, -2, -78, 69, // through doorway
  103, -17, -15, 60, // Floor 4, NE Cell
  -7, -23, 98, 59, // through doorway
  -71, 2, -49, 136, // SE Cell
  40, -8, -64, 45, // Floor 5, SW Cell // x was 57
  -30, 9, -97, 76, // through doorway
  -67, -19, 66, 71, // Floor 6, NW Cell
  91, -2, 39, 30, // through doorway
  11, -18, 98, 70, // NE Cell
  -91, 11, -17, 92, // SE Cell
  71, 4, -49, 40, // Floor 7, SW Cell
  56, -15, 35, 180, // Rise and look at ghost
  48, -26, -44, 143, // Rise and look at ghost
  15, -30, -67, 0, // Rise and look at ghost

#if 0
  -14, -24, 11, 50, // Floor 1 start
  0, -26, 16, 50, // through doorway
  12, -25, 14, 50, // SE cell
  12, -26, 0, 50, // through doorway
  10, -26, -10, 50, // NE cell
  0, -26, -10, 50, // through doorway
  -5, -26, -10, 50, // Floor 1 end NW cell
  -8, -20, -13, 50, // Floor 2 start NW cell
  0, -20, -16, 50, // through doorway
  10, -20, -9, 50, // NE Cell
  10, -20, 0, 50, // through doorway
  9, -21, 14, 50, // SE Cell
  3, -21, 16, 50, // through doorway, Floor 2 end
  -14, -14, 12, 50, // Floor 3, SW Cell
  0, -15, 8, 50, // through doorway
  11, -15, 7, 50, // SE Cell
  11, -15, 0, 50, // through doorway
  7, -10, -10, 50, // Floor 4, NE Cell
  9, -10, 0, 50, // through doorway
  9, -10, 10, 50, // SE Cell
  0, -10, 10, 50, // through doorway
  -13, -5, 7, 50, // Floor 5, SW Cell
  -10, -5, 0, 50, // through doorway
  -9, 0, -12, 50, // Floor 6, NW Cell
  -2, 0, -2, 50, // through doorway
  3, 0, -2, 50, // NE Cell
  4, 0, 10, 50, // SE Cell
  0, 0, 12, 50, // through doorway
  -12, 7, 7, 50, // Floor 7, SW Cell
  -2, 14, 7, 50, // Rise and look at ghost
  -5, 20, 11, 50, // Rise and look at ghost
  -16, 20, 15, 50, // Rise and look at ghost
#endif

  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 125, 0, 0,
  0, 23, 1, 200,
  4, 23, 10, 200,
  0, 0, 0, 0,
  -16, -11, -81, 200,
  -27, -4, -91, 200,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 50, 0, 0,
  4, 25, 14, 100,
  5, 25, 16, 100,
  0, 0, 0, 0,
  -20, -5, -71, 100,
  -19, -5, -69, 100,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 50, 0, 0,
  9, 27, 21, 100,
  10, 29, 24, 100,
  0, 0, 0, 0,
  -15, -2, -64, 100,
  -14, 2, -56, 100,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 350, 0, 0,
  -15, 14, -11, 350,
  -10, 15, -18, 350,
  0, 0, 0, 0,
  16, 10, -105, 350,
  -92, 8, 37, 350,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 140, 0, 0,
#if 0
  -19, 15, -10, 250,
  -20, 15, -9, 250,
#else
  -18, 16, -8, 140,
  -19, 16, -6, 140,
#endif
  0, 0, 0, 0,
#if 0
  -58, -12, -99, 250,
  -78, 1, -55, 250,
#else
  -40, -5, -101, 140,
  -36, -5, -98, 140,
#endif
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 50, 0, 0,
  -23, 17, -4, 60,
  -21, 18, 0, 60,
  0, 0, 0, 0,
  -34, -16, -97, 60,
  -34, -16, -97, 60,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 50, 0, 0,
  -19, 19, 4, 50,
  -17, 21, 7, 50,
  0, 0, 0, 0,
  -38, -14, -84, 50,
  -38, -14, -84, 50,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 50, 0, 0,
  0, 25, 25, 50,
  6, 27, 33, 50,
  0, 0, 0, 0,
  -40, -2, -63, 50,
  -40, -2, -63, 50,
  0, 0, 0, 0,

  static_cast<int16_t>(SceneEnum::OPENING), 160, 0, 0,
  29, 35, 57, 145,
  38, 36, 69, 145,
  0, 0, 0, 0,
  6, 9, -32, 145,
  6, 9, -32, 145,
  0, 0, 0, 0,

  // Aqueduct approach.
  static_cast<int16_t>(SceneEnum::AQUEDUCT), 230, 0, 0,
  142, 36, 892, 230,
  -30, 27, 722, 230,
  0, 0, 0, 0,
  0, 19, 819, 230,
  0, 28, 768, 230,
  0, 0, 0, 0,

  // Behind the aqueduct, before construction.
  static_cast<int16_t>(SceneEnum::AQUEDUCT), 310, 0, 0,
  -70, 1, 809, 200,
  -67, 1, 820, 200,
  0, 0, 0, 0,
  27, 1, 777, 200,
  27, 1, 740, 350,
  0, 0, 0, 0,

  // Follow aqueduct construction.
  static_cast<int16_t>(SceneEnum::AQUEDUCT), 310, 280, 0,
  28, 42, 870, 195,
  27, 37, 750, 195,
  26, 33, 630, 195,
  26, 33, 510, 195,
  26, 33, 390, 195,
  0, 0, 0, 0,
  0, 27, 885, 195,
  -2, 28, 765, 195,
  -4, 29, 645, 195,
  -4, 29, 525, 195,
  -4, 29, 405, 195,
  0, 0, 0, 0,

  // Follow aqueduct construction from behind.
  static_cast<int16_t>(SceneEnum::AQUEDUCT), 340, 145, 0,
  -26, 45, 635, 165,
  -26, 45, 535, 165,
  -26, 42, 435, 165,
  -26, 38, 335, 165,
  -26, 34, 235, 165,
  -26, 34, 135, 165,
  0, 0, 0, 0,
  4, 29, 645, 165,
  4, 29, 545, 165,
  4, 29, 450, 165,
  4, 28, 350, 165,
  4, 27, 250, 165,
  4, 27, 150, 165,
  0, 0, 0, 0,

  // Greets blurb.
  static_cast<int16_t>(SceneEnum::GREETS), 550, 0, 0,
  -25, 29, -34, 600,
  -24, 29, -33, 600,
  0, 0, 0, 0,
  36, 25, 44, 600,
  0, 0, 0, 0,

  // Hellraiser thing.
  static_cast<int16_t>(SceneEnum::HELLRAISER), 320, 0, 0,
  -23, 29, -32, 360,
  -15, 28, -38, 360,
  0, 0, 0, 0,
  36, 25, 44, 360,
  0, 0, 0, 0,

  // Hellraiser thing.
  static_cast<int16_t>(SceneEnum::HELLRAISER), 40, 0, 0,
  -17, 29, -30, 100,
  -14, 29, -29, 100,
  0, 0, 0, 0,
  24, 20, 69, 100,
  0, 0, 0, 0,

  // Hellraiser thing.
  static_cast<int16_t>(SceneEnum::HELLRAISER), 240, 0, 0,
  -5, 26, -18, 150,
  -1, 26, -17, 150,
  0, 0, 0, 0,
  16, 46, 77, 240,
  0, 0, 0, 0,

  // Coliseum entry deconstruct.
  static_cast<int16_t>(SceneEnum::COLISEUM_ENTRY_FIRST), 360, 0, 0,
  -30, 53, 115, 600,
  -10, 49, 70, 600,
  0, 0, 0, 0,
  -14, 50, 121, 600,
  -13, 45, 110, 600,
  -10, 45, 109, 600,
  0, 0, 0, 0,

  // Coliseum overlook 1.
  static_cast<int16_t>(SceneEnum::COLISEUM_ENTRY_SECOND), 150, 0, 0,
  0, 46, 126, 150,
  -12, 46, 48, 200,
  0, 0, 0, 0,
  8, 46, 27, 150,
  16, 47, -48, 200,
  0, 0, 0, 0,

  // Coliseum overlook 2.
  static_cast<int16_t>(SceneEnum::COLISEUM_ENTRY_SECOND), 150, 0, 0,
#if 0
  -64, 83, 24, 200,
  -58, 83, 13, 200,
  0, 0, 0, 0,
  16, 23, 18, 200,
  22, 25, 6, 200,
  0, 0, 0, 0,
#else
  -73, 78, 16, 200,
  -62, 78, 4, 200,
  0, 0, 0, 0,
  6, 15, 31, 200,
  17, 15, 18, 200,
  0, 0, 0, 0,
#endif

  // Coliseum entry fade.
  static_cast<int16_t>(SceneEnum::COLISEUM_ENTRY_SECOND), 150, 0, 0,
  -25, 68, 13, 150,
  -27, 62, 10, 150,
  0, 0, 0, 0,
  0, 61, 0, 200,
  0, 0, 0, 0,

  // Coliseum opening moelli.
  static_cast<int16_t>(SceneEnum::COLISEUM), 480, 0, 0,
  -6, 55, -26, 500,
  -2, 55, -27, 500,
  0, 0, 0, 0,
  0, 60, 0, 800,
  0, 0, 0, 0,

  // Looking at rotating moelli under the arc.
  static_cast<int16_t>(SceneEnum::COLISEUM), 240, 0, 0,
  9, 56, -40, 300,
  11, 56, -40, 300,
  0, 0, 0, 0,
  -6, 61, 57, 300,
  -23, 62, 53, 300,
  0, 0, 0, 0,

  // Half-close moelli and rotate again.
  static_cast<int16_t>(SceneEnum::COLISEUM), 355, 0, 50,
  25, 74, -34, 140,
  31, 76, -29, 140,
  39, 78, -20, 140,
  43, 78, -14, 140,
  0, 0, 0, 0,
  -21, 19, 36, 140,
  -33, 19, 24, 140,
  -39, 19, 16, 140,
  -44, 19, 12, 140,
  0, 0, 0, 0,

  // Moelli looking for things.
  static_cast<int16_t>(SceneEnum::COLISEUM), 280, 50, 0,
  -8, 74, 40, 160,
  -16, 74, 38, 160,
  -28, 74, 33, 160,
  0, 0, 0, 0,
  14, 18, -36, 160,
  20, 11, -30, 160,
  0, 40, 0, 160,
  0, 0, 0, 0,

  // Moelli destroying haamu veryfront.
  static_cast<int16_t>(SceneEnum::COLISEUM), 95, 20, 0,
  -21, 56, -6, 100,
  -22, 56, -5, 100,
  0, 0, 0, 0,
  -110, 28, -53, 100,
  -102, 33, -55, 100,
  0, 0, 0, 0,

  // Moelli destroying haamu front.
  static_cast<int16_t>(SceneEnum::COLISEUM), 95, 20, 0,
  -22, 56, -4, 100,
  -23, 56, -4, 100,
  0, 0, 0, 0,
  -62, 25, -90, 150,
  -58, 27, -92, 150,
  0, 0, 0, 0,

  // Moelli destroying haamu back.
  static_cast<int16_t>(SceneEnum::COLISEUM), 95, 20, 0,
  -26, 55, -5, 90,
  -27, 55, -5, 90,
  0, 0, 0, 0,
  60, 58, -55, 120,
  61, 56, -54, 120,
  0, 0, 0, 0,

  // Mölli rotating to 'neutral' position and opening and closing itself.
  static_cast<int16_t>(SceneEnum::COLISEUM), 460, 20, 0,
  -34, 66, -9, 400,
  -27, 65, -25, 400,
  -21, 65, -35, 400,
  0, 0, 0, 0,
  54, 29, 11, 400,
  37, 33, 43, 400,
  27, 34, 53, 400,
  0, 0, 0, 0,

  // Mölli closing for the last time.
  static_cast<int16_t>(SceneEnum::COLISEUM), 260, 20, 0,
  -22, 55, -10, 300,
  -21, 55, -13, 300,
  0, 0, 0, 0,
  62, 70, 39, 300,
  59, 59, 43, 300,
  0, 0, 0, 0,

  // Credits blurb.
  static_cast<int16_t>(SceneEnum::CREDITS), 450, 100, 0,
  -106, 101, 13, 300, //-79, 101, 102, 300,
  -94, 95, 36, 300, //-93, 101, 87, 300,
  0, 0, 0, 0,
  -18, 53, 20, 300, //-3, 65, 48, 300,
  -8, 49, 31, 300, //-15, 65, 36, 300,
   0, 0, 0, 0,

  // Padding, no crash plox?
  static_cast<int16_t>(SceneEnum::COLISEUM), 32767, 0, 0,
  0, 0, 0, 32767,
  0, 0, 0, 0,
  0, 0, 0, 32767,
  0, 0, 0, 0,

  // Terminator.
  0, 0, 0, 0
};

/// Scene class.
///
/// Contains camera path and id.
class Scene
{
  private:
    /// Id of scene.
    SceneEnum m_id;

    /// Scene length.
    unsigned m_length;

    /// Offset into this scene.
    unsigned m_offset;

    /// Camera path for scene.
    Spline m_camera;

    /// Eye point path for scene.
    Spline m_eye;

  public:
    /// Constructor.
    ///
    /// \param data Input data.
    Scene(const int16_t* data) :
      m_id(static_cast<SceneEnum>(data[0])),
      m_length(static_cast<unsigned>(data[1])),
      m_offset(static_cast<unsigned>(data[2])),
      m_camera(static_cast<SplineMode>(data[3])),
      m_eye(static_cast<SplineMode>(data[3]))
    {
#if defined(USE_LD)
      if(0 > data[1])
      {
        std::ostringstream sstr;
        sstr << "invalid scene settings: " << data[0] << ", " << data[1] << ", " << data[2];
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      m_eye.readData(m_camera.readData(data + 4));
    }

  public:
    /// Accessor.
    ///
    /// \return Scene ID.
    SceneEnum getId() const
    {
      return m_id;
    }

    /// Accessor.
    ///
    /// \return Scene length.
    unsigned getLength() const
    {
      return m_length;
    }

    /// Resolve a camera position.
    ///
    /// \param stamp Position in frames.
    vec3 resolveCamera(unsigned stamp) const
    {
      return m_camera.resolvePosition(stamp + m_offset);
    }

    /// Resolve an eye position.
    ///
    /// \param stamp Position in frame.
    vec3 resolveEye(unsigned stamp) const
    {
      return m_eye.resolvePosition(stamp + m_offset);
    }
};

/// Intro direction.
///
/// Gives camera position and tells which scene it is.
class Direction
{
  private:
    /// Sequence of scenes.
    seq<Scene*> m_scenes;

  public:
    /// Constructor.
    Direction()
    {
      initialize(g_direction);
    }

    /// Destructor.
    ~Direction()
    {
      for(Scene *vv : m_scenes)
      {
        delete vv;
      }
    }
   
  private:
    /// Initializes the complete intro direction.
    ///
    /// \param data Data blob to initialize from.
    void initialize(const int16_t *data)
    {
      const int16_t* iter = data;

      for(;;)
      {
        m_scenes.push_back(new Scene(iter));

        iter = next_segment(next_segment(iter + 4));

        if(Spline::is_segment_end(iter))
        {
          break;
        }
      }
    }

  public:
    /// Resolve the scene.
    ///
    /// Writes camera and eye positions.
    ///
    /// \param stamp Timestamp (frames).
    /// \param out_id Scene id.
    /// \param out_cpos Camera position.
    /// \param out_epos Eye position.
    /// \param out_totaltime Total time in this scene.
    void resolveScene(unsigned stamp, SceneEnum &out_id, vec3 &out_cpos, vec3 &out_epos,
        unsigned &out_totaltime) const
    {
      for(unsigned ii = 0; (m_scenes.size() > ii); ++ii)
      {
        Scene* vv = m_scenes[ii];

        if(vv->getLength() > stamp)
        {
          out_id = vv->getId();
          out_cpos = vv->resolveCamera(stamp);
          out_epos = vv->resolveEye(stamp);

          // Calculate downwards to get total time spent in this scene.
          unsigned total_time = 0;
          for(;;)
          {
            if(0 >= ii)
            {
              break;
            }
            --ii;

            if(m_scenes[ii]->getId() == out_id)
            {
              total_time += m_scenes[ii]->getLength();
            }
          }
          out_totaltime = total_time + stamp;
          //std::cout << out_cpos << " ; " << out_epos << " ; " << out_totaltime << std::endl;
          return;
        }
        stamp -= vv->getLength();
      }

#if defined(USE_LD)
      BOOST_THROW_EXCEPTION(std::runtime_error("timestamp outside boundary"));
#else
      out_id = NONE;
      out_cpos = vec3(0.0f, 0.0f, 0.0f);
      out_epos = vec3(0.0f, 0.0f, 0.0f);
      out_totaltime = 0;
#endif
    }

  private:
    /// Get pointer past next termination point.
    ///
    /// \param data Data pointer.
    /// \return Pointer past termination point.
    const int16_t* next_segment(const int16_t *data)
    {
      const int16_t *iter = data;
      for(;;)
      {
        const int16_t *next = iter + 4;
        if(Spline::is_segment_end(iter))
        {
          return next;
        }
        iter = next;
      }
    }
};

#endif
