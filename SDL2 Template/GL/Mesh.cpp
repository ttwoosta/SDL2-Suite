//
//  World.cpp
//  GLProject1
//
//  Created by Nevin Flanagan on 1/17/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace std;

class OBJmesh {
public:
	using vertex_type = gl::Vertex;
	using index_type = gl::Ushort;
	using count_type = gl::Ushort;

	OBJmesh(const string& filename);
	const vector<vertex_type>& vData() const { return vertices; }
	const vector<index_type>& fData() const { return faces; }
	const vector<count_type>& faceSegments() const { return ranges; }
private:
	vector<vertex_type> vertices;
	vector<index_type> faces;
	vector<count_type> ranges;

	struct Index {
		gl::Short p, t, n;
		Index(const string& encoding, gl::Short pCount, gl::Short tCount, gl::Short nCount);

		bool operator==(const Index& other) const
		{
			return ((p == other.p) || (p < 0 && other.p < 0))
				&& ((t == other.t) || (t < 0 && other.t < 0))
				&& ((n == other.n) || (n < 0 && other.n < 0));
		}
	private:
		gl::Short Enter(istream& source, gl::Short limit)
		{
			gl::Short result;
			if (source.peek() != '/' && source >> result) {
				result = result < 0 ? limit - result : --result;
			}
			else {
				result = -1;
			}
			source.ignore();
			return result;
		}
	};

	class hash : public std::hash<string> {
	public:
		size_t operator() (Index const i) const {
			return std::hash<string>::operator () (string(reinterpret_cast<char const*>(&i), sizeof(Index)));
		}
	};
};

OBJmesh::Index::Index(const string& encoding, gl::Short pCount, gl::Short tCount, gl::Short nCount)
{
	istringstream description(encoding);

	p = Enter(description, pCount);
	t = Enter(description, tCount);
	n = Enter(description, nCount);
}

OBJmesh::OBJmesh(const string& filename)
{
/*
	string line;
	vector<gl::Point3> vList;
	vector<gl::Vector3> vnList;
	vector<gl::Vector2> vtList;
	unordered_map<Index, gl::Short, OBJmesh::hash> verts;
	for (ifstream source(filename); getline(source, line); ) {
		istringstream input(line);
		string command;
		input >> command;
		if (command == "v") {
			gl::Point3 v;
			input >> v.x >> v.y >> v.z;
			vList.push_back(v);
		}
		else if (command == "vt") {
			gl::Vector2 t;
			input >> t.x >> t.y;
			vtList.push_back(t);
		}
		else if (command == "vn") {
			gl::Vector3 n;
			input >> n.x >> n.y >> n.z;
			vnList.push_back(n);
		}
		else if (command == "f") {
			string corner;
			int corners = 0;
			while (input >> corner) {
				Index id(corner, vList.size(), vtList.size(), vnList.size());
				if (!verts.count(id)) {
					gl::Vertex v;
					if (id.p >= 0) v.position = vList[id.p];
					if (id.t >= 0) v.uv = vtList[id.t];
					if (id.n >= 0) v.normal = vnList[id.n];
					verts[id] = vertices.size();
					vertices.push_back(v);
				}
				corners++;
				faces.push_back(verts[id]);
			}
			ranges.push_back(corners);
		}
	}
*/
}

vector<OBJmesh::count_type> loadShape(const string& filename, gl::Vertex::Array& vertexArray)
{
	OBJmesh source{ filename };

	//gl::ArrayBuffer vbo;
	//gl::ElementArrayBuffer ebo;
	//vbo.Load(GL_STATIC_DRAW, source.vData());
	//vertexArray.Bind(vbo);

	//vertexArray.Activate();
	//ebo.Load(GL_STATIC_DRAW, source.fData());
	//vertexArray.Deactivate();

	//ebo.Release();
	//vbo.Release();

	return source.faceSegments();
}

namespace gl {
	Mesh::Mesh(const std::string& filename)
	{}

	void Mesh::Render() const 
	{
		vertices.Activate();
		GLushort position = 0;
		for (auto& surface : surfaces) {
			glDrawElements(
				surface.mode, 
				surface.count, 
				static_cast<GLenum>(elementType), 
				reinterpret_cast<void*>(surface.start * TypeAlloc[elementType])
			);
		}
		vertices.Deactivate();
	}

	Object::Object(Mesh*&& mesh, Program& prog)
		: Object{ shared_ptr<const Mesh>{mesh}, prog }
	{}

	Object::Object(std::shared_ptr<const Mesh> mesh, Program& program)
		:	_mesh { mesh }, _program {program}, color{1}, highlight {0, 0, 0, 1}
	{}

	void Object::Render() const 
	{
		_program.Activate();
		_program.Uniform<ColorAlpha>("color") = color;
		_program.Uniform<Matrix4>("transform") = _transform;
		_program.Uniform<Float>("shininess") = highlight.a;
		_program.Uniform<Color>("specular_color") = Color{ highlight };
		_mesh->Render();
	}

	void Object::Rotate(float angle, Vector3 axis) 
	{
		_transform = glm::rotate(glm::mat4{}, angle, axis) * _transform;
	}

	void Object::Translate(Vector3 distance) 
	{
		_transform = glm::translate(glm::mat4{}, distance) * _transform;
	}

	void Object::Scale(Vector3 proportion) 
	{
		_transform = glm::scale(glm::mat4{}, proportion) * _transform;
	}

	void Object::PreRotate(float angle, Vector3 axis) 
	{
		_transform = glm::rotate(_transform, angle, axis);
	}

	void Object::PreTranslate(Vector3 distance) 
	{
		_transform = glm::translate(_transform, distance);
	}

	void Object::PreScale(Vector3 proportion) 
	{
		_transform = glm::scale(_transform, proportion);
	}
}
