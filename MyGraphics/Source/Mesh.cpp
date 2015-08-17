
#include "Mesh.h"
#include "vertex.h"

/******************************************************************************/
/*!
\brief
Default constructor - generate VBO/IBO here

\param meshName - name of mesh
*/
/******************************************************************************/
Mesh::Mesh(const std::string &meshName, std::vector<Vertex> vertex_buffer_data, std::vector<GLuint> index_buffer_data)
	: name(meshName)
	, mode(DRAW_TRIANGLES)
{
	// Generate buffers
	textureID = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Position));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Position) + sizeof(Color) + sizeof(Vector3)));

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size()  * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &MBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, MBuffer);

	for (unsigned int i = 0; i < 4; i++) 
	{
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mtx44), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(4 + i, 1);
	}

	glGenBuffers(1, &texOffset);
	glBindBuffer(GL_ARRAY_BUFFER, texOffset);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoord), &TexCoord(0,0), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(TexCoord), (void*)0);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);
}

/******************************************************************************/
/*!
\brief
Destructor - delete VBO/IBO here
*/
/******************************************************************************/
Mesh::~Mesh()
{
	if (textureID > 0)
		glDeleteTextures(1, &textureID);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

/******************************************************************************/
/*!
\brief
OpenGL render code
*/
/******************************************************************************/
void Mesh::Render()
{
	glBindVertexArray(VAO);

	if (mode == DRAW_TRIANGLES)
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
		if (mode == DRAW_TRIANGLE_STRIP)
		glDrawElements(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0);
	if (mode == DRAW_LINES)
		glDrawElements(GL_LINES, indexSize, GL_UNSIGNED_INT, 0);
}

void Mesh::Render(unsigned NumInstance, const Mtx44* MMat)
{
	glBindBuffer(GL_ARRAY_BUFFER, MBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Mtx44) * NumInstance, MMat, GL_DYNAMIC_DRAW);

	glBindVertexArray(VAO);

	if (mode == DRAW_TRIANGLES)
		glDrawElementsInstanced(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0, NumInstance);
	if (mode == DRAW_TRIANGLE_STRIP)
		glDrawElementsInstanced(GL_TRIANGLE_STRIP, indexSize, GL_UNSIGNED_INT, 0, NumInstance);
	if (mode == DRAW_LINES)
		glDrawElementsInstanced(GL_LINES, indexSize, GL_UNSIGNED_INT, 0, NumInstance);
}

void Mesh::Render(unsigned NumInstance, const Mtx44* MMat, const TexCoord* offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, texOffset);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoord) * NumInstance, offset, GL_DYNAMIC_DRAW);

	Render(NumInstance, MMat);
}

void Mesh::Render(unsigned NumInstance, const Mtx44* MMat, unsigned offset, unsigned count)
{
	glBindBuffer(GL_ARRAY_BUFFER, MBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Mtx44) * NumInstance, MMat, GL_DYNAMIC_DRAW);

	glBindVertexArray(VAO);

	if (mode == DRAW_LINES)
		glDrawElementsInstanced(GL_LINES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)), NumInstance);
	else if (mode == DRAW_TRIANGLE_STRIP)
		glDrawElementsInstanced(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)), NumInstance);
	else
		glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)), NumInstance);
}

