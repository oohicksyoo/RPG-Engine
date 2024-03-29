//
// Created by Alex on 12/13/2020.
//

#define TINYOBJLOADER_IMPLEMENTATION

#include "Assets.hpp"
#include "Log.hpp"
#include <sstream>
#include <tiny_obj_loader.h>
#include <unordered_map>
#include <vector>
#include <SDL_image.h>
#include <stdio.h>

using json = nlohmann::json;

std::string RPG::Assets::LoadTextFile(const std::string& path) {
    //TODO: Have some sort of error fallback when a file is not found
	SDL_RWops* file{SDL_RWFromFile(path.c_str(), "r")};
	size_t fileLength{static_cast<size_t>(SDL_RWsize(file))};
	void* data{SDL_LoadFile_RW(file, nullptr, 1)};
	std::string result(static_cast<char*>(data), fileLength);
	SDL_free(data);

	return result;
}

void RPG::Assets::SaveTextFile(const std::string &content, const std::string &path) {
	std::ofstream file;
	file.open(path);
	file << content << std::endl;
	file.close();
}

void RPG::Assets::RemoveFile(const std::string &path) {
	remove(path.c_str());
}

RPG::Mesh RPG::Assets::LoadOBJFile(const std::string& path) {
	std::istringstream sourceStream(RPG::Assets::LoadTextFile(path));

	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	std::string error;

	if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, &sourceStream)) {
		throw std::runtime_error("LoadOBJFile: Error: " + warning + error);
	}

	std::vector<RPG::Vertex> vertices;
	std::vector<uint32_t> indices;
	std::unordered_map<RPG::Vertex, uint32_t> uniqueVertices;

	for (const auto& shape:shapes) {
		for(const auto& index:shape.mesh.indices) {
			glm::vec3 position{
					attributes.vertices[3 * index.vertex_index + 0],
					attributes.vertices[3 * index.vertex_index + 1],
					attributes.vertices[3 * index.vertex_index + 2]
			};

			glm::vec2 texCoord{
					attributes.texcoords[2 * index.texcoord_index + 0],
					1.0f - attributes.texcoords[2 * index.texcoord_index + 1]
			};

            glm::vec3 normal{
                    attributes.normals[3 * index.normal_index + 0],
                    attributes.normals[3 * index.normal_index + 1],
                    attributes.normals[3 * index.normal_index + 2]
            };

			RPG::Vertex vertex{position, texCoord, normal};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	return RPG::Mesh{vertices, indices};
}

RPG::Texture RPG::Assets::LoadBitmapFile(const std::string& path) {
	SDL_RWops* file{SDL_RWFromFile(path.c_str(), "rb")};
	SDL_Surface* source{IMG_Load_RW(file, 1)};
	SDL_Rect imageFrame{0, 0, source->w, source->h};

	uint32_t redMask;
	uint32_t greenMask;
	uint32_t blueMask;
	uint32_t alphaMask;

	#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		redMask = 0xff000000;
		greenMask = 0x00ff0000;
		blueMask = 0x0000ff00;
		alphaMask = 0x000000ff;
	#else
		redMask = 0x000000ff;
		greenMask = 0x0000ff00;
		blueMask = 0x00ff0000;
		alphaMask = 0xff000000;
	#endif

	SDL_Surface* target{SDL_CreateRGBSurface(
			0,
			imageFrame.w,
			imageFrame.h,
			32,
			redMask,
			greenMask,
			blueMask,
			alphaMask
	)};

	SDL_BlitSurface(source, &imageFrame, target, &imageFrame);

	SDL_FreeSurface(source);

	return RPG::Texture(target, path);
}

std::vector<char> RPG::Assets::LoadBinaryFile(const std::string& path) {
	// Open a file operation handle to the asset file.
	SDL_RWops* file{SDL_RWFromFile(path.c_str(), "rb")};

	// Determine how big the file is.
	size_t fileLength{static_cast<size_t>(SDL_RWsize(file))};

	// Ask SDL to load the content of the file into a data pointer.
	char* data{static_cast<char*>(SDL_LoadFile_RW(file, nullptr, 1))};

	// Make a copy of the data as a vector of characters.
	std::vector<char> result(data, data + fileLength);

	// Let SDL free the data memory (we took a copy into a vector).
	SDL_free(data);

	// Hand back the resulting vector which is the content of the file.
	return result;
}

RPG::Material RPG::Assets::LoadMaterial(const std::string &path) {
    auto text = LoadTextFile(path);
    json j = json::parse(text);
    auto name = j["Name"].get<std::string>();
    auto renderQueue = j["RenderQueue"].get<int>();
    auto shader = j["Shader"].get<std::string>();

    RPG::Material material = RPG::Material(name, renderQueue, shader);

    if (j.contains("Properties")) {
        std::vector<std::shared_ptr<RPG::Property>> properties;

        for (auto [key, value] : j["Properties"].items()) {
            auto property = RPG::Assets::LoadProperty(value);
            if (property != nullptr) {
                properties.push_back(property);
            }
        }

        material.SetProperties(properties);
    }

    return material;
}

std::shared_ptr<RPG::Property> RPG::Assets::LoadProperty(nlohmann::json j) {
    auto type = j["Type"].get<std::string>();
    auto v = j["Value"];
    std::any p;

    if (type == "float") {
        float vec = v.get<float>();
        return std::make_unique<RPG::Property>(vec, j["Name"].get<std::string>(), type);
    }

    if (type == "glm::vec2") {
        glm::vec2 vec = glm::vec2{v["x"].get<float>(),v["y"].get<float>()};
        return std::make_unique<RPG::Property>(vec, j["Name"].get<std::string>(), type);
    }

    if (type == "glm::vec3") {
        glm::vec3 vec = glm::vec3{v["x"].get<float>(),v["y"].get<float>(),v["z"].get<float>()};
        return std::make_unique<RPG::Property>(vec, j["Name"].get<std::string>(), type);
    }

    if (type == "glm::vec4") {
        glm::vec4 vec = glm::vec4{v["x"].get<float>(),v["y"].get<float>(),v["z"].get<float>(),v["w"].get<float>()};
        return std::make_unique<RPG::Property>(vec, j["Name"].get<std::string>(), type);
    }

    if (type == "RPG::Resource::String") {
        return std::make_unique<RPG::Property>(v.get<std::string>(), j["Name"].get<std::string>(), "RPG::Resource::String", true, "Texture");
    }

    return nullptr;
}
