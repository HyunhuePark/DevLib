#include "../../include/GL/DevLibGLUtility.hpp"

#include <iomanip>

#include "../../include/IO/CFile.hpp"
#include "../../include/Visualization/CImageViewer.hpp"


namespace DevLib
{
	namespace GL
	{
		void LoadMTLFile(const std::string& mtlName, CGlModel& model)
		{
			IO::CFile file;


			if (file.Open(mtlName, "rt"))
			{
				const auto nSizeFile = file.GetFileSize();

				while (file.GetPosition() < nSizeFile)
				{
					const auto line = file.ReadStringLine();

					std::vector<std::string> vLine;
					std::string value;
					std::stringstream ss(line);

					while (std::getline(ss, value, ' '))
					{
						if (!value.empty()) vLine.emplace_back(value);
					}

					if (!vLine.empty())
					{
						/////////////////////////////////////////////////
						/// https://timeboxstory.tistory.com/87
						// Material
						if (vLine[0] == "newmtl") // New Material
						{
							model.Materials().emplace_back(vLine[1]);
						}
						else if (vLine[0] == "Ns") // 
						{
						}
						else if (vLine[0] == "d") // Transparent
						{
							model.Materials()[model.Materials().size() - 1].SetTransparent(std::stof(vLine[1]));
						}
						else if (vLine[0] == "Tr") // 
						{

						}
						else if (vLine[0] == "Tf") // 
						{

						}
						else if (vLine[0] == "illum") // 0 ~ 10
						{

						}
						else if (vLine[0] == "Ka") // Ambient 
						{
							model.Materials()[model.Materials().size() - 1].SetAmbient(std::stof(vLine[1]), std::stof(vLine[2]), std::stof(vLine[3]), 1.0f);
						}
						else if (vLine[0] == "Kd") // Diffuse Color
						{
							model.Materials()[model.Materials().size() - 1].SetDiffuse(std::stof(vLine[1]), std::stof(vLine[2]), std::stof(vLine[3]), 1.0f);
						}
						else if (vLine[0] == "Ks") // Specular Color
						{
							model.Materials()[model.Materials().size() - 1].SetSpecular(std::stof(vLine[1]), std::stof(vLine[2]), std::stof(vLine[3]), 1.0f);
						}
						else if (vLine[0] == "Ke") // Emission Color
						{
							model.Materials()[model.Materials().size() - 1].SetEmission(std::stof(vLine[1]), std::stof(vLine[2]), std::stof(vLine[3]), 1.0f);
						}
						else if (vLine[0] == "map_Kd") // Diffuse GlTexture map
						{
							const auto pos = mtlName.rfind('/');
							std::string texturePath = mtlName.substr(0, pos + 1) + vLine[1];

							model.Materials()[model.Materials().size() - 1].GetTexture().LoadImageFile(texturePath);
						}
						else if (vLine[0] == "map_Ks") // Specular GlTexture map
						{

						}
						else if (vLine[0] == "map_Ka") // Ambient GlTexture map
						{

						}
						else if (vLine[0] == "map_Ns") // Specular Highlight map
						{

						}
						else if (vLine[0] == "map_bump") // bump map ( normal map )
						{

						}
						else if (vLine[0] == "map_d") // Alpha GlTexture map
						{

						}
						else if (vLine[0] == "map_bump") // bump map ( normal map )
						{

						}
					}
				}
			}
		}

