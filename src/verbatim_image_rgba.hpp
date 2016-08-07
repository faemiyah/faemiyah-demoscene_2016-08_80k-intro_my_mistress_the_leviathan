#ifndef VERBATIM_IMAGE_RGBA_HPP
#define VERBATIM_IMAGE_RGBA_HPP

#include "verbatim_image.hpp"
#include "verbatim_uptr.hpp"

/// RGB image class.
class ImageRGBA : public Image
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    ImageRGBA(unsigned width, unsigned height) :
      Image(width, height, 4) { }

  public:
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pr Red component.
    /// \param pg Green component.
    /// \param pb Blue component.
    /// \param pa Alpha component.
    void setPixel(unsigned px, unsigned py, float pr, float pg, float pb, float pa)
    {
      setValue(px, py, 0, pr);
      setValue(px, py, 1, pg);
      setValue(px, py, 2, pb);
      setValue(px, py, 3, pa);
    }

    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param col Color.
    /// \param pa Alpha component.
    void setPixel(unsigned px, unsigned py, const vec3 &col, float pa)
    {
      setPixel(px, py, col[0], col[1], col[2], pa);
    }
};

/// Convenience typedef.
typedef uptr<ImageRGBA> ImageRGBAUptr;

#endif
