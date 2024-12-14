#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#include "defaults.hpp"

#include "cylinder.hpp"
#include "cone.hpp"
#include "cube.hpp"
#include "loadcustom.hpp"
#include "loadobj.hpp"
#include "texture.hpp"
#include "simple_mesh.hpp"

#include "fontstash.h"


namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";
	//by sandra
	constexpr float kPi_ = 3.1415926f;

	float kMovementSpeed = 5.f; // units per second
	float kMouseSensitivity_ = 0.01f; // radians per pixel
	struct GraphicsState
	{
		ShaderProgram* shaderProg;

		struct CameraControl
		{
			bool isCameraActive, zoomIn, zoomOut, zoomLeft, zoomRight;
			bool moveForward, moveBackward, moveLeft, moveRight, moveUp, moveDown;
			
			float pitch, yaw;
			float distance;
			Vec3f movementDirection;

			float prevX, prevY;
		} cameraControl;
	};

	void glfw_callback_error_( int, char const* );
	void glfw_callback_key_( GLFWwindow*, int, int, int, int );

	void glfw_callback_motion_(GLFWwindow*, double, double); //function for mouse motion

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

namespace
{
	// Mesh rendering function
	void renderMesh(
		GLuint vertexArrayObj,
		size_t numVertices,
		const GraphicsState& graphicsState,
		GLuint textureID,
		GLuint shaderID,
		Mat44f projectionViewMatrix,
		Mat33f normalMatrix
	)
	{
		glUseProgram(shaderID);

		// Set projection-view matrix for camera
		glUniformMatrix4fv(0, 1, GL_TRUE, projectionViewMatrix.v);

		// Set normal matrix
		glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);

		Vec3f lightDirection = normalize(Vec3f{ 0.f, 1.f, -1.f });

		glUniform3fv(2, 1, &lightDirection.x);      // Ambient light 
		glUniform3f(3, 0.9f, 0.9f, 0.9f);          // Diffuse light
		glUniform3f(4, 0.05f, 0.05f, 0.05f);       // Specular light

		glBindVertexArray(vertexArrayObj);
		if (textureID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}
}


int main() try
{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper resourceCleaner;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_);

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#if !defined(NDEBUG)
	// Request OpenGL debug context for debugging mode
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Graphics Engine", nullptr, nullptr);

	if (!window)
	{
		const char* errorMsg = nullptr;
		int errorCode = glfwGetError(&errorMsg);
		throw Error("glfwCreateWindow() failed with '%s' (%d)", errorMsg, errorCode);
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	GraphicsState graphicsState{};
	glfwSetWindowUserPointer(window, &graphicsState);
	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);

	// Set up OpenGL context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable V-Sync

	// Initialize GLAD (to load OpenGL functions)
	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		throw Error("gladLoadGLLoader() failed - cannot load GL API!");

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

#if !defined(NDEBUG)
	setup_gl_debug_output();
