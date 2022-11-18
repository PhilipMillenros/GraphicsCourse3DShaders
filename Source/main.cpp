#include <stdio.h>
#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/Gl.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace glm;
bool mouse_camera;
void handle_key_event(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
	if (action != GLFW_PRESS)
	{
		return;
	}
	if (key == GLFW_KEY_ESCAPE)
	{
		glfwDestroyWindow(window);
	}
}
void handle_mouse_event(GLFWwindow* window, int button, int action, int modifier)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			mouse_camera = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mouse_camera = false;
		}
	}
	
}

GLuint load_texture(const char* path)
{
	int width, height;
	int channels;
	stbi_uc* tex_data = stbi_load(path, &width, &height, &channels, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return texture;
}

void handle_framebuffer_resize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "Graphics Window", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSetKeyCallback(window, handle_key_event);
	glfwSetMouseButtonCallback(window, handle_mouse_event);
	
	glfwSetFramebufferSizeCallback(window, handle_framebuffer_resize);

	glEnable(GL_DEPTH_TEST);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	

	float vertexData[] = 
	{
		-0.5f, -0.5f,  1.0f, 0.f, 0.f, 0.f, 1.f,
		0.5f, -0.5f,   0.f, 1.f, 0.f,  1.f, 1.f,
		0.5f, 0.5f,    0.f, 0.f, 1.f,  1.f, 0.f,
		-0.5f, 0.5f,   1.f, 1.f, 1.f,  0.f, 0.f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	GLuint element_data[] =
	{
		0, 1, 2, 0, 2, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element_data), element_data, GL_STATIC_DRAW);
	//Vertex array object - bindings from buffers to attributes

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 7 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 7 * sizeof(float), (void*)( 2 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 7 * sizeof(float), (void*)(5 * sizeof(float)));

	GLuint program = load_shader_program("Shaders/test.vert", "Shaders/test.frag");
	glUseProgram(program);
	
	GLint u_Color = glGetUniformLocation(program, "u_Color");
	GLint u_Time = glGetUniformLocation(program, "u_Time");
	GLint u_Sampler1 = glGetUniformLocation(program, "u_Sampler1");
	
	GLint u_Scale = glGetUniformLocation(program, "u_Scale");
	GLint u_Model = glGetUniformLocation(program, "u_Model");
	GLint u_ViewProjection = glGetUniformLocation(program, "u_ViewProjection");
	glUniform4f(u_Color, 0.2f, 0.5f, 1.f, 1.f);
	glUniform1i(u_Sampler1, 1);
	
	GLuint texture0 = load_texture("texture.jpg");
	glBindTexture(GL_TEXTURE_2D, texture0);
	glActiveTexture(GL_TEXTURE1);

	GLuint texture1 = load_texture("texture2.jpg");

	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);

	vec3 camera_position = vec3(0.f, 0.f, 3.f);
	vec3 camera_rotation = vec3(0.f, 0.f, 0.f);
	double last_time = glfwGetTime();

	double last_mouse_x, last_mouse_y;
	glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);

	while (!glfwWindowShouldClose(window))
	{
		//Calculate delta time
		float time = glfwGetTime();
		float delta_time = time - last_time;
		last_time = time;
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		float mouse_delta_x = mouse_x - last_mouse_x;
		float mouse_delta_y = mouse_y - last_mouse_y;
		last_mouse_x = mouse_x;
		last_mouse_y = mouse_y;

		glUniform1f(u_Time, time);

		if (mouse_camera)
		{
			vec3 camera_rot = vec3(mouse_delta_y, mouse_delta_x, 0.f);

			camera_rotation += camera_rot * radians(-0.1f);
		}
		
		quat camera_quat = angleAxis(camera_rotation.y, vec3(0.f, 1.f, 0.f)) * angleAxis(camera_rotation.x, vec3(1.f, 0.f, 0.f)) * angleAxis(camera_rotation.z, vec3(0.f, 0.f, 1.f));
		vec3 camera_forward = camera_quat * vec3(0.f, 0.f, -1.f);
		vec3 camera_right = camera_quat * vec3(1.f, 0.f, 0.f);

		vec3 camera_move = vec3(0);
		

		float movement_speed = 2;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera_move -= camera_right;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera_move += camera_right;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera_move += camera_forward;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera_move -= camera_forward;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			camera_move.y += 1.f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			camera_move.y -= 1.f;
		}
		camera_position += camera_move * movement_speed * delta_time;
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float nearPlane = 0.5f, farPlane = 10.f;
		mat4 projection = perspective(radians(50.f), width / (float) height, nearPlane, farPlane);
		mat4 view = lookAt(camera_position, camera_position + camera_forward, vec3(0.f, 1.f, 0.f));
		mat4 view_projection_matrix = projection * view;

		glUniformMatrix4fv(u_ViewProjection, 1, false, value_ptr(view_projection_matrix));


		mat4 translation = translate(identity<mat4>(), vec3(0.f, -0.5f, 0.f));
		mat4 scale_matrix = scale(identity<mat4>(), vec3(1.5f));
		mat4 rotation_matrix = rotate(identity<mat4>(), radians(90.f), vec3(1.f, 0.f, 0.f));

		mat4 model_matrix = translation * rotation_matrix * scale_matrix;

		glUniformMatrix4fv(u_Model, 1, false, value_ptr(model_matrix));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		int time_seconds = (int)time;

		translation = translate(identity<mat4>(), vec3(0.f));
		scale_matrix = scale(identity<mat4>(), vec3(1.f));
		rotation_matrix = rotate(identity<mat4>(), radians(time * 90.f * 0), vec3(0.f, 1.f, 0.f));

		model_matrix = translation * rotation_matrix * scale_matrix;

		glUniformMatrix4fv(u_Model, 1, false, value_ptr(model_matrix));
	

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}
