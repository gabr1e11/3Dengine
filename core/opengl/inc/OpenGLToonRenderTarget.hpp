/**
 * @class	OpenGLToonRenderTarget
 * @brief	Render target for OpenGL. A render target allows to render objects to it
 *          instead of to the main screen. Then the target can be rendered to the main screen as
 *          a texture
 *
 *          The Toon render target applies Fast Approximate Anti-Aliasing from
 *          Timothy Lottes paper t Nvidia
 *
 * @author	Roberto Cano (http://www.robertocano.es)
 */
#pragma once

#include "OpenGL.h"
#include "Shader.hpp"
#include "ToonRenderTarget.hpp"
#include "OpenGLFilterRenderTarget.hpp"

#pragma warning( disable : 4250 )

class OpenGLToonRenderTarget : public ToonRenderTarget, public OpenGLFilterRenderTarget
{
	private:
		bool customInit() {
			std::string error;
			if (_shader->use("filters/toon", error) == false) {
				printf("ERROR loading shader filters/toon: %s\n", error.c_str());
				return false;
			}
			return true;
		}
		void setCustomParams(void)
		{
			float nearFrag = 2.0f * (_far - _near) / 1000.0f;
			float distantFrag = 16.0*nearFrag;

			GL( glActiveTexture(GL_TEXTURE1) );
			GL( glBindTexture(GL_TEXTURE_2D, _depthBuffer) );
			_shader->setUniformTexture2D("depthTexture", 1);
			_shader->setUniformFloat("zNear", _near);
			_shader->setUniformFloat("zFar", _far);
			_shader->setUniformFloat("nearFrag", nearFrag);
			_shader->setUniformFloat("distantFrag", distantFrag);
			_shader->setUniformVec4("borderColor", _color);

			GL( glDisable(GL_DEPTH_TEST) );
			GL( glEnable(GL_BLEND) );
			GL( glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
		}
		void unsetCustomParams(void)
		{
			GL( glDisable(GL_BLEND) );
			GL( glEnable(GL_DEPTH_TEST) );
		}
};