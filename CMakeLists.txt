cmake_minimum_required(VERSION 2.8)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
project(my_mistress_the_leviathan)

enable_language(C)
enable_language(CXX)

include("defaults.cmake")

find_boost_program_options(TRUE)
find_freetype(TRUE STATIC)
find_png(TRUE)
find_sdl2(TRUE)

check_mali()
check_videocore()
if(MALI_FOUND)
  add_definitions(-DDNLOAD_MALI -DDNLOAD_GLESV2)
  set(EGL_LIBRARY "EGL")
  set(OPENGL_gl_LIBRARY "GLESv2")
elseif(VIDEOCORE_FOUND)
  add_definitions(-DDNLOAD_VIDEOCORE -DDNLOAD_GLESV2)
  set(BCM_HOST_LIBRARY "bcm_host")
  set(EGL_LIBRARY "EGL")
  set(OPENGL_gl_LIBRARY "GLESv2")
else()
  find_opengl(TRUE)
  find_glew(TRUE)
endif()

output_flags("DEBUG" on)

add_definitions(-DUSE_LD)

include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/src/synth")

add_executable(my_mistress_the_leviathan
  "src/bsd_rand.c"
  "src/bsd_rand.h"
  "src/glsl_shader_source.cpp"
  "src/glsl_shader_source.hpp"
  "src/image_png.cpp"
  "src/image_png.hpp"
  "src/intro.cpp"
  "src/intro_aqueduct.hpp"
  "src/intro_csg.hpp"
  "src/intro_direction.hpp"
  "src/intro_floating_island.hpp"
  "src/intro_haamu.hpp"
  "src/intro_maze.hpp"
  "src/intro_mazeresources.hpp"
  "src/intro_mazeprimitives.hpp"
  "src/intro_moelli.hpp"
  "src/intro_skybox.hpp"
  "src/intro_sprite.hpp"
  "src/model_body.hpp"
  "src/verbatim_animation.hpp"
  "src/verbatim_animation_frame.hpp"
  "src/verbatim_animation_state.hpp"
  "src/verbatim_armature.hpp"
  "src/verbatim_bone.hpp"
  "src/verbatim_bone_state.hpp"
  "src/verbatim_bounding_volume.hpp"
  "src/verbatim_buffer.hpp"
  "src/verbatim_character.hpp"
  "src/verbatim_compiled_mesh.hpp"
  "src/verbatim_edge.hpp"
  "src/verbatim_edge_buffer.hpp"
  "src/verbatim_edge_vertex.hpp"
  "src/verbatim_element.hpp"
  "src/verbatim_font.hpp"
  "src/verbatim_frame_buffer.hpp"
  "src/verbatim_geometry_buffer.hpp"
  "src/verbatim_gl.hpp"
  "src/verbatim_image.hpp"
  "src/verbatim_image_gray.hpp"
  "src/verbatim_image_la.hpp"
  "src/verbatim_image_rgb.hpp"
  "src/verbatim_image_rgba.hpp"
  "src/verbatim_index_block.hpp"
  "src/verbatim_index_buffer.hpp"
  "src/verbatim_index_run.hpp"
  "src/verbatim_ivec4.hpp"
  "src/verbatim_logical_edge.hpp"
  "src/verbatim_logical_face.hpp"
  "src/verbatim_logical_mesh.hpp"
  "src/verbatim_logical_vertex.hpp"
  "src/verbatim_mat3.hpp"
  "src/verbatim_mat4.hpp"
  "src/verbatim_mesh.hpp"
  "src/verbatim_object.hpp"
  "src/verbatim_object_database.hpp"
  "src/verbatim_object_group.hpp"
  "src/verbatim_object_reference.hpp"
  "src/verbatim_program.hpp"
  "src/verbatim_quat.hpp"
  "src/verbatim_realloc.hpp"
  "src/verbatim_seq.hpp"
  "src/verbatim_shader.hpp"
  "src/verbatim_spline.hpp"
  "src/verbatim_state.hpp"
  "src/verbatim_state_queue.hpp"
  "src/verbatim_synth.hpp"
  "src/verbatim_texture.hpp"
  "src/verbatim_threading.hpp"
  "src/verbatim_uptr.hpp"
  "src/verbatim_uvec4.hpp"
  "src/verbatim_vec2.hpp"
  "src/verbatim_vec3.hpp"
  "src/verbatim_vertex.hpp"
  "src/verbatim_vertex_buffer.hpp"
  "src/synth/bus_data.hpp"
  "src/synth/compiled_song.hpp"
  "src/synth/compressor.hpp"
  "src/synth/controller.hpp"
  "src/synth/dcfilter.hpp"
  "src/synth/delay.hpp"
  "src/synth/effect_bus.hpp"
  "src/synth/ghostsyn.hpp"
  "src/synth/globalconfig.hpp"
  "src/synth/instrument.hpp"
  "src/synth/instrument_data.hpp"
  "src/synth/rt_controls.hpp"
  "src/synth/tracker_data_static.hpp"
  "src/synth/utils.hpp"
  "src/synth/voice.hpp"
  )
if(MSVC)
  target_link_libraries(my_mistress_the_leviathan "WINMM")
  target_link_libraries(my_mistress_the_leviathan "${OPENGL_gl_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan debug "${GLEW_LIBRARY_DEBUG}" optimized "${GLEW_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan debug "${FREETYPE_LIBRARY_DEBUG}" optimized "${FREETYPE_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan debug "${PNG_LIBRARY_DEBUG}" optimized "${PNG_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan debug "${SDL2_LIBRARY_DEBUG}" optimized "${SDL2_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan debug "${ZLIB_LIBRARY_DEBUG}" optimized "${ZLIB_LIBRARY}")
else()
  if(MALI_FOUND)
    target_link_libraries(my_mistress_the_leviathan "${EGL_LIBRARY}")
    target_link_libraries(my_mistress_the_leviathan "${OPENGL_gl_LIBRARY}")
  elseif(VIDEOCORE_FOUND)
    target_link_libraries(my_mistress_the_leviathan "${BCM_HOST_LIBRARY}")
    target_link_libraries(my_mistress_the_leviathan "${EGL_LIBRARY}")
    target_link_libraries(my_mistress_the_leviathan "${OPENGL_gl_LIBRARY}")
  else()
    target_link_libraries(my_mistress_the_leviathan "${GLEW_LIBRARY}")
  endif()
  target_link_libraries(my_mistress_the_leviathan "${BOOST_PROGRAM_OPTIONS_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan "${FREETYPE_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan "${PNG_LIBRARY}")
  target_link_libraries(my_mistress_the_leviathan "${SDL2_LIBRARY}")
endif()
