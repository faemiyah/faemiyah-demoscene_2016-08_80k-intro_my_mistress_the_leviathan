/// \file
/// Very small intro stub.

//######################################
// Include #############################
//######################################

#include "dnload.h"

#if defined(WIN32)
/// Use MT random since windows rand/srand seem to be scheisse.
#define USE_MT_RAND
/// Windows math.h has M_PI.
#define HAVE_M_PI
#endif

#if defined(USE_LD)
#include "image_png.hpp"
#include <iomanip>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_array.hpp>
#include <boost/tuple/tuple.hpp>
namespace po = boost::program_options;
#endif

//######################################
// Define ##############################
//######################################

/// Screen mode.
///
/// Negative values windowed.
/// Positive values fullscreen.
#if defined(DNLOAD_GLESV2)
#define DISPLAY_MODE 480
#else
#define DISPLAY_MODE -720
#endif

/// \cond
#if (0 > (DISPLAY_MODE))
#define SCREEN_F 0
#define SCREEN_H (-(DISPLAY_MODE))
#elif (0 < (DISPLAY_MODE))
#define SCREEN_F 1
#define SCREEN_H (DISPLAY_MODE)
#else
#error "invalid display mode (pre)"
#endif
#if ((800 == SCREEN_H) || (1200 == SCREEN_H))
#define SCREEN_W ((SCREEN_H / 10) * 16)
#else
#define SCREEN_W (((SCREEN_H * 16) / 9) - (((SCREEN_H * 16) / 9) % 4))
#endif
/// \endcond

/// Size of one sample in bytes.
#define AUDIO_SAMPLE_SIZE 4

/// \cond
#if (4 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_F32SYS
typedef float sample_t;
#elif (2 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_S16SYS
typedef int16_t sample_t;
#elif (1 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_U8
typedef uint8_t sample_t;
#else
#error "invalid audio sample size"
#endif
#define AUDIO_POSITION_SHIFT (9 - (4 / sizeof(sample_t)))
/// \endcond

/// Audio channels.
#define AUDIO_CHANNELS 2

/// Audio samplerate.
#define AUDIO_SAMPLERATE 44100

/// Audio byterate.
#define AUDIO_BYTERATE (AUDIO_CHANNELS * AUDIO_SAMPLERATE * sizeof(sample_t))

/// Milliseconds per frame.
#define FRAME_MILLISECONDS 20

/// Intro length (in seconds).
#define INTRO_LENGTH_SECONDS 184

/// Intro length (in frames).
#define INTRO_LENGTH_FRAMES (INTRO_LENGTH_SECONDS * 1000 / FRAME_MILLISECONDS)

/// Clear all buffers when clearing - advantageous on some architectures.
#define RENDER_CLEAR_ALL_BUFFERS

/// Enable depth textures.
#define RENDER_ENABLE_DEPTH_TEXTURE

/// Enable font kerning.
#undef RENDER_ENABLE_KERNING

/// How many bytes RGB24 images should be converted into. 3 for no convert.
#define RENDER_RGB24_BYTES 3

// Logitech k830 ALTRGR.
#define SDLK_RALT_EXTRA 1073741824

//######################################
// Global functions ####################
//######################################

/// Global SDL window storage.
SDL_Window *g_sdl_window;

#if defined(DNLOAD_GLESV2) && defined(DNLOAD_VIDEOCORE)
#include "dnload_egl.h"
#include "dnload_videocore.h"
#endif

/// Swap buffers.
///
/// Uses global data.
static void swap_buffers()
{
#if defined(DNLOAD_GLESV2) && defined(DNLOAD_VIDEOCORE)
  dnload_eglSwapBuffers(g_egl_display, g_egl_surface);
#else
  dnload_SDL_GL_SwapWindow(g_sdl_window);
#endif
}

/// Tear down initialized systems.
///
/// Uses global data.
static void teardown()
{
#if defined(DNLOAD_GLESV2) && defined(DNLOAD_VIDEOCORE)
  egl_quit(g_egl_display);
  dnload_bcm_host_deinit();
#endif
  dnload_SDL_Quit();
}

#if defined(USE_LD)

/// Get SDL attribute.
///
/// \param op Attribute name.
static int get_sdl_gl_attr(SDL_GLattr op)
{
  int ret;
  int success = SDL_GL_GetAttribute(op, &ret);
  if(success)
  {
    std::ostringstream sstr;
    sstr << "SDL_GL_GetAttribute(" << op << ") failed";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  return ret;
}

/// Get SDL framebuffer attributes.
static std::string get_sdl_framebuffer_str()
{
  std::ostringstream sstr;

  sstr << "R" << get_sdl_gl_attr(SDL_GL_RED_SIZE) << "G" << get_sdl_gl_attr(SDL_GL_GREEN_SIZE) << "B" <<
    get_sdl_gl_attr(SDL_GL_GREEN_SIZE) << "D" << get_sdl_gl_attr(SDL_GL_DEPTH_SIZE) << "S" <<
    get_sdl_gl_attr(SDL_GL_STENCIL_SIZE);

  return sstr.str();
}

/// Get OpenGL extensions.
///
/// \return Extension string.
static std::string get_opengl_info()
{
  std::ostringstream sstr;

  sstr << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << " || " <<
    reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)) << " || ";
#if !defined(DNLOAD_VIDEOCORE)
  sstr << get_sdl_framebuffer_str() << " || ";
#endif
  sstr << reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

  return sstr.str();
}

/// Developer mode toggle.
static bool g_developer = false;

/// Tell if developer mode is on.
///
/// \return True if yes, false if no.
static bool is_developer()
{
  return g_developer;
}
/// Turn developer mode on or off.
///
/// \param op True to enable, false to disable.
static void set_developer(bool op)
{
  g_developer = op;
}

/// Extra verbosity toggle.
static bool g_verbose = false;

/// Tell if extra verbosity is on.
///
/// \return True if yes, false if no.
static bool is_verbose()
{
  return g_verbose;
}
/// Turn extra verbosity on or off.
///
/// \param op True to enable, false ot disable.
static void set_verbose(bool op)
{
  g_verbose = op;
}

#else

/// Globally disable developer mode.
///
/// \return Always false.
static bool is_developer()
{
  return false;
}

#endif

//######################################
// Random ##############################
//######################################

#if defined(USE_LD)
#include "bsd_rand.h"
#else
#include "bsd_rand.c"
#endif

/// Boolean random value.
///
/// \returns True or false.
bool brand()
{
  return static_cast<bool>(bsd_rand() & 0x1);
}

/// Boolean random value that returns either of two floating point values.
///
/// \param aa First floating point value.
/// \param bb Second floating point value.
/// \return Either aa or bb.
float brand(float aa, float bb)
{
  return brand() ? aa : bb;
}

/// Random float value.
///
/// \param op Given maximum value.
/// \return Random value between 0 and given value.
static float frand(float op)
{
  return static_cast<float>(bsd_rand() & 0xFFFF) * ((1.0f / 65535.0f) * op);
}

/// Random float value.
///
/// \param minimum Given minimum value.
/// \param maximum Given maximum value.
/// \return Random value between minimum and maximum value.
static float frand(float minimum, float maximum)
{
  return frand(maximum - minimum) + minimum;
}

/// Random unsigned value.
///
/// \param op Random cap.
/// \return Random value from range [0, op[
static unsigned urand(unsigned op)
{
  return static_cast<unsigned>(bsd_rand() % static_cast<int>(op));
}

#if 0 // Stateless random disabled for now, not needed.
#if defined(USE_MT_RAND)

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

/// Mersenne random number generator.
static boost::mt19937 g_mt_rand;

#endif

/// \brief Random float value from non-deterministic source.
///
/// \param op Given maximum value.
/// \return Random value between 0 and given value.
static float frandom(float op)
{
#if defined(USE_MT_RAND)
  boost::uniform_real<> distribution(0.0, static_cast<double>(op));
  boost::variate_generator<boost::mt19937&, boost::uniform_real<> > randomize(g_mt_rand, distribution);
  return static_cast<float>(randomize());
#else
  return static_cast<float>(dnload_random() & 0xFFFF) * ((1.0f / 65535.0f) * op);
#endif
}

/// \brief Random float value from non-deterministic source.
///
/// \param minimum Given minimum value.
/// \param maximum Given maximum value.
/// \return Random value between minimum and maximum value.
static float frandom(float minimum, float maximum)
{
#if defined(USE_MT_RAND)
  boost::uniform_real<> distribution(static_cast<double>(minimum), static_cast<double>(maximum));
  boost::variate_generator<boost::mt19937&, boost::uniform_real<> > randomize(g_mt_rand, distribution);
  return static_cast<float>(randomize());
#else
  return frandom(maximum - minimum) + minimum;
#endif
}
#endif

//######################################
// Verbatim source #####################
//######################################

#include "verbatim_font.hpp"
#include "verbatim_image_gray.hpp"
#include "verbatim_image_la.hpp"
#include "verbatim_image_rgb.hpp"
#include "verbatim_image_rgba.hpp"
#include "verbatim_logical_mesh.hpp"
#include "verbatim_spline.hpp"
#include "verbatim_state_queue.hpp"

// Additional program logic.
#include "intro_aqueduct.hpp"
#include "intro_direction.hpp"
#include "intro_floating_island.hpp"
#include "intro_haamu.hpp"
#include "intro_moelli.hpp"
#include "intro_skybox.hpp"

//######################################
// Global data #########################
//######################################

/// Field of view.
#define CAMERA_FOV 94.0f

/// Field of view (radians).
#define CAMERA_FOV_RADIANS (CAMERA_FOV * static_cast<float>(M_PI / 180.0))

/// Camera near plane.
#define CAMERA_NEAR 0.5f

/// Camera far plane.
#define CAMERA_FAR 1448.0f

/// \cond
#define STARTING_POS_X 0.0f
#define STARTING_POS_Y 0.0f
#define STARTING_POS_Z 35.0f
#define STARTING_ROT_PITCH 0.0f
#define STARTING_ROT_YAW 0.0f
#define STARTING_ROT_ROLL 0.0f
/// \endcond

/// Font options.
const char *g_font_options[] = 
{
  "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", // Primary font choise (Hardkernel Ubuntu).
#if defined(USE_LD)
  "/usr/local/share/fonts/dejavu/DejaVuSans-Bold.ttf", // Secondaty font choise (FreeBSD).
  "DejaVuSans-Bold.ttf", // Tertiary font choise (running from rel/).
  "rel/DejaVuSans-Bold.ttf", /// Last choise (running base dir).
#endif
  NULL
};

#if defined(USE_LD)

/// Developer mode position.
static vec3 g_pos(0.0f, 0.0f, 0.0f);

/// Developer mode yaw rotation.
static float g_rot_pitch = 0.0f;

/// Developer mode pitch rotation.
static float g_rot_yaw = 0.0f;

/// Developer mode roll rotation.
static float g_rot_roll = 0.0f;

/// Advance multiplier.
int g_advance_multiplier = 1;

/// Advance flag.
int g_advance = 0;

/// Frame jump, used to jump in the intro.
int g_frame_jump = 1;

/// Debug scene.
SceneEnum g_force_scene = NONE;

/// Lock camera to direction.
bool g_camera_lock = true;

/// Print position next frame.
bool g_print_position = false;

/// Shadow mode.
bool g_shadow_mode_stencil = false;

/// Shadow mode.
bool g_shadow_debug = false;

/// Usage string.
static const char *g_usage = ""
"Usage: my_mistress_the_leviathan <options>\n"
"Release version does not pertain to any size limitations.\n";

/// Intro name.
static const char *g_intro_name = "my_mistress_the_leviathan";

#endif

/// Struct for grouping text locations.
struct TextLocation
{
  /// Text to display.
  const char *text;

  /// Parameters in 1/10, order: starting time, x position, y position.
  int8_t params[3];
};

/// Light path in initial maze.
static const int16_t g_ghost_light_path[] =
{
  //-16, -27, 16, 400, // Hold ghost still for the beginning, not needed because it does not exist yet.
  -16, -27, 15, 200, // Ghost flickers into existence
  -13, -26, 15, 52, // Start of path
  -1, -26, 16, 10,
  1, -26, 14, 35,
  1, -26, 4, 13,
  4, -26, 2, 7,
  6, -26, 2, 13,
  9, -26, 0, 8,
  10, -26, -2, 14,
  14, -26, -2, 8,
  16, -26, -3, 35,
  16, -26, -13, 10,
  14, -26, -15, 35,
  4, -26, -15, 8,
  2, -26, -14, 15,
  2, -25, -10, 16,
  0, -25, -6, 7,
  -2, -25, -6, 16,
  -4, -24, -2, 34,
  -13, -20, -1, 11,
  -15, -21, -3, 35,
  -15, -20, -13, 10,
  -13, -20, -15, 86,
  12, -20, -15, 13,
  15, -21, -13, 28,
  16, -20, -5, 13,
  14, -20, -2, 11,
  11, -20, -2, 15,
  10, -21, 2, 21,
  4, -21, 1, 15,
  1, -21, 4, 31,
  2, -21, 13, 13,
  0, -21, 16, 45,
  -13, -20, 15, 9,
  -15, -19, 14, 34,
  -15, -15, 5, 13,
  -13, -14, 2, 28,
  -5, -14, 2, 13,
  -2, -15, 4, 15,
  -1, -15, 8, 13,
  2, -15, 10, 18,
  3, -15, 15, 31,
  12, -14, 15, 13,
  15, -15, 13, 28,
  15, -15, 5, 15,
  12, -15, 2, 10,
  10, -15, 0, 7,
  10, -15, -2, 21,
  4, -15, -2, 10,
  2, -15, -4, 28,
  2, -15, -12, 15,
  4, -13, -15, 31,
  12, -9, -15, 15,
  15, -9, -12, 28,
  15, -10, -4, 13,
  12, -10, -2, 11,
  9, -10, -1, 11,
  8, -10, 2, 14,
  4, -10, 2, 10,
  2, -10, 4, 11,
  1, -10, 7, 11,
  -2, -10, 7, 21,
  -2, -9, 13, 10,
  -4, -9, 15, 28,
  -12, -9, 15, 13,
  -15, -8, 13, 34,
  -14, -4, 4, 10,
  -12, -4, 2, 28,
  -4, -5, 2, 11,
  -4, -5, -1, 9,
  -2, -4, -2, 35,
  -2, -3, -12, 16,
  -4, -2, -16, 30,
  -12, 1, -15, 15,
  -15, 2, -12, 25,
  -15, 1, -5, 11,
  -14, 1, -2, 52,
  1, 1, -1, 8,
  3, 1, -2, 14,
  3, 1, 2, 5,
  2, 1, 3, 24,
  2, 1, 10, 5,
  1, 1, 11, 11,
  -2, 1, 11, 16,
  -4, 1, 15, 28,
  -12, 1, 15, 12,
  -15, 2, 14, 39,
  -15, 7, 4, 13,
  -12, 7, 2, 28,
  -4, 6, 2, 10,
  -2, 6, 4, 11,
  -1, 6, 7, 11,
  2, 6, 7, 18,
  2, 6, 12, 11,
  3, 6, 15, 31,
  12, 6, 15, 13,
  15, 6, 13, 45,
  16, 6, 0, 45,
  15, 11, -12, 13,
  13, 11, -15, 48,
  -1, 12, -15, 46,
  -14, 15, -16, 36,
  -16, 16, -26, 100,
  0, 0, 0, 0
};

