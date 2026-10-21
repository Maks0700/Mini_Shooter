#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "..\utils.h"
#include "..\global.h"

#include "render.h"
#include "render_internal.h"


SDL_Window* render_init_window(u32 width, u32 height)//fucntion returned pointer SDL_Window
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ERROR_EXIT(SDL_GetError());
		exit(1);
	}
	

	SDL_Window* window = SDL_CreateWindow(
		"Shooter", width, height,
		SDL_WINDOW_OPENGL
	);
	
	if (!window)
	{
		ERROR_EXIT(SDL_GetError());
		exit(1);
	}
	SDL_GL_CreateContext(window);

	

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		ERROR_EXIT(SDL_GetError());
		exit(1);
	}
	return window;
}

void render_init_shaders(Render_State_Internal* state)
{
	state->shader_default = render_shader_create("engine/shaders_scratch/vertex_shade.vert",
		"engine/shaders_scratch/frag_shade.frag");

	mat4x4_ortho(state->projection, 0, global.render.width, 0, global.render.height, -2, 2);
	glUseProgram(state->shader_default);
	glUniformMatrix4fv(
		glGetUniformLocation(state->shader_default, "projection"),
		1,
		GL_FALSE,
		&state->projection[0][0]
	);

}

void render_init_color_texture(u32* texture)
{
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	u8 solid_white[4] = { 255, 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, solid_white);

	glBindTexture(GL_TEXTURE_2D, 0);
}



void render_init_quad(u32* vao, u32* vbo, u32* ebo)

{
	//x y z u v
	f32 vertices[] = { 
		0.5f,  0.5f, 0.0f, 0.f, 0.f,
		 0.5f, -0.5f, 0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.f, 1.f, 1.f,
		-0.5f,  0.5f, 0.f, 1.f, 0.f

	};
	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glGenBuffers(1, ebo);


	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//ebo(ibo) reuse vertices and the same vertices is not load in memory
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices), indices, GL_STATIC_DRAW);

	//xyz
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), NULL);
	glEnableVertexAttribArray(0);//void

	// uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));//void
	glEnableVertexAttribArray(1);
	
	//reset vertexArray
	glBindVertexArray(0);
}
