#ifndef VERBATIM_IMAGE_LA_HPP
#define VERBATIM_IMAGE_LA_HPP

#include "verbatim_image.hpp"
#include "verbatim_uptr.hpp"

/// RGB image class.
class ImageLA : public Image
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    ImageLA(unsigned width, unsigned height) :
      Image(width, height, 2) { }

  public:
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param luminance Luminance component.
    /// \param alpha Alpha component.
    void setPixel(unsigned px, unsigned py, float luminance, float alpha)
    {
      setValue(px, py, 0, luminance);
      setValue(px, py, 1, alpha);
    }
};

/// Convenience typedef.
typedef uptr<ImageLA> ImageLAUptr;

#endif
