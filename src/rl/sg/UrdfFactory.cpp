//
// Copyright (c) 2009, Markus Rickert
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <deque>
#include <iostream> // TODO remove
#include <map>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoToVRML2Action.h>
#ifdef HAVE_SOSTLFILEKIT_H
#include <Inventor/annex/ForeignFiles/SoSTLFileKit.h>
#endif
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/VRMLnodes/SoVRMLAppearance.h>
#include <Inventor/VRMLnodes/SoVRMLBox.h>
#include <Inventor/VRMLnodes/SoVRMLCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLCylinder.h>
#include <Inventor/VRMLnodes/SoVRMLGeometry.h>
#include <Inventor/VRMLnodes/SoVRMLIndexedFaceSet.h>
#include <Inventor/VRMLnodes/SoVRMLMaterial.h>
#include <Inventor/VRMLnodes/SoVRMLSphere.h>
#include <Inventor/VRMLnodes/SoVRMLImageTexture.h>
#include <rl/math/Constants.h>
#include <rl/math/Quaternion.h>
#include <rl/math/Rotation.h>
#include <rl/xml/Attribute.h>
#include <rl/xml/Document.h>
#include <rl/xml/DomParser.h>
#include <rl/xml/Node.h>
#include <rl/xml/Object.h>
#include <rl/xml/Path.h>

#include "Body.h"
#include "Exception.h"
#include "Model.h"
#include "Scene.h"
#include "Shape.h"
#include "SimpleScene.h"
#include "UrdfFactory.h"

namespace rl
{
	namespace sg
	{
		UrdfFactory::UrdfFactory()
		{
		}
		
		UrdfFactory::~UrdfFactory()
		{
		}
		
