/**
 * @class	Shader
 * @brief	Class to abstract shader functionality
 *
 * @author	Roberto Sosa Cano
 */
#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>

class Shader
{
	public:
		/**
		 * Destructor
		 */
		virtual ~Shader() {}

		/**
		 * Loads a new vertex shader code and compiles it
		 *
		 * @param		filename  File name of the vertex shader to be compiled
		 * @param[out]	error	  If compilation fails error will contain
		 *						  a description of the error
		 *
		 * @return true or false
		 */
		virtual bool loadVertexShader(const std::string &filename, std::string &error) = 0;

		/**
		 * Loads a new fragment shader code and compiles it
		 *
		 * @param		filename  File name of the fragment shader to be compiled
		 * @param[out]	error	  If compilation fails error will contain
		 *						  a description of the error
		 *
		 * @return true or false
		 */
		virtual bool loadFragmentShader(const std::string &filename, std::string &error) = 0;

		/**
		 * Links the program together and prepares the shader to be used
		 *
		 * @param[out] error	If linking fails error will contain a
		 *						description of the error
		 *
		 * @return true or false
		 */
		virtual bool linkProgram(std::string &error) = 0;

		/**
		 * Makes the shader active
		 */
		virtual bool attach(void) = 0;

		/**
		 * Makes the shader inactive
		 */
		virtual bool detach(void) = 0;

		/**
		 * Gets a list of all the shader uniforms
		 *
		 * @return A vector with all the available uniform names
		 */
		virtual const std::map<std::string, uint32_t> & getUniforms(void) = 0;

        /**
         * Retrieves the ID of a specific uniform
         *
         * @param name  Name of the uniform
         * @param id    Output ID
         *
         */
        virtual const bool getUniformID(const std::string &name, uint32_t *id) = 0;
        virtual const bool getAttributeID(const std::string &name, uint32_t *id) = 0;

		/**
		 * Sets the value of a shader uniform as a mat4x4
		 *
		 * @param name  Name of the shader uniform
		 * @param value Value of the uniform to be set
		 *
		 * @return true if the value was set or false if the
		 *         attribute cannot be found
		 */
		virtual bool setUniform(const std::string &name, glm::mat4 &value) = 0;

        /**
         * Sets the value of a uniform as a 2D texture
         *
         * @param name       Name of the shader uniform
         * @param unitID     ID of the texture unit
         */
        virtual bool setUniformTexture2D(const std::string &name, uint32_t unitID) = 0;
        virtual bool setUniformFloat(const std::string &name, float value) = 0;
};