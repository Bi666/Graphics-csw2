#include "texture.hpp"

#include <cassert>

#include <stb_image.h>

#include "../support/error.hpp"

GLuint load_texture_2d( char const* aPath )
{
	assert( aPath );

	stbi_set_flip_vertically_on_load(true);

	int w, h, channels;
	stbi_uc* ptr = stbi_load( aPath, &w, &h, &channels, 4 );
	if( !ptr )
		throw Error( "Unable to load image \"%s\"", aPath );

	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//target texture/ level of detail /colour components/ width/ height/ border/ format/ type/ data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

	stbi_image_free(ptr);

	//generates a complete set of mipmaps - for better rednering at different distances and angles 
	glGenerateMipmap(GL_TEXTURE_2D);

	//for when you zoom in and zoom out 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	//prevents seams and repeating patterns at the edges 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	//filtering for when viewing at oblique angles 
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 6.f );

	return texture;
}