//######################################
// Music ###############################
//######################################

/// Audio buffer size (bytes), with a bit of slack to be sure.
#define AUDIO_BUFFER_SIZE ((INTRO_LENGTH_SECONDS + 8) * AUDIO_BYTERATE)

/// Audio buffer for output.
static sample_t g_audio_buffer[AUDIO_BUFFER_SIZE / sizeof(sample_t)];

/// Current audio position.
static uint8_t *g_audio_position = reinterpret_cast<uint8_t*>(g_audio_buffer);

/// \brief Update audio stream.
///
/// \param userdata Not used.
/// \param stream Target stream.
/// \param len Number of bytes to write.
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
  const uint8_t *audio_stream_in = g_audio_position;

#if defined(USE_LD)
  const uint8_t *audio_stream_end = reinterpret_cast<const uint8_t*>(g_audio_buffer) + AUDIO_BUFFER_SIZE;
  if(audio_stream_in + len > audio_stream_end)
  {
    const int len_audio = static_cast<int>(audio_stream_end - audio_stream_in);

    for(int ii = 0; (ii < len_audio); ++ii)
    {
      stream[ii] = audio_stream_in[ii];
    }
    for(int ii = len_audio; (ii < len); ++ii)
    {
      stream[ii] = 0;
    }

    g_audio_position += len_audio;
  }
  else
#endif
  {
    for(int ii = 0; (ii < len); ++ii)
    {
      stream[ii] = audio_stream_in[ii];
    }

    g_audio_position += len;
  }

  // Unused.
  (void)userdata;
}

/// SDL audio specification struct.
static SDL_AudioSpec audio_spec =
{
  AUDIO_SAMPLERATE,
  AUDIO_SAMPLE_TYPE_SDL,
  AUDIO_CHANNELS,
  0,
#if defined(USE_LD)
  4096,
#else
  1024, // ~43Hz, relatively safe
#endif
  0,
  0,
  audio_callback,
  NULL
};

#if 0
/// Stub for audio generation.
///
/// \param data Audio buffer output.
/// \param length Number of bytes to generate.
static void generate_audio(void* data, const size_t length)
{
  sample_t *iter = reinterpret_cast<sample_t*>(data);

  for(size_t ii = 0; (length > ii); ii += sizeof(sample_t))
  {
    *iter++ = static_cast<sample_t>(0);
  }
}
#else
#include "verbatim_synth.hpp"
#endif

//######################################
// Shaders #############################
//######################################

/// Haamu shape vertex shader.
static const char *g_shader_vertex_geometry_haamu_shape = ""
"attribute vec3 P;"
"attribute vec4 T;"
"attribute vec3 C;"
"uniform highp mat4 M;"
"uniform mediump vec3 V;"
"uniform lowp vec2 K;"
"varying lowp vec4 c;"
#if !defined(HAAMU_SHAPE_SIMPLE)
"attribute vec4 Q;"
"attribute vec4 R;"
"uniform mediump mat4 E[30];" // 30 bones maximum
#endif
"precision mediump float;"
"void main()"
"{"
#if defined(HAAMU_SHAPE_SIMPLE)
"vec4 p=vec4(P-V*T.x,1.);"
#else
"ivec4 q=ivec4(R);"
"mat4 e=E[q.x]*Q.x+E[q.y]*Q.y+E[q.z]*Q.z;"
"vec4 p=e*vec4(P,1.);"
"p.xyz-=V*T.x;"
#endif
"float a=K.x*.7;"
"c=vec4(C*K.y*a,a)*(1.-T.x);"
"gl_Position=M*p;"
"}";

/// Haamu shape fragment shader.
static const char *g_shader_fragment_geometry_haamu_shape = ""
"varying lowp vec4 c;"
"precision lowp float;"
"void main()"
"{"
"gl_FragColor=vec4(c);"
"}";

/// Haamu sprite vertex shader.
static const char *g_shader_vertex_geometry_haamu_sprite = ""
"attribute vec3 P;"
"attribute vec4 T;"
"attribute vec3 C;"
"uniform highp mat4 M;"
"varying lowp vec4 c;"
"varying mediump vec2 t;"
"precision mediump float;"
"void main()"
"{"
"vec4 p=vec4(P,1.);"
"c=vec4(C,1.);"
"t=T.xy;"
"gl_Position=M*p;"
"}";

/// Haamu sprite fragment shader.
static const char *g_shader_fragment_geometry_haamu_sprite = ""
"uniform sampler2D I;"
"varying lowp vec4 c;"
"varying mediump vec2 t;"
"precision lowp float;"
"void main()"
"{"
"vec4 l=texture2D(I,t);"
"gl_FragColor=vec4(c*l*.22);"
"}";

/// Filled geometry vertex shader.
static const char *g_shader_vertex_geometry_plain = ""
"attribute vec3 P;"
"attribute vec4 T;"
"attribute vec3 N;"
"attribute vec3 C;"
"uniform highp mat4 M;"
"uniform highp mat4 W;"
"uniform highp mat3 B;"
"uniform mediump float V;"
"varying lowp vec3 f;"
"varying lowp vec3 n;"
"varying mediump vec3 r;"
"varying mediump vec2 t;"
"precision mediump float;"
"void main()"
"{"
"vec4 p=vec4(P,1.);"
"f=C;"
"n=B*N;"
"r=(W*p).xyz;"
"t=T.xy+T.zw*V;"
"gl_Position=M*p;"
"}";

/// Filled geometry vertex shader - animated version.
static const char *g_shader_vertex_geometry_plain_animated = ""
"attribute vec3 P;"
"attribute vec4 T;"
"attribute vec3 N;"
"attribute vec3 C;"
"attribute vec4 Q;"
"attribute vec4 R;"
"uniform highp mat4 M;"
"uniform highp mat4 W;"
"uniform highp mat3 B;"
"uniform mediump float V;"
"uniform mediump mat4 E[30];" // 30 bones maximum
"varying lowp vec3 f;"
"varying lowp vec3 n;"
"varying mediump vec3 r;"
"varying mediump vec2 t;"
"precision mediump float;"
#if !defined(DNLOAD_GLESV2)
// From Khronos registry, needed for non-GLES2 GPUs that may only have GLSL 1.10 support:
// https://www.khronos.org/registry/webgl/sdk/tests/conformance/glsl/matrices/glsl-mat4-to-mat3.html
"mat3 mat3_from_mat4(mat4 op)"
"{"
"return mat3(op[0][0],op[0][1],op[0][2],"
"op[1][0],op[1][1],op[1][2],"
"op[2][0],op[2][1],op[2][2]);"
"}"
#endif
"void main()"
"{"
"ivec4 q=ivec4(R);"
"mat4 e=E[q.x]*Q.x+E[q.y]*Q.y+E[q.z]*Q.z;"
"vec4 p=e*vec4(P,1.);"
#if !defined(DNLOAD_GLESV2)
"vec3 s=mat3_from_mat4(e)*N;"
#else
"vec3 s=mat3(e)*N;"
#endif
"f=C;"
"n=B*s;"
"r=(W*p).xyz;"
"t=T.xy+T.zw*V;"
"gl_Position=M*p;"
"}";

/// Filled geometry fragment shader.
static const char *g_shader_fragment_geometry_plain = ""
"uniform sampler2D I;"
"uniform mediump vec2 U;"
"uniform mediump vec3 L;"
"uniform lowp vec3 K;"
"varying lowp vec3 f;"
"varying lowp vec3 n;"
"varying mediump vec3 r;"
"varying mediump vec2 t;"
"precision lowp float;"
"void main()"
"{"
"vec3 u=texture2D(I,gl_FragCoord.xy*U+t).xyz;"
"mediump vec3 d=L-r;"
"mediump float l=sqrt(dot(d,d));"
"float i=dot(d*(1./l),n)*max(K.x-l*K.z,.0)+K.y;"
//"float i = dot(d*(1. / l), n)*K.x + K.y;"
"gl_FragColor=vec4(u*f*i,1.);"
"}";

/// Filled geometry vertex shader.
static const char *g_shader_vertex_geometry_shadow = ""
"attribute vec3 P;"
"attribute vec4 T;"
"attribute vec3 N;"
"attribute vec3 C;"
"uniform highp mat4 M;"
"uniform highp mat4 S;"
"uniform highp mat3 B;"
"uniform mediump vec3 L;"
"uniform mediump float V;"
"varying mediump float d;"
"varying lowp vec4 f;"
"varying mediump vec2 s;"
"varying mediump vec2 t;"
"precision mediump float;"
"void main()"
"{"
"vec4 p=vec4(P,1.);"
"vec4 v=S*p;"
"vec3 u=v.xyz/v.w*.5+.5;"
"s=u.xy;"
#if defined(RENDERER_ENABLE_DEPTH_TEXTURE)
"d=u.z;"
#else
"d=u.z*255.;"
#endif
"f=vec4(C,dot(B*N,L));"
"t=T.xy+T.zw*V;"
"gl_Position=M*p;"
"}";

/// Filled geometry fragment shader.
static const char *g_shader_fragment_geometry_shadow = ""
"uniform sampler2D I;"
"uniform sampler2D H;"
"uniform mediump vec2 U;"
"uniform lowp vec3 K;"
"varying mediump float d;"
"varying lowp vec4 f;"
"varying mediump vec2 s;"
"varying mediump vec2 t;"
"precision lowp float;"
"void main()"
"{"
"vec3 u=texture2D(I,gl_FragCoord.xy*U+t).xyz;"
"float l=(1.+f.w)*K.y;"
"if(f.w>.0)"
"{"
#if defined(RENDERER_ENABLE_DEPTH_TEXTURE)
"mediump float h=texture2D(H,s).x;"
#else
"mediump vec2 c=texture2D(H,s).xy;"
"mediump float h=c.x*255.+c.y;"
#endif
"l=mix(f.w*K.x+K.y,f.w*K.x*K.z+K.y,smoothstep(h,h+.01,d));"
"}"
"float k=max(1.-l,.0);"
"k=1.-k*k;"
"gl_FragColor=vec4(mix((u-.8)*5.,u,k)*f.xyz*l,1.);"
//"gl_FragColor=vec4(u*f.xyz*l,1.);"
"}";

/// Shadow map vertex shader.
static const char *g_shader_vertex_shadow_map = ""
"attribute vec3 P;"
"uniform highp mat4 S;"
#if !defined(RENDERER_ENABLE_DEPTH_TEXTURE)
"varying mediump float d;"
#endif
"precision mediump float;"
"void main()"
"{"
"vec4 p=S*vec4(P,1.);"
#if !defined(RENDERER_ENABLE_DEPTH_TEXTURE)
"d=(p.z/p.w*.5+.5)*255.;"
#endif
"gl_Position=p;"
"}";

/// Shadow map fragment shader.
static const char *g_shader_fragment_shadow_map = ""
#if !defined(RENDERER_ENABLE_DEPTH_TEXTURE)
"varying mediump float d;"
#endif
"precision lowp float;"
"void main()"
"{"
#if defined(RENDERER_ENABLE_DEPTH_TEXTURE)
"gl_FragColor=vec4(.0);"
#else
"mediump float i = floor(d);"
"mediump float f = d - i;"
"gl_FragColor=vec4(i*(1./255.),f,.0,1.);"
#endif
"}";

/// Shadow extrusion vertex shader.
static const char *g_shader_vertex_shadow_extrude = ""
"attribute vec3 P;"
"attribute vec3 N;"
"uniform highp mat4 S;"
"uniform highp mat4 W;"
"uniform highp mat3 B;"
"uniform mediump vec3 L;"
"precision mediump float;"
"void main()"
"{"
"vec3 l=vec3(.0);"
"vec3 p=(W*vec4(P,1.)).xyz;"
"vec3 n=B*N;"
"vec3 d=normalize(L-p);"
"if(dot(d,n)<.0)"
"{"
"l=d*-999.;"
"}"
"gl_Position=S*vec4(p+l,1.);"
"}";

/// 'Blank' fragment shader.
static const char *g_shader_fragment_shadow_extrude = ""
"precision lowp float;"
"void main()"
"{"
"gl_FragColor=vec4(1.,0.,.5,.2);"
"}";

/// Darkening vertex shader.
static const char *g_shader_vertex_darken = ""
"attribute vec3 P;"
"precision mediump float;"
"void main()"
"{"
"gl_Position=vec4(P,1.);"
"}";

/// Darkening fragment shader.
static const char *g_shader_fragment_darken = ""
"uniform lowp vec4 K;"
"precision lowp float;"
"void main()"
"{"
"gl_FragColor=K;"
"}";

/// Skybox vertex shader.
static const char *g_shader_vertex_skybox = ""
"attribute vec3 P;"
"attribute vec2 T;"
"uniform highp mat4 M;"
"varying mediump vec2 t;"
"precision mediump float;"
"void main()"
"{"
"t=T;"
"gl_Position=M*vec4(P,1.);"
"}";

/// Blit fragment shader.
static const char *g_shader_fragment_skybox = ""
"uniform sampler2D I;"
"uniform lowp vec3 L;"
"uniform lowp vec3 J;"
"uniform lowp vec3 K;"
"varying mediump vec2 t;"
"precision lowp float;"
"void main()"
"{"
"vec3 c=texture2D(I,t).xyz;"
"gl_FragColor=vec4(mix(vec3(.0),L,c.x)+mix(vec3(.0),J,c.y)+mix(K,vec3(.0),c.z),1.);"
"}";

/// Text vertex shader.
static const char *g_shader_vertex_text = ""
"attribute vec3 P;"
"attribute vec2 T;"
"uniform highp mat4 M;"
"varying mediump vec2 t;"
"precision mediump float;"
"void main()"
"{"
"t=T;"
"gl_Position=M*vec4(P,1.);"
"}";

/// Blit fragment shader.
static const char *g_shader_fragment_text = ""
"uniform sampler2D I;"
"uniform lowp vec3 K;"
"varying mediump vec2 t;"
"precision lowp float;"
"void main()"
"{"
"float c=texture2D(I,t).r;"
"gl_FragColor=vec4(K*c,c);"
"}";