#endif

	// Global GL state setup
	OGL_CHECKPOINT_ALWAYS();
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	OGL_CHECKPOINT_ALWAYS();

	// Get framebuffer size
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Load shader program
	ShaderProgram shaderProgram({
		{ GL_VERTEX_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\default.vert" },
		{ GL_FRAGMENT_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\default.frag" }
		});
	graphicsState.shaderProg = &shaderProgram;
	graphicsState.cameraControl.distance = 10.f;

	auto lastFrame = Clock::now();
	float rotationAngle = 0.f;

	auto parlahti = load_wavefront_obj("C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\parlahti.obj");
	GLuint vao = create_vao(parlahti);
	std::size_t vertexCount = parlahti.positions.size();

	GLuint textures = load_texture_2d("C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\L4343A-4k.jpeg");

	// Load additional shader program for launchpad
	ShaderProgram prog2({
		{ GL_VERTEX_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\launch.vert" }, 
		{ GL_FRAGMENT_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\launch.frag" } 
	}); 

	// Load and modify launchpad model
	auto launchpad = load_wavefront_obj("C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\landingpad.obj");
	std::size_t launchpadVertexCount = launchpad.positions.size();
	std::vector<Vec3f> originalPositions = launchpad.positions;

	// Move first launchpad model
	for (size_t i = 0; i < launchpadVertexCount; i++)
	{
		launchpad.positions[i] += Vec3f{ 0.f, -0.975f, -50.f };
	}

	GLuint launchpadVAO1 = create_vao(launchpad);

	// Restore positions and move second launchpad model
	launchpad.positions = originalPositions;
	for (size_t i = 0; i < launchpadVertexCount; i++)
	{
		launchpad.positions[i] += Vec3f{ -50.f, -0.975f, -20.f };
	}
	GLuint launchpadVAO2 = create_vao(launchpad);

	OGL_CHECKPOINT_ALWAYS();

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Handle window resize
		float framebufferWidth, framebufferHeight;
		int newWidth, newHeight;
		glfwGetFramebufferSize(window, &newWidth, &newHeight);
		framebufferWidth = static_cast<float>(newWidth);
		framebufferHeight = static_cast<float>(newHeight);

		if (newWidth == 0 || newHeight == 0)
		{
			do
			{
				glfwWaitEvents();
				glfwGetFramebufferSize(window, &newWidth, &newHeight);
			} while (newWidth == 0 || newHeight == 0);
		}

		glViewport(0, 0, newWidth, newHeight);

		auto const currentFrame = Clock::now();
		float deltaTime = std::chrono::duration_cast<Secondsf>(currentFrame - lastFrame).count(); //difference in time since last frame
		lastFrame = currentFrame;

		rotationAngle += deltaTime * kPi_ * 0.3f;
		if (rotationAngle >= 2.f * kPi_)
			rotationAngle -= 2.f * kPi_;

		Mat44f modelToWorld = make_rotation_y(0);
		Mat33f normalMat = mat44_to_mat33(transpose(invert(modelToWorld)));

		Mat44f rotationX = make_rotation_x(graphicsState.cameraControl.pitch);
		Mat44f rotationY = make_rotation_y(graphicsState.cameraControl.yaw);
		Mat44f translation = kIdentity44f;
		
		// Camera movement
		if (graphicsState.cameraControl.moveForward)
		{
			graphicsState.cameraControl.movementDirection.x -= kMovementSpeed * deltaTime * sin(graphicsState.cameraControl.yaw);
			graphicsState.cameraControl.movementDirection.z += kMovementSpeed * deltaTime * cos(graphicsState.cameraControl.yaw);
		}
		if (graphicsState.cameraControl.moveBackward)
		{
			graphicsState.cameraControl.movementDirection.x += kMovementSpeed * deltaTime * sin(graphicsState.cameraControl.yaw);
			graphicsState.cameraControl.movementDirection.z -= kMovementSpeed * deltaTime * cos(graphicsState.cameraControl.yaw);
		}
		if (graphicsState.cameraControl.moveLeft)
		{
			graphicsState.cameraControl.movementDirection.x += kMovementSpeed * deltaTime * cos(graphicsState.cameraControl.yaw);
			graphicsState.cameraControl.movementDirection.z += kMovementSpeed * deltaTime * sin(graphicsState.cameraControl.yaw);
		}
		if (graphicsState.cameraControl.moveRight)
		{
			graphicsState.cameraControl.movementDirection.x -= kMovementSpeed * deltaTime * cos(graphicsState.cameraControl.yaw);
			graphicsState.cameraControl.movementDirection.z -= kMovementSpeed * deltaTime * sin(graphicsState.cameraControl.yaw);
		}
		if (graphicsState.cameraControl.moveUp)
		{
			graphicsState.cameraControl.movementDirection.y -= kMovementSpeed * deltaTime;
		}
		if (graphicsState.cameraControl.moveDown)
		{
			graphicsState.cameraControl.movementDirection.y += kMovementSpeed * deltaTime;
		}
		// Update camera position
		translation = make_translation(graphicsState.cameraControl.movementDirection);
		Mat44f cameraTranslation = rotationX * rotationY * translation;

		// Projection and view matrices
		Mat44f projectionMatrix = make_perspective_projection(45.f, framebufferWidth / framebufferHeight, 0.1f, 100.f);
		Mat44f projectionViewMatrix = projectionMatrix * (cameraTranslation * modelToWorld);

		// Rendering code
		OGL_CHECKPOINT_DEBUG();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the correct shader for the launchpad
		glUseProgram(prog2.programId());

		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, textures);

		// Render other models like landing pad, etc.
		renderMesh(vao, vertexCount, graphicsState, textures, shaderProgram.programId(), projectionViewMatrix, normalMat);

		// Render the launchpad
		renderMesh(launchpadVAO1, launchpadVertexCount, graphicsState, 0, prog2.programId(), projectionViewMatrix, normalMat);
		renderMesh(launchpadVAO2, launchpadVertexCount, graphicsState, 0, prog2.programId(), projectionViewMatrix, normalMat);

		glBindVertexArray(0);
		glUseProgram(0);
		OGL_CHECKPOINT_DEBUG();

		// Display results
		glfwSwapBuffers( window );
	}
	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}

namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_(GLFWwindow* window, int key, int, int action, int)
	{
		if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			return;
		}

		if (auto* graphicsState = static_cast<GraphicsState*>(glfwGetWindowUserPointer(window)))
		{
			// R-key reloads shaders
			if (GLFW_KEY_R == key && GLFW_PRESS == action)
			{
				if (graphicsState->shaderProg)
				{
					try
					{
						graphicsState->shaderProg->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& error)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", error.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			// Shift key increases movement speed
			if (GLFW_KEY_LEFT_SHIFT == key && GLFW_PRESS == action)
				kMovementSpeed *= 2.f;
			else if (GLFW_KEY_LEFT_SHIFT == key && GLFW_RELEASE == action)
				kMovementSpeed /= 2.f;

			// Ctrl key decreases movement speed
			if (GLFW_KEY_LEFT_CONTROL == key && GLFW_PRESS == action)
				kMovementSpeed /= 2.f;
			else if (GLFW_KEY_LEFT_CONTROL == key && GLFW_RELEASE == action)
				kMovementSpeed *= 2.f;

			// Space toggles camera
			if (GLFW_KEY_SPACE == key && GLFW_PRESS == action)
			{
				graphicsState->cameraControl.isCameraActive = !graphicsState->cameraControl.isCameraActive;

				if (graphicsState->cameraControl.isCameraActive)
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			// Camera controls when active
			if (graphicsState->cameraControl.isCameraActive)
			{
				if (GLFW_KEY_W == key)
				{
					graphicsState->cameraControl.moveForward = (GLFW_PRESS == action);
				}
				else if (GLFW_KEY_S == key)
				{
					graphicsState->cameraControl.moveBackward = (GLFW_PRESS == action);
				}
				else if (GLFW_KEY_A == key)
				{
					graphicsState->cameraControl.moveLeft = (GLFW_PRESS == action);
				}
				else if (GLFW_KEY_D == key)
				{
					graphicsState->cameraControl.moveRight = (GLFW_PRESS == action);
				}
				else if (GLFW_KEY_Q == key)
				{
					graphicsState->cameraControl.moveDown = (GLFW_PRESS == action);
				}
				else if (GLFW_KEY_E == key)
				{
					graphicsState->cameraControl.moveUp = (GLFW_PRESS == action);
				}
				else if (GLFW_KEY_RIGHT_SHIFT == key)
				{
					kMovementSpeed *= (GLFW_PRESS == action) ? 2.f : 0.5f;
				}
				else if (GLFW_KEY_RIGHT_CONTROL == key)
				{
					kMovementSpeed *= (GLFW_PRESS == action) ? 0.5f : 2.f;
				}
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* window, double xpos, double ypos)
	{
		if (auto* graphicsState = static_cast<GraphicsState*>(glfwGetWindowUserPointer(window)))
		{
			if (graphicsState->cameraControl.isCameraActive)
			{
				auto const deltaX = float(xpos - graphicsState->cameraControl.prevX);
				auto const deltaY = float(ypos - graphicsState->cameraControl.prevY);

				graphicsState->cameraControl.yaw += deltaX * kMouseSensitivity_;
				graphicsState->cameraControl.pitch += deltaY * kMouseSensitivity_;

				if (graphicsState->cameraControl.pitch > kPi_ / 2.f)
					graphicsState->cameraControl.pitch = kPi_ / 2.f;
				else if (graphicsState->cameraControl.pitch < -kPi_ / 2.f)
					graphicsState->cameraControl.pitch = -kPi_ / 2.f;
			}

			graphicsState->cameraControl.prevX = float(xpos);
			graphicsState->cameraControl.prevY = float(ypos);
		}
	}

}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if (window)
			glfwDestroyWindow(window);
	}
}

