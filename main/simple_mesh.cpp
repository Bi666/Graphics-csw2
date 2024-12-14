#include "simple_mesh.hpp"

SimpleMeshData concatenate( SimpleMeshData aM, SimpleMeshData const& aN )
{
	aM.positions.insert( aM.positions.end(), aN.positions.begin(), aN.positions.end() );
	aM.colors.insert( aM.colors.end(), aN.colors.begin(), aN.colors.end() );
	aM.textureCoords.insert(aM.textureCoords.end(), aN.textureCoords.begin(), aN.textureCoords.end());
	return aM;
}


GLuint create_vao( SimpleMeshData const& aMeshData )
{
	GLuint positionsbVBO = 0;
	GLuint colorsVBO = 0;
	GLuint normalsVBO = 0;
	GLuint textureCoordVBO = 0;
	GLuint vao = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &positionsbVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsbVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &colorsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &textureCoordVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.textureCoords.size() * sizeof(Vec2f), aMeshData.textureCoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glDeleteBuffers(1, &positionsbVBO);
	//glDeleteBuffers(1, &colorsVBO);
	//glDeleteBuffers(1, &normalsVBO);

	glGetError();

	return vao;
}

