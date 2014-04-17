#include "scene.hpp"
#include <iostream>
#include <algorithm>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
  angle = angle * M_PI / 180.0;
  Matrix4x4 t;
  switch(axis) {
    case 'x': {
      t = Matrix4x4(
		Vector4D(1, 0, 0, 0),
		Vector4D(0, cos(angle), -sin(angle), 0),
	    Vector4D(0, sin(angle), cos(angle), 0),
	    Vector4D(0, 0, 0, 1)
      );
	  break;
    }
    case 'y': {
      t = Matrix4x4(
		Vector4D(cos(angle), 0, sin(angle), 0),
		Vector4D(0, 1, 0, 0),
	    Vector4D(-sin(angle), 0, cos(angle), 0),
	    Vector4D(0, 0, 0, 1)
      );
	  break;
	}
    case 'z': {
      t = Matrix4x4(
		Vector4D(cos(angle), -sin(angle), 0, 0),
	    Vector4D(sin(angle), cos(angle), 0, 0),
	    Vector4D(0, 0, 1, 0),
	    Vector4D(0, 0, 0, 1)
      );
	  break;
	}
  }
  set_transform(m_trans * t);
}

void SceneNode::scale(const Vector3D& amount)
{
  Matrix4x4 t = Matrix4x4(
    Vector4D(amount[0], 0.0, 0.0, 0.0),
    Vector4D(0.0, amount[1], 0.0, 0.0),
    Vector4D(0.0, 0.0, amount[2], 0.0),
    Vector4D(0.0, 0.0, 0.0, 1.0)
  );
  set_transform(m_trans * t);
}

void SceneNode::translate(const Vector3D& amount)
{
  Matrix4x4 t = Matrix4x4(
    Vector4D(1.0, 0.0, 0.0, amount[0]),
    Vector4D(0.0, 1.0, 0.0, amount[1]),
    Vector4D(0.0, 0.0, 1.0, amount[2]),
    Vector4D(0.0, 0.0, 0.0, 1.0)
  );
  set_transform(m_trans * t);
}

bool SceneNode::is_joint() const
{
  return false;
}

Colour SceneNode::get_colour(Point3D origin, Vector3D dir, const Colour& ambient, 
							 const Colour& bg, const std::list<Light*>& lights, 
							 int num_glossy_rays, int limit) {
  HitInfo info = intersects(origin, dir);
  if (!info.empty() && limit > 0) {
	Hit closest = info.get_closest();
	Material* close_mat = closest.material;
	Colour c(0.0, 0.0, 0.0); 
	for (std::list<Light*>::const_iterator it = lights.begin(); it != lights.end(); ++it) {
	  Vector3D shadow_ray = (*it)->position - closest.intersection;
	  HitInfo shadow_info = intersects(closest.intersection, shadow_ray);
	  closest = close_mat->apply_material(-dir, *it, closest);

	  if (!shadow_info.empty()) {
		std::vector<Hit> all_hits = shadow_info.get_all_hits();
		int num_translucent = 0;
		Colour shadow(0.0, 0.0, 0.0);
		for (unsigned int i = 0; i < all_hits.size(); i++) {
		  Hit h = all_hits.at(i);
		  if (!h.in_shadow) {
			if (num_translucent == 0) {
			  shadow = shadow + closest.diffuse;
			  num_translucent = 1;
			}
			break;
		  } else {
			if ((h.material)->get_transparency() > 0.0) {
			  shadow = shadow + (h.material)->get_transparency() * closest.diffuse;
			  num_translucent++;
			} else {
			  num_translucent = 0;
			  break;
			}
		  }
		}
		if (num_translucent > 0) {
		  c = c + (1.0 / num_translucent) * shadow;
		  c = c + closest.specular;
		}
	  } else {
		c = c + closest.diffuse;
		c = c + closest.specular;
	  }
	}

	if ((closest.material)->should_reflect()) {
	  Vector3D V = dir, N = closest.normal; 
	  V.normalize(); N.normalize();
	  Vector3D reflected = V - (2 * V.dot(N) * N);
	  Colour r = get_colour(closest.intersection, reflected, ambient, bg, lights, 
							num_glossy_rays, limit - 1);

	  if (num_glossy_rays > 0) {
		Vector3D w = reflected;
		w.normalize();
		Vector3D u = w.cross(Vector3D(1.0, 0.0, 0.0));
		u.normalize();
		Vector3D v = w.cross(Vector3D(0.0, 1.0, 0.0));
		v.normalize();
		for (int i = 0; i < num_glossy_rays; i++) {
		  double dx = 0.0;
		  double dy = 0.0;
		  while (dx == 0.0 && dy == 0.0) {
			dx = ((double)(rand() % 100) / 100.0) * 0.125;
			dy = ((double)(rand() % 100) / 100.0) * 0.125;
		  }
		  Vector3D refi = reflected + dx * u + dy * v;
		  if (refi.dot(closest.normal) < 0) {
			refi = -refi;
		  }
		  Colour ri = get_colour(closest.intersection, refi, ambient, bg, lights, 
								 num_glossy_rays, limit - 1);
		  r = r + ri;
		}
		r = (double) (1.0 / (num_glossy_rays + 1)) * r;
	  }
	  c = 0.7 * c + 0.3 * close_mat->get_reflect(r);
	}

	Vector3D refracted;
	if (close_mat->should_refract() && close_mat->get_transmit_ray(refracted, dir, closest)) {
	  Colour refr = get_colour(closest.intersection, refracted, ambient, bg, lights, 
							   num_glossy_rays, limit + 1);
	  c = c + close_mat->get_transparency() * refr;
	}
	c = c + (1.0 - close_mat->get_transparency()) * close_mat->get_ambient(ambient);
	return c;
  } else {
	return bg;
  }
}

