/**
 * @class	OpenGLRenderer
 * @brief	OpenGL based 3D renderer
 *
 * @author	Roberto Sosa Cano
 */
#include "OpenGL.h"
#include "OpenGLRenderer.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLObject3D.hpp"

void OpenGLRenderer::init()
{
	//glClearColor(1.0, 1.0, 1.0, 1.0);
	GL( glClearColor(0.0, 0.0, 0.0, 1.0) );
	GL( glEnable(GL_DEPTH_TEST) );
	GL( glEnable(GL_CULL_FACE) );
	GL( glCullFace(GL_BACK) );

    GL( glDisable(GL_DITHER) );
    GL( glDisable(GL_LINE_SMOOTH) );
    GL( glDisable(GL_POLYGON_SMOOTH) );
    GL( glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE) );
#define GL_MULTISAMPLE_ARB 0x809D
    GL( glDisable( GL_MULTISAMPLE_ARB)  );

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

Shader * OpenGLRenderer::getShader(void)
{
	return new OpenGLShader();
}

RendererObject3D *OpenGLRenderer::prepareObject3D(const Object3D &object)
{
    OpenGLObject3D *glObject = new OpenGLObject3D();
    if (glObject != NULL) {
        glObject->init(object);
    }
    return glObject;
}

bool OpenGLRenderer::renderObject3D(RendererObject3D &object, Shader &shader,
                                    const glm::mat4 &projection, const glm::mat4 &view,
                                    RenderTarget &renderTarget)
{
	/* Model matrix : an identity matrix (model will be at the origin) */
	glm::mat4 model = glm::mat4(1.0f);

	/* Our ModelViewProjection : multiplication of our 3 matrices */
	glm::mat4 MVP = projection * view * model; // Remember, matrix multiplication is the other way around

    /* Cast the object into an internal type */
    OpenGLObject3D &glObject = dynamic_cast<OpenGLObject3D&>(object);

    /* Bind the render target */
    GL( glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.getID()) );
    {
        GL( glClearColor(0.0, 0.0, 0.0, 1.0) );
        GL( glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT) );

        /* Bind program to upload the uniform */
        shader.attach();

        /* Send our transformation to the currently bound shader, in the "MVP" uniform */
        shader.setUniform("MVP", MVP);

        /* Draw the object */
        GL( glBindVertexArray(glObject.getVertexArrayID()) );
        {
            GL( glDrawElements(GL_TRIANGLES, glObject.getIndicesArrayLen(), GL_UNSIGNED_INT, NULL) );
        }
        GL( glBindVertexArray(0) );

        /* Unbind */
        shader.detach();
    }
    GL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );

    return true;
}

bool OpenGLRenderer::resize(uint16_t width, uint16_t height)
{
	_width  = width;
	_height = height;
    return true;
}