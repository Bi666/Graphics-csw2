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
#include "loadobj.hpp"
#include "simple_mesh.hpp"
#include "loadcustom.hpp"

#include "cube.hpp"
#include "texture.hpp"

#include "fontstash.h"

namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";

	constexpr float kPi_ = 3.1415926f;

	float kMovementPerSecond_ = 5.f; // units per second
	float kMouseSensitivity_ = 0.01f; // radians per pixel
	struct State_ //struct for camera control
	{
		ShaderProgram* prog;

		struct CamCtrl_
		{
			bool cameraActive;
			bool actionZoomIn, actionZoomOut;
			bool actionZoomleft, actionZoomRight;
			bool actionMoveForward, actionMoveBackward;
			bool actionMoveLeft, actionMoveRight;
			bool actionMoveUp, actionMoveDown;

			float phi, theta;
			float radius;
			Vec3f movementVec;

			float lastX, lastY;
		} camControl;
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
	// Mesh rendering
	void mesh_renderer(
		GLuint vao,
		size_t vertexCount,
		State_ const& state,
		GLuint textureObjectId,
		GLuint programID,
		Mat44f projCameraWorld,
		Mat33f normalMatrix
		//Mat44f localTransform
	)
	{
		glUseProgram(programID);

		// for camera
		glUniformMatrix4fv(
			0,
			1, GL_TRUE,
			projCameraWorld.v);

		//for normals
		glUniformMatrix3fv(
			1,
			1, GL_TRUE,
			normalMatrix.v);

		Vec3f lightDir = normalize(Vec3f{ 0.f, 1.f, -1.f });

		glUniform3fv(2, 1, &lightDir.x);      // Ambient 
		glUniform3f(3, 0.9f, 0.9f, 0.9f);	  // Diffusion
		glUniform3f(4, 0.05f, 0.05f, 0.05f);  // Spectral

		glBindVertexArray(vao);
		if (textureObjectId != 0)
		{	glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureObjectId);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
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
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	State_ state{};

	//TODO: Additional event handling setup
	//setting up keyboard event handling
	glfwSetWindowUserPointer(window, &state);
	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_); //!!need to make this frame-rate independent
	//endofTODO

	glfwSetKeyCallback( window, &glfw_callback_key_ );

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	//endofTODO

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	// Load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\default.vert" },
		{ GL_FRAGMENT_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\default.frag" }
		});

	state.prog = &prog; //set shader program to state
	state.camControl.radius = 10.f; //set initial radius

	auto last = Clock::now();
	float angle = 0.f;

	auto parlahti = load_wavefront_obj("C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\parlahti.obj");
	GLuint vao = create_vao(parlahti);
	std::size_t vertexCount = parlahti.positions.size();

	GLuint textures = load_texture_2d("C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\L4343A-4k.jpeg");

	//----------------------------------------------------------------
	//load shader program for launchpad
	 ShaderProgram prog2({
	 	{ GL_VERTEX_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\launch.vert" }, 
	 	{ GL_FRAGMENT_SHADER, "C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\launch.frag" } 
	 	}); 

	//state.prog = &prog2;  //set shader program to state

	 // Load the launchpad
	 auto launch = load_wavefront_obj("C:\\Users\\32939\\Desktop\\graphics-engine-master\\assets\\landingpad.obj");
	 std::size_t launchVertexCount = launch.positions.size();
	 std::vector<Vec3f> positions = launch.positions;

	 // Move the 1st launch object
	 for (size_t i = 0; i < launchVertexCount; i++)
	 {
		 launch.positions[i] = launch.positions[i] + Vec3f{ 0.f, -0.975f, -50.f };
	 }

	 // Create a VAO for the first launchpad
	 GLuint launch_vao_1 = create_vao(launch);

	 // Return positions back to normal
	 launch.positions = positions;

	 // Move the 1st launch object
	 for (size_t i = 0; i < launchVertexCount; i++)
	 {
		 launch.positions[i] = launch.positions[i] + Vec3f{ -20.f, -0.975f, -15.f };
	 }
	 // Create a VAO for the first launchpad
	 GLuint launch_vao_2 = create_vao(launch);

	//-------------------------------------------------------------------

	 // Draw all arrays


	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
		}

		//TODO: update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count(); //difference in time since last frame
		last = now;

		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		Mat44f model2World = make_rotation_y(0);
		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2World)));

		Mat44f Rx = make_rotation_x(state.camControl.theta);
		Mat44f Ry = make_rotation_y(state.camControl.phi);
		Mat44f T = kIdentity44f;

		//camera movement vector is added to current movement
		if (state.camControl.actionMoveForward)
		{
			state.camControl.movementVec.x -= kMovementPerSecond_* dt * sin(state.camControl.phi);
			state.camControl.movementVec.z += kMovementPerSecond_* dt * cos(state.camControl.phi);
			//state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 0.f,0.f,1.f };
		}
	    if (state.camControl.actionMoveBackward)
		{
			state.camControl.movementVec.x += kMovementPerSecond_* dt * sin(state.camControl.phi);
			state.camControl.movementVec.z -= kMovementPerSecond_* dt * cos(state.camControl.phi);
			//state.camControl.movementVec -= kMovementPerSecond_ * dt * Vec3f{ 0.f,0.f,1.f };
		}
	    if (state.camControl.actionMoveLeft)
		{
			state.camControl.movementVec.x += kMovementPerSecond_ * dt * cos(state.camControl.phi);
			state.camControl.movementVec.z += kMovementPerSecond_ * dt * sin(state.camControl.phi);
			//state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 1.f,0.f,0.f };
		}
	    if (state.camControl.actionMoveRight)
		{
			state.camControl.movementVec.x -= kMovementPerSecond_ * dt * cos(state.camControl.phi);
			state.camControl.movementVec.z -= kMovementPerSecond_ * dt * sin(state.camControl.phi);
			//state.camControl.movementVec -= kMovementPerSecond_ * dt * Vec3f{ 1.f,0.f,0.f };
		}
	    if (state.camControl.actionMoveUp)
		{
			state.camControl.movementVec -= kMovementPerSecond_ * dt * Vec3f{ 0.f,1.f,0.f };
		}
	    if (state.camControl.actionMoveDown)
		{
			state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 0.f,1.f,0.f };
		}

		T = make_translation(state.camControl.movementVec);

		Mat44f world2Camera = Rx * Ry * T;

		//Mat44f world2Camera = make_translation({ 0.f, 0.f, -10.f });

		Mat44f projection = make_perspective_projection(
			60 * kPi_ / 180.f,			//FOV:60 converted to radians
			fbwidth / float(fbheight), //aspect ratio
			0.1f,					  //near plane
			100.f					 //far plane
		);

		Mat44f projCameraWorld = projection * (world2Camera * model2World);

		//ENDOF TODO

		// Draw scene
		OGL_CHECKPOINT_DEBUG();

		//TODO: draw frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Draw the map
		mesh_renderer(vao, vertexCount,  state, textures, prog.programId(), projCameraWorld, normalMatrix);

		// Draw the first launchpad
		mesh_renderer(launch_vao_1, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, normalMatrix);

		// Draw the second launchpad
		mesh_renderer(launch_vao_2, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, normalMatrix);

		glBindVertexArray(0);
		//glBindVertexArray(1);

		glUseProgram(0);
		//glUseProgram(1);

		//ENDOF TODO

		OGL_CHECKPOINT_DEBUG();

		// Display results
		glfwSwapBuffers( window );
	}

	// Cleanup.
	//TODO: additional cleanup
	
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

	void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			// TODO SHIFT INCREASES SPEED

			if (GLFW_KEY_LEFT_SHIFT == aKey && GLFW_PRESS == aAction)
				kMovementPerSecond_ *= 2.f;
			else if (GLFW_KEY_LEFT_SHIFT == aKey && GLFW_RELEASE == aAction)
				kMovementPerSecond_ /= 2.f;


			// TODO CTRL DECREASES SPEED
			if (GLFW_KEY_LEFT_CONTROL == aKey && GLFW_PRESS == aAction) 
				kMovementPerSecond_ /= 2.f;
			else if (GLFW_KEY_LEFT_CONTROL == aKey && GLFW_RELEASE == aAction)
				kMovementPerSecond_ *= 2.f;


			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;

				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			// Camera controls if camera is active
			if (state->camControl.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveForward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveForward = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveBackward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveBackward = false;
				}
				else if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveLeft = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveLeft = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveRight = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveRight = false;
				}
				else if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveDown = false;
				}
				else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveUp = false;
				}
				else if (GLFW_KEY_RIGHT_SHIFT == aKey)
				{
					if (GLFW_PRESS == aAction)
						kMovementPerSecond_ *= 2.f;
					else if (GLFW_RELEASE == aAction)
						kMovementPerSecond_ /= 2.f;
				}
				else if (GLFW_KEY_RIGHT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						kMovementPerSecond_ /= 2.f;
					else if (GLFW_RELEASE == aAction)
						kMovementPerSecond_ *= 2.f;
				}
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = float(aX - state->camControl.lastX);
				auto const dy = float(aY - state->camControl.lastY);

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}

			state->camControl.lastX = float(aX);
			state->camControl.lastY = float(aY);
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
		if( window )
			glfwDestroyWindow( window );
	}
}

//different use program 

