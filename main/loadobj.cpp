#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

SimpleMeshData load_wavefront_obj(char const* aPath)
{
	//get rapidOBJ to load the file at requested path
	auto result = rapidobj::ParseFile(aPath);

	if (result.error)
	{
		throw Error("Failed to load OBJ file: '%s' \nError: '%s'", aPath, result.error.code.message().c_str());
	}

	//we must triangulate all faces that aren't triangles since the most complex shape openGL can draw is a triangle
	rapidobj::Triangulate(result);

	SimpleMeshData ret;

	for (auto const& shape : result.shapes) //all of the results in the shape
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i) //all of the indices in the shape
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			//get the face index by dividing by 3 since each face has 3 indices
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];

			//replace the material ambient colour for each vertex
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0] + mat.diffuse[0],
				mat.ambient[1] + mat.diffuse[1],
				mat.ambient[2] + mat.diffuse[2]
				});

			
			//load the texture from the file in here 
			ret.textureCoords.emplace_back(Vec2f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1]
				});


		}
	}

	return ret;
}

