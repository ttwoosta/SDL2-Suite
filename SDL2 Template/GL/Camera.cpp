//
//  Texture.cpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace gl {

	Camera::Camera(const Matrix4& projection, Matrix4 view)
	{
		Reserve(StaticDraw, sizeof(View));
		auto link = Access<View>();
		link->facing = view;
		link->projection = projection;
	}
	Camera& Camera::operator<<(const Program& pr)&
	{
		pr.Activate();
		Activate(pr["view"]);
		return *this;
	}
	const Camera& Camera::operator<<(const Program& pr) const&
	{
		pr.Activate();
		Activate(pr["view"]);
		return *this;
	}
	Camera& Camera::operator<<(const Vertex::Array& vao)&
	{
		vao.Activate();
		return *this;
	}
	const Camera& Camera::operator<<(const Vertex::Array& vao) const &
	{
		vao.Activate();
		return *this;
	}
	Camera& Camera::operator<<(Vector3 displacement)
	{
		auto data = Access<View>();
		data->facing = glm::translate(Matrix4{}, displacement) * data->facing;
		return *this;
	}
}
