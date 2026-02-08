#pragma once

#include "../../../../includes/glm/glm.hpp"

namespace SnazzCraft
{
	class Vertice3D
	{
	public:
		glm::vec3 Position;
		glm::vec2 TextureCoordinate;
		float Brightness = 1.0f;

		Vertice3D();

		Vertice3D(glm::vec3 Position, glm::vec2 TextureCoordinate);

		Vertice3D(glm::vec3 Position, glm::vec2 TextureCoordinate, float Brightness);

	private:

	
	};
}
