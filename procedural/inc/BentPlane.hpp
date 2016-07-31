/**
 * @module  BentPlane
 * @brief	BentPlane procedural generation, generates a plane bent around the z-axis
 *
 * @author	Roberto Cano (http://www.robertocano.es)
 */
#pragma once

#include <stdint.h>
#include <glm/glm.hpp>
#include "Asset3D.hpp"

namespace Procedural
{
/**
 * BentPlane class to generate axis aligned planes
 */
class BentPlane : public Asset3D
{
  public:
      /**
       * Constructor
       *
       * @param width  Width of the plane along the x-axis
       * @param height Height of the plane along the z-axis
       * @param color  Default color for the plane material
       * @param angle  Bending angle for the plane. If 0.0f then a degenerate plane is,
       *               otherwise the plane is bent around the z-axis preserving its width on an
       *               arc indicated by 'angle'
       * @param numVertsWidth  Number of vertices to generate along the x-axis
       * @param numVertsHeight Number of vertices to generate along the y-axis
       */
    BentPlane(float width = 1.0f, float height = 1.0f,
          const glm::vec3 &color = glm::vec3(1.0f, 1.0f, 1.0f),
          float angle = 1.0f, uint32_t numVertsWidth = 2, uint32_t numVertsHeight = 2);

    /**
     * Accessors
     */
    float getWidth() const { return _width; }
    float getHeight() const { return _height; }
    const glm::vec3 &getColor() const { return _color; }
    uint32_t getNumVertsWidth() const { return _numVertsWidth; }
    uint32_t getNumVertsHeight() const { return _numVertsHeight; }

  private:
    float _width;
    float _height;
    glm::vec3 _color;
    float _angle;
    uint32_t _numVertsWidth;
    uint32_t _numVertsHeight;
};
};