#if defined(USE_LD)
/// Blit vertex shader.
static const char *g_shader_vertex_blit = ""
"attribute vec2 P;"
"attribute vec2 T;"
"varying mediump vec2 t;"
"precision mediump float;"
"void main()"
"{"
"t=T;"
"gl_Position=vec4(P,.0,1.);"
"}";

/// Blit fragment shader.
static const char *g_shader_fragment_blit = ""
"uniform sampler2D I;"
"varying mediump vec2 t;"
"precision lowp float;"
"void main()"
"{"
"gl_FragColor=texture2D(I,t);"
"}";
#endif

//######################################
// Generation. #########################
//######################################

/// Generate screen-space image.
///
/// \param width Width Screen width.
/// \param height Height Screen height.
/// \param ambient Ambient level.
/// \param seed Random seed.
static ImageGrayUptr generate_image_screenspace(unsigned width, unsigned height, float ambient,
    unsigned seed)
{
  uptr<ImageGray> ret(new ImageGray(width, height));
  ImageGrayUptr noise[9];

  bsd_srand(seed);

  for(unsigned ii = 0; (ii < 9); ++ii)
  {
    noise[ii] = new ImageGray((ii + 1) * 143 / 5, (ii + 1) * 17 / 5);
    noise[ii]->noise();
  }

  for(unsigned jj = 0; (height > jj); ++jj)
  {
    float fj = static_cast<float>(jj) / static_cast<float>(height);
    float jstr = (3.0f - static_cast<float>(jj % 7)) / 3.0f;
    unsigned iadd = ((jj / 7) % 2) ? 2 : 0;

    for(unsigned ii = 0; (width > ii); ++ii)
    {
      float fi = static_cast<float>(ii) / static_cast<float>(width);
      float istr = (3.0f - static_cast<float>((ii + iadd) % 7)) / 3.0f;
      float strength = std::min((istr * istr) + (jstr * jstr), 1.0f);
      float sampled =
        (noise[0]->sampleLinear(fi, fj) * 1.0f +
         noise[1]->sampleLinear(fi, fj) * 0.8f +
         noise[2]->sampleLinear(fi, fj) * 0.7f +
         noise[3]->sampleLinear(fi, fj) * 0.6f +
         noise[4]->sampleLinear(fi, fj) * 0.5f +
         noise[5]->sampleLinear(fi, fj) * 0.4f +
         noise[6]->sampleLinear(fi, fj) * 0.3f +
         noise[7]->sampleLinear(fi, fj) * 0.2f +
         noise[8]->sampleLinear(fi, fj) * 0.1f);

      ret->setPixel(ii, jj, (strength * 0.5f) + sampled);
    }
  }

  ret->normalize(0, ambient);
  return ret;
}

//######################################
// Plain data ##########################
//######################################

/// Global data block.
///
/// Struct for convenience.
///
/// It does not make sense to complicate access to some globals, no invariants here.
struct GlobalContainer
{
  public:
    /// Number of arrangements.
    ///
    /// This means different sets of objects to display. Current sets are:
    /// 0: Initial islands (swaying).
    /// 1: Maze island bottom, fake maze.
    /// 2: Maze island cap (both in maze and in opening).
    /// 3: Inside maze.
    /// 4: Aqueduct.
    /// 5: Trip islands.
    /// 6: Coliseum aqueduct displayed upon entry.
    /// 7: Coliseum island bottom..
    /// 8: Coliseum island top and coliseum.
    /// 9: Hellraiser big maze.
    static const unsigned ARRANGEMENT_COUNT = 10;
    /// \cond
    static const unsigned ARRANGEMENT_OPENING = 0;
    static const unsigned ARRANGEMENT_MAZE_CULLED = 1;
    static const unsigned ARRANGEMENT_MAZE_SUPPORT = 2;
    static const unsigned ARRANGEMENT_MAZE = 3;
    static const unsigned ARRANGEMENT_AQUEDUCT = 4;
    static const unsigned ARRANGEMENT_TRIP_ISLANDS = 5;
    static const unsigned ARRANGEMENT_COLISEUM_AQUEDUCT = 6;
    static const unsigned ARRANGEMENT_COLISEUM_CULLED = 7;
    static const unsigned ARRANGEMENT_COLISEUM = 8;
    static const unsigned ARRANGEMENT_HELLRAISER = 9;
    /// \endcond

  private:
    /// Number of different aqueduct variations.
    static const unsigned AQUEDUCT_VARIATION_COUNT = Aqueduct::AQUEDUCT_COUNT * Aqueduct::AQUEDUCT_COUNT;

    /// Number of islands.
    ///
    /// Islands are arranged as follows:_
    /// 0: Maze island.
    /// 1: Coliseum island.
    /// 2-3: Islands seen on the 'trip'.
    /// 4+ Filler islands for opening.
    static const unsigned ISLAND_COUNT = 9;
    /// \cond
    static const unsigned ISLAND_MAZE = 0;
    static const unsigned ISLAND_COLISEUM = 1;
    static const unsigned ISLAND_TRIP = 2;
    static const unsigned ISLAND_TRIP_LAST = 4;
    static const unsigned ISLAND_HELLRAISER = 5;
    static const unsigned ISLAND_FILLER = 6;
    static const unsigned ISLAND_FILLER_LAST = 8;
    /// \endcond

  public:
    /// \cond
    Program program_haamu_shape;
    Program program_haamu_sprite;
    Program program_geometry_plain;
    Program program_geometry_plain_animated;
    Program program_geometry_shadow;
    Program program_shadow_extrude;
    Program program_shadow_map;
    Program program_darken;
    Program program_sky;
    Program program_text;
#if defined(USE_LD)
    Program program_blit;
#endif
    FrameBuffer fbo_shadow_map;
    Direction direction;
    HaamuUptr haamu;
    MoelliUptr moelli;
    Skybox skybox_horrori;
    Skybox skybox_normal;
    Skybox skybox_overcast;
    Spline spline_ghost;
    Texture texture_screenspace_creepy;
    Texture texture_screenspace_mild;
    Font fnt;
    mat4 projection;
    unsigned screen_width;
    unsigned screen_height;
    /// \endcond
    
  private:
    /// \cond
    AqueductUptr aqueduct[AQUEDUCT_VARIATION_COUNT];
    FloatingIslandUptr island[ISLAND_COUNT];
    MazeResourcesUptr maze_resources;
    MazeUptr maze_fake;
    MazeUptr maze_full;
    MazeUptr maze_hellraiser;
    ImageGrayUptr image_screenspace_creepy;
    ImageGrayUptr image_screenspace_mild;
    MeshUptr mesh_coliseum;
    MeshUptr mesh_lol;
    GeometryBuffer geometry_aqueduct;
    GeometryBuffer geometry_generic;
    GeometryBuffer geometry_maze;
    /// \endcond
    
    /// Object database.
    ObjectDatabase m_object_database[ARRANGEMENT_COUNT];

  public:
    GlobalContainer(unsigned screen_w, unsigned screen_h, unsigned shadow_w, unsigned shadow_h) :
      program_haamu_shape(g_shader_vertex_geometry_haamu_shape, g_shader_fragment_geometry_haamu_shape),
      program_haamu_sprite(g_shader_vertex_geometry_haamu_sprite, g_shader_fragment_geometry_haamu_sprite),
      program_geometry_plain(g_shader_vertex_geometry_plain, g_shader_fragment_geometry_plain),
      program_geometry_plain_animated(g_shader_vertex_geometry_plain_animated, g_shader_fragment_geometry_plain),
      program_geometry_shadow(g_shader_vertex_geometry_shadow, g_shader_fragment_geometry_shadow),
      program_shadow_extrude(g_shader_vertex_shadow_extrude, g_shader_fragment_shadow_extrude),
      program_shadow_map(g_shader_vertex_shadow_map, g_shader_fragment_shadow_map),
      program_darken(g_shader_vertex_darken, g_shader_fragment_darken),
      program_sky(g_shader_vertex_skybox, g_shader_fragment_skybox),
      program_text(g_shader_vertex_text, g_shader_fragment_text),
#if defined(USE_LD)
      program_blit(g_shader_vertex_blit, g_shader_fragment_blit),
#endif
#if defined(RENDER_ENABLE_DEPTH_TEXTURE)
      fbo_shadow_map(shadow_w, shadow_h, false, true, BILINEAR),
#else
      fbo_shadow_map(shadow_w, shadow_h, true, false, BILINEAR),
#endif
      spline_ghost(BEZIER),
      fnt(36, g_font_options),
      projection(mat4::projection(CAMERA_FOV_RADIANS, screen_w, screen_h, CAMERA_NEAR, CAMERA_FAR)),
      screen_width(screen_w),
      screen_height(screen_h)
    {
      for(unsigned ii = static_cast<unsigned>('+'); (static_cast<unsigned>('z') >= ii); ++ii)
      {
        fnt.createCharacter(ii, geometry_generic);
      }
    }

  private:
    /// Add object.
    ///
    /// \param object Object.
    /// \param transform Transformation.
    /// \param idx Arrangement idx.
    /// \param tex Texture to use.
    void addObject(const Mesh &msh, const mat4 &transform, unsigned idx, const Texture *tex = NULL)
    {
      m_object_database[idx].addObject(msh, transform, tex);
    }

  public:
    /// Accessor.
    ///
    /// \param idx Arrangement index.
    /// \return Object database.
    const ObjectDatabase& getObjectDatabase(unsigned idx) const
    {
      return m_object_database[idx];
    }

    /// Precalculation of visuals.
    void precalculateVisuals()
    {
#if defined(USE_LD)
      uint32_t precalc_start = dnload_SDL_GetTicks();
#endif

      // Read spline data for ghost.
      spline_ghost.readData(g_ghost_light_path);

      // Skyboxes.
      skybox_horrori.construct(geometry_generic, 1741.0f, Skybox::coloring_func_horrori);
      skybox_horrori.setColorForward1(vec3(0.9f, 0.6f, 0.01f));
      skybox_horrori.setColorForward2(vec3(0.2f, 0.0f, 0.0f));
      skybox_horrori.setColorBackward(vec3(-1.0f, -1.0f, -1.0f));
      skybox_normal.construct(geometry_generic, 1741.0f, Skybox::coloring_func_normal);
      skybox_normal.setColorForward1(vec3(0.5f, 0.5f, 0.45f));
      skybox_normal.setColorForward2(vec3(0.0f, 0.0f, 0.0f));
      skybox_normal.setColorBackward(vec3(0.4f, 0.4f, 0.7f));
      skybox_overcast.construct(geometry_generic, 1741.0f, Skybox::coloring_func_overcast);
      skybox_overcast.setColorForward1(vec3(0.5f, 0.5f, 0.45f));
      skybox_overcast.setColorForward2(vec3(0.6f, 0.6f, 0.7f));
      skybox_overcast.setColorBackward(vec3(-0.4f, -0.4f, -0.4f));

      haamu = new Haamu(geometry_generic);
      moelli = new Moelli(1.0f, 5.0f, geometry_generic);
      maze_resources = new MazeResources(geometry_maze);

      // Main island.
      island[ISLAND_MAZE] = new FloatingIsland(130.0f, 50.0f, 19, geometry_generic, 1,
          (2*MAZE_CELL_WIDTH+MAZE_CELL_WALL_THICKNESS)/100.0f, (MAZE_CELL_HEIGHT*8)/130.0f);

      // Filler islands.
      for(unsigned ii = ISLAND_FILLER; (ISLAND_FILLER_LAST >= ii); ++ii)
      {
        island[ii] = new FloatingIsland(130.0f, 30.0f, 17, geometry_generic, ii);
      }
      {
        mat4 tr = mat4::translation(-449.0f, 48.0f, -501.0f);
        addObject(island[ISLAND_FILLER + 0]->getMeshLower(), tr, ARRANGEMENT_OPENING);
        addObject(island[ISLAND_FILLER + 0]->getMeshUpper(), tr, ARRANGEMENT_OPENING);
      }
      {
        mat4 tr = mat4::translation(32.0f, 98.0f, -130.0f);
        addObject(island[ISLAND_FILLER + 1]->getMeshLower(), tr, ARRANGEMENT_OPENING);
        addObject(island[ISLAND_FILLER + 1]->getMeshUpper(), tr, ARRANGEMENT_OPENING);
      }
      {
        mat4 tr = mat4::translation(291.0f, 8.0f, -670.0f);
        addObject(island[ISLAND_FILLER + 2]->getMeshLower(), tr, ARRANGEMENT_OPENING);
        addObject(island[ISLAND_FILLER + 2]->getMeshUpper(), tr, ARRANGEMENT_OPENING);
      }

      addObject(island[ISLAND_MAZE]->getMeshLower(), mat4::identity(), ARRANGEMENT_MAZE_CULLED);
      addObject(island[ISLAND_MAZE]->getMeshUpper(), mat4::identity(), ARRANGEMENT_MAZE_SUPPORT);

      // Designer maze.
      {
        static const uint8_t ledzideita[2 * 8 * 2] =
        {
          //SW    SE    NW    NE
          0x08, 0x04, 0x01, 0x0F,
          0x08, 0x04, 0x06, 0x03,
          0x03, 0x09, 0x0F, 0x04,
          0x08, 0x06, 0x09, 0x0D,
          0x03, 0x02, 0x09, 0x09,
          0x0E, 0x02, 0x0C, 0x0C,
          0x03, 0x0D, 0x0E, 0x0C,
          0x03, 0x0D, 0x09, 0x0E
        };
        static const uint8_t pakkorampit[2 * 8 * 2] =
        {
          //SW    SE    NW    NE
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x08, 0x00, 0x22, 0x00
        };
        const vec3 maze_position(-MAZE_CELL_WIDTH - MAZE_CELL_WALL_THICKNESS,
            -MAZE_CELL_HEIGHT * 8 + 13.0f,
            MAZE_CELL_WIDTH + MAZE_CELL_WALL_THICKNESS);

        bsd_srand(1904783453); // FFS

        maze_full = new Maze(2, 8, *maze_resources, m_object_database[ARRANGEMENT_MAZE], geometry_maze,
            NULL, maze_position, ledzideita, pakkorampit);

        // Small fake maze since we're not going to look down.
        maze_fake = new Maze(2, 3, *maze_resources, m_object_database[ARRANGEMENT_MAZE_CULLED], geometry_maze,
            NULL, maze_position + vec3(0.0f, 26.0f, 0.0f), ledzideita + (5 * 4), pakkorampit + (5 * 4));
      }

      // Coliseum island and coliseum.
      {
        LogicalMesh msh(COLOR_COLISEUM);
        mesh_generate_end_scene(msh);
        mesh_coliseum = msh.insert(geometry_generic, 1);
      }
      island[ISLAND_COLISEUM] = new FloatingIsland(440.0f, 160.0f, 21, geometry_generic, 2, 0.33f, 0.003f);

      addObject(*mesh_coliseum, mat4::translation(0.0f, 38.0f, 0.0f), ARRANGEMENT_COLISEUM);
      addObject(island[ISLAND_COLISEUM]->getMeshLower(), mat4::identity(), ARRANGEMENT_COLISEUM_CULLED);
      addObject(island[ISLAND_COLISEUM]->getMeshUpper(), mat4::identity(), ARRANGEMENT_COLISEUM);

      // Hellraiser scene.
      {
        island[ISLAND_HELLRAISER] = new FloatingIsland(105.0f, 105.0f, 8, geometry_generic, 1, 0.4f, 0.1f);
        addObject(island[ISLAND_HELLRAISER]->getMeshUpper(), mat4::identity(), ARRANGEMENT_HELLRAISER);

        maze_hellraiser = new Maze(5, 2, *maze_resources, m_object_database[ARRANGEMENT_HELLRAISER],
            geometry_maze, NULL, vec3(-MAZE_CELL_WIDTH * 2.5f - 2.5f, 14.4f, MAZE_CELL_WIDTH * 2.5f + 2.5f));
      }

      // Aqueduct(s) with trip islands.
      for(unsigned ii = 0; (ii < Aqueduct::AQUEDUCT_COUNT); ++ii)
      {
        for(unsigned jj = 0; (jj < Aqueduct::AQUEDUCT_COUNT); ++jj)
        {
          unsigned idx = (ii * Aqueduct::AQUEDUCT_COUNT) + jj;
          aqueduct[idx] = new Aqueduct(ii, jj, geometry_aqueduct, idx + 1);
        }
      }
      bsd_srand(8);
      {
        const Aqueduct *curr = aqueduct[0].get();
        for(float pos = 52.5f; (pos < 1000.0f); pos += 19.0f)
        {
          addObject(curr->getMesh(), mat4::translation(0.0f, 26.0f, pos), ARRANGEMENT_AQUEDUCT);

          // Also add to coliseum entry.
          if(pos <= 300.0f)
          {
            addObject(curr->getMesh(), mat4::translation(0.0f, 41.5f, pos + 62.0f), ARRANGEMENT_COLISEUM_AQUEDUCT);
          }
          // And to opening.
          if(pos <= 120.0f)
          {
            addObject(curr->getMesh(),
                mat4::rotation_euler(static_cast<float>(M_PI * 1.5), 0.0f, 0.0f, vec3(-pos + 9.0f, 12.0f, 0.0f)),
                ARRANGEMENT_MAZE_CULLED);
          }

          for(;;)
          {
            Aqueduct *next = aqueduct[urand(AQUEDUCT_VARIATION_COUNT)].get();

            if((curr->hasLeftExtentForward() != next->hasLeftExtentBackward()) ||
                (curr->hasRightExtentForward() != next->hasRightExtentBackward()))
            {
              continue;
            }

            curr = next;
            break;
          }
        }
      }

      bsd_srand(8);

      // Aqueduct scene islands on the way.
      for(unsigned ii = ISLAND_TRIP; (ii <= ISLAND_TRIP_LAST); ++ii)
      {
        island[ii] = new FloatingIsland(80.0f, 15.0f, 13, geometry_generic, ii);
      }
      {
        const mat4 trn = mat4::translation(50.0f, 40.0f, 850.0f);
        addObject(island[ISLAND_TRIP + 0]->getMeshLower(), trn, ARRANGEMENT_TRIP_ISLANDS);
        addObject(island[ISLAND_TRIP + 0]->getMeshUpper(), trn, ARRANGEMENT_TRIP_ISLANDS);
      }
      {
        const mat4 trn = mat4::translation(-60.0f, 20.0f, 760.0f);
        addObject(island[ISLAND_TRIP + 1]->getMeshLower(), trn, ARRANGEMENT_TRIP_ISLANDS);
        addObject(island[ISLAND_TRIP + 1]->getMeshUpper(), trn, ARRANGEMENT_TRIP_ISLANDS);
      }
      {
        const mat4 trn = mat4::translation(40.0f, 45.0f, 430.0f);
        addObject(island[ISLAND_TRIP + 2]->getMeshLower(), trn, ARRANGEMENT_TRIP_ISLANDS);
        addObject(island[ISLAND_TRIP + 2]->getMeshUpper(), trn, ARRANGEMENT_TRIP_ISLANDS);
      }

      // Done adding objects, sort the databases.
      for(ObjectDatabase &vv : m_object_database)
      {
        vv.sort();
      }

      // Generate images.
      image_screenspace_creepy = generate_image_screenspace(256, 256, 0.18f, 1);
      image_screenspace_mild = generate_image_screenspace(256, 256, 0.84f, 2);

      //gfx::image_png_save(std::string("lol.png"), image_senspace->getWidth(),
      //    image_screenspace->getHeight(), 24, image_screenspace->getExportData());

#if defined(USE_LD)
      uint32_t precalc_end = dnload_SDL_GetTicks();
      std::cout << "|precalc(visuals): " << (static_cast<float>(precalc_end - precalc_start) * .001f) << std::endl;
#endif
    }

