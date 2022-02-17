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

#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <rl/math/Constants.h>
#include <rl/math/Quaternion.h>
#include <rl/math/Rotation.h>
#include <rl/xml/Attribute.h>
#include <rl/xml/Document.h>
#include <rl/xml/DomParser.h>
#include <rl/xml/Node.h>
#include <rl/xml/Object.h>
#include <rl/xml/Path.h>
#include <rl/xml/Stylesheet.h>

#include "Body.h"
#include "Cylindrical.h"
#include "Dynamic.h"
#include "Exception.h"
#include "Fixed.h"
#include "Helical.h"
#include "Joint.h"
#include "Kinematic.h"
#include "Model.h"
#include "Prismatic.h"
#include "Revolute.h"
#include "SixDof.h"
#include "Spherical.h"
#include "World.h"
#include "XmlFactory.h"

namespace rl
{
	namespace mdl
	{
		XmlFactory::XmlFactory()
		{
		}
		
		XmlFactory::~XmlFactory()
		{
		}
		
		void
		XmlFactory::load(const ::std::string& filename, Model* model)
		{
			::rl::xml::DomParser parser;
			
			::rl::xml::Document document = parser.readFile(filename, "", XML_PARSE_NOENT | XML_PARSE_XINCLUDE);
			document.substitute(XML_PARSE_NOENT | XML_PARSE_XINCLUDE);
			
			if ("stylesheet" == document.getRootElement().getName() || "transform" == document.getRootElement().getName())
			{
				if ("1.0" == document.getRootElement().getProperty("version"))
				{
					if (document.getRootElement().hasNamespace() && "http://www.w3.org/1999/XSL/Transform" == document.getRootElement().getNamespace().getHref())
					{
						::rl::xml::Stylesheet stylesheet(document);
						document = stylesheet.apply();
					}
				}
			}
			
			::rl::xml::Path path(document);
			
			::rl::xml::NodeSet models = path.eval("(/rl/mdl|/rlmdl)/model").getValue<::rl::xml::NodeSet>();
			
			if (models.empty())
			{
				throw Exception("rl::mdl::XmlFactory::load() - No models found in file '" + filename + "'");
			}
			
			for (int i = 0; i < ::std::min(1, models.size()); ++i)
			{
				::rl::xml::Path path(document, models[i]);
				
				// manufacturer
				
				model->setManufacturer(path.eval("string(manufacturer)").getValue<::std::string>());
				
				// name
				
				model->setName(path.eval("string(name)").getValue<::std::string>());
				
				// frame
				
				::rl::xml::NodeSet frames = path.eval("body|frame|world").getValue<::rl::xml::NodeSet>();
				
				::std::unordered_map<::std::string, Frame*> id2frame;
				
				for (int j = 0; j < frames.size(); ++j)
				{
					::rl::xml::Path path(document, frames[j]);
					
					Frame* frame = nullptr;
					
					if ("body" == frames[j].getName())
					{
						::std::shared_ptr<Body> b = ::std::make_shared<Body>();
						
						model->add(b);
						
						b->setCenterOfMass(
							path.eval("number(cm/x)").getValue<::rl::math::Real>(0),
							path.eval("number(cm/y)").getValue<::rl::math::Real>(0),
							path.eval("number(cm/z)").getValue<::rl::math::Real>(0)
						);
						
						b->setInertia(
							path.eval("number(i/xx)").getValue<::rl::math::Real>(1),
							path.eval("number(i/yy)").getValue<::rl::math::Real>(1),
							path.eval("number(i/zz)").getValue<::rl::math::Real>(1),
							path.eval("number(i/yz)").getValue<::rl::math::Real>(0),
							path.eval("number(i/xz)").getValue<::rl::math::Real>(0),
							path.eval("number(i/xy)").getValue<::rl::math::Real>(0)
						);
						
						b->setMass(
							path.eval("number(m)").getValue<::rl::math::Real>(1)
						);
						
						frame = b.get();
					}
					else if ("frame" == frames[j].getName())
					{
						::std::shared_ptr<Frame> f = ::std::make_shared<Frame>();
						
						model->add(f);
						
						frame = f.get();
					}
					else if ("world" == frames[j].getName())
					{
						::std::shared_ptr<World> w = ::std::make_shared<World>();
						
						model->add(w);
						
						w->x.linear() = ::rl::math::AngleAxis(
							path.eval("number(rotation/z)").getValue<::rl::math::Real>(0) * ::rl::math::constants::deg2rad,
							::rl::math::Vector3::UnitZ()
						) * ::rl::math::AngleAxis(
							path.eval("number(rotation/y)").getValue<::rl::math::Real>(0) * ::rl::math::constants::deg2rad,
							::rl::math::Vector3::UnitY()
						) * ::rl::math::AngleAxis(
							path.eval("number(rotation/x)").getValue<::rl::math::Real>(0) * ::rl::math::constants::deg2rad,
							::rl::math::Vector3::UnitX()
						).toRotationMatrix();
						
						w->x.translation().x() = path.eval("number(translation/x)").getValue<::rl::math::Real>(0);
						w->x.translation().y() = path.eval("number(translation/y)").getValue<::rl::math::Real>(0);
						w->x.translation().z() = path.eval("number(translation/z)").getValue<::rl::math::Real>(0);
						
						w->setGravity(
							::rl::math::Vector3(
								path.eval("number(g/x)").getValue<::rl::math::Real>(0),
								path.eval("number(g/y)").getValue<::rl::math::Real>(0),
								path.eval("number(g/z)").getValue<::rl::math::Real>(0)
							)
						);
						
						frame = w.get();
					}
					
					if (nullptr != frame)
					{
						frame->setName(path.eval("string(@id)").getValue<::std::string>());
						
						if (id2frame.find(frame->getName()) != id2frame.end())
						{
							throw Exception("rl::mdl::XmlFactory::load() - Frame with ID '" + frame->getName() + "' not unique in file '" + filename + "'");
						}
						
						id2frame[frame->getName()] = frame;
					}
				}
				
				// selfcollision
				
				for (int j = 0; j < frames.size(); ++j)
				{
					::rl::xml::Path path(document, frames[j]);
					
					if ("body" == frames[j].getName())
					{
						::std::string b1Id = path.eval("string(@id)").getValue<::std::string>();
						
						if (id2frame.find(b1Id) == id2frame.end())
						{
							throw Exception("rl::mdl::XmlFactory::load() - Body with ID '" + b1Id + "' not found in file '" + filename + "'");
						}
						
						Body* b1 = dynamic_cast<Body*>(id2frame[b1Id]);
						
						::rl::xml::NodeSet ignores = path.eval("ignore").getValue<::rl::xml::NodeSet>();
						
						for (int k = 0; k < ignores.size(); ++k)
						{
							if (!ignores[k].getProperty("idref").empty())
							{
								::std::string b2IdRef = ignores[k].getProperty("idref");
								
								if (id2frame.find(b2IdRef) == id2frame.end())
								{
									throw Exception("rl::mdl::XmlFactory::load() - Body with IDREF '" + b2IdRef + "' in Body with ID '" + b1Id + "' not found in file '" + filename + "'");
								}
								
								Body* b2 = dynamic_cast<Body*>(id2frame[b2IdRef]);
								
								b1->selfcollision.insert(b2);
								b2->selfcollision.insert(b1);
							}
							else
							{
								b1->collision = false;
							}
						}
					}
				}
				
				// transforms
				
				::rl::xml::NodeSet transforms = path.eval("cylindrical|fixed|helical|prismatic|revolute|sixDof|spherical").getValue<::rl::xml::NodeSet>();
				
				for (int j = 0; j < transforms.size(); ++j)
				{
					::rl::xml::Path path(document, transforms[j]);
					
					::std::string name = path.eval("string(@id)").getValue<::std::string>();
					
					::std::string aIdRef = path.eval("string(frame/a/@idref)").getValue<::std::string>();
					
					if (id2frame.find(aIdRef) == id2frame.end())
					{
						throw Exception("rl::mdl::XmlFactory::load() - Frame A with IDREF '" + aIdRef + "' in Transform with ID '" + name + "' not found in file '" + filename + "'");
					}
					
					Frame* a = id2frame[aIdRef];
					
					::std::string bIdRef = path.eval("string(frame/b/@idref)").getValue<::std::string>();
					
					if (id2frame.find(bIdRef) == id2frame.end())
					{
						throw Exception("rl::mdl::XmlFactory::load() - Frame B with IDREF '" + bIdRef + "' in Transform with ID '" + name + "' not found in file '" + filename + "'");
					}
					
					Frame* b = id2frame[bIdRef];
					
					Transform* transform = nullptr;
					
					if ("cylindrical" == transforms[j].getName())
					{
						::std::shared_ptr<Cylindrical> c = ::std::make_shared<Cylindrical>();
						
						model->add(c, a, b);
						
						c->max(0) = path.eval("number(max[1])").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						c->min(0) = path.eval("number(min[1])").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						c->offset(0) = path.eval("number(offset[1])").getValue<::rl::math::Real>(0);
						c->speed(0) = path.eval("number(speed[1])").getValue<::rl::math::Real>(0);
						c->wraparound(0) = path.eval("count(wraparound[1]) > 0").getValue<bool>();
						
						c->max(1) = path.eval("number(max[2])").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						c->min(1) = path.eval("number(min[2])").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						c->offset(1) = path.eval("number(offset[2])").getValue<::rl::math::Real>(0);
						c->speed(1) = path.eval("number(speed[2])").getValue<::rl::math::Real>(0);
						c->wraparound(1) = path.eval("count(wraparound[2]) > 0").getValue<bool>();
						
						c->S(0, 0) = path.eval("number(axis[1]/x)").getValue<::rl::math::Real>(0);
						c->S(1, 0) = path.eval("number(axis[1]/y)").getValue<::rl::math::Real>(0);
						c->S(2, 0) = path.eval("number(axis[1]/z)").getValue<::rl::math::Real>(1);
						
						c->S(3, 1) = path.eval("number(axis[2]/x)").getValue<::rl::math::Real>(0);
						c->S(4, 1) = path.eval("number(axis[2]/y)").getValue<::rl::math::Real>(0);
						c->S(5, 1) = path.eval("number(axis[2]/z)").getValue<::rl::math::Real>(1);
						
						transform = c.get();
					}
					else if ("fixed" == transforms[j].getName())
					{
						::std::shared_ptr<Fixed> f = ::std::make_shared<Fixed>();
						
						model->add(f, a, b);
						
						f->x.linear() = ::rl::math::AngleAxis(
							path.eval("number(rotation/z)").getValue<::rl::math::Real>(0) * ::rl::math::constants::deg2rad,
							::rl::math::Vector3::UnitZ()
						) * ::rl::math::AngleAxis(
							path.eval("number(rotation/y)").getValue<::rl::math::Real>(0) * ::rl::math::constants::deg2rad,
							::rl::math::Vector3::UnitY()
						) * ::rl::math::AngleAxis(
							path.eval("number(rotation/x)").getValue<::rl::math::Real>(0) * ::rl::math::constants::deg2rad,
							::rl::math::Vector3::UnitX()
						).toRotationMatrix();
						
						f->x.translation().x() = path.eval("number(translation/x)").getValue<::rl::math::Real>(0);
						f->x.translation().y() = path.eval("number(translation/y)").getValue<::rl::math::Real>(0);
						f->x.translation().z() = path.eval("number(translation/z)").getValue<::rl::math::Real>(0);
						
						transform = f.get();
					}
					else if ("helical" == transforms[j].getName())
					{
						::std::shared_ptr<Helical> h = ::std::make_shared<Helical>();
						
						model->add(h, a, b);
						
						h->max(0) = path.eval("number(max)").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						h->min(0) = path.eval("number(min)").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						h->offset(0) = path.eval("number(offset)").getValue<::rl::math::Real>(0);
						h->speed(0) = path.eval("number(speed)").getValue<::rl::math::Real>(0);
						h->wraparound(0) = path.eval("count(wraparound) > 0").getValue<bool>();
						
						h->S(0, 0) = path.eval("number(axis[1]/x)").getValue<::rl::math::Real>(0);
						h->S(1, 0) = path.eval("number(axis[1]/y)").getValue<::rl::math::Real>(0);
						h->S(2, 0) = path.eval("number(axis[1]/z)").getValue<::rl::math::Real>(1);
						
						h->S(3, 0) = path.eval("number(axis[2]/x)").getValue<::rl::math::Real>(0);
						h->S(4, 0) = path.eval("number(axis[2]/y)").getValue<::rl::math::Real>(0);
						h->S(5, 0) = path.eval("number(axis[2]/z)").getValue<::rl::math::Real>(1);
						
						h->setPitch(path.eval("number(pitch)").getValue<::rl::math::Real>(1));
						
						transform = h.get();
					}
					else if ("prismatic" == transforms[j].getName())
					{
						::std::shared_ptr<Prismatic> p = ::std::make_shared<Prismatic>();
						
						model->add(p, a, b);
						
						p->max(0) = path.eval("number(max)").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						p->min(0) = path.eval("number(min)").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						p->offset(0) = path.eval("number(offset)").getValue<::rl::math::Real>(0);
						p->speed(0) = path.eval("number(speed)").getValue<::rl::math::Real>(0);
						p->wraparound(0) = path.eval("count(wraparound) > 0").getValue<bool>();
						
						p->S(3, 0) = path.eval("number(axis/x)").getValue<::rl::math::Real>(0);
						p->S(4, 0) = path.eval("number(axis/y)").getValue<::rl::math::Real>(0);
						p->S(5, 0) = path.eval("number(axis/z)").getValue<::rl::math::Real>(1);
						
						transform = p.get();
					}
					else if ("revolute" == transforms[j].getName())
					{
						::std::shared_ptr<Revolute> r = ::std::make_shared<Revolute>();
						
						model->add(r, a, b);
						
						r->max(0) = path.eval("number(max)").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						r->min(0) = path.eval("number(min)").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						r->offset(0) = path.eval("number(offset)").getValue<::rl::math::Real>(0);
						r->speed(0) = path.eval("number(speed)").getValue<::rl::math::Real>(0);
						r->wraparound(0) = path.eval("count(wraparound) > 0").getValue<bool>();
						
						r->S(0, 0) = path.eval("number(axis/x)").getValue<::rl::math::Real>(0);
						r->S(1, 0) = path.eval("number(axis/y)").getValue<::rl::math::Real>(0);
						r->S(2, 0) = path.eval("number(axis/z)").getValue<::rl::math::Real>(1);
						
						r->max *= ::rl::math::constants::deg2rad;
						r->min *= ::rl::math::constants::deg2rad;
						r->offset *= ::rl::math::constants::deg2rad;
						r->speed *= ::rl::math::constants::deg2rad;
						
						transform = r.get();
					}
					else if ("sixDof" == transforms[j].getName())
					{
						::std::shared_ptr<SixDof> s = ::std::make_shared<SixDof>();
						
						model->add(s, a, b);
						
						s->max(0) = path.eval("number(max[1])").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						s->max(1) = path.eval("number(max[2])").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						s->max(2) = path.eval("number(max[3])").getValue<::rl::math::Real>(::std::numeric_limits<::rl::math::Real>::max());
						s->min(0) = path.eval("number(min[1])").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						s->min(1) = path.eval("number(min[2])").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						s->min(2) = path.eval("number(min[3])").getValue<::rl::math::Real>(-::std::numeric_limits<::rl::math::Real>::max());
						s->offset(0) = path.eval("number(offset[1])").getValue<::rl::math::Real>(0);
						s->offset(1) = path.eval("number(offset[2])").getValue<::rl::math::Real>(0);
						s->offset(2) = path.eval("number(offset[3])").getValue<::rl::math::Real>(0);
						s->speed(0) = path.eval("number(speed[1])").getValue<::rl::math::Real>(0);
						s->speed(1) = path.eval("number(speed[2])").getValue<::rl::math::Real>(0);
						s->speed(2) = path.eval("number(speed[3])").getValue<::rl::math::Real>(0);
						
						transform = s.get();
					}
					else if ("spherical" == transforms[j].getName())
					{
						::std::shared_ptr<Spherical> s = ::std::make_shared<Spherical>();
						
						model->add(s, a, b);
						
						transform = s.get();
					}
					
					if (nullptr != transform)
					{
						transform->setName(name);
					}
				}
				
				model->update();
				
				::rl::xml::NodeSet home = path.eval("home/q").getValue<::rl::xml::NodeSet>();
				
				if (home.size() > 0)
				{
					if (home.size() != model->getDofPosition())
					{
						throw Exception("rl::mdl::XmlFactory::load() - Incorrect size of home position vector in file '" + filename + "'");
					}
					
					::rl::math::Vector q(home.size());
					
					for (int j = 0; j < home.size(); ++j)
					{
						q(j) = ::boost::lexical_cast<::rl::math::Real>(home[j].getContent().c_str());
						
						if ("deg" == home[j].getProperty("unit"))
						{
							q(j) *= ::rl::math::constants::deg2rad;
						}
					}
					
					model->setHomePosition(q);
				}
				
				::rl::xml::NodeSet gammaPosition = path.eval("gammaPosition").getValue<::rl::xml::NodeSet>();
				
				for (int j = 0; j < gammaPosition.size(); ++j)
				{
					::rl::xml::Path path(document, gammaPosition[j]);
					
					::std::size_t m = path.eval("count(row)").getValue<::std::size_t>(0);
					::std::size_t n = path.eval("count(row[1]/col)").getValue<::std::size_t>(0);
					
					if (m != model->getDofPosition())
					{
						throw Exception("rl::mdl::XmlFactory::load() - Incorrect number of rows in gamma position matrix in file '" + filename + "'");
					}
					
					::rl::math::Matrix G(m, n);
					
					::rl::xml::NodeSet rows = path.eval("row").getValue<::rl::xml::NodeSet>();
					
					for (int k = 0; k < rows.size(); ++k)
					{
						::rl::xml::Path path(document, rows[k]);
						
						if (path.eval("count(col)").getValue<::std::size_t>(0) != model->getDofPosition())
						{
							throw Exception("rl::mdl::XmlFactory::load() - Incorrect number of columns in gamma position matrix in file '" + filename + "'");
						}
						
						::rl::xml::NodeSet cols = path.eval("col").getValue<::rl::xml::NodeSet>();
						
						for (int l = 0; l < cols.size(); ++l)
						{
							G(k, l) = ::boost::lexical_cast<::rl::math::Real>(cols[l].getContent().c_str());
						}
					}
					
					model->setGammaPosition(G);
				}
				
				::rl::xml::NodeSet gammaVelocity = path.eval("gammaVelocity").getValue<::rl::xml::NodeSet>();
				
				for (int j = 0; j < gammaVelocity.size(); ++j)
				{
					::rl::xml::Path path(document, gammaVelocity[j]);
					
					::std::size_t m = path.eval("count(row)").getValue<::std::size_t>(0);
					::std::size_t n = path.eval("count(row[1]/col)").getValue<::std::size_t>(0);
					
					if (m != model->getDof())
					{
						throw Exception("rl::mdl::XmlFactory::load() - Incorrect number of rows in gamma velocity matrix in file '" + filename + "'");
					}
					
					::rl::math::Matrix G(m, n);
					
					::rl::xml::NodeSet rows = path.eval("row").getValue<::rl::xml::NodeSet>();
					
					for (int k = 0; k < rows.size(); ++k)
					{
						::rl::xml::Path path(document, rows[k]);
						
						if (path.eval("count(col)").getValue<::std::size_t>(0) != model->getDof())
						{
							throw Exception("rl::mdl::XmlFactory::load() - Incorrect number of columns in gamma velocity matrix in file '" + filename + "'");
						}
						
						::rl::xml::NodeSet cols = path.eval("col").getValue<::rl::xml::NodeSet>();
						
						for (int l = 0; l < cols.size(); ++l)
						{
							G(k, l) = ::boost::lexical_cast<::rl::math::Real>(cols[l].getContent().c_str());
						}
					}
					
					model->setGammaVelocity(G);
				}
			}
		}
	}
}