		void
		UrdfFactory::load(const ::std::string& filename, Scene* scene)
		{
			::rl::xml::DomParser parser;
			
			::rl::xml::Document document = parser.readFile(filename, "", XML_PARSE_NOENT);
			document.substitute(XML_PARSE_NOENT);
			
			::rl::xml::Path path(document);
			
			::rl::xml::NodeSet robots = path.eval("/robot").getValue<::rl::xml::NodeSet>();
			
			if (robots.empty())
			{
				throw Exception("rl::sg::UrdfFactory::load() - URDF is missing robot node");
			}
			
			::SoDB::init();
			::SoNodeKit::init();
			
			for (int i = 0; i < robots.size(); ++i)
			{
				::rl::xml::Path path(document, robots[i]);
				
				Model* model = scene->create();
				
				// name
				
				model->setName(path.eval("string(@name)").getValue<::std::string>());
::std::cout << model->getName() << ::std::endl;
				
				// materials
				
				::rl::xml::NodeSet materials = path.eval("material").getValue<::rl::xml::NodeSet>();
				
				::std::unordered_map<::std::string, ::SoVRMLAppearance*> name2appearance;
				
				for (int j = 0; j < materials.size(); ++j)
				{
::std::cout << "material: " << j << ::std::endl;
					::rl::xml::Path path(document, materials[j]);
					
					::std::string name = path.eval("string(@name)").getValue<::std::string>();
					
					::SoVRMLAppearance* vrmlAppearance = new ::SoVRMLAppearance();
					vrmlAppearance->ref();
					
					vrmlAppearance->setName(name.c_str());
					name2appearance[name] = vrmlAppearance;
::std::cout << "\tname: " << name << ::std::endl;
					
					::SoVRMLMaterial* vrmlMaterial = new ::SoVRMLMaterial();
					
					if (path.eval("count(color/@rgba) > 0").getValue<bool>())
					{
						::std::vector<::std::string> rgba;
						::std::string tmp = path.eval("string(color/@rgba)").getValue<::std::string>();
						::boost::split(rgba, tmp, ::boost::algorithm::is_space(), ::boost::algorithm::token_compress_on);
::std::cout << "\trgba: " << rgba[0] << " " << rgba[1] << " " << rgba[2] << " " << rgba[3] << ::std::endl;
						
						vrmlMaterial->diffuseColor.setValue(
							::boost::lexical_cast<float>(rgba[0]),
							::boost::lexical_cast<float>(rgba[1]),
							::boost::lexical_cast<float>(rgba[2])
						);
						vrmlMaterial->transparency.setValue(
							1 - ::boost::lexical_cast<float>(rgba[3])
						);
					}
					
					vrmlAppearance->material = vrmlMaterial;
					
					SoVRMLImageTexture* vrmlImageTexture = new SoVRMLImageTexture();
					
					if (path.eval("count(texture/@filename) > 0").getValue<bool>())
					{
						::rl::xml::NodeSet texture = path.eval("texture").getValue<::rl::xml::NodeSet>();
						::std::string textureFilename = texture[0].getLocalPath(texture[0].getProperty("filename"));
::std::cout << "\tfilename: " << textureFilename << ::std::endl;
						
						SbImage image;
						
						if (image.readFile(textureFilename.c_str()))
						{
							vrmlImageTexture->setImage(image);
						}
					}
					
					vrmlAppearance->texture = vrmlImageTexture;
				}
				
				// joints
				
				::std::unordered_map<::std::string, ::std::string> child2parent;
				::std::unordered_map<::std::string, ::rl::xml::Node> name2link;
				::std::multimap<::std::string, ::std::string> parent2child;
				
				::rl::xml::NodeSet joints = path.eval("joint").getValue<::rl::xml::NodeSet>();
				
				for (int j = 0; j < joints.size(); ++j)
				{
::std::cout << "joint: " << j << ::std::endl;
					::rl::xml::Path path(document, joints[j]);
					
					::std::string child = path.eval("string(child/@link)").getValue<::std::string>();
					::std::string parent = path.eval("string(parent/@link)").getValue<::std::string>();
					
					child2parent[child] = parent;
					parent2child.insert(::std::pair<::std::string, ::std::string>(parent, child));
::std::cout << "\tconnect: " << parent << " -> " << child << ::std::endl;
				}
				
				// links
				
				::rl::xml::NodeSet links = path.eval("link").getValue<::rl::xml::NodeSet>();
				
				::std::string root;
				
				for (int j = 0; j < links.size(); ++j)
				{
					::rl::xml::Path path(document, links[j]);
					
					::std::string name = path.eval("string(@name)").getValue<::std::string>();
					
					name2link[name] = links[j];
					
					if (child2parent.end() == child2parent.find(name))
					{
						if (root.empty())
						{
							root = name;
::std::cout << "root: " << name << ::std::endl;
						}
						else
						{
							throw Exception("rl::sg::UrdfFactory::load() - URDF has more than one root node");
						}
					}
				}
				
				::std::deque<::std::string> dfs;
				dfs.push_front(root);
				
				while (!dfs.empty())
				{
					::std::string name = dfs.front();
::std::cout << "link: " << name << ::std::endl;
					
					::rl::xml::Path path(document, name2link[name]);
					
					Body* body = model->create();
					
					body->setName(name);
::std::cout << "\tname: " << body->getName() << ::std::endl;
					
					::rl::xml::NodeSet shapes;
					
					if (SimpleScene* simple = dynamic_cast<SimpleScene*>(scene))
					{
						if (path.eval("count(collision) > 0").getValue<bool>())
						{
							shapes = path.eval("collision").getValue<::rl::xml::NodeSet>();
						}
						else if (path.eval("count(visual) > 0").getValue<bool>())
						{
							shapes = path.eval("visual").getValue<::rl::xml::NodeSet>();
						}
					}
					else
					{
						if (path.eval("count(visual) > 0").getValue<bool>())
						{
							shapes = path.eval("visual").getValue<::rl::xml::NodeSet>();
						}
						else if (path.eval("count(collision) > 0").getValue<bool>())
						{
							shapes = path.eval("collision").getValue<::rl::xml::NodeSet>();
						}
					}
					
					for (int j = 0; j < shapes.size(); ++j)
					{
						::rl::xml::Path path(document, shapes[j]);
						
						::SoVRMLShape* vrmlShape = new ::SoVRMLShape();
						vrmlShape->ref();
						
						if (path.eval("count(material/@name) > 0 and count(material//*) = 0").getValue<bool>())
						{
							::std::string name = path.eval("string(material/@name)").getValue<::std::string>();
::std::cout << "\tmaterial: " << name << ::std::endl;
							vrmlShape->appearance = name2appearance[name];
						}
						else
						{
							::std::string name = path.eval("string(material/@name)").getValue<::std::string>();
							
							::SoVRMLAppearance* vrmlAppearance = new ::SoVRMLAppearance();
							
							if (!name.empty())
							{
								vrmlAppearance->setName(name.c_str());
::std::cout << "\tmaterial name: " << name << ::std::endl;
							}
							
							::SoVRMLMaterial* vrmlMaterial = new ::SoVRMLMaterial();
							
							if (path.eval("count(material/color/@rgba) > 0").getValue<bool>())
							{
								::std::vector<::std::string> rgba;
								::std::string tmp = path.eval("string(material/color/@rgba)").getValue<::std::string>();
								::boost::split(rgba, tmp, ::boost::algorithm::is_space(), ::boost::algorithm::token_compress_on);
::std::cout << "\tmaterial rgba: " << rgba[0] << " " << rgba[1] << " " << rgba[2] << " " << rgba[3] << ::std::endl;
								
								vrmlMaterial->diffuseColor.setValue(
									::boost::lexical_cast<float>(rgba[0]),
									::boost::lexical_cast<float>(rgba[1]),
									::boost::lexical_cast<float>(rgba[2])
								);
								vrmlMaterial->transparency.setValue(
									1 - ::boost::lexical_cast<float>(rgba[3])
								);
							}
							
							vrmlAppearance->material = vrmlMaterial;
							
							SoVRMLImageTexture* vrmlImageTexture = new SoVRMLImageTexture();
							
							if (path.eval("count(material/texture/@filename) > 0").getValue<bool>())
							{
								::rl::xml::NodeSet texture = path.eval("material/texture").getValue<::rl::xml::NodeSet>();
								::std::string textureFilename = texture[0].getLocalPath(texture[0].getProperty("filename"));
::std::cout << "\tmaterial filename: " << textureFilename << ::std::endl;
								
								SbImage image;
								
								if (image.readFile(textureFilename.c_str()))
								{
									vrmlImageTexture->setImage(image);
								}
							}
							
							vrmlAppearance->texture = vrmlImageTexture;
							
							vrmlShape->appearance = vrmlAppearance;
						}
						
						::rl::xml::NodeSet geometries = path.eval("geometry/box|geometry/cylinder|geometry/mesh|geometry/sphere").getValue<::rl::xml::NodeSet>();
						
						for (int k = 0; k < geometries.size(); ++k)
						{
							if ("box" == geometries[k].getName())
							{
								::SoVRMLBox* box = new ::SoVRMLBox();
								
								if (!geometries[k].getProperty("size").empty())
								{
									::std::vector<::std::string> size;
									::std::string tmp = geometries[k].getProperty("size");
									::boost::split(size, tmp, ::boost::algorithm::is_space(), ::boost::algorithm::token_compress_on);
									
									box->size.setValue(
										::boost::lexical_cast<float>(size[0]),
										::boost::lexical_cast<float>(size[1]),
										::boost::lexical_cast<float>(size[2])
									);
								}
								
								vrmlShape->geometry = box;
::std::cout << "\tbox size: " << box->size.getValue()[0] << " " << box->size.getValue()[1] << " " << box->size.getValue()[2] << ::std::endl;
							}
							else if ("cylinder" == geometries[k].getName())
							{
								::SoVRMLCylinder* cylinder = new ::SoVRMLCylinder();
								
								if (!geometries[k].getProperty("length").empty())
								{
									cylinder->height.setValue(
										::boost::lexical_cast<float>(geometries[k].getProperty("length"))
									);
								}
								
								if (!geometries[k].getProperty("radius").empty())
								{
									cylinder->radius.setValue(
										::boost::lexical_cast<float>(geometries[k].getProperty("radius"))
									);
								}
								
								vrmlShape->geometry = cylinder;
::std::cout << "\tcylinder height: " << cylinder->height.getValue() << ::std::endl;
::std::cout << "\tcylinder radius: " << cylinder->radius.getValue() << ::std::endl;
							}
							else if ("mesh" == geometries[k].getName())
							{
								if (!geometries[k].getProperty("filename").empty())
								{
									::std::string meshFilename = geometries[k].getLocalPath(geometries[k].getProperty("filename"));
::std::cout << "\tmesh filename: " << meshFilename << ::std::endl;
									
#if defined(HAVE_SOSTLFILEKIT_H) && defined(HAVE_SOSTLFILEKIT_CONVERT)
									if (!boost::iends_with(meshFilename, "stl"))
									{
										throw Exception("rl::sg::UrdfFactory::load() - Only STL meshes currently supported");
									}
									
									::SoSTLFileKit* stlFileKit = new ::SoSTLFileKit();
									stlFileKit->ref();
									
									if (!stlFileKit->readFile(meshFilename.c_str()))
									{
										throw Exception("rl::sg::UrdfFactory::load() - Failed to open file '" + meshFilename + "'");
									}
									
									::SoSeparator* stl = stlFileKit->convert();
									stl->ref();
									stlFileKit->unref();
									
									::SoToVRML2Action toVrml2Action;
									toVrml2Action.apply(stl);
									::SoVRMLGroup* vrml2 = toVrml2Action.getVRML2SceneGraph();
									vrml2->ref();
									stl->unref();
									
									::SoSearchAction searchAction;
									searchAction.setInterest(::SoSearchAction::ALL);
									searchAction.setType(::SoVRMLShape::getClassTypeId());
									searchAction.apply(vrml2);
									
									for (int l = 0; l < searchAction.getPaths().getLength(); ++l)
									{
										vrmlShape->geometry = static_cast<::SoVRMLShape*>(static_cast<::SoFullPath*>(searchAction.getPaths()[l])->getTail())->geometry;
										
										if (vrmlShape->geometry.getValue()->isOfType(::SoVRMLIndexedFaceSet::getClassTypeId()))
										{
											::SoVRMLIndexedFaceSet* vrmlIndexedFaceSet = static_cast<::SoVRMLIndexedFaceSet*>(vrmlShape->geometry.getValue());
											vrmlIndexedFaceSet->convex.setValue(false);
											
											::SoVRMLCoordinate* vrmlCoordinate = static_cast<::SoVRMLCoordinate*>(vrmlIndexedFaceSet->coord.getValue());
											
											if (!geometries[k].getProperty("scale").empty())
											{
												::SbVec3f scale(1.0f, 1.0f, 1.0f);
												
												::std::vector<::std::string> scales;
												::std::string scaleProperty = geometries[k].getProperty("scale");
												::boost::split(scales, scaleProperty, ::boost::algorithm::is_space(), ::boost::algorithm::token_compress_on);
::std::cout << "\tscale: " << scales[0] << " " << scales[1] << " " << scales[2] << ::std::endl;
												scale.setValue(::std::stof(scales[0]), ::std::stof(scales[1]), ::std::stof(scales[2]));
												
												for (int m = 0; m < vrmlCoordinate->point.getNum(); ++m)
												{
													vrmlCoordinate->point.set1Value(
														m,
														vrmlCoordinate->point[m][0] * scale[0],
														vrmlCoordinate->point[m][1] * scale[1],
														vrmlCoordinate->point[m][2] * scale[2]
													);
												}
											}
										}
									}
									
									vrml2->unref();
#else
									throw Exception("rl::sg::UrdfFactory::load() - Mesh support currently not available");
#endif
								}
							}
							else if ("sphere" == geometries[k].getName())
							{
								::SoVRMLSphere* sphere = new ::SoVRMLSphere();
								
								if (!geometries[k].getProperty("radius").empty())
								{
									sphere->radius.setValue(
										::boost::lexical_cast<float>(geometries[k].getProperty("radius"))
									);
								}
								
								vrmlShape->geometry = sphere;
::std::cout << "\tsphere radius: " << sphere->radius.getValue() << ::std::endl;
							}
						}
						
						if (nullptr != vrmlShape->geometry.getValue())
						{
							Shape* shape = body->create(vrmlShape);
							
							::rl::math::Transform origin = ::rl::math::Transform::Identity();
							
							if (path.eval("count(origin/@rpy) > 0").getValue<bool>())
							{
								::std::vector<::std::string> rpy;
								::std::string tmp = path.eval("string(origin/@rpy)").getValue<::std::string>();
								::boost::split(rpy, tmp, ::boost::algorithm::is_space(), ::boost::algorithm::token_compress_on);
::std::cout << "\torigin rpy: " << rpy[0] << " " << rpy[1] << " " << rpy[2] << ::std::endl;
								
								origin.linear() = ::rl::math::AngleAxis(
									::boost::lexical_cast<::rl::math::Real>(rpy[2]),
									::rl::math::Vector3::UnitZ()
								) * ::rl::math::AngleAxis(
									::boost::lexical_cast<::rl::math::Real>(rpy[1]),
									::rl::math::Vector3::UnitY()
								) * ::rl::math::AngleAxis(
									::boost::lexical_cast<::rl::math::Real>(rpy[0]),
									::rl::math::Vector3::UnitX()
								).toRotationMatrix();
							}
							
							if (vrmlShape->geometry.getValue()->isOfType(::SoVRMLCylinder::getClassTypeId()))
							{
								origin *= ::rl::math::AngleAxis(90 * ::rl::math::constants::deg2rad, ::rl::math::Vector3::UnitX());
							}
							
							if (path.eval("count(origin/@xyz) > 0").getValue<bool>())
							{
								::std::vector<::std::string> xyz;
								::std::string tmp = path.eval("string(origin/@xyz)").getValue<::std::string>();
								::boost::split(xyz, tmp, ::boost::algorithm::is_space(), ::boost::algorithm::token_compress_on);
::std::cout << "\torigin xyz: " << xyz[0] << " " << xyz[1] << " " << xyz[2] << ::std::endl;
								
								origin.translation().x() = ::boost::lexical_cast<::rl::math::Real>(xyz[0]);
								origin.translation().y() = ::boost::lexical_cast<::rl::math::Real>(xyz[1]);
								origin.translation().z() = ::boost::lexical_cast<::rl::math::Real>(xyz[2]);
							}
							
							shape->setTransform(origin);
						}
						
						vrmlShape->unref();
					}
					
					// depth first search
					
					dfs.pop_front();
					
					::std::pair<
						::std::multimap<::std::string, ::std::string>::const_iterator,
						::std::multimap<::std::string, ::std::string>::const_iterator
					> range = parent2child.equal_range(name);
					
					// reverse order
					
					::std::multimap<::std::string, ::std::string>::const_iterator first = --range.second;
					::std::multimap<::std::string, ::std::string>::const_iterator second = --range.first;
					
					for (::std::multimap<::std::string, ::std::string>::const_iterator k = first; k != second; --k)
					{
						dfs.push_front(k->second);
					}
				}
				
				for (::std::unordered_map<::std::string, ::SoVRMLAppearance*>::iterator j = name2appearance.begin(); j != name2appearance.end(); ++j)
				{
					j->second->unref();
				}
			}
		}
	}
}
