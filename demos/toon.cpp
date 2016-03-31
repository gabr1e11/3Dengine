#include <stdio.h>
#include "Game.hpp"
#include "OBJFormat.hpp"
#include "OpenGL.h"
#include "Shader.hpp"
#include "FXAA2RenderTarget.hpp"

#define PI 3.14159265358979323846

class AntiaAliasingDemo : public GameHandler
{
    public:
        AntiaAliasingDemo() {
            _MouseSensibility = 10.0;
			_InvertMouse = 1.0;
			_prevX = 0xFFFFFF;
			_prevY = 0xFFFFFF;
			_angle = 0.0;
        }

        bool handleInit(Game *game)
        {
			Light *light = new Light(glm::vec3(4.0, 4.0, 5.0),
									 glm::vec3(4.0, 4.0, 5.0),
									 glm::vec3(4.0, 4.0, 5.0),
									 glm::vec3(50.0, 250.0, 100.0));

            _lights.push_back(light);

            game->getWindowManager()->getWindowSize(&_width, &_height);

			/* Create a render target to allow post-processing */
			_renderTargetFXAA2 = FXAA2RenderTarget::New();
			if (_renderTargetFXAA2 == NULL) {
				fprintf(stderr, "ERROR allocating render target\n");
				return false;
			}

			_renderTargetFXAA2->init(_width, _height);
			_renderTargetFXAA2->setClearColor(1.0, 1.0, 1.0, 0.0);

            /* Register the key and mouse listener */
			std::vector<uint32_t> keys; // The keys should be read from a config file

            keys.push_back('W');
            keys.push_back('S');
            keys.push_back('A');
            keys.push_back('D');
            keys.push_back('R');
            keys.push_back('1');
            keys.push_back('2');
            keys.push_back('3');
            keys.push_back('4');
            keys.push_back('5');
            keys.push_back(GLFW_KEY_ESCAPE);

			game->getWindowManager()->getKeyManager()->registerListener(_inputManager, keys);
			game->getWindowManager()->getMouseManager()->registerListener(_inputManager);

            /* Create a Blinn-phong shader for the geometry */
            _shader = Shader::New();

            std::string error;
            if (_shader->use("lighting/toon", error) == false) {
                printf("ERROR compiling shader lighting/toon: %s\n", error.c_str());
                return false;
            }

            /* Load the geometry */
            std::string meshPath = "data/objects/daxter";

            OBJFormat obj3D;
            if (obj3D.load(meshPath) == false) {
                printf("ERROR loading OBJ file\n");
                return false;
            }

            /* Wrap the geometry for the renderer, this typically generates any
             * renderer API specific structures and uploads data to the graphics card */
            _model3D = game->getRenderer()->prepareObject3D(obj3D);

            /* Create the game camera */
            _camera = new FlyCamera();
            _camera->setProjection(45, _width/(float)_height, 0.1, 1000.0);
            /* Daxter */
            _camera->setPosition( glm::vec4(-25.87f, 13.62f, 25.90f, 1.0f) );
			_camera->rotatePitch(5.59f);
			_camera->rotateYaw(46.07f);

            return true;
        }

        bool handleTick(Game *game, double elapsedMs)
        {
			/* Keyboard */
            if (_inputManager._keys[GLFW_KEY_ESCAPE]) {
                return false;
            }
            if (_inputManager._keys['W']) {
                _camera->forward(0.1*elapsedMs);
            }
            if (_inputManager._keys['S']) {
                _camera->forward(-0.1*elapsedMs);
            }
            if (_inputManager._keys['A']) {
                _camera->right(-0.1*elapsedMs);
            }
            if (_inputManager._keys['D']) {
                _camera->right(0.1*elapsedMs);
            }
            if (_inputManager._keys['R']) {
                game->resetStats();
            }

			/* Mouse */
			if (_prevX == 0xFFFFFF) {
				_prevX = _inputManager._xMouse;
			}
			if (_prevY == 0xFFFFFF) {
				_prevY = _inputManager._yMouse;
			}

			int32_t diffMouseX = _inputManager._xMouse - _prevX;
			int32_t diffMouseY = _InvertMouse*(_inputManager._yMouse - _prevY);

            if (diffMouseX) {
                _camera->rotateYaw(_MouseSensibility*M_PI*diffMouseX/_width);
            }
            if (diffMouseY) {
                _camera->rotatePitch(_MouseSensibility*M_PI*diffMouseY/_height);
            }

			_prevX = _inputManager._xMouse;
			_prevY = _inputManager._yMouse;

            return true;
        }

        bool handleRender(Game *game)
        {
            /* Render all objects */
            game->getRenderer()->renderObject3D(*_model3D, *_shader, _lights, 0.05,
                                                _camera->getProjection(), _camera->getView(),
                                                *_renderTargetFXAA2);

            _renderTargetFXAA2->blit(0, 0, _width, _height);
            return true;
        }

    private:
        Camera             *_camera;
        RendererObject3D   *_model3D;
        Shader             *_shader;
		FXAA2RenderTarget  *_renderTargetFXAA2;
		std::string         _renderTargetName;
        std::vector<Light*> _lights;
		InputManager        _inputManager;

        float _MouseSensibility;
		float _InvertMouse;
		int32_t _prevX;
		int32_t	_prevY;
        uint32_t _width;
        uint32_t _height;
		float _angle;
};

int main()
{
    Game *game;
    AntiaAliasingDemo antiAliasingDemo;

    game = new Game("Anti Aliasing Comparison demo");
    if (game == NULL) {
        fprintf(stderr, "ERROR creating new game\n");
        return 1;
    }

    game->setHandler(&antiAliasingDemo);
    game->setWindowSize(800, 600, false);
    game->setFPS(60);

    if (game->init() == false) {
        fprintf(stderr, "ERROR initializing game\n");
        return 1;
    }

    game->loop();
    return 0;
}