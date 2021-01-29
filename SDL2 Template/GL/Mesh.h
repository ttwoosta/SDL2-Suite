//
//  World.hpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "OpenGL.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Shader.h"

#include "glm/glm.hpp"

namespace gl {
	class Mesh {
	public:
		enum Assembly : GLenum {
			Points = GL_POINTS, 
			LineStrip = GL_LINE_STRIP, 
			LineLoop = GL_LINE_LOOP, 
			Lines = GL_LINES, 
			TriangleStrip = GL_TRIANGLE_STRIP, 
			TriangleFan = GL_TRIANGLE_FAN, 
			Triangles = GL_TRIANGLES, 
			Patches = GL_PATCHES
		};
		struct SubMesh {
			Assembly mode;
			Size start;
			Size count;
		};
		Mesh(const std::string& filename);
		void Render() const;
		void Render(std::size_t index) const;
	private:
		Vertex::Array vertices;
		TypeCode elementType;
		Size elementSize;
		std::vector<SubMesh> surfaces;
	};

	class Object {
	public:
		Object(Mesh*&& mesh, Program& program);
		Object(std::shared_ptr<const Mesh> mesh, Program& program);

		void Render() const;

		void Rotate(float angle, Vector3 axis);
		void Translate(Vector3 distance);
		void Scale(Vector3 distance);
		void PreRotate(float angle, Vector3 axis);
		void PreTranslate(Vector3 distance);
		void PreScale(Vector3 distance);

		ColorAlpha color;
		ColorAlpha highlight;
	protected:
		std::shared_ptr<const Mesh> _mesh;
		Program& _program;
		Matrix4 _transform;
	};
}
