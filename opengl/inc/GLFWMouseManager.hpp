/**
 * @class	GLFWMouseManager
 * @brief	GLFW based mouse manager
 *
 * @author	Roberto Sosa Cano (http://www.robertocano.es)
 */
#pragma once

#include <stdint.h>
#include <vector>
#include "MouseManager.hpp"
#include "OpenGL.h"

class GLFWMouseManager : public MouseManager
{
  public:
    /**
     * Singleton pattern
     */
    static GLFWMouseManager *GetMouseManager(void);

    /**
     * Disposes the singleton
     */
    static void DisposeMouseManager();

    /**
     * Constructor of the class
     */
    GLFWMouseManager(void);

    /**
     * Destructor of the class
     */
    ~GLFWMouseManager(void);

    /**
     * Registers a listener for the mouse
     *
     * @param	listener	Listener that will process the mouse
     *
     * @return true if the listener was correctly registered, false otherwise
     */
    bool registerListener(MouseListener &listener);

  private:
    /**
     * Static callback for key processing
     *
     * @param	x	X coordinate of the mouse
     * @param	y	Y coordinate of the mouse
     */
    static void mouseCallback(GLFWwindow *w, double x, double y);

    /**
     * Object wrapper callback
     *
     * @param	x	X coordinate of the mouse
     * @param	y	Y coordinate of the mouse
     */
    void processMouse(int32_t x, int32_t y);

    /**
     * Static GLFW key manager
     */
    static GLFWMouseManager *_mouseManager;

    /**
     * Map for mouse listeners
     */
    std::vector<MouseListener *> _listeners;
};
