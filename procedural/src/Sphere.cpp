#include <GL/glew.h>
#include <GL/glfw.h>
#include <string.h>
#include <glm/glm.hpp>
#include <string>
#include "Sphere.hpp"
#include "Logging.hpp"

using namespace Logging;
using namespace procedural;
using namespace glm;

static uint32_t numIndices = 0;
bool Sphere::init()
{
    /**
     * Subdivision of a cube into more triangles t use it for a sphere creation
     *
     * For a subdivision level of N, the number of vertices for the main body
     * strip are:
     *
     *     - H = 2ˆ(N+2)
     *     - V = 2^N + 1
     *     - TotalVerts = H*V
     *
     *     At level 0 we get H = 4, V = 2
     */

    /* Preallocate vertices and color */
    typedef struct {
        GLfloat x, y, z;
    } vertex;

    typedef struct {
        GLfloat r, g, b;
    } color;

    uint8_t level = 0; /* Beware! max 8 for now! */
    uint32_t H = (1 << level) + 1;
    uint32_t V = (1 << level) + 1;
    uint32_t horizontal = (H - 1) * 4; /* 4 sides */
    uint32_t vertical = V;

    uint32_t top = 2 * H * H - 6 * H + 5; /* Formula for top and bottom using a triangle fan and subdivided
                                             strip. The border is already generated by the sides, it is only
                                             needed to generate the inner vertices */
    uint32_t bottom = top;

    vertex *vertices = new vertex[horizontal * vertical + top + bottom];
    color *colors = new color[horizontal * vertical + top + bottom];

    uint32_t vertNum = 0;

    /* Create the sides of the cube */
    uint32_t i, j;
    for (i = 0; i < vertical; ++i) {
        for (j = 0; j < horizontal; ++j) {
            if (j < H - 1) { /* z=0 */
                vertices[vertNum].x = j / (H - 1.0);
                vertices[vertNum].y = 1.0 - (i / (V - 1.0));
                vertices[vertNum].z = 0.0;
            } else if (j < 2 * (H - 1)) { /* x=1 */
                vertices[vertNum].x = 1.0;
                vertices[vertNum].y = 1.0 - (i / (V - 1.0));
                vertices[vertNum].z = 0.0 - ((j - (H - 1)) / (H - 1.0));
            } else if (j < 3 * (H - 1)) { /* z=-1 */
                vertices[vertNum].x = 1.0 - ((j - 2 * (H - 1)) / (H - 1.0));
                vertices[vertNum].y = 1.0 - (i / (V - 1.0));
                vertices[vertNum].z = -1.0;
            } else if (j < 4 * (H - 1)) { /* x=0.0 */
                vertices[vertNum].x = 0.0;
                vertices[vertNum].y = 1.0 - (i / (V - 1.0));
                vertices[vertNum].z = ((j - 3 * (H - 1)) / (H - 1.0)) - 1.0;
            }
            colors[vertNum].r = 1.0;
            colors[vertNum].g = 1.0;
            colors[vertNum].b = 1.0;

            vertNum++;
        }
    }

    /* Generate bottom */
    uint32_t iter = H - 2;
    for (i = iter; i > 1; i -= 2) { /* Loop through all the inner squares */
        for (j = 0; j < 4 * (i - 1); ++j) {
            uint32_t offset = (iter - i) / 2 + 1;

            if (j < (i - 1)) {
                vertices[vertNum].x = (j + offset) / (H - 1.0);
                vertices[vertNum].y = 0.0;
                vertices[vertNum].z = 0.0 - (offset / (H - 1.0));
            } else if (j < 2 * (i - 1)) {
                vertices[vertNum].x = 1.0 - (offset / (H - 1.0));
                vertices[vertNum].y = 0.0;
                vertices[vertNum].z = 0.0 - (((j - (i - 1)) + offset) / (H - 1.0));
            } else if (j < 3 * (i - 1)) {
                vertices[vertNum].x = 1.0 - ((j - 2 * (i - 1) + offset) / (H - 1.0));
                vertices[vertNum].y = 0.0;
                vertices[vertNum].z = -1.0 + (offset / (H - 1.0));
                ;
            } else if (j < 4 * (i - 1)) {
                vertices[vertNum].x = offset / (H - 1.0);
                vertices[vertNum].y = 0.0;
                vertices[vertNum].z = -1.0 + ((j - 3 * (i - 1) + offset) / (H - 1.0));
            }
            colors[vertNum].r = 1.0;
            colors[vertNum].g = 1.0;
            colors[vertNum].b = 1.0;

            vertNum++;
        }
    }

    /* Add the center */
    vertices[vertNum].x = 0.5;
    vertices[vertNum].y = 0.0;
    vertices[vertNum].z = -0.5;

    colors[vertNum].r = 1.0;
    colors[vertNum].g = 1.0;
    colors[vertNum].b = 1.0;

    vertNum++;

    /* Generate top */
    iter = H - 2;
    for (i = iter; i > 1; i -= 2) { /* Loop through all the inner squares */
        for (j = 0; j < 4 * (i - 1); ++j) {
            uint32_t offset = (iter - i) / 2 + 1;

            if (j < (i - 1)) {
                vertices[vertNum].x = (j + offset) / (H - 1.0);
                vertices[vertNum].y = 1.0;
                vertices[vertNum].z = 0.0 - (offset / (H - 1.0));
            } else if (j < 2 * (i - 1)) {
                vertices[vertNum].x = 1.0 - (offset / (H - 1.0));
                vertices[vertNum].y = 1.0;
                vertices[vertNum].z = 0.0 - (((j - (i - 1)) + offset) / (H - 1.0));
            } else if (j < 3 * (i - 1)) {
                vertices[vertNum].x = 1.0 - ((j - 2 * (i - 1) + offset) / (H - 1.0));
                vertices[vertNum].y = 1.0;
                vertices[vertNum].z = -1.0 + (offset / (H - 1.0));
                ;
            } else if (j < 4 * (i - 1)) {
                vertices[vertNum].x = offset / (H - 1.0);
                vertices[vertNum].y = 1.0;
                vertices[vertNum].z = -1.0 + ((j - 3 * (i - 1) + offset) / (H - 1.0));
            }
            colors[vertNum].r = 1.0;
            colors[vertNum].g = 1.0;
            colors[vertNum].b = 1.0;

            vertNum++;
        }
    }

    /* Add the center */
    vertices[vertNum].x = 0.5;
    vertices[vertNum].y = 1.0;
    vertices[vertNum].z = -0.5;

    colors[vertNum].r = 1.0;
    colors[vertNum].g = 1.0;
    colors[vertNum].b = 1.0;

    vertNum++;

    log(stderr, "Vertices: %d, allocated: %d, H*V: %d, top: %d, bottom: %d\n", vertNum, horizontal * vertical + top + bottom,
            horizontal * vertical, top, bottom);

    /* Generate indices */
    numIndices = 2 * (horizontal + 1) * (vertical - 1) + 4 * ((1 << level) * ((1 << (level - 1)) + 1) - 4) +
                 (((1 << (level - 1)) - 2)) * 2 + 18 + (((1 << (level - 1)) - 2)) * 2 + 18;

    GLushort *indices = new GLushort[numIndices];
    int count = 0;
    for (i = 0; i < vertical - 1; i++) {
        for (j = 0; j < horizontal; j++) {
            indices[count++] = i * horizontal + j;
            indices[count++] = (i + 1) * horizontal + j;
        }
        /* Repeat the first ones */
        indices[count++] = i * horizontal;
        indices[count++] = (i + 1) * horizontal;
    }

    /* Generate bottom indices */
    uint32_t k;
    uint32_t _h = (1 << level) * 4;
    uint32_t _v = (1 << level) + 1;
    uint32_t base = _h * (_v - 1);
    uint32_t side = 1 << level;

    log("numIndices: %d, count: %d\n", numIndices, count);
    uint32_t total_levels = ((1 << (level - 1)) - 1);

    for (k = 0; k <= total_levels; k++) {
        uint32_t shift = _h;
        uint32_t first = base;

        if (k != 0) {
            indices[count++] = base;
        }

        for (i = 0; i < 4; i++) {
            for (j = 0; j < side - 1; j++) {
                indices[count++] = first + j;
                if (i == 3 && j == side - 2) {
                    indices[count++] = base + _h;
                } else {
                    indices[count++] = first + shift + j;
                }
            }

            indices[count++] = first + j;

            if (i == 3) {
                indices[count++] = base;
            } else {
                indices[count++] = first + j + 1;
            }

            first += side;
            shift -= 2;
        }

        if (k != total_levels) {
            indices[count++] = base;
        }

        base += _h;
        _h -= 2 * 4;
        side -= 2;
    }

    /* Generate top indices */
    _h = (1 << level) * 4;
    _v = (1 << level) + 1;
    base = _h * (_v - 2) + _v * _v;
    side = 1 << level;

    total_levels = ((1 << (level - 1)) - 1);

    for (k = 0; k < total_levels; k++) {
        uint32_t shift = _h - 8;
        uint32_t first = base;

        if (k != 0) {
            indices[count++] = base;
        }

        for (i = 0; i < 4; i++) {
            for (j = 0; j < side - 1; j++) {
                if (k == 0) {
                    /* Alias area */
                    if (i == 0 && j == 0) {
                        indices[count++] = 0;
                    } else {
                        indices[count++] = 4 * (4 - i) - j;
                    }
                } else {
                    indices[count++] = first + j;
                }

                if (i == 3 && j == side - 2) {
                    indices[count++] = base + _h;
                } else {
                    indices[count++] = first + shift + j;
                }
            }

            if (k == 0) {
                indices[count++] = 4 * (4 - i) - j;
                indices[count++] = 4 * (4 - i) - j - 1;
            } else {
                indices[count++] = first + j;
                if (i == 3) {
                    indices[count++] = base;
                } else {
                    indices[count++] = first + j + 1;
                }
            }

            first += side;
            shift -= 2;
        }

        if (k != total_levels) {
            indices[count++] = base;
        }

        base += _h;
        _h -= 2 * 4;
        side -= 2;
    }

    log("numIndices: %d, count: %d\n", numIndices, count);

    /* Generate a vertex array to reference the attributes */
    glGenVertexArrays(1, &_gVAO);
    glBindVertexArray(_gVAO);

    /* Generate a buffer object for the vertices positions */
    glGenBuffers(1, &_verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _verticesVBO);

    /* Upload the data for this buffer */
    glBufferData(GL_ARRAY_BUFFER, vertNum * sizeof(vertex), vertices, GL_STATIC_DRAW);

    /* Link the data with the first shader attribute */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,         // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,         // size
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,         // stride
                          (void *)0  // array buffer offset
                          );

    /* Generate a buffer for the vertices colors */
    glGenBuffers(1, &_colorsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, _colorsVBO);

    /* Upload the data for this buffer */
    glBufferData(GL_ARRAY_BUFFER, vertNum * sizeof(color), colors, GL_STATIC_DRAW);

    /* Link the data with the second shader attribute */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,         // attribute
                          3,         // size
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,         // stride
                          (void *)0  // array buffer offset
                          );

    /* Generate the buffer for the indices */
    glGenBuffers(1, &_indicesVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVBO);

    /* Upload the data */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), indices, GL_STATIC_DRAW);
    return true;
}

bool Sphere::destroy()
{
    glDeleteBuffers(1, &_colorsVBO);
    glDeleteBuffers(1, &_verticesVBO);
    glDeleteVertexArrays(1, &_gVAO);
    return true;
}

uint32_t Sphere::getVertexArrayIndex() { return _gVAO; }