    /// Update data to GPU.
    void update()
    {
      // Update textures to GPU, can free images after update.
      texture_screenspace_creepy.update(*image_screenspace_creepy, NEAREST);
      image_screenspace_creepy.reset();
      texture_screenspace_mild.update(*image_screenspace_mild, NEAREST);
      image_screenspace_mild.reset();

      // Update skybox textures to GPU.
      skybox_horrori.update();
      skybox_normal.update();
      skybox_overcast.update();

      // Update haamu and moelli textures to GPU.
      haamu->update();
      moelli->update();

      // Upload vertex buffers.
      geometry_aqueduct.update();
      geometry_generic.update();
      geometry_maze.update();

#if defined(USE_LD)
      std::cout << LogicalVertex::get_merge_count() << " vertices with maximum error " <<
        LogicalVertex::get_max_merge_error() << " merged, next error: " <<
        LogicalVertex::get_min_position_error() << std::endl <<
        LogicalMesh::get_discarded_edge_count() << " non-visible edges discarded\n" <<
        LogicalMesh::get_discarded_face_count() << " non-visible faces discarded, " <<
        LogicalFace::get_degenerate_count() << " degenerate faces removed\n" <<
        vgl::get_data_size_edge() << " bytes used for edge data\n" <<
        vgl::get_data_size_index() << " bytes used for index data\n" <<
        vgl::get_data_size_texture() << " bytes used for texture data\n" <<
        vgl::get_data_size_vertex() << " bytes used for vertex data\n";
#endif
    }
};

//######################################
// Global intro state ##################
//######################################

/// Global state.
///
/// Holds global state and global data.
class GlobalState : public StateQueue
{
  public:
    /// Pass index.
    static const unsigned PASS_DEFAULT = 0;
    
    /// Pass index.
    static const unsigned PASS_SKY = 1;

    /// Pass index (haamu sprite form).
    static const unsigned PASS_HAAMU_SPRITE = 2;

    /// Pass index (animated haamu with normal shader).
    static const unsigned PASS_HAAMU_NORMAL = 3;

    /// Pass index (animated haamu with shady shader).
    static const unsigned PASS_HAAMU_SHADY = 4;

    /// Pass index.
    static const unsigned PASS_TEXT = 5;

  private:
    /// Global data block.
    GlobalContainer m_globals;

    /// Done flag.
    bool m_done;

  public:
    /// Constructor.
    ///
    /// \param screen_w Screen width.
    /// \param screen_h Screen height.
    GlobalState(unsigned screen_w, unsigned screen_h) :
      m_globals(screen_w, screen_h, 1024, 1024),
      m_done(false) { }

  private:
    /// Add a string.
    ///
    /// \param px X position.
    /// \param py Y position.
    /// \param content Content string.
    void drawText(State &op, float px, float py, float fs, float phase, const char *content) const
    {
      static const float PHASE_AREA = 0.3f;
      static const float PHASE_EXPAND = 9.0f;
      static const float PHASE_IN = 0.1f;
      static const float PHASE_OUT = 1.0f - PHASE_AREA - PHASE_IN;
      float x_aspect = fs * static_cast<float>(m_globals.screen_height) /
        static_cast<float>(m_globals.screen_width);
      float cx = px;
#if defined(RENDER_ENABLE_KERNING)
      unsigned previous_char = 0;
#endif

      bsd_srand(static_cast<unsigned>(*content));

      for(;; ++content)
      {
        char cc = *content;
        if(!cc)
        {
          break;
        }

        // Do not try to render whitespace.
        if(' ' == cc)
        {
#if defined(RENDER_ENABLE_KERNING)
          previous_char = 0;
#endif
          cx += x_aspect * 0.5f;
          continue;
        }

        const Character &chr = m_globals.fnt.getCharacter(static_cast<unsigned>(cc));
        const Object &obj = chr.getObject();

#if defined(RENDER_ENABLE_KERNING)
        unsigned current_char = chr.getFontIndex();
        if(previous_char)
        {
          float kerning = m_globals.fnt.getKerning(previous_char, current_char);
          cx += kerning * fs;
        }
#endif

        // Check for phasing and rendering at all.
        {
          float char_phase = frand(PHASE_AREA);
          float instep = linear_step(char_phase, char_phase + PHASE_IN, phase);
          float outstep = linear_step_down(PHASE_OUT + char_phase, PHASE_OUT + PHASE_IN + char_phase, phase);
          float squish = instep * outstep;

          if(squish > 0.0f)
          {
            float ymul = ((1.0f - squish) * PHASE_EXPAND);
            mat4 tr = mat4::translation(cx, py - (ymul * fs), 0.0f);

            tr[0] = x_aspect * squish;
            tr[5] = fs + ymul;

            op.addObjectScreenspace(obj, tr, PASS_TEXT);
          }
        }

        cx += chr.getAdvanceX() * x_aspect;
#if defined(RENDER_ENABLE_KERNING)
        previous_char = current_char;
#endif
      }
    }

    /// Fill haamu.
    ///
    /// Haamu data is arranged as follows.
    /// 0: Blink time start.
    /// 1: Blink time.
    /// 2: X position.
    /// 3: Y position.
    /// 4: Z position.
    /// 5: Haamu lookat X.
    /// 6: Haamu lookat Y.
    /// 7: Haamu lookat Z.
    /// 8: X speed as 8.8 fixed point.
    /// 9: Y speed as 8.8 fixed point.
    /// 10: Z speed as 8.8 fixed point.
    /// 11: Trail direction X.
    /// 12: Trail direction Y.
    /// 13: Trail direction Z.
    /// 14: Trail length (expressed as 8.8 signed fixed point).
    /// 15: Haamu intensity (expressed as 8.8 signed fixed point).
    /// 16: Haamu animation (0 = curse, 1 = dead, 2 = turn, 3 = walk).
    /// 17: Haamu animation offset (expressed as 8.8 signed fixed point).
    /// 18: How many frames from start animation is at time 1.0.
    ///
    /// 0 at blink time start terminates.
    /// Trail direction can be 0, 0, 0. if so, the currently set light direction will be used.
    ///
    /// \param op State to fill to.
    /// \param data Haamu scene location data.
    /// \param usceme Frame position in scene.
    void fillHaamu(State &op, const int16_t *data, unsigned uscene) const
    {
      int iscene = static_cast<int>(uscene);

      for(const int16_t *iter = data; (*iter); iter += 19)
      {
        if((iter[0] <= iscene) && (iter[0] + iter[1] >= iscene))
        {
          int iframe = iscene - iter[0];
          float interp = static_cast<float>(iframe) / static_cast<float>(iter[1]);

          // Haamu has fade in and fade out.
          {
            float intensity = 1.0f - ((0.5f - std::abs(interp - 0.5f)) * 2.0f);
            intensity = intensity * intensity;
            op.storeFloat('X', 1.0f - intensity);
          }
          op.storeFloat('Y', fixed_8_8_to_float(iter[15]));

          mat4 look = mat4::lookat(vec3(0.0f, 0.0f, 0.0f), vec3(static_cast<float>(iter[5]),
                static_cast<float>(iter[6]), static_cast<float>(iter[7])));
#if defined(HAAMU_SHAPE_SIMPLE)
          mat4 ori = look;
#else
          // Undo Blender space.
          mat4 rot = mat4::rotation_euler(static_cast<float>(M_PI), -static_cast<float>(M_PI / 2.0), 0.0f);
          mat4 ori = look * rot;
#endif
          ori.setTranslation(static_cast<float>(iter[2]) + interp * fixed_8_8_to_float(iter[8]),
              static_cast<float>(iter[3]) + interp * fixed_8_8_to_float(iter[9]),
              static_cast<float>(iter[4]) + interp * fixed_8_8_to_float(iter[10]));

#if 0
          // Use light direction instead of explicit trail.
          // If light direction is not yet set, results are undefined.
          if((0 == iter[11]) && (0 == iter[12]) && (0 == iter[13]))
          {
            op.storeFloat('R', 'S', 'T', op.getLightDirection());
          }
          else
#endif
          {
            vec3 dir(static_cast<float>(iter[11]), static_cast<float>(iter[12]),
              static_cast<float>(iter[13]));
            // Trail is added in object space, must undo haamu transform.
            vec3 unit_dir = normalize(transpose(ori.getRotation()) * dir);

            op.storeFloat('R', 'S', 'T', unit_dir * fixed_8_8_to_float(iter[14]));
          }

          float anim_pos = fixed_8_8_to_float(iter[17]);
          if(iter[18])
          {
            anim_pos += static_cast<float>(iframe) / static_cast<float>(iter[18]);
          }

          //std::cout << "filled haamu at " << anim_pos << " to " << ori.getTranslation() << std::endl;
          
          m_globals.haamu->insertShape(op, ori, static_cast<Haamu::AnimationEnum>(iter[16]),
              anim_pos, PASS_HAAMU_SHADY, uscene + 1);
          return;
        }
      }
    }
    
