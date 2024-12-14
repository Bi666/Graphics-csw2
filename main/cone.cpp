#include "cone.hpp"

SimpleMeshData make_cone( bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform )
{
	std::vector<Vec3f> pos;

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);

	if (aCapped)
	{
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			//calculate angle at current subDivision, based on number of subDivisions
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926535897932384626433832795f;
			// get y and z at current angle
			float y = std::cos(angle);
			float z = std::sin(angle);

			//first triangle of rectangle
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 0.f, y , z });
			pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			//second triangle of rectangle

			/*
			pos.emplace_back(Vec3f{ 0.f, y, z });
			pos.emplace_back(Vec3f{ 1.f, y , z });
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			*/

			//front cap
			pos.emplace_back(Vec3f{ -0.f, 0.f, 0.f });  //front cap center vertex
			pos.emplace_back(Vec3f{ -0.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ -0.f, y, z });

			//back cap center vertex
			/*
			pos.emplace_back(Vec3f{ 1.f, 0.f ,0.f });
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 1.f, y, z });
			*/
			prevY = y;
			prevZ = z;
		}
	}
	else
	{
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			//calculate angle at current subDivision, based on number of subDivisions
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926535897932384626433832795f;
			// get y and z at current angle
			float y = std::cos(angle);
			float z = std::sin(angle);

			//first triangle of rectangle
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 0.f, y , z });
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			//second triangle of rectangle
			pos.emplace_back(Vec3f{ 0.f, y, z });
			pos.emplace_back(Vec3f{ 1.f, y , z });
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });

			prevY = y;
			prevZ = z;
		}
	}
		
	//transformation of all positions
	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		p = Vec3f{ t.x, t.y, t.z };
	}

	SimpleMeshData cylinder;
	cylinder.positions = pos;
	cylinder.colors = std::vector<Vec3f>(pos.size(), aColor);
	return cylinder;
}