HitInfo SceneNode::intersects(Point3D origin, Vector3D dir) const {
  HitInfo info;
  origin = get_inverse() * origin;
  dir = get_inverse() * dir;
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); ++it) {
	HitInfo h = (*it)->intersects(origin, dir);
	for (unsigned int i = 0; i < h.hits.size(); i++) {
	  h.hits.at(i).intersection = get_transform() * h.hits.at(i).intersection;
	  h.hits.at(i).normal = get_inverse().transpose() * h.hits.at(i).normal;
	}
	for (unsigned int i = 0; i < h.lines.size(); i++) {
	  h.lines.at(i).first.intersection = get_transform() * 
	  	h.lines.at(i).first.intersection;
	  h.lines.at(i).second.intersection = get_transform() * 
	  	h.lines.at(i).second.intersection;	
	  h.lines.at(i).first.normal = get_inverse().transpose() * 
	  	h.lines.at(i).first.normal;
	  h.lines.at(i).second.normal = get_inverse().transpose() * 
	  	h.lines.at(i).second.normal;
	}
    info = HitInfo::merge_info(info, h);
  }
  return info;
}

CSGNode::CSGNode(const std::string& name, CSGNode::Type type) : SceneNode(name), m_type(type)
{
}

CSGNode::~CSGNode()
{
}

