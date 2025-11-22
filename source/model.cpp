#include "model.hpp"

using namespace graphics;
using namespace glm;

Model::Model(Mesh *mesh)
: mesh(mesh), position(0.0f), rotation(0.0f), scale(1.0f), color(1.0f)
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
