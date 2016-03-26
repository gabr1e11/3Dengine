/**
 * @class	OpenGLRenderer
 * @brief	OpenGL based 3D renderer
 *
 * @author	Roberto Cano (http://www.robertocano.es)
 */
#include "OpenGL.h"
#include "OpenGLRenderer.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLObject3D.hpp"

void OpenGLRenderer::init()
{
	GL( glClearColor(0.0, 0.0, 0.0, 1.0) );
	GL( glEnable(GL_DEPTH_TEST) );
	GL( glEnable(GL_CULL_FACE) );
	GL( glEnable(GL_BLEND) );
    GL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
    /* There seems to be a bug with sRGB default framebuffer, as
     * the buffer is always linear, but if we enable this the
     * drivers are doing the linear->sRGB conversion anyway!
     * So don't enable it for now, we will have to compensate
     * in the shaders */
    //GL( glEnable(GL_FRAMEBUFFER_SRGB) );
	GL( glCullFace(GL_BACK) );
    GL( glDisable(GL_DITHER) );
    GL( glDisable(GL_LINE_SMOOTH) );
    GL( glDisable(GL_POLYGON_SMOOTH) );
    GL( glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE) );
#define GL_MULTISAMPLE_ARB 0x809D
    GL( glDisable(GL_MULTISAMPLE_ARB)  );
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

Shader * OpenGLRenderer::newShader(void)
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
                                    std::vector<Light*> &lights, float ambientK,
                                    const glm::mat4 &projection, const glm::mat4 &view,
                                    RenderTarget &renderTarget)
{
	uint32_t numLights = 0;

	/* Model matrix : an identity matrix (model will be at the origin) */
	glm::mat4 model = glm::mat4(1.0f);

	/* Our ModelViewProjection : multiplication of our 3 matrices */
	glm::mat4 MVP = projection * view * model; // Remember, matrix multiplication is the other way around

    /* Cast the object into an internal type */
    OpenGLObject3D &glObject = dynamic_cast<OpenGLObject3D&>(object);

	renderTarget.clear();

    /* Bind the render target */
    renderTarget.bind();
    {
        GL( glEnable(GL_MULTISAMPLE) );
        GL( glActiveTexture(GL_TEXTURE0) );

        /* Bind program to upload the uniform */
        shader.attach();

        ShaderMaterial *shaderMaterial = shader.getMaterial();
        if (shaderMaterial == NULL) {
            fprintf(stderr, "ERROR getting shader material\n");
            return false;
        }

        /* Send our transformation to the currently bound shader, in the "MVP" uniform */
        shader.setUniformMat4("MVP", MVP);
        shader.setUniformMat4("view", view);
        shader.setUniformTexture2D("diffuseMap", 0);
        shader.setUniformFloat("ambientK", ambientK);

		for (numLights=0; numLights<lights.size(); ++numLights) {
			ShaderLight *shaderLight = shader.getLight(numLights);
			if (shaderLight == NULL) {
				fprintf(stderr, "ERROR getting shader light\n");
				break;
			}
			shaderLight->copyLight(*lights[numLights]);
		}
        shader.setUniformUint("numLights", numLights);

        /* Draw the object */
        GL( glBindVertexArray(glObject.getVertexArrayID()) );
        {
            std::vector<Material> materials   = glObject.getMaterials();
            std::vector<uint32_t> texturesIDs = glObject.getTextures();
            std::vector<uint32_t> offset      = glObject.getIndicesOffsets();
            std::vector<uint32_t> count       = glObject.getIndicesCount();

            for (size_t  i=0; i<materials.size(); ++i) {
                GL( glBindTexture(GL_TEXTURE_2D, texturesIDs[i]) );
                shaderMaterial->copyMaterial(materials[i]);

                GL( glDrawElements(GL_TRIANGLES, count[i], GL_UNSIGNED_INT, (void*)(offset[i] * sizeof(GLuint))) );
            }
        }
        GL( glBindVertexArray(0) );

        /* Unbind */
        shader.detach();
    }
    renderTarget.unbind();

    return true;
}

bool OpenGLRenderer::resize(uint16_t width, uint16_t height)
{
	_width  = width;
	_height = height;
    return true;
}

void OpenGLRenderer::flush()
{
    glFinish();
}