    /// Get distance of a certain part of the moelli at the scene timeline.
    ///
    /// All parts of moelli start as closed, so initial distance is 0.
    ///
    /// Data is as follows:
    /// 0: Timestamp.
    /// 1: Delta time until next state is active.
    /// 2: Part index (0-3 upper, 4-7 lower, rotating in euler circle starting from x+ z+).
    /// 3: Distance expressed as 8.8 signed fixed point.
    ///
    /// 0 at timestamp terminates.
    ///
    /// \param data Moelli part position data.
    /// \param part Part to query.
    /// \param fscene Frame in scene.
    /// \param mtr Whole moelli transform.
    void fillMoelliPart(State &op, const int16_t *data, unsigned part, float fscene, const mat4 &mtr) const
    {
      const MoelliPart &vv = m_globals.moelli->getPart(part);
      float dist = 0.0f;

      for(const int16_t *iter = data; (*iter); iter += 4)
      {
        float ftime = static_cast<float>(iter[0]);

        if(ftime > fscene)
        {
          break;
        }

        if(static_cast<unsigned>(iter[2]) == part)
        {
          float fdelta = static_cast<float>(iter[1]);
          float newdist = fixed_8_8_to_float(iter[3]);

          if(ftime + fdelta <= fscene)
          {
            dist = newdist;
          }
          else
          {
            float interp = linear_step(ftime, ftime + fdelta, fscene);
            dist = mix(dist, newdist, interp);
            break;
          }
        }
      }

      op.addObject(vv.getObject(), mtr * mat4::translation(vv.getDirection() * dist));
    }

    /// Fill text.
    ///
    /// \param data Text location data.
    /// \param fscene Scene frame time.
    void fillText(State &op, const TextLocation *data, float fscene) const
    {
      static const float TEXT_TIME_MUL = (1.0f / 50.0f) * 10.0f;
      static const float TEXT_DURATION = 1.3f * 10.0f;
      static const float TEXT_SIZE = 0.22f;

      const TextLocation *iter = data;
      float text_time = fscene * TEXT_TIME_MUL;
      while(iter->text)
      {
        float tstart = static_cast<float>(iter->params[0]);
        float phase = linear_step(tstart, tstart + TEXT_DURATION, text_time);

        if((phase > 0.0f) && (phase < 1.0f))
        {
          float tx = static_cast<float>(iter->params[1]) * 0.1f;
          float ty = static_cast<float>(iter->params[2]) * 0.1f;
          drawText(op, tx, ty, TEXT_SIZE, phase, iter->text);
        }

        ++iter;
      }
    }

    /// \brief Set the done flag.
    void setDone()
    {
      m_done = true;
    }

  public:
    /// Fill objects into a state.
    ///
    /// \param op State to fill into.
    void fillState(State &op) const
    {
      unsigned uframe = static_cast<unsigned>(op.getFrame());
      float fframe = static_cast<float>(op.getFrame());
      const Skybox* sb = NULL;

      // Resolve scene settings.
      SceneEnum scene;
      vec3 cpos;
      vec3 epos;
      unsigned uscene;
      m_globals.direction.resolveScene(uframe/* + 7800*/, scene, cpos, epos, uscene);
      float fscene = static_cast<float>(uscene);

      // Skybox and camera locations depend on camera from direction.
      mat4 sb_transform = mat4::translation(cpos);
      mat4 camera;

#if defined(USE_LD)
      if(is_developer() && (g_force_scene != NONE))
      {
        scene = g_force_scene;
      }

      if(is_developer() && !g_camera_lock)
      {
        float rot_yaw = g_rot_yaw + STARTING_ROT_YAW;
        float rot_pitch = g_rot_pitch + STARTING_ROT_PITCH;
        float rot_roll = g_rot_roll + STARTING_ROT_ROLL;

        camera = mat4::rotation_euler(rot_yaw, rot_pitch, rot_roll, cpos + g_pos);
      }
      else
#endif
      {
        camera = mat4::lookat(cpos, epos);
      }
      op.setPosition(cpos);

#if defined(USE_LD)
      if(g_print_position)
      {
        vec3 rpos = cpos + (g_camera_lock ? vec3(0.0f) : g_pos);
        vec3 look = rpos + camera.getForward() * 100.0f;

        std::cout << rpos << " ; " << look << " ; " << uframe << " (" <<
          (fframe / (1000.0f / static_cast<float>(FRAME_MILLISECONDS))) << "s)\n";

        g_print_position = false;
      }
#endif
      // Default:
      // - light as in default scene
      // - full lighting
      // - shadow maps off
      // - blit darken for stencil off
      op.storeFloat('O', 'P', 'Q', vec3(0.9f, 0.2f, 0.003f));
      op.storeFloat('K', 1.0f);
      op.storeFloat('V', 0.0007f * fscene);
      op.storeBool('B', false);
      op.storeBool('G', true);
      op.storeBool('S', false);

      /// Initialize state.
      op.initialize(m_globals.projection, camera);

      // Act differently depending on scene.
      if(OPENING == scene)
      {
        static const int16_t HAAMU_POSITION[] =
        {
          240, 5, -15, 14, -17, 1, 0, 2, 0, 0, 0, 1, 0, 0, 110, 16, 2, 0, 297,
          250, 8, -15, 14, -17, 1, 0, 2, 0, 0, 0, 1, 0, 0, 84, 16, 2, 0, 297,
          263, 10, -15, 14, -17, 1, 0, 2, 0, 0, 0, 1, 0, 0, 58, 16, 2, 0, 297,
          278, 297, -15, 14, -17, 1, 0, 2, 0, 0, 0, 1, 0, 0, 32, 16, 2, 0, 297,

          590, 400, -18, 14, -15, -1, 0, 1, -3840, 256, 3584, 1, 0, 0, 32, 16, 3, 0, 48,

          0
        };

        // Need to change light position to prevent the shittiest shadow map artifacts. Yes, horrible.
        if(uscene >= 575)
        {
          op.setLight(vec3(-1.0f, -0.5f, -0.3f), vec3(0.0f, 0.0f, 0.0f), 128.0f, 0.2f, 96.0f, 128.0f);
        }
        else if(uscene >= 225)
        {
          op.setLight(vec3(-1.0f, -0.5f, -0.3f), vec3(-20.0f, 0.0f, 16.0f), 96.0f, 0.2f, 24.0f, 96.0f);
        }
        else
        {
          op.setLight(vec3(-1.0f, -0.5f, -0.3f), vec3(0.0f, 0.0f, -20.0f), 96.0f, 0.2f, 52.0f, 58.0f);
        }

        op.addObjectDatabaseSwaying(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_OPENING),
            vec3(5.0f, 20.0f, 5.0f), 0.002f * fframe, 2);
        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_MAZE_SUPPORT));
        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_MAZE_CULLED));

        fillHaamu(op, HAAMU_POSITION, uscene);

        op.storeFloat('K', linear_step_down(0.0f, 120.0f, fscene) * 4.0f + 1.0f);
        op.storeBool('S', true);
        sb = &(m_globals.skybox_overcast);
      }
      else if(MAZE == scene)
      {
        static const float FADE_START = 2140.0f;
        static const float FADE_END = 2240.0f;
        static const float HAAMU_SPEED = 0.3f;
        static const float HAAMU_JITTER = 0.04f;
        static const int HAAMU_START = 200;
        static const int SPRITE_DETAIL = 18;

        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_MAZE_SUPPORT));
        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_MAZE));

        {
          // Undo Blender space.
          mat4 pos = mat4::rotation_euler(static_cast<float>(M_PI / 2.0), -static_cast<float>(M_PI / 2.0), 0.0f,
              vec3(-16.2f, -27.1f, 15.6f));

          m_globals.haamu->insertShape(op, pos, Haamu::ANIMATION_DEAD, 0.0f, PASS_HAAMU_NORMAL);
        }

        // Laziest billboard on earth. We have infinite time on CPU anyway.
        for(int ii = 0; (ii < SPRITE_DETAIL); ++ii)
        {
          int iscene = static_cast<int>(uscene);
          int istamp = std::max(iscene - HAAMU_START - ii, 0);
          float fi = static_cast<float>(ii);
          float fstamp = std::max(static_cast<float>(iscene - HAAMU_START) - (fi * HAAMU_SPEED), 0.0f);
          vec3 ghost_pos = m_globals.spline_ghost.resolvePosition(fstamp);

          if(istamp)
          {
            bsd_srand(istamp);

            float jitter = HAAMU_JITTER * fi;
            float jitter_x = frand(-jitter, jitter);
            float jitter_y = frand(-jitter, jitter);
            float jitter_z = frand(-jitter, jitter);
            vec3 ghost_real_pos = vec3(jitter_x, jitter_y, jitter_z) + ghost_pos;
            mat4 ghost_transform = mat4::lookat(ghost_real_pos, cpos);

            op.addObject(m_globals.haamu->getSprite(uframe + ii), ghost_transform, PASS_HAAMU_SPRITE);
          }

          if(ii == 0)
          {
            op.storeFloat('L', 'M', 'N', ghost_pos);
          }
        }

        float whiteout = 1.0f + linear_step(FADE_START, FADE_END, fscene) * 5.0f;

        op.storeFloat('O', 'P', 'Q', vec3(2.3f, std::max(0.05f, whiteout - 1.0f), 0.15f));

        op.storeFloat('K', whiteout);
        op.storeBool('G', false);
        sb = &(m_globals.skybox_overcast);
      }
      else if(AQUEDUCT == scene)
      {
        static const int16_t HAAMU_POSITION[] =
        {
          1,        210, 0, 27, 790, 0, 0, -1, 0, 0, -2850, 5, 1, 0,  722, 8, 3, 0, 47,
          161 + 70, 221, 0, 27, 755, 0, 0, -1, 0, 0, -3000, 5, 1, 0,  722, 8, 3, 0, 47,
          387 + 70, 35,  0, 27, 736, 0, 0, -1, 0, 0,  -475, 5, 1, 0, 2888, 8, 3, 0, 47,
          424 + 70, 35,  0, 27, 718, 0, 0, -1, 0, 0,  -950, 5, 1, 0, 2888, 8, 3, 3, 24,
          460 + 70, 35,  0, 27, 698, 0, 0, -1, 0, 0, -1900, 5, 1, 0, 2888, 8, 3, 4, 12,
          500 + 70, 35,  0, 27, 680, 0, 0, -1, 0, 0, -1900, 5, 1, 0, 2888, 8, 3, 5, 12,
          538 + 70, 25,  0, 27, 662, 0, 0, -1, 0, 0, -1900, 5, 1, 0, 2000, 8, 3, 6, 12,
          576 + 70, 20,  0, 27, 644, 0, 0, -1, 0, 0, -1900, 5, 1, 0, 1500, 8, 3, 7, 12,
          615 + 70, 15,  0, 27, 626, 0, 0, -1, 0, 0, -1900, 5, 1, 0, 1000, 8, 3, 8, 12,
          0
        };
        static const float SLOW_START = 340.0f;
        static const float SLOW_END = SLOW_START + 115.0f;
        static const float SLOW_MUL = 0.05f;
        static const float FAST_MUL = 0.5f;
        static const float FADE_START = 1100.0f;
        static const float FADE_END = 1190.0f;
        static const float ZPOS_START = 839;
        float zpos = ZPOS_START;

        if(fscene <= SLOW_END)
        {
          zpos -= std::max(0.0f, fscene - SLOW_START) * SLOW_MUL;
        }
        else
        {
          zpos -= ((SLOW_END - SLOW_START) * SLOW_MUL) + (fscene - SLOW_END) * FAST_MUL;
        }
        vec3 pos(0.0f, 0.0f, zpos);

        // Keep camera still in the beginning.
        if(uscene < 540)
        {
          op.setLight(vec3(-1.0f, -0.6f, 0.4f), vec3(0.0f, 0.0f, ZPOS_START - 20.0f), 180.0f, 0.2f, 112.0f, 96.0f);
        }
        else
        {
          op.setLight(vec3(-1.0f, -0.6f, 0.4f), pos, 180.0f, 0.2f, 192.0f, 96.0f);
        }

        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_TRIP_ISLANDS));

#if defined(USE_LD)
        if(is_developer() && !g_camera_lock)
        {
          op.addObjectDatabaseConstructing(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_AQUEDUCT),
              pos + g_pos, 100.0f, 100.0f, 6.5f);
        }
        else
#endif
        {
          op.addObjectDatabaseConstructing(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_AQUEDUCT),
              pos, 100.0f, 100.0f, 6.5f);
        }

        fillHaamu(op, HAAMU_POSITION, uscene);

        op.storeFloat('K', linear_step_down(FADE_START, FADE_END, fscene));
        op.storeBool('S', true);
        sb = &(m_globals.skybox_normal);
      }
      else if(COLISEUM_ENTRY_FIRST == scene)
      {
        static const int16_t HAAMU_POSITION[] =
        {
          1, 350, 0, 43, 142, 0, 0, -1, 0, 0, -4749, 4, 0, 1, 512, 8, 3, 0, 47,
          0
        };
        static const float FLICKER_END = 40.0f;
        static const float FLICKER_AMPLITUDE = 0.6f;
        static const float SLOW_START = 235.0f;
        static const float SLOW_MUL = 0.05f;
        static const float FAST_MUL = 0.5f;
        float zpos = 155.0f;

        if(fscene <= SLOW_START)
        {
          zpos -= fscene * FAST_MUL;
        }
        else
        {
          zpos -= SLOW_START * FAST_MUL + (fscene - SLOW_START) * SLOW_MUL;
        }

        vec3 pos(0.0f, 0.0f, std::max(zpos, 30.4f));

        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM));
        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM_CULLED));

#if defined(USE_LD)
        if(is_developer() && !g_camera_lock)
        {
          op.addObjectDatabaseConstructing(
              m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM_AQUEDUCT),
              pos + g_pos, 100.0f, 100.0f, 6.0f);
        }
        else
