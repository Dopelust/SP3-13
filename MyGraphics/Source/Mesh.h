#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include "vertex.h"
#include "Material.h"
#include "GL\glew.h"
#include "Mtx44.h"

/******************************************************************************/
/*!
		Class Mesh:
\brief	To store VBO (vertex & color buffer) and IBO (index buffer)
*/
/******************************************************************************/

class Mesh
{
public:
	enum DRAW_MODE
	{
		DRAW_TRIANGLES, //default mode
		DRAW_TRIANGLE_STRIP,
		DRAW_LINES,
		DRAW_MODE_LAST,
	};

	Mesh(const std::string &meshName, std::vector<Vertex> vertex_buffer_data, std::vector<GLuint> index_buffer_data);
	virtual ~Mesh();
	virtual void Render();
	void Render(unsigned NumInstance, const Mtx44* MMat);
	void Render(unsigned NumInstance, const Mtx44* MMat, const TexCoord* offset);
	void Render(unsigned NumInstance, const Mtx44* MMat, unsigned offset, unsigned count);

	Material material;
	unsigned textureID;
	const std::string name;
	DRAW_MODE mode;
	unsigned vertexBuffer;
	unsigned indexBuffer;
	unsigned indexSize;
	unsigned MBuffer;
	unsigned texOffset;

	GLuint VAO;
};

#endif