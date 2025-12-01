#include "mesh.hpp"
#include "graphics.hpp"
#include "shader.hpp"

using namespace graphics;

Shader *Mesh::shader;
Shader *Mesh::phongShader;
GLuint Mesh::UBO_Shared_Camera;
GLuint Mesh::UBO_Shared_Light;
Mesh::GPU_UBO_LIGHT Mesh::UBO_Light_Data;
GLuint Mesh::defaultTexture;

// Static storage for Phong uniforms
Mesh::MATERIAL Mesh::pMaterial;
bool Mesh::pEnable = false;

void Mesh::initialize()
{
	Mesh::shader = new Shader("../shader/mesh.vs", "../shader/mesh.fs");
	Mesh::phongShader = new Shader("../shader/phong.vs", "../shader/phong.fs");

	glCreateBuffers(1, &Mesh::UBO_Shared_Camera);
	glNamedBufferData(Mesh::UBO_Shared_Camera, sizeof(Mesh::GPU_UBO_CAMERA), nullptr, GL_STREAM_DRAW);
	glClearNamedBufferData(Mesh::UBO_Shared_Camera, GL_R32F, GL_RED, GL_FLOAT, nullptr); // memset to zero

	glCreateBuffers(1, &Mesh::UBO_Shared_Light);
	glNamedBufferData(Mesh::UBO_Shared_Light, sizeof(Mesh::GPU_UBO_LIGHT), nullptr, GL_STREAM_DRAW);
	glClearNamedBufferData(Mesh::UBO_Shared_Light, GL_R32F, GL_RED, GL_FLOAT, nullptr); // memset to zero

	Mesh::UBO_Light_Data.CurrentCount = 0;

	unsigned char whitePixel[] = { 255, 255, 255, 255 };
	glCreateTextures(GL_TEXTURE_2D, 1, &Mesh::defaultTexture);
	glTextureStorage2D(Mesh::defaultTexture, 1, GL_RGBA8, 1, 1);
	glTextureSubImage2D(Mesh::defaultTexture, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
}

void Mesh::finalize()
{
	glDeleteTextures(1, &Mesh::defaultTexture);
	glDeleteBuffers(1, &Mesh::UBO_Shared_Camera);

	delete Mesh::shader;

	if (Mesh::phongShader)
	{
		delete Mesh::phongShader;
		Mesh::phongShader = nullptr;
	}
}

int Mesh::addLight(glm::vec3 position, glm::vec3 color, glm::vec3 attenuation)
{
	if (Mesh::UBO_Light_Data.CurrentCount >= 8)
		return Mesh::UBO_Light_Data.CurrentCount;

	Mesh::UBO_Light_Data.Light[Mesh::UBO_Light_Data.CurrentCount].position    = glm::vec4(position, 0.0f);
	Mesh::UBO_Light_Data.Light[Mesh::UBO_Light_Data.CurrentCount].color       = glm::vec4(color, 0.0f);
	Mesh::UBO_Light_Data.Light[Mesh::UBO_Light_Data.CurrentCount].attenuation = glm::vec4(attenuation, 0.0f);

	return Mesh::UBO_Light_Data.CurrentCount++;
}

void Mesh::removeLight(int index)
{
	if (index >= Mesh::UBO_Light_Data.CurrentCount or index < 0)
		return;

	// Move last light to the removed spot
	if (index != Mesh::UBO_Light_Data.CurrentCount - 1)
		Mesh::UBO_Light_Data.Light[index] = Mesh::UBO_Light_Data.Light[Mesh::UBO_Light_Data.CurrentCount - 1];

	Mesh::UBO_Light_Data.CurrentCount--;
}

Mesh::Mesh(const char *file, Texture *texture, unsigned int maxInstances)
: texture(texture), VAO(0), VBO(0), EBO(0), SSBO(0), SSBO_Data(nullptr), maxInstances(maxInstances), currentInstance(0), indexCount(0), internalTexture(false)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = false;
	std::string fileStr(file);

	if (fileStr.substr(fileStr.find_last_of(".") + 1) == "glb")
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, file);
	else
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, file);

	if (!warn.empty())
		printf("Warn: %s\n", warn.c_str());

	if (!err.empty())
		printf("Err: %s\n", err.c_str());

	if (!ret)
	{
		printf("Failed to parse glTF\n");
		return;
	}

	std::vector<Vertex3D> vertices;
	std::vector<unsigned int> indices;

	// Iterate through meshes
	for (const auto& mesh : model.meshes)
	{
		for (const auto& primitive : mesh.primitives)
		{
			// Helper to get data pointer and stride
			auto getAttribute = [&](const std::string& name, const tinygltf::Accessor** outAccessor, int* outStride) -> const unsigned char*
			{
				if (primitive.attributes.find(name) == primitive.attributes.end())
					return nullptr;

				*outAccessor = &model.accessors[primitive.attributes.at(name)];
				const auto& bufferView = model.bufferViews[(*outAccessor)->bufferView];
				*outStride = bufferView.byteStride ? bufferView.byteStride : tinygltf::GetComponentSizeInBytes((*outAccessor)->componentType) * tinygltf::GetNumComponentsInType((*outAccessor)->type);
				return &model.buffers[bufferView.buffer].data[(*outAccessor)->byteOffset + bufferView.byteOffset];
			};

			const tinygltf::Accessor* posAccessor = nullptr;
			const tinygltf::Accessor* normAccessor = nullptr;
			const tinygltf::Accessor* uvAccessor = nullptr;
			const tinygltf::Accessor* colAccessor = nullptr;

			int posStride = 0, normStride = 0, uvStride = 0, colStride = 0;

			const unsigned char* posData = getAttribute("POSITION", &posAccessor, &posStride);
			const unsigned char* normData = getAttribute("NORMAL", &normAccessor, &normStride);
			const unsigned char* uvData = getAttribute("TEXCOORD_0", &uvAccessor, &uvStride);
			const unsigned char* colData = getAttribute("COLOR_0", &colAccessor, &colStride);

			// Indices
			if (primitive.indices >= 0)
			{
				const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				const unsigned char* buffer = &(model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);

				for (unsigned int i = 0; i < accessor.count; ++i)
				{
					unsigned int index = 0;
					if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
						index = (unsigned int)((const unsigned short*)buffer)[i];
					else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
						index = ((const unsigned int*)buffer)[i];
					else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
						index = (unsigned int)((const unsigned char*)buffer)[i];
					indices.push_back(index + vertices.size());
				}
			}

			unsigned int vertexCount = 0;
			
			if (posAccessor)
				vertexCount = posAccessor->count;

			// Append vertices
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				Vertex3D v;
				
				// Position
				if (posData)
				{
					const float* p = (float*)(posData + i * posStride);
					v.position = glm::vec3(p[0], p[1], p[2]);
				}

				// Normal
				if (normData)
				{
					const float* n = (float*)(normData + i * normStride);
					v.normal = glm::vec3(n[0], n[1], n[2]);
				}

				// UV
				if (uvData)
				{
					const float* u = (float*)(uvData + i * uvStride);
					v.uv = glm::vec2(u[0], u[1]);
				}

				// Color
				if (colData)
				{
					const unsigned char* c = colData + i * colStride;
					if (colAccessor->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
					{
						const float* cf = (float*)(c);
						if (colAccessor->type == TINYGLTF_TYPE_VEC4)
							v.color = glm::vec4(cf[0], cf[1], cf[2], cf[3]);
						else if (colAccessor->type == TINYGLTF_TYPE_VEC3)
							v.color = glm::vec4(cf[0], cf[1], cf[2], 1.0f);
					}
					else if (colAccessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						// Normalized unsigned byte
						float scale = 1.0f / 255.0f;
						if (colAccessor->type == TINYGLTF_TYPE_VEC4)
							v.color = glm::vec4(c[0] * scale, c[1] * scale, c[2] * scale, c[3] * scale);
						else if (colAccessor->type == TINYGLTF_TYPE_VEC3)
							v.color = glm::vec4(c[0] * scale, c[1] * scale, c[2] * scale, 1.0f);
					}
					else if (colAccessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						// Normalized unsigned short
						float scale = 1.0f / 65535.0f;
						const unsigned short* us = (unsigned short*)(c);
						if (colAccessor->type == TINYGLTF_TYPE_VEC4)
							v.color = glm::vec4(us[0] * scale, us[1] * scale, us[2] * scale, us[3] * scale);
						else if (colAccessor->type == TINYGLTF_TYPE_VEC3)
							v.color = glm::vec4(us[0] * scale, us[1] * scale, us[2] * scale, 1.0f);
					}
				}
				else
					v.color = glm::vec4(1.0f);

				vertices.push_back(v);
			}
			
			// Load texture if present and we don't have one
			if (this->texture == nullptr && primitive.material >= 0)
			{
				const auto& mat = model.materials[primitive.material];
				if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0)
				{
					const auto& tex = model.textures[mat.pbrMetallicRoughness.baseColorTexture.index];
					if (tex.source >= 0)
					{
						const auto& img = model.images[tex.source];
						
						if (!img.image.empty())
						{
							// Create texture from image data	
							this->texture = new Texture(img.image.data(), img.width, img.height, 1);
							this->internalTexture = true;
						}
					}
				}
			}
		}
	}

	indexCount = indices.size();

	glCreateVertexArrays(1, &VAO);

	glCreateBuffers(1, &VBO);
	glNamedBufferStorage(VBO, sizeof(Vertex3D) * vertices.size(), vertices.data(), 0);

	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, sizeof(unsigned int) * indices.size(), indices.data(), 0);

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

	if (internalTexture && texture)
		delete texture;
}