#endif
        {
          op.addObjectDatabaseConstructing(
              m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM_AQUEDUCT),
              pos, 100.0f, 100.0f, 6.0f);
        }

        // Flicker fadeout.
        if(fscene < FLICKER_END)
        {
          bsd_srand(uframe);
          op.storeFloat('K', 1.0f - linear_step_down(0.0f, FLICKER_END, fscene) * frand(FLICKER_AMPLITUDE));
        }

        fillHaamu(op, HAAMU_POSITION, uscene);

        op.storeFloat('L', 'M', 'N', vec3(0.0f, 50.0f, zpos + 80.0f));
        sb = &(m_globals.skybox_horrori);
      }
      else if(COLISEUM_ENTRY_SECOND == scene)
      {
        static const int16_t HAAMU_POSITION[] =
        {
          10,  140,  -9, 41, 73, -3, 0, -100,  -37, 0, -1221, -3, 0, -100, 256, 8, 3, 0, 47,
          190, 100, -47, 39, 13,  5, 0,   -2, 1260, 0, -504,   5, 0,   -2, 512, 8, 3, 0, 47,
          0
        };
        static const float FLICKER_START = 300;
        static const float FLICKER_END = 380;
        static const float FLICKER_AMPLITUDE = 0.8f;
        static const float YSPEED = -0.06f;
        float ypos = 91.0f + (YSPEED * fscene);

        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM));

        if(fscene > FLICKER_START)
        {
          op.storeFloat('K', 1.0f - linear_step(FLICKER_START, FLICKER_END, fscene) * frand(FLICKER_AMPLITUDE));
        }

        fillHaamu(op, HAAMU_POSITION, uscene);

        op.storeFloat('L', 'M', 'N', vec3(0.0f, ypos, 0.0f));
        op.storeFloat('O', 'P', 'Q', vec3(1.5f, 0.05f, 0.017f));
        sb = &(m_globals.skybox_horrori);
      }
      else if(COLISEUM == scene)
      {
        static const int16_t MOELLI_PART_POSITION[] =
        {
          200, 200, 0, 255,
          200, 200, 1, 255,
          200, 200, 2, 255,
          200, 200, 3, 255,
          200, 200, 4, 255,
          200, 200, 5, 255,
          200, 200, 6, 255,
          200, 200, 7, 255,
          900, 100, 2, -8,
          900, 100, 3, -8,
          900, 100, 6, -8,
          900, 100, 7, -8,
          // Open up again
          1600 + 45, 80, 2, 255,
          1600 + 45, 80, 3, 255,
          1600 + 45, 80, 6, 255,
          1600 + 45, 80, 7, 255,
          // 'random' opening and closing.
          1740 + 60 + 45, 50, 1, -8,
          1760 + 60 + 45, 50, 3, -8,
          1770 + 60 + 45, 50, 4, -8,
          1780 + 60 + 45, 50, 2, -8,
          1800 + 60 + 45, 50, 6, -8,
          1800 + 60 + 45, 50, 1, 255,
          1820 + 60 + 45, 50, 4, 255,
          1840 + 60 + 45, 50, 5, -8,
          1860 + 60 + 45, 50, 7, -8,
          1860 + 60 + 45, 50, 0, -8,
          1890 + 60 + 45, 50, 2, 255,
          1890 + 60 + 45, 50, 6, 255,
          1900 + 60 + 45, 50, 3, 255,
          1900 + 60 + 45, 50, 5, 255,
          1910 + 60 + 45, 50, 7, 255,
          1910 + 60 + 45, 50, 0, 255,
          // Close everything.
          2060 + 45, 80, 0, 0,
          2060 + 45, 80, 1, 0,
          2060 + 45, 80, 2, 0,
          2060 + 45, 80, 3, 0,
          2060 + 45, 80, 4, 0,
          2060 + 45, 80, 5, 0,
          2060 + 45, 80, 6, 0,
          2060 + 45, 80, 7, 0,
          0
        };
        static const int16_t MOELLI_TRANSFORM[] =
        {
          500,     0,   0,   0,
          700,  1460, -30, -60,
          900,   860,   0,  70,
          1000,  690,   0,  70,
          1070,  660,   0, -30,
          1120,  200,   0, -20,
          1180,  170,   0, -30,
          1270, -590,   0,  40,
          1320, -595,   0,  30,
          1330, -470,   0,  10,
          1595 + 45, -470,   0,   0,
          1790 + 45,  780,   0,   0,
          2060 + 45,  1040,  0,   0,
          0
        };
        static const int16_t HAAMU_POSITION[] =
        {
          1040,  55,  12, 55,  21, -1, 0, -2, 0, 0, 0, -1, 0, -2, 1444,   8, 2,   0, 120,
          1150,  55, -11, 55,  19,  1, 0,  0, 0, 0, 0,  2, 0, -3, 1444,   8, 2,   0, 120,
          1310,  15, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4, 1444,   8, 2,   0, 225,
          1325,  30, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  666, 255, 0,   0, 225,
          1355,  35, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  128, 255, 0,   0, 260,
          1390,  25, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  256, 255, 0,  34, 260, //35
          1415,  45, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  384, 255, 0,  59, 260, //60
          1460,  20, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  512, 255, 0, 103, 260, //105
          1480,  30, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  640, 255, 0, 123, 260, //125
          1510,  40, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  768, 255, 0, 153, 260, //155
          1550,  30, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  778, 255, 0, 192, 260, //195
          1580,  35, -24, 55,  -7, 13, 0,  4, 0, 0, 0, 13, 0,  4,  820, 255, 0, 221, 260, //225
          0
        };
        static const float FADE_START = 2170 + 45;
        static const float FADE_END = 2270 + 45;
        mat4 mtr = get_moelli_transform(MOELLI_TRANSFORM, fscene);

        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM));

        for(unsigned ii = 0; (Moelli::NUM_PARTS > ii); ++ii)
        {
          fillMoelliPart(op, MOELLI_PART_POSITION, ii, fscene, mtr);
        }

        // Add eye.
        {
          static const float EYE_JITTER = 0.15f;
          static const unsigned EYE_COMPLEXITY = 7;
  
          for(unsigned ii = 0; (EYE_COMPLEXITY > ii); ++ii)
          {
            bsd_srand(uframe + ii);

            vec3 eye_jitter(frand(-EYE_JITTER, EYE_JITTER),
                frand(-EYE_JITTER, EYE_JITTER),
                frand(-EYE_JITTER, EYE_JITTER));
            mat4 eye_transform = mat4::lookat(vec3(0.0f, 60.0f, 0.0f) + eye_jitter, cpos);    
            
            op.addObject(m_globals.moelli->getSprite(uframe + ii), eye_transform, PASS_HAAMU_SPRITE);
          }
        }

        fillHaamu(op, HAAMU_POSITION, uscene);

        op.storeFloat('K', linear_step_down(FADE_START, FADE_END, fscene));
        op.storeFloat('L', 'M', 'N', vec3(mtr[12], mtr[13], mtr[14]));
        //op.storeFloat('L', 'M', 'N', vec3(0.0f, 60.0f, 0.0f));
        op.storeFloat('O', 'P', 'Q', vec3(1.4f, 0.05f, 0.017f));
        op.storeBool('B', true);
        sb = &(m_globals.skybox_horrori);
      }
      else if(CREDITS == scene)
      {
        static const TextLocation TEXT_LOCATIONS[] =
        {
          { "Juippi", { 9, -9, 7 } },
          { "Trilkk", { 18, -3, 6 } },
          { "Warma", { 27, 4, 4 } },
          { "Odroid-C1+ intro", { 38, -9, 7 } },
          { "Faemiyah 2016-08", { 49, -4, 6 } },
          { NULL, { 0, 0, 0 } }
        };

        fillText(op, TEXT_LOCATIONS, fscene);

        op.setLight(vec3(-1.0f, -0.5f, -0.2f), vec3(-20.0f, 60.0f, 0.0f), 192.0f, 0.2f, 96.0f, 96.0f);

        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_COLISEUM));

        op.storeFloat('Z', 0.0f);
        op.storeBool('S', true);
        sb = &(m_globals.skybox_normal);
      }
      else // Default is hellraiser/greets scene.
      {
        op.addObjectDatabase(m_globals.getObjectDatabase(GlobalContainer::ARRANGEMENT_HELLRAISER));

        if(GREETS == scene)
        {
          static const TextLocation TEXT_LOCATIONS[] =
          {
            { "Blobtrox", { 7, -2, -4 } },
            { "Ctrl-Alt-Test", { 14, 1, 3 } },
            { "Desire", {21, 3, -6 } },
            { "Epoch", { 28, -7, 4 } },
            { "Fit", { 35, -2, -6 } },
            { "Forsaken", { 42, -6, 6 } },
            { "G-INcorporated", { 49, -4, -4 } },
            { "Hedelmae", { 56, -2, -3 } },
            { "Lonely Coders", { 63, -1, 6 } },
            { "Royal Elite Ninjas", { 70, -7, -5 } },
            { "The Digital Artists", { 77, -5, 7 } },
            { "Tekotuotanto", { 84, -2, -4 } },
            { "Traction", { 91, -6, -1 } },
            { NULL, { 0, 0, 0 } }
          };
          static const float FLICKER_AMBIENT = 0.12f;
          static const float FLICKER_AMPLITUDE_MIN = 0.07f;
          static const float FLICKER_AMPLITUDE_MAX = 0.1f;
          static const float FLICKER_PHASE = 0.024f;

          fillText(op, TEXT_LOCATIONS, fscene);

          // Very faint background flicker.
          bsd_srand(uframe);
          float amplitude = frand(FLICKER_AMPLITUDE_MIN, FLICKER_AMPLITUDE_MAX);

          op.storeFloat('K', FLICKER_AMBIENT - amplitude * dnload_cosf(FLICKER_PHASE * fscene));
          op.storeFloat('Z', 1.0f);
          sb = &(m_globals.skybox_horrori);
        }
        else
        {
          static const float FLICKER_START = 490.0f;
          static const float FLICKER_GROW = 0.002f;

          for(unsigned ii = 0; (Moelli::NUM_PARTS > ii); ++ii)
          {
            const MoelliPart &vv = m_globals.moelli->getPart(ii);

            op.addObject(vv.getObject(), mat4::translation(vec3(0.0f, 30.0f, 0.0f)));
          }

          // Not so faint background flicker.
          if(fscene > FLICKER_START)
          {
            bsd_srand(uframe);
            op.storeFloat('K', 1.0f - ((fscene - FLICKER_START) * FLICKER_GROW * frand(1.0f)));
          }
        }

        op.storeFloat('L', 'M', 'N', vec3(5.0f, 40.0f, 5.0f));
        op.storeFloat('O', 'P', 'Q', vec3(1.1f, 0.05f, 0.012f));
        sb = &(m_globals.skybox_horrori);
        sb_transform = mat4::rotation_euler(5.3f, 4.55f, 0.0f);
      }

      // TODO: Do something with state position.
      {
        op.setPosition(cpos);
      }

      // Skybox is not obligatory.
      if(sb)
      {
        // Set skybox colors.
        op.storeFloat('A', 'B', 'C', sb->getColorForward1());
        op.storeFloat('D', 'E', 'F', sb->getColorForward2());
        op.storeFloat('G', 'H', 'I', sb->getColorBackward());

#if defined(USE_LD)
        if(is_developer() && !g_camera_lock)
        {
          sb_transform.addTranslation(g_pos);
        }
        else
#endif
        {
          // Would you believe if I said there's artfacts without this line?
          sb_transform.addTranslation(vec3(1.0f));
        }

        for(unsigned ii = 0; (Skybox::OBJECT_COUNT > ii); ++ii)
        {
          const Object& vv = sb->getObject(ii);

          op.addObject(vv, sb_transform, PASS_SKY);
        }
      }
    }

    /// Generate initial state.
    ///
    /// \return State generated.
    State* generateInitialState()
    {
      State *state = acquireEmpty();

      state->setPosition(vec3(STARTING_POS_X, STARTING_POS_Y, STARTING_POS_Z));
      state->setFrameInitial();

      fillState(*state);
 
      finishEmpty(*state);

      return state;
    }

    /// Generates next state.
    ///
    /// \return True if state was generated, false if should exit.
    bool generateNextState()
    {
      State *state = acquireEmpty();
      if(!state)
      {
        return false;
      }

      generateState(*getLastState(), *state);

      finishEmpty(*state);
      return true;
    }

    /// Generate next state from a state.
    ///
    /// \param prev Previous state.
    /// \param next Next state.
    void generateState(const State &prev, State &next)
    {
#if !defined(USE_LD)
      const int g_frame_jump = 1;
#endif
      int prev_frame = prev.getFrame();
      int next_frame = prev_frame + g_frame_jump;

#if defined(USE_LD)
      if(is_developer() && (1 >= std::abs(g_advance)))
      {
        next_frame += g_advance * g_advance_multiplier - 1; // Counter one-frame jump.
      }

      next_frame = std::min(std::max(next_frame, 0), INTRO_LENGTH_FRAMES);

      // Only jump once.
      if(g_frame_jump != 1)
      {
        float fpos = static_cast<float>(next_frame * FRAME_MILLISECONDS) / 1000.0f;

        g_audio_position = reinterpret_cast<uint8_t*>(g_audio_buffer) +
          static_cast<size_t>(static_cast<float>(AUDIO_BYTERATE) * fpos);
        g_frame_jump = 1;
      }
#endif

      next.setFrame(prev, next_frame);

      fillState(next);
    }

    /// Accessor.
    ///
    /// \return Global data.
    GlobalContainer& getGlobals()
    {
      return m_globals;
    }

    /// \brief Is calculation done?
    ///
    /// \return True if yes, false if no.
    bool isDone() const
    {
      return m_done;
    }

  private:
    /// Get transformation for moelli.
    ///
    /// Data is as follows:
    /// 0: timestamp.
    /// 1: Yaw rotation.
    /// 2: Pitch rotation.
    /// 3: Roll rotation.
    ///
    /// 0 at timestamp terminates.
    ///
    /// All rotations are expressed in 8.8 signed fixed point radians. Initial rotation is 0 for everything.
    ///
    /// \param data Rotation data.
    /// \param fscene Frame in scene.
    static mat4 get_moelli_transform(const int16_t *data, float fscene)
    {
      vec3 rot(0.0f, 0.0f, 0.0f);
      float last_stamp = 0.0f;

      for(const int16_t *iter = data; (*iter); iter += 4)
      {
        float fstamp = static_cast<float>(iter[0]);
        vec3 next(fixed_8_8_to_float(iter[1]),
            fixed_8_8_to_float(iter[2]),
            fixed_8_8_to_float(iter[3]));

        if(fstamp > fscene)
        {
          float interp = linear_step(last_stamp, fstamp, fscene);

          interp = fnorm_weigh_away(interp);

          rot = mix(rot, next, interp);
          break;
        }

        rot = next;
        last_stamp = fstamp;
      }

      return mat4::rotation_euler(rot[0], rot[1], rot[2], vec3(0.0f, 60.0f, 0.0f));
    }

    /// Generator thread function. 
    ///
    /// \param user_data Parameters to thread.
    /// \return Trhead exit code.
    static int precalc_function_audio(void *user_data)
    {
#if defined(USE_LD)
      uint32_t precalc_start = dnload_SDL_GetTicks();
#endif

      dnload_memset(g_audio_buffer, 0, AUDIO_BUFFER_SIZE);
      if(!is_developer())
      {
        generate_audio(g_audio_buffer, AUDIO_BUFFER_SIZE);
      }

#if defined(USE_LD)
      uint32_t precalc_end = dnload_SDL_GetTicks();
      std::cout << "|precalc(audio): " << (static_cast<float>(precalc_end - precalc_start) * .001f) <<
        std::endl;
#endif

      (void)user_data; // Not needed for anything.
      return 0;
    }

    /// Generator thread function. 
    ///
    /// \param user_data Parameters to thread.
    /// \return Trhead exit code.
    static int precalc_function_visuals(void *user_data)
    {
      GlobalState *gstate = static_cast<GlobalState*>(user_data);

      gstate->getGlobals().precalculateVisuals();

      return 0;
    }

  public:
    /// Generator thread function. 
    ///
    /// \param user_data Parameters to thread.
    /// \return Trhead exit code.
    static int precalc_function(void *user_data)
    {
      GlobalState *globals = static_cast<GlobalState*>(user_data);

      {
        Thread thread_audio(precalc_function_audio, globals);
        Thread thread_visuals(precalc_function_visuals, globals);
      }

      globals->setDone();

      return 0;
    }

    /// \brief State generator thread function.
    ///
    /// \param data Data (actually state queue.
    /// \return Return value (always 0).
    static int state_function(void *data)
    {
      GlobalState *gstate = static_cast<GlobalState*>(data);
      unsigned frames_generated = 0;

      for(;;)
      {
        if(!gstate->generateNextState())
        {
          break;
        }

#if defined(USE_LD)
        if(!is_developer())
#endif
        {
          if(INTRO_LENGTH_FRAMES < ++frames_generated)
          {
            gstate->terminate();
          }
        }
      }

      return 0;
    }
};

