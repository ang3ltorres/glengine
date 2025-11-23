#include "mesh.hpp"
#include "graphics.hpp"

using namespace graphics;

Shader *Mesh::shader;
GLuint Mesh::UBO_Shared;

void Mesh::initialize()
{
	Mesh::shader = new Shader("../shader/mesh.vs", "../shader/mesh.fs");

	glCreateBuffers(1, &Mesh::UBO_Shared);
	glNamedBufferData(Mesh::UBO_Shared, sizeof(glm::mat4), nullptr, GL_STREAM_DRAW);
}

void Mesh::finalize()
{
	glDeleteBuffers(1, &Mesh::UBO_Shared);
	delete Mesh::shader;
}

Mesh::Mesh(Vertex3D *vertices, unsigned int numVertices, unsigned int *indices, unsigned int numIndices, Texture *texture, unsigned int maxInstances)
: texture(texture), maxInstances(maxInstances), currentInstance(0), indexCount(numIndices)
{
	glCreateVertexArrays(1, &VAO);

	glCreateBuffers(1, &VBO);
	glNamedBufferStorage(VBO, sizeof(Vertex3D) * numVertices, vertices, 0);

	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, sizeof(unsigned int) * numIndices, indices, 0);

	glVertexArrayElementBuffer(VAO, EBO);

	GLuint vbufIndex = 0;
	glVertexArrayVertexBuffer(VAO, vbufIndex, VBO, 0, sizeof(Vertex3D));

	// Position
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, position));
	glVertexArrayAttribBinding(VAO, 0, vbufIndex);

	// UV
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, uv));
	glVertexArrayAttribBinding(VAO, 1, vbufIndex);

	// Normal
	glEnableVertexArrayAttrib(VAO, 2);
	glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, normal));
	glVertexArrayAttribBinding(VAO, 2, vbufIndex);

	// Color
	glEnableVertexArrayAttrib(VAO, 3);
	glVertexArrayAttribFormat(VAO, 3, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, color));
	glVertexArrayAttribBinding(VAO, 3, vbufIndex);

	// SSBO
	glCreateBuffers(1, &SSBO);
	glNamedBufferData(SSBO, sizeof(GPU_SSBO) * maxInstances, nullptr, GL_STREAM_DRAW);
	SSBO_Data = new GPU_SSBO[maxInstances];
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &SSBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	delete[] SSBO_Data;
}