HitInfo CSGNode::intersects(Point3D origin, Vector3D dir) const {
  HitInfo info;
  origin = get_inverse() * origin;
  dir = get_inverse() * dir;
  bool first = true;

  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); ++it) {
	HitInfo h = (*it)->intersects(origin, dir);
	for (unsigned int i = 0; i < h.hits.size(); i++) {
 	  h.hits.at(i).intersection = get_transform() * h.hits.at(i).intersection;
	  h.hits.at(i).normal = get_inverse().transpose() * h.hits.at(i).normal;
	}
	for (unsigned int i = 0; i < h.lines.size(); i++) {
	  h.lines.at(i).first.intersection = get_transform() * 
	  	h.lines.at(i).first.intersection;
	  h.lines.at(i).second.intersection = get_transform() * 
	  	h.lines.at(i).second.intersection;	
	  h.lines.at(i).first.normal = get_inverse().transpose() * 
	  	h.lines.at(i).first.normal;
	  h.lines.at(i).second.normal = get_inverse().transpose() * 
	  	h.lines.at(i).second.normal;
	}

	switch(m_type) {
	  case UNION: {
		info = HitInfo::merge_info(info, h);
		break;
	  }
	  case INTERSECTION: {
		if (first) {
		  info = h;
		} else if (h.empty()) {
		  info.clear();
		  return info;
		} else {
		  std::vector<HitInfo::LineSegment> lines;
		  for (unsigned int i = 0; i < info.lines.size(); i++) {
			HitInfo::LineSegment line = info.lines.at(i);
			for (unsigned int j = 0; j < h.lines.size(); j++) {
			  HitInfo::LineSegment sub = h.lines.at(j);

			  if (line.first.t >= sub.first.t && line.second.t <= sub.second.t) {
				// subtracting line contains the first line
				lines = HitInfo::insert_line_in_order(lines, line);
			  } else if (line.first.t < sub.first.t && line.second.t > sub.second.t) {
				// first line contains subtracting line
				lines = HitInfo::insert_line_in_order(lines, sub);
			  } else if (line.first.t > sub.first.t && line.first.t < sub.second.t && 
						 line.second.t > sub.second.t) {
				// tail end of line overlaps with subtracting line.
				double len = (sub.second.intersection-line.first.intersection).length();
				if (len > EPSILON) {
				  lines = HitInfo::insert_line_in_order(lines, 
				    std::make_pair(line.first, sub.second));				
				}
			  } else if (line.first.t < sub.first.t && line.second.t > sub.first.t && 
						 line.second.t < sub.second.t) {
				// head end of line overlaps with subtracting line
				double len = (line.second.intersection-sub.first.intersection).length();
				if (len > EPSILON) {
				  lines = HitInfo::insert_line_in_order(lines,
				    std::make_pair(sub.first, line.second));				
				}
			  } else {
				// no overlap, do nothing
				continue;
			  }
			}
		  }
		  info.lines = lines;
		  std::vector<Hit> hits;
		  for (unsigned i = 0; i < info.hits.size(); i++) {
			Hit hit = info.hits.at(i);
			for (unsigned j = 0; j < h.lines.size(); j++) {
			  HitInfo::LineSegment sub = h.lines.at(j);
			  if (hit.t > sub.first.t && hit.t < sub.second.t) {
				hits.push_back(hit);
				break;
			  }
			}
		  }
		  info.hits = hits;
		}
		break;
	  }
	  case DIFFERENCE: {
		if (first && h.empty()) {
		  return info;
		} else if (first && !h.empty()) {
		  info = HitInfo::merge_info(info, h);
		} else if (!h.empty()) {
		  std::vector<HitInfo::LineSegment> lines;
		  for (unsigned int i = 0; i < info.lines.size(); i++) {
			HitInfo::LineSegment line = info.lines.at(i);
			for (unsigned int j = 0; j < h.lines.size(); j++) {
			  HitInfo::LineSegment sub = h.lines.at(j);

			  if (line.first.t >= sub.first.t && line.second.t <= sub.second.t) {
				// subtracting line contains the first line
				continue;
			  } else if (line.first.t < sub.first.t && line.second.t > sub.second.t) {
				// first line contains subtracting line; divide into two lines.
				sub.first.normal = -sub.first.normal;
				sub.second.normal = -sub.second.normal;
				double len1 = (sub.first.intersection - 
				line.first.intersection).length();
				double len2 = (line.second.intersection-
				sub.second.intersection).length();
				if (len1 > EPSILON) {
				  lines = HitInfo::insert_line_in_order(lines,
					std::make_pair(line.first, sub.first));
				}

				if (len2 > EPSILON) {
				  lines = HitInfo::insert_line_in_order(lines,
					std::make_pair(sub.second, line.second));
				}
			  } else if (line.first.t > sub.first.t && line.first.t < sub.second.t && 
						 line.second.t > sub.second.t) {
				// head end of line overlaps with subtracting line.
				sub.second.normal = -sub.second.normal;
				double len = (line.second.intersection-sub.second.intersection).length();
				if (len > EPSILON) {
				  lines = HitInfo::insert_line_in_order(lines, 
				    std::make_pair(sub.second, line.second));				
				}
			  } else if (line.first.t < sub.first.t && line.second.t > sub.first.t && 
						 line.second.t < sub.second.t) {
				// tail end of line overlaps with subtracting line
				sub.first.normal = -sub.first.normal;
				double len = (line.first.intersection-
				sub.first.intersection).length();
				if (len > EPSILON) {
				  lines = HitInfo::insert_line_in_order(lines,
				    std::make_pair(line.first, sub.first));				
				  }
			  } else {
				// no overlap, line remains unchanged.
				lines = HitInfo::insert_line_in_order(lines, line);
			  }
			}
		  }
		  info.lines = lines;
		  std::vector<Hit> hits;
		  for (unsigned i = 0; i < info.hits.size(); i++) {
			Hit hit = info.hits.at(i);
			bool insert = true;
			for (unsigned j = 0; j < h.lines.size(); j++) {
			  HitInfo::LineSegment sub = h.lines.at(j);
			  if (hit.t > sub.first.t && hit.t < sub.second.t) {
				insert = false;
			  }
			}
			if (insert) {
			  hits.push_back(hit);
			}
		  }
		  info.hits = hits;
		}
		break;
	  }
	}
	first = false;
  }
  return info;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

HitInfo GeometryNode::intersects(Point3D origin, Vector3D dir) const {
  origin = get_inverse() * origin;
  dir = get_inverse() * dir;
  HitInfo info = m_primitive->intersects(origin, dir);

  for (unsigned int i = 0; i < info.hits.size(); i++) {
	info.hits.at(i) = m_material->apply_normal_map(info.hits.at(i));
	info.hits.at(i) = m_material->apply_bump_map(info.hits.at(i));
	info.hits.at(i).material = m_material;
	info.hits.at(i).intersection = get_transform() * info.hits.at(i).intersection;
	info.hits.at(i).normal = get_inverse().transpose() * info.hits.at(i).normal;
  }
  for (unsigned int i = 0; i < info.lines.size(); i++) {
	info.lines.at(i).first = m_material->apply_normal_map(info.lines.at(i).first);
	info.lines.at(i).second = m_material->apply_normal_map(info.lines.at(i).second);
	info.lines.at(i).first = m_material->apply_bump_map(info.lines.at(i).first);
	info.lines.at(i).second = m_material->apply_bump_map(info.lines.at(i).second);
	info.lines.at(i).first.material = m_material;
	info.lines.at(i).second.material = m_material;
	info.lines.at(i).first.intersection = get_transform() * 
		info.lines.at(i).first.intersection;
	info.lines.at(i).second.intersection = get_transform() * 
		info.lines.at(i).second.intersection;	
	info.lines.at(i).first.normal = get_inverse().transpose() * 
		info.lines.at(i).first.normal;
	info.lines.at(i).second.normal = get_inverse().transpose() * 
		info.lines.at(i).second.normal;
  }

  return info;
} 
