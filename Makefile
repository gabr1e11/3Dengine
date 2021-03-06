#
#Roberto Sosa Cano
#Copyright 2013
#

### Project name
PROJECT=engine

### System config
UNAME=$(shell uname)

CXX=g++
CC=gcc

#
#Files to be compiled
#
VPATH=core/src:opengl/src:procedural/src:utils/src:tools/

CORE_FILES=Game.cpp InputManager.cpp WindowManager.cpp TimeManager.cpp \
		   Model3D.cpp Asset3D.cpp \
		   TextConsole.cpp TrueTypeFont.cpp FreeTypeFont.cpp FontRenderer.cpp \
		   Scene.cpp Camera.cpp \
           Renderer.cpp NOAARenderTarget.cpp MSAARenderTarget.cpp SSAARenderTarget.cpp \
		   FXAARenderTarget.cpp FXAA2RenderTarget.cpp FBRenderTarget.cpp ToonRenderTarget.cpp \
		   HDRRenderTarget.cpp GaussianBlurRenderTarget.cpp \
		   ShadowMapRenderTarget.cpp \
		   Shader.cpp FlatShader.cpp LightEmitShader.cpp SolidColorShader.cpp \
		   BlinnPhongShader.cpp ToonLightingShader.cpp NormalShadowMapShader.cpp \
		   FlyMotion.cpp FreeFlyMotion.cpp WalkingMotion.cpp \
		   Logging.cpp

UTILS_FILES=MathUtils.cpp ImageLoaders.c Asset3DLoaders.cpp Asset3DStorage.cpp Asset3DTransform.cpp \
			ZCompression.cpp

OPENGL_FILES=GLFWKeyManager.cpp GLFWMouseManager.cpp GLFWWindowManager.cpp \
			 OpenGLAsset3D.cpp \
			 OpenGLFontRenderer.cpp \
			 OpenGLRenderer.cpp OpenGLFilterRenderTarget.cpp \
             OpenGLMSAARenderTarget.cpp OpenGLSSAARenderTarget.cpp OpenGLFBRenderTarget.cpp \
			 OpenGLShadowMapRenderTarget.cpp \
             OpenGLShader.cpp OpenGLShaderMaterial.cpp \
			 OpenGLShaderPointLight.cpp OpenGLShaderSpotLight.cpp OpenGLShaderDirectLight.cpp \
			 OpenGLUniformBlock.cpp

PROCEDURAL_FILES=Terrain.cpp Triangle.cpp Plane.cpp BentPlane.cpp Cube.cpp Cylinder.cpp Circle.cpp Torus.cpp Sphere.cpp ProceduralUtils.cpp

FILES=$(CORE_FILES) $(OPENGL_FILES) $(PROCEDURAL_FILES) $(UTILS_FILES)

OBJDIR=obj
CPP_OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(FILES))
OBJECTS=$(patsubst %.c,$(OBJDIR)/%.o,$(CPP_OBJECTS))

LIBDIR=lib
LIBNAME=lib$(PROJECT).$(SHAREDEXT)

DEMODIR=demos

#Mac OS alternate cmdline link options
ifeq ($(UNAME), Darwin)
LDFLAGS= -Llib -lengine -L/usr/local/lib/ -lfreetype -lGLEW -lglfw -ljpeg -framework Cocoa -framework OpenGL -framework IOKit -fPIC
FLAGS=-I/opt/X11/include -I/usr/local/include/freetype2/ -Wno-deprecated-register
SHAREDGEN= -dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,$(LIBNAME)
SHAREDEXT=dylib
PREFIX=/usr/local/lib
else
LDFLAGS+= -Llib -lengine -lGL -lGLEW -lglfw3 -lpng -ljpeg -lfreetype -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -ldl -pthread -fPIC
FLAGS=-I/usr/include -I/usr/include/freetype2
SHAREDGEN= -shared
SHAREDEXT=so
PREFIX=/usr/local/lib
endif

#
#Compilation flags
#
FLAGS+=-Werror -MMD -fPIC -Icore/inc -Iopengl/inc -Iprocedural/inc -Iutils/inc -I3rdparty -O0 -g -DDEBUG_OPENGL_PIPELINE
CXXFLAGS=$(FLAGS) -std=c++11
CFLAGS=$(FLAGS) -std=c11

#
#Demos
#
DEMO_FILES=$(shell \ls demos/*.cpp)
DEMO_TARGETS=$(DEMO_FILES:.cpp=)

#
#Tools
#
TOOLS_FILES=$(shell \ls tools/*.cpp)
TOOLS_TARGETS=$(TOOLS_FILES:.cpp=)

#
# Main rules
#
.PHONY: release headers

all: engine $(DEMO_TARGETS) $(TOOLS_TARGETS)

release:
	$(MAKE) clean
	$(MAKE) all

engine: dirs $(LIBDIR)/$(LIBNAME)

demos/%: demos/%.cpp $(LIBDIR)/$(LIBNAME)
	@echo "- Compiling demo $@"
	@g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS) 

tools/%: tools/%.cpp $(LIBDIR)/$(LIBNAME)
	@echo "- Compiling tool $@"
	@g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS) 

dirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBNAME): $(OBJECTS)
	@echo "- Generating $@...\c"
	@$(CXX) $(SHAREDGEN) -o $@ $(OBJECTS)
	@echo "done"

-include $(OBJECTS:.o=.d)

$(OBJDIR)/%.o: %.cpp
	@echo "- Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	@echo "- Compiling $<..."
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "- Cleaning project directories...\c"
	@rm -fr $(OBJDIR)
	@rm -fr $(LIBDIR)
	@echo "done"
