#include "model.hpp"

#include "graphics.hpp"

using namespace graphics;
using namespace glm;

Model::Model(Mesh *mesh, Mesh::MATERIAL material)
: mesh(mesh), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f), material(material)
{
	updateModel();
}

void Model::updateModel()
{
	modelMatrix = mat4(1.0f);
	modelMatrix = translate(modelMatrix, position);
	modelMatrix = rotate(modelMatrix, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = rotate(modelMatrix, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, scale);
}

void Model::batch()
{
	if (mesh->currentInstance >= mesh->maxInstances) return;

	mesh->SSBO_Data[mesh->currentInstance++] =
	{
		modelMatrix,
		color
	};
}

void Model::draw()
{
	if (mesh->currentInstance == 0)
		return;

	Mesh::shader->use();
	GLuint prog = Mesh::shader->program;

	Mesh::UBO_Light_Data.Material = material;
	glBindBuffer(GL_UNIFORM_BUFFER, Mesh::UBO_Shared_Light);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mesh::GPU_UBO_LIGHT), &Mesh::UBO_Light_Data);

	// Material
	glUniform1f(glGetUniformLocation(prog, "uTime"), glfwGetTime());

	Graphics::setVAO(mesh->VAO);

	if (mesh->texture)
		Graphics::setTexture(mesh->texture->id);
	else
		Graphics::setTexture(Mesh::defaultTexture);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Mesh::GPU_SSBO) * mesh->currentInstance, mesh->SSBO_Data);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Mesh::UBO_Shared_Camera);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, Mesh::UBO_Shared_Light);

	glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0, mesh->currentInstance);
	mesh->currentInstance = 0;
}

