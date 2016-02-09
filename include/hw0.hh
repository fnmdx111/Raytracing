//
// Created by wo on 1/30/16.
//

#ifndef __H_HW0__
#define __H_HW0__

#include <iostream>
#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace std;

void
write_rgba(const string& filename,
           const Imf::Rgba* pixels,
           int width, int height)
{
  Imf::RgbaOutputFile file(filename.c_str(), width, height, Imf::WRITE_RGBA);
  file.setFrameBuffer(pixels, 1, width);
  file.writePixels(height);
}

void
read_rgba(const string& filename,
          Imf::Array2D<Imf::Rgba>& pixels,
          int& width, int& height)
{
  Imf::RgbaInputFile file(filename.c_str());
  Imath::Box2i dw = file.dataWindow();

  width = dw.max.x - dw.min.x + 1;
  height = dw.max.y - dw.min.y + 1;
  pixels.resizeErase(height, width);

  file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width,
		      1,
		      width);
  file.readPixels(dw.min.y, dw.max.y);
}

#endif
