#pragma once

#include "glm/glm.hpp"

namespace SnazzCraft
{
	class VoxelVertice
	{
	public:
		glm::vec3 Position;
		glm::vec2 TextureCoordinate;
		float Brightness = 1.0f;

		VoxelVertice();

		VoxelVertice(glm::vec3 Position, glm::vec2 TextureCoordinate);

		VoxelVertice(glm::vec3 Position, glm::vec2 TextureCoordinate, float Brightness);

	private:

	
	};
} // SnazzCraft