		CGlModel LoadObjFile(const string_t& path)
		{
			CGlModel model;

			IO::CFile file;

			if (file.Open(path, "rt"))
			{
				DevLib::GL::GlTextureCoords textureCoords;
				DevLib::GL::GlNormals		normals;
				DevLib::GL::GlVertices    vertices;

				auto nSizeFile = file.GetFileSize();

				while (file.GetPosition() < nSizeFile)
				{
					const auto line = file.ReadStringLine();

					std::vector<std::string> vLine;
					std::string value;
					std::stringstream ss(line);

					while (std::getline(ss, value, ' '))
					{
						if (!value.empty()) vLine.emplace_back(value);
					}

					if (!vLine.empty())
					{
						////////////////////////////////////////////////
						/// https://teraphonia.tistory.com/435 
						// Vertex Data
						if (vLine[0] == "v") // Geometric GlVertices
						{
							vertices.emplace_back(std::stof(vLine[1]), std::stof(vLine[2]), std::stof(vLine[3]));
						}
						else if (vLine[0] == "vt") // GlTexture vertices
						{
							textureCoords.emplace_back(std::stof(vLine[1]), 1.F - std::stof(vLine[2]));
						}
						else if (vLine[0] == "vn") // Vertex normals
						{
							normals.emplace_back(std::stof(vLine[1]), std::stof(vLine[2]), std::stof(vLine[3]));
						}
						else if (vLine[0] == "vp") // Parameter space vertices
						{

						}
						// Free - form curve / surface attributes :
						else if (vLine[0] == "deg") // Degree
						{

						}
						else if (vLine[0] == "bmat") // Basis matrix
						{

						}
						else if (vLine[0] == "step") // Step size
						{

						}
						else if (vLine[0] == "cstype") // Curve or surface type
						{

						}
						// Elements
						else if (vLine[0] == "p") // Point
						{

						}
						else if (vLine[0] == "l") // Line
						{

						}
						else if (vLine[0] == "f") // Face
						{
							// face : v/vt/vn
							// Vertex/VertexTexture/VertexNormal
							vLine.erase(vLine.begin());

							GlIndices tempFace;
							GlIndices tempTexture;
							GlIndices tempNormal;

							for (auto& face : vLine)
							{
								std::vector<std::string> vFace;
								std::stringstream ssFace(face);

								while (std::getline(ssFace, value, '/'))
								{
									vFace.emplace_back(value);
								}

								switch (vFace.size())
								{
								case 1:
									tempFace.emplace_back(std::stoi(vFace[0]) - 1);
									break;
								case 2:
									tempFace.emplace_back(std::stoi(vFace[0]) - 1);
									tempTexture.emplace_back(std::stoi(vFace[1]) - 1);
									break;
								case 3:
									tempFace.emplace_back(std::stoi(vFace[0]) - 1);
									if (!vFace[1].empty())tempTexture.emplace_back(std::stoi(vFace[1]) - 1);
									tempNormal.emplace_back(std::stoi(vFace[2]) - 1);
									break;
								default: ;
								}
							}
							
							if (tempFace.size() >= 3)
							{
								if (!model.Objects().empty())
								{
									auto& object = model.Objects()[model.Objects().size() - 1];

									for (uint32_t nFace = 1; nFace < tempFace.size() - 1; nFace++)
									{
										//#define DEBUG_MESH
#ifdef DEBUG_MESH
										// Triangle 1
										object.AddPoint(vertices[tempFace[0]]);
										object.AddPoint(vertices[tempFace[nFace]]);

										object.AddPoint(vertices[tempFace[nFace]]);
										object.AddPoint(vertices[tempFace[nFace + 1]]);

										object.AddPoint(vertices[tempFace[nFace + 1]]);
										object.AddPoint(vertices[tempFace[0]]);
#else

										object.AddPoint(vertices[tempFace[0]]);
										object.AddPoint(vertices[tempFace[nFace]]);
										object.AddPoint(vertices[tempFace[nFace + 1]]);

										// Sync Point sequence
										if (!tempTexture.empty())
										{
											object.AddTextureCoord(textureCoords[tempTexture[0]]);
											object.AddTextureCoord(textureCoords[tempTexture[nFace]]);
											object.AddTextureCoord(textureCoords[tempTexture[nFace + 1]]);
										}

										if (!tempNormal.empty())
										{
											object.AddNormal(normals[tempNormal[0]]);
											object.AddNormal(normals[tempNormal[nFace]]);
											object.AddNormal(normals[tempNormal[nFace + 1]]);
										}
#endif
									}
								}
							}
						}
						else if (vLine[0] == "curv") // Curve
						{

						}
						else if (vLine[0] == "curv2") // 2D curve
						{

						}
						else if (vLine[0] == "surf") // Surface
						{

						}
						// Free-form curve/surface body statements:
						else if (vLine[0] == "parm") // Parameter values
						{

						}
						else if (vLine[0] == "trim") // Outer trimming loop
						{

						}
						else if (vLine[0] == "hole") // Inner trimming loop
						{

						}
						else if (vLine[0] == "scrv") // Special curve
						{

						}
						else if (vLine[0] == "sp") // Special point
						{

						}
						else if (vLine[0] == "end") // End statement
						{

						}
						// Connectivity between free - form surfaces :
						else if (vLine[0] == "con") // Connect
						{

						}
						// Grouping:
						else if (vLine[0] == "g") // Group name
						{
#ifdef DEBUG_MESH
							if (vLine.size() > 1)
							{
								model.Objects().emplace_back(DevLib::GL::GlObjectType::GlLines, vLine[1]);

								// Enable Lightning
								model.Objects()[model.Objects().size() - 1].EnableLightning();
							}
#else

							if (vLine.size() > 1)
							{
								model.Objects().emplace_back(DevLib::GL::GlObjectType::GlTriangles, vLine[1]);

								// Enable Lightning
								model.Objects()[model.Objects().size() - 1].EnableLightning();
							}
#endif
						}
						else if (vLine[0] == "s") // Smoothing group
						{
						}
						else if (vLine[0] == "mg") // Merging group
						{

						}
						else if (vLine[0] == "o") // Object name
						{
#ifdef DEBUG_MESH
							if (vLine.size() > 1)
							{
								model.Objects().emplace_back(DevLib::GL::GlObjectType::GlLines, vLine[1]);

								// Enable Lightning
								model.Objects()[model.Objects().size() - 1].EnableLightning();
							}
#else

							if (vLine.size() > 1)
							{
								model.Objects().emplace_back(DevLib::GL::GlObjectType::GlTriangles, vLine[1]);

								// Enable Lightning
								model.Objects()[model.Objects().size() - 1].EnableLightning();
							}
#endif
						}
						else if (vLine[0] == "sp") // Special point
						{

						}
						else if (vLine[0] == "end") // End statement
						{

						}
						// Display/render attributes:
						else if (vLine[0] == "bevel") // Bevel interpolation
						{

						}
						else if (vLine[0] == "c_interp") // Color interpolation
						{

						}
						else if (vLine[0] == "d_interp") // Dissolve interpolation
						{

						}
						else if (vLine[0] == "lod") // Level of detail
						{

						}
						else if (vLine[0] == "usemtl") // Material name
						{
#ifdef DEBUG_MESH
							if (vLine.size() > 1)
							{
								model.Objects().emplace_back(DevLib::GL::GlObjectType::GlLines, vLine[1]);

								// Enable Lightning
								model.Objects()[model.Objects().size() - 1].EnableLightning();
								model.Objects()[model.Objects().size() - 1].SetMaterialName(vLine[1]);

								for (auto& material : model.Materials())
								{
									if (material.GetName() == vLine[1])
									{
										// Set Blend
										if (material.Diffuse().w() < 1.0f)
										{
											model.Objects()[model.Objects().size() - 1].EnableBlend();
										}

										printf("%s : %f, %f, %f\n", material.GetName().c_str(), material.Diffuse().x(), material.Diffuse().y(), material.Diffuse().z());

										// Set Color
										model.Objects()[model.Objects().size() - 1].SetDrawColor(material.Diffuse());

										break;
									}
								}
							}
#else

							if (vLine.size() > 1)
							{
								model.Objects().emplace_back(DevLib::GL::GlObjectType::GlTriangles, vLine[1]);

								// Enable Lightning
								model.Objects()[model.Objects().size() - 1].EnableLightning();
								model.Objects()[model.Objects().size() - 1].SetMaterialName(vLine[1]);

								for (auto& material : model.Materials())
								{
									if (material.GetName() == vLine[1])
									{
										// Set Blend
										if (material.Diffuse().w() < 1.0f)
										{
											model.Objects()[model.Objects().size() - 1].EnableBlend();
										}

										// Set Color
										model.Objects()[model.Objects().size() - 1].SetDrawColor(material.Diffuse());

										break;
									}
								}
							}
#endif

						}
						else if (vLine[0] == "mtllib") // Material libraty
						{
							auto pos = path.rfind('/');
							std::string mtlPath = path.substr(0, pos + 1) + vLine[1];

							LoadMTLFile(mtlPath, model);
						}
						else if (vLine[0] == "shadow_obj") // Shadow casting
						{

						}
						else if (vLine[0] == "trace_obj") // Ray tracing
						{

						}
						else if (vLine[0] == "ctech") // Curve approximation technique
						{

						}
						else if (vLine[0] == "ctstechech") // Surface approximation technique
						{

						}
					}
				}
			}

			return model;
		}
	}
}