//######################################
// Draw process ########################
//######################################

/// Draw a quad.
///
/// Program must already be in use.
///
/// By default, draws a fullscreen unit quad.
///
/// \param program Program to use.
/// \param x1 First X coordinate.
/// \param y1 First Y coordinate.
/// \param x2 Second X coordinate.
/// \param y2 Second Y coordinate.
static void draw_quad(const Program &prg, float x1 = -1.0f, float y1 = -1.0f, float x2 = 1.0f, float y2 = 1.0f)
{
  static const uint8_t TEXCOORDS[] =
  {
    1, 0,
    1, 1,
    0, 0,
    0, 1
  };
  float quad[] =
  {
    x2, y1,
    x2, y2,
    x1, y1,
    x1, y2
  };

  GeometryBuffer::unbind();
  Program::reset_array_buffer();

  vgl::disable_excess_attrib_arrays(2);
  prg.attribPointer('P', 2, GL_FLOAT, false, 0, quad);
  prg.attribPointer('T', 2, GL_UNSIGNED_BYTE, false, 0, TEXCOORDS);
  dnload_glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/// Draw the world.
///
/// Float storage used by world drawing:
/// 'A', 'B', 'C': Skybox first additive color.
/// 'D', 'E', 'F': Skybox second additive color.
/// 'G', 'H', 'I': Skybox substractive color.
/// 'K': Lighting intensity.
/// 'L', 'M', 'N': Point light position.
/// 'O', 'P', 'Q': Point light parameters.
/// 'R', 'S', 'T': Haamu trail direction (and length).
/// 'V': Texture creep offset.
/// 'X', 'Y': Haamu opacity and intensity.
/// 'Z': Text intensity.
///
/// Bool storage used by world drawing:
/// 'B': Is a darken quad blit over the screen if stencils are used?
/// 'G': Haamu render mode, true for geometry, false for billboard.
/// 'S': Shadow drawing mode, true for shadow maps, false for stencils.
///
/// \param globals Global storage.
/// \param state State to render.
static void draw(const GlobalContainer &globals, State &state)
{
  unsigned scrw = globals.screen_width;
  unsigned scrh = globals.screen_height;
  float fscrh = 2.0f / static_cast<float>(scrh);
  float fscrw = static_cast<float>(scrw) / static_cast<float>(scrh) * fscrh;
  float lighting = state.retrieveFloat('K');
  float creep = state.retrieveFloat('V');

  // Normal mode - shadow maps.
  if(state.retrieveBool('S'))
  {
    const vec3 &light = state.getLightDirection();

    // Shadow map pass.
    {
      const Program &prg = globals.program_shadow_map;

      globals.fbo_shadow_map.bind();

      vgl::blend_mode(vgl::DISABLED);
      vgl::cull_face(GL_BACK);
      vgl::depth_test(GL_LESS);
      vgl::depth_write(true);

#if defined(RENDER_ENABLE_DEPTH_TEXTURE)
      vgl::color_write(false);
      vgl::clear_buffers(GL_DEPTH_BUFFER_BIT);
#else
      vgl::color_write(true);
      vgl::color_clear(0xFFFFFFFF);
      vgl::clear_buffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif

      prg.use();
      //prg.uniform('I', 0);
      //prg.uniform('K', vec2(0.4f, 0.6f));
      //prg.uniform('L', light);
      state.drawGeometry(prg);
    }

    // Screen space pass.
    {
      const Program &prg = globals.program_geometry_shadow;

      FrameBuffer::bind_default_frame_buffer(scrw, scrh);

#if defined(RENDER_ENABLE_DEPTH_TEXTURE)
      globals.fbo_shadow_map.getDepthTexture().bind(1);
#else
      globals.fbo_shadow_map.getColorTexture().bind(1);
#endif
      globals.texture_screenspace_mild.bind(0);

      vgl::color_write(true);
      vgl::clear_buffers(GL_DEPTH_BUFFER_BIT);

      prg.use();
      prg.uniform('I', 0);
      prg.uniform('H', 1);
      prg.uniform('K', vec3(0.67f, 0.33f, 0.11f) * lighting);
      prg.uniform('L', light);
      prg.uniform('U', vec2(fscrw, fscrh));
      prg.uniform('V', creep);
      state.drawGeometry(prg);
    }
  }
  // Hard mode - stencil shadows.
  else
  {
    vec3 light = state.retrieveFloat('L', 'M', 'N');
    vec3 light_intensity = state.retrieveFloat('O', 'P', 'Q');

    // Normal geometry pass.
    {
      const Program &prg = globals.program_geometry_plain;

      globals.texture_screenspace_creepy.bind(0);

      vgl::blend_mode(vgl::DISABLED);
      vgl::color_write(true);
      vgl::cull_face(GL_BACK);
      vgl::depth_test(GL_LESS);
      vgl::depth_write(true);

      dnload_glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      prg.use();
      prg.uniform('I', 0);
      prg.uniform('K', light_intensity * lighting);
      prg.uniform('L', light);
      prg.uniform('U', vec2(fscrw, fscrh));
      prg.uniform('V', creep);
      state.drawGeometry(prg);

      // Draw haamu normal form.
      if(state.hasPass(GlobalState::PASS_HAAMU_NORMAL))
      {
#if defined(HAAMU_SHAPE_SIMPLE)
        const Program &hprg = globals.program_geometry_plain;
#else
        const Program &hprg = globals.program_geometry_plain_animated;
#endif

        globals.texture_screenspace_creepy.bind(0);

        hprg.use();
        hprg.uniform('I', 0);
        hprg.uniform('K', light_intensity * lighting);
        hprg.uniform('L', light);
        hprg.uniform('U', vec2(fscrw, fscrh));
        hprg.uniform('V', creep);
        state.drawGeometry(hprg, GlobalState::PASS_HAAMU_NORMAL);
      }
    }

    // Might not need to extrude stencils.
    if(state.retrieveBool('B'))
    {
#if defined(USE_LD)
      if(state.retrieveBool('S'))
      {
        BOOST_THROW_EXCEPTION(std::runtime_error("extrusion and shadow maps both specified"));
      }
#endif
      {
        // Stencil extrusion pass.
        const Program &prg = globals.program_shadow_extrude;

#if 0
        vgl::blend_mode(vgl::ALPHA);
        vgl::color_write(true);
        vgl::cull_face(GL_FALSE);
        vgl::depth_test(GL_FALSE);
        vgl::depth_write(false);
#else
        vgl::color_write(false);
        vgl::cull_face(GL_FALSE);
        vgl::depth_write(false);
        vgl::stencil_mode(GL_ALWAYS);
        vgl::stencil_operation(vgl::CARMACK);
#endif

        prg.use();
        prg.uniform('L', light);
        prg.uniform('S', state.getScreenTransform());
        state.drawShadowEdges(prg);

        // Horrible Z-fighting if no offset for caps.
        vgl::polygon_offset(1);
        state.drawShadowCaps(prg);

        // Revert immediately.
        vgl::polygon_offset(0); 
        vgl::cull_face(GL_BACK);
      }

      // Darkening blit pass.
      {
        const Program &prg = globals.program_darken;

        vgl::blend_mode(vgl::PREMULTIPLIED);
        vgl::color_write(true);
        vgl::cull_face(GL_BACK);
        vgl::depth_test(GL_FALSE);
        vgl::stencil_mode(GL_EQUAL);
        vgl::stencil_operation(vgl::DISABLED);

        prg.use();
        prg.uniform('K', 0.0f, 0.0f, 0.0f, 0.71f);
        draw_quad(prg, -1.0f, -1.0f, 1.0f, 1.0f);
      }

      // Revert.
      vgl::stencil_mode(GL_FALSE);
    }
  }

  // Skybox behind other elements.
  if(state.hasPass(GlobalState::PASS_SKY))
  {
    const Program &prg = globals.program_sky;

    vgl::blend_mode(vgl::DISABLED);
    vgl::depth_test(GL_LESS);
    vgl::depth_write(false);

    prg.use();
    prg.uniform('I', 0);
    prg.uniform('L', state.retrieveFloat('A', 'B', 'C') * lighting);
    prg.uniform('J', state.retrieveFloat('D', 'E', 'F') * lighting);
    prg.uniform('K', state.retrieveFloat('G', 'H', 'I') * lighting);
    state.drawGeometry(prg, GlobalState::PASS_SKY);
  }

  // Draw haamu shady form.
  if(state.hasPass(GlobalState::PASS_HAAMU_SHADY))
  {
    const Program &prg = globals.program_haamu_shape;

    vgl::blend_mode(vgl::PREMULTIPLIED);
    vgl::depth_test(GL_LESS);
    vgl::depth_write(false);
    vgl::cull_face(GL_FALSE);

    prg.use();
    prg.uniform('K', state.retrieveFloat('X', 'Y'));
    prg.uniform('V', state.retrieveFloat('R', 'S', 'T'));
    state.drawGeometry(prg, GlobalState::PASS_HAAMU_SHADY);
  }

  // Draw haamu sprite form.
  if(state.hasPass(GlobalState::PASS_HAAMU_SPRITE))
  {
    const Program &prg = globals.program_haamu_sprite;
    
    vgl::blend_mode(vgl::PREMULTIPLIED);
    vgl::depth_test(GL_LESS);
    vgl::depth_write(false);

    prg.use();
    prg.uniform('I', 0);
    state.drawGeometry(prg, GlobalState::PASS_HAAMU_SPRITE);
  }

  // Draw text on top of everything if it's there.
  if(state.hasPass(GlobalState::PASS_TEXT))
  {
    const Program &prg = globals.program_text;

    vgl::blend_mode(vgl::PREMULTIPLIED);
    vgl::cull_face(GL_BACK);
    vgl::depth_test(GL_FALSE);
    vgl::depth_write(false);

    prg.use();
    prg.uniform('I', 0);
    prg.uniform('K', vec3(state.retrieveFloat('Z')));
    state.drawGeometry(prg, GlobalState::PASS_TEXT);
  }

#if defined(USE_LD)
  // Debug pass.
  if(is_developer() && g_shadow_debug)
  {
    const Program &prg = globals.program_blit;

#if defined(RENDER_ENABLE_DEPTH_TEXTURE)
    globals.fbo_shadow_map.getDepthTexture().bind(1);
#else
    globals.fbo_shadow_map.getColorTexture().bind(1);
#endif

    vgl::depth_test(GL_FALSE);

    prg.use();
    prg.uniform('I', 1);
    draw_quad(prg, 0.5f, 0.5f, 1.0f, 1.0f);
  }
#endif

#if defined(USE_LD)
  vgl::error_check();
#endif
}

//######################################
// Utility #############################
//######################################

#if defined(USE_LD)

/// Parse resolution from string input.
///
/// \param op Resolution string.
/// \return Tuple of width and height.
boost::tuple<unsigned, unsigned> parse_resolution(const std::string &op)
{
  size_t cx = op.find("x");
  
  if(std::string::npos == cx)
  {
    cx = op.rfind("p");

    if((std::string::npos == cx) || (0 >= cx))
    {
      std::ostringstream sstr;
      sstr << "invalid resolution string '" << op << '\'';
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    std::string sh = op.substr(0, cx);

    unsigned rh = boost::lexical_cast<unsigned>(sh);
    unsigned rw = (rh * 16) / 9;
    unsigned rem4 = rw % 4;

    return boost::make_tuple(rw - rem4, rh);
  }

  std::string sw = op.substr(0, cx);
  std::string sh = op.substr(cx + 1);

  return boost::make_tuple(boost::lexical_cast<int>(sw), boost::lexical_cast<int>(sh));
}

/// \brief Audio writing callback.
///
/// \param data Raw audio data.
/// \param size Audio data size (in samples).
void write_audio(void *data, unsigned size)
{
  std::string fname = std::string(g_intro_name) + std::string(".raw");
  FILE *fd = fopen(fname.c_str(), "wb");

  if(fd != NULL)
  {
    fwrite(data, size, 1, fd);
  }

  fclose(fd);

  if(is_verbose())
  {
    std::cout << "wrote audio: '" << fname << "'\n";
  }
}

/// \brief Image writing callback.
///
/// \param screen_w Screen width.
/// \param screen_h Screen height.
/// \param idx Frame index to write.
void write_frame(unsigned screen_w, unsigned screen_h, unsigned idx)
{
#if defined(DNLOAD_GLESV2)
  unsigned export_bpp = 4;
  GLenum export_format = GL_RGBA;
#else
  unsigned export_bpp = 3;
  GLenum export_format = GL_RGB;
#endif
  boost::scoped_array<uint8_t> image(new uint8_t[screen_w * screen_h * export_bpp]);
  std::ostringstream sstr;

  glReadPixels(0, 0, static_cast<GLsizei>(screen_w), static_cast<GLsizei>(screen_h), export_format,
      GL_UNSIGNED_BYTE, image.get());

  sstr << g_intro_name << "_" << std::setfill('0') << std::setw(4) << idx << ".png";

  gfx::image_png_save(sstr.str(), screen_w, screen_h, export_bpp * 8, image.get());

  if(is_verbose())
  {
    std::cout << "wrote frame: '" << sstr.str() << "'\n";
  }
}

/// Record the intro to disk.
///
/// \param queue State queue.
/// \param globals Global storage.
static void perform_record(GlobalState &gstate)
{
  const GlobalContainer &globals = gstate.getGlobals();

  // audio
  write_audio(g_audio_buffer, AUDIO_BUFFER_SIZE);

  // video
  for(unsigned frame_idx = 0; (INTRO_LENGTH_FRAMES > frame_idx); ++frame_idx)
  {
    SDL_Event event;
    if(SDL_PollEvent(&event) && (event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))
    {
      break;
    }

    State *state = gstate.acquireReady();
    if(!state)
    {
      break;
    }

    draw(globals, *state);
    gstate.finishReady();
    gstate.generateNextState();

    write_frame(globals.screen_width, globals.screen_height, frame_idx);
    swap_buffers();
  }
}

/// Update window position.
///
/// May be NOP depending on platform.
void update_window_position()
{
  static int window_x = INT_MIN;
  static int window_y = INT_MIN;
  static int window_width = INT_MIN;
  static int window_height = INT_MIN;
  int current_window_x;
  int current_window_y;
  int current_window_width;
  int current_window_height;
  
  SDL_GetWindowPosition(g_sdl_window, &current_window_x, &current_window_y);
  SDL_GetWindowSize(g_sdl_window, &current_window_width, &current_window_height);
  if((current_window_x != window_x) || (current_window_y != window_y) ||
      (current_window_width != window_width) || (current_window_height != window_height))
  {
    window_x = current_window_x;
    window_y = current_window_y;
    window_width = current_window_width;
    window_height = current_window_height;
#if defined(DNLOAD_VIDEOCORE)
    videocore_move_native_window(window_x, window_y, window_width, window_height);
#endif
  }
}

/// Set frame jump.
///
/// \param seconds Seconds to jump.
void set_frame_jump(float seconds)
{
  g_frame_jump = static_cast<int>(seconds * (1000.0f / static_cast<float>(FRAME_MILLISECONDS)) + 0.5f);
}

#endif

//######################################
// _start ##############################
//######################################

/// \cond
#if defined(DNLOAD_VIDEOCORE)
#define DEFAULT_SDL_WINDOW_FLAGS SDL_WINDOW_BORDERLESS
#else
#define DEFAULT_SDL_WINDOW_FLAGS SDL_WINDOW_OPENGL
#endif
/// \endcond

/// Intro execution.
///
/// \param screen_w Screen width.
/// \param screen_h Screen height.
/// \param flag_fullscreen Fullscreen on/off.
/// \param flag_record Record mode on/off.
void intro(unsigned screen_w, unsigned screen_h, bool flag_fullscreen, bool flag_record)
{
  dnload();
  dnload_SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
#if !defined(DNLOAD_VIDEOCORE)
  dnload_SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  dnload_SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#if defined(DNLOAD_GLESV2)
  dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  dnload_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
#endif
  g_sdl_window = dnload_SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      static_cast<int>(screen_w), static_cast<int>(screen_h),
      DEFAULT_SDL_WINDOW_FLAGS | (flag_fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
#if defined(DNLOAD_GLESV2) && defined(DNLOAD_VIDEOCORE)
#if defined(USE_LD)
  if(!flag_fullscreen)
  {
    videocore_create_native_window_extended(screen_w, screen_h, MODE_ORIGO);
  }
  else
#endif
  {
    videocore_create_native_window(screen_w, screen_h);
  }
  bool egl_result = egl_init(reinterpret_cast<NativeWindowType>(&g_egl_native_window), &g_egl_display,
      &g_egl_surface);
#if defined(USE_LD)
  if(!egl_result)
  {
    teardown();
    exit(1);
  }
#else
  (void)egl_result;
#endif
#else
  dnload_SDL_GL_CreateContext(g_sdl_window);
#endif
  dnload_SDL_ShowCursor(is_developer());

#if defined(USE_LD)
#if !defined(DNLOAD_GLESV2)
  {
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
      std::cerr  << "glewInit(): " << glewGetErrorString(err) << std::endl;
      teardown();
      exit(1);
    }
  }
#endif
  if(!flag_fullscreen)
  {
    update_window_position();
  }
#else
  (void)flag_record;
#endif

  // Set GL states that only need to be set once.
  dnload_glStencilMask(0xFFFFFFFFU);
  vgl::color_clear(0x00000000U);

#if defined(USE_LD)
  uint32_t precalc_start = dnload_SDL_GetTicks();
#endif

#if 0 // In case of stack overflow.
  uptr<GlobalState> global_state(new GlobalState(screen_w, screen_h));
  GlobalState &gstate = *global_state;
#else
  GlobalState gstate(screen_w, screen_h);
#endif

#if defined(USE_LD)
  uint32_t precalc_init = dnload_SDL_GetTicks();
  std::cout << get_opengl_info() << "\n|precalc(initial): " <<
    (static_cast<float>(precalc_init - precalc_start) * .001f) << std::endl;
#endif

  // Parallel loading phase.
  {
    GlobalContainer &globals = gstate.getGlobals();
    Thread precalc_thread(GlobalState::precalc_function, &gstate);
    //const Program *prg = gstate.getProgramLoader();
    //uint32_t start_ticks = dnload_SDL_GetTicks();

    //bind_unit_quad(prg, &gstate);
    dnload_glViewport(0, 0, static_cast<GLsizei>(globals.screen_width),
        static_cast<GLsizei>(globals.screen_height));

    while(!gstate.isDone())
    {
      swap_buffers();
    }

    // Upload to GPU when ready.
    globals.update();
  }
  // Generate first state when generation done.
  gstate.generateInitialState();

#if defined(USE_LD)
  if(flag_record)
  {
    perform_record(gstate);
    teardown();
    exit(0);
  }

  if(!is_developer())
#endif
  {
    dnload_SDL_OpenAudio(&audio_spec, NULL);
    dnload_SDL_PauseAudio(0);
  }

  // Scope will ensure destruction of threading.
  {
    Thread state_thread(GlobalState::state_function, &gstate);
    uint32_t prev_ticks = dnload_SDL_GetTicks();
#if defined(USE_LD)
    unsigned successful_frames = 0;
    float move_speed = 0.1f;
    uint8_t mouse_look = 0;
    int8_t move_backward = 0;
    int8_t move_down = 0;
    int8_t move_forward = 0;
    int8_t move_left = 0;
    int8_t move_right = 0;
    int8_t move_up = 0;
    bool camera_lock_toggle = true;
#endif

    for(;;)
    {
#if defined(USE_LD)
      int mouse_look_x = 0;
      int mouse_look_y = 0;
#endif
      SDL_Event event;

#if defined(USE_LD)
      while(SDL_PollEvent(&event))
      {
        if(SDL_QUIT == event.type)
        {
          gstate.terminate();
        }
        else if(SDL_KEYDOWN == event.type)
        {
          switch(event.key.keysym.sym)
          {
            case SDLK_F1:
              g_shadow_debug = !g_shadow_debug;
              break;

            case SDLK_F2:
              g_shadow_mode_stencil = !g_shadow_mode_stencil;
              break;

            case SDLK_1:
              g_force_scene = OPENING;
              break;

            case SDLK_2:
              g_force_scene = MAZE;
              break;

            case SDLK_3:
              g_force_scene = AQUEDUCT;
              break;

            case SDLK_4:
              g_force_scene = GREETS;
              break;

            case SDLK_5:
              g_force_scene = COLISEUM_ENTRY_FIRST;
              break;

            case SDLK_6:
              g_force_scene = COLISEUM_ENTRY_SECOND;
              break;

            case SDLK_7:
              g_force_scene = COLISEUM;
              break;

            case SDLK_8:
              g_force_scene = HELLRAISER;
              break;

            case SDLK_0:
              g_force_scene = NONE;
              break;

            case SDLK_a:
              move_left = 1;
              break;

            case SDLK_d:
              move_right = 1;
              break;

            case SDLK_e:
              move_up = 1;
              break;

            case SDLK_q:
              move_down = 1;
              break;

            case SDLK_s:
              move_backward = 1;
              break;

            case SDLK_w:
              move_forward = 1;
              break;

            case SDLK_l:
              camera_lock_toggle = !camera_lock_toggle;
              break;

            case SDLK_p:
              g_advance = 2;
              g_camera_lock = true;
              break;

            case SDLK_LEFT:
              set_frame_jump(-5.0f);
              break;

            case SDLK_RIGHT:
              set_frame_jump(5.0f);
              break;

            case SDLK_UP:
              set_frame_jump(30.0f);
              break;

            case SDLK_DOWN:
              set_frame_jump(-30.0f);
              break;

            case SDLK_SPACE:
              g_print_position = true;
              break;

            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
              move_speed = 2.0f;
              g_advance_multiplier = 16;
              break;            

            case SDLK_LALT:
              g_advance = -1;
              g_camera_lock = camera_lock_toggle;
              break;

            case SDLK_MODE:
            case SDLK_RALT:
            case SDLK_RALT_EXTRA:
              g_advance = 1;
              g_camera_lock = camera_lock_toggle;
              break;

            case SDLK_ESCAPE:
              gstate.terminate();
              break;

            default:
              //std::cout << event.key.keysym.sym << std::endl;
              break;
          }
        }
        else if(SDL_KEYUP == event.type)
        {
          switch(event.key.keysym.sym)
          {
            case SDLK_a:
              move_left = 0;
              break;

            case SDLK_d:
              move_right = 0;
              break;

            case SDLK_e:
              move_up = 0;
              break;

            case SDLK_q:
              move_down = 0;
              break;

            case SDLK_s:
              move_backward = 0;
              break;

            case SDLK_w:
              move_forward = 0;
              break;

            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
              move_speed = 1.0f / 10.0f;
              g_advance_multiplier = 1;
              break;            

            case SDLK_MODE:
            case SDLK_LALT:
            case SDLK_RALT:
            case SDLK_RALT_EXTRA:
              g_advance = 0;
              break;

            default:
              break;
          }
        }
        else if(SDL_MOUSEBUTTONDOWN == event.type)
        {
          if(1 == event.button.button)
          {
            mouse_look = 1;
          }
        }
        else if(SDL_MOUSEBUTTONUP == event.type)
        {
          if(1 == event.button.button)
          {
            mouse_look = 0;
          }
        }
        else if(SDL_MOUSEMOTION == event.type)
        {
          if(0 != mouse_look)
          {
            mouse_look_x += event.motion.xrel;
            mouse_look_y += event.motion.yrel;
          }
        }
        else if(SDL_WINDOWEVENT == event.type)
        {
          if(!flag_fullscreen)
          {
            update_window_position();
          }
        }
      }

      if(is_developer())
      {
        float rt_x = cosf(g_rot_yaw);
        float rt_z = -sinf(g_rot_yaw);
        float fw_x = rt_z;
        float fw_z = -rt_x;

        if(0 != mouse_look_x)
        {
          float angle = static_cast<float>(mouse_look_x) / static_cast<float>(screen_h / 4) * 0.25f;

          g_rot_yaw += angle;
          g_camera_lock = false;
        }
        if(0 != mouse_look_y)
        {
          float angle = static_cast<float>(mouse_look_y) / static_cast<float>(screen_h / 4) * 0.25f;

          g_rot_pitch += angle;
          g_camera_lock = false;
        }

        int movement_x = move_right - move_left;
        int movement_y = move_up - move_down;
        int movement_z = move_forward - move_backward;
        if((0 != movement_x) || (0 != movement_y) || (0 != movement_z))
        {
          float movement_rt = static_cast<float>(movement_x) * move_speed;
          float movement_up = static_cast<float>(movement_y) * move_speed;
          float movement_fw = static_cast<float>(movement_z) * move_speed;
          g_pos += vec3(movement_rt * rt_x + movement_fw * fw_x, movement_up,
              movement_rt * rt_z + movement_fw * fw_z);
          g_camera_lock = false;
        }
      }
#else
      dnload_SDL_PollEvent(&event);
      if(event.type == SDL_KEYDOWN)
      {
        break;
      }
#endif

      State *state = gstate.acquireReady();
      if(!state)
      {
        break;
      }

      // First check, discard frame if we're too far (2 frames) behind.
      {
        unsigned tick_diff = dnload_SDL_GetTicks() - prev_ticks;
        if(FRAME_MILLISECONDS * 3 < tick_diff)
        {
          prev_ticks += FRAME_MILLISECONDS;
#if defined(USE_LD)
          std::cout << "frameskip(" << successful_frames << "): " << tick_diff - FRAME_MILLISECONDS * 3 <<
            std::endl;
          successful_frames = 0;
#endif
          continue;
        }
#if defined(USE_LD)
        else
        {
          ++successful_frames;
        }
#endif
      }

      draw(gstate.getGlobals(), *state);
      gstate.finishReady();
      swap_buffers();

      // Second check, wait until we can proceed (less than one frame left).
      for(;;)
      {
        unsigned tick_diff = dnload_SDL_GetTicks() - prev_ticks;
        if(FRAME_MILLISECONDS <= tick_diff)
        {
          break;
        }
        dnload_SDL_Delay(0);
      }
      prev_ticks += FRAME_MILLISECONDS;
    }

    gstate.terminate();
  }

  teardown();
}

//######################################
// Main ################################
//######################################

#if defined(USE_LD)
/// Main function.
///
/// \param argc Argument count.
/// \param argv Arguments.
/// \return Program return code.
int DNLOAD_MAIN(int argc, char **argv)
{
  unsigned screen_w = SCREEN_W;
  unsigned screen_h = SCREEN_H;
  bool fullscreen = true;
  bool record = false;

  try
  {
    if(argc > 0)
    {
      po::options_description desc("Options");
      desc.add_options()
        ("developer,d", "Developer mode.")
        ("help,h", "Print help text.")
        ("record,R", "Do not play intro normally, instead save audio as .wav and frames as .png -files.")
        ("resolution,r", po::value<std::string>(), "Resolution to use, specify as 'WIDTHxHEIGHT' or 'HEIGHTp'.")
        ("verbose,v", "Display extra debug info.")
        ("window,w", "Start in window instead of full-screen.");

      po::variables_map vmap;
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vmap);
      po::notify(vmap);

      if(vmap.count("developer"))
      {
        set_developer(true);
      }
      if(vmap.count("help"))
      {
        std::cout << g_usage << desc << std::endl;
        return 0;
      }
      if(vmap.count("record"))
      {
        record = true;
      }
      if(vmap.count("resolution"))
      {
        boost::tie(screen_w, screen_h) = parse_resolution(vmap["resolution"].as<std::string>());
      }
      if(vmap.count("verbose"))
      {
        set_verbose(true);
      }
      if(vmap.count("window"))
      {
        fullscreen = false;
      }
    }

    intro(screen_w, screen_h, fullscreen, record);
  }
  catch(const boost::exception &err)
  {
    std::cerr << boost::diagnostic_information(err);
    return 1;
  }
  catch(const std::exception &err)
  {
    std::cerr << err.what() << std::endl;
    return 1;
  }
  catch(...)
  {
    std::cerr << __FILE__ << ": unknown exception caught\n";
    return -1;
  }
  return 0;
}
#else
void _start()
{
  intro(SCREEN_W, SCREEN_H, static_cast<bool>(SCREEN_F), false);
  asm_exit();
}
#endif

//######################################
// End #################################
//######################################

