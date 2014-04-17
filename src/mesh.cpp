#include "mesh.hpp"
#include <iostream>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{
  // store normals
  for(unsigned int i = 0; i < m_faces.size(); i++) {
	Face f = m_faces.at(i);
	Point3D p0 = m_verts.at(f.at(0)), p1 = m_verts.at(f.at(1)), p2 = m_verts.at(f.at(2));
	Vector3D v1 = p1 - p0, v2 = p2 - p0;
	Vector3D normal = v1.cross(v2);
	m_normals.push_back(normal);
  }
}

double Mesh::det3x3(Vector3D x, Vector3D y, Vector3D z) {
  return x[0] * y[1] * z[2] + x[1] * y[2] * z[0] + x[2] * y[0] * z[1] 
	- x[2] * y[1] * z[0] - x[0] * y[2] * z[1] - x[1] * y[0] * z[2];
}

HitInfo Mesh::intersects(Point3D origin, Vector3D dir) const {
  HitInfo info;
  std::vector<double> bounds = determine_bounds(m_verts);
  info = hits_bounding_box(origin, dir, bounds);
  if (info.empty()) {
	return info;
  }
  info.clear();

  // check for intersection and stuff
  for (unsigned int i = 0; i < m_faces.size(); i++) {
	Hit h;
	Face f = m_faces.at(i);
	Point3D p0 = m_verts.at(f.at(0));

	for (unsigned int j = 1; j < f.size(); j++) {
	  Vector3D vert1 = m_verts.at(f.at(j)) - p0;
	  Vector3D vert2 = m_verts.at(f.at((j+1) % f.size())) - p0;
	  double D = det3x3(Vector3D(vert1[0], vert2[0], -dir[0]),
						Vector3D(vert1[1], vert2[1], -dir[1]),
						Vector3D(vert1[2], vert2[2], -dir[2]));
	  if (D > -EPSILON && D < EPSILON) {
		continue;
	  }
	  Vector3D R = origin - p0;
	  h.t = det3x3(Vector3D(vert1[0], vert2[0], R[0]), 
				   Vector3D(vert1[1], vert2[1], R[1]), 
				   Vector3D(vert1[2], vert2[2], R[2]))  / D;
	  if (h.t < 0.0) {
		continue;
	  }
	  double gamma = det3x3(Vector3D(vert1[0], R[0], -dir[0]), 
							Vector3D(vert1[1], R[1], -dir[1]), 
							Vector3D(vert1[2], R[2], -dir[2]))  / D;
	  if (gamma < 0.0 || gamma > 1.0) {
		continue;
	  }
	  double beta = det3x3(Vector3D(R[0], vert2[0], -dir[0]), 
						   Vector3D(R[1], vert2[1], -dir[1]), 
						   Vector3D(R[2], vert2[2], -dir[2]))  / D;
	  if (beta < 0.0 || beta > 1.0 - gamma) {
		continue;
	  }
	  h.intersection = origin + h.t * dir;
	  if ((h.intersection - origin).length() <= EPSILON) {
		continue;
	  }
	  h.tex_coords = Point2D(beta, gamma);
	  h.normal = m_normals.at(i);
	  h.in_shadow = ((h.intersection - origin).length() > EPSILON) && 
		(dir.length() > (h.intersection - origin).length());
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}
  }
  return info;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}

Plane::Plane(Type t) {
  Face f;
  f.push_back(0);
  f.push_back(1);
  f.push_back(2);
  f.push_back(3);
  m_faces.push_back(f);

  m_type = t;

  switch (t) {
    case XY: {
	  m_verts.push_back(Point3D(-1,  1, 0));
	  m_verts.push_back(Point3D(-1, -1, 0));
	  m_verts.push_back(Point3D( 1, -1, 0));	  
	  m_verts.push_back(Point3D( 1,  1, 0));
	  m_normals.push_back(Vector3D(0, 0, 1));
	}
    case YZ: {
	  m_verts.push_back(Point3D(0, -1,  1));
	  m_verts.push_back(Point3D(0, -1, -1));
	  m_verts.push_back(Point3D(0,  1, -1));	  
	  m_verts.push_back(Point3D(0,  1,  1));
	  m_normals.push_back(Vector3D(1, 0, 0));
	}
    case XZ: {
	  m_verts.push_back(Point3D(-1, 0,  1));
	  m_verts.push_back(Point3D(-1, 0, -1));
	  m_verts.push_back(Point3D( 1, 0, -1));	  
	  m_verts.push_back(Point3D( 1, 0,  1));
	  m_normals.push_back(Vector3D(0, 1, 0));
	}
  }

  Mesh(m_verts, m_faces);
}

HitInfo Plane::intersects(Point3D origin, Vector3D dir) const {
  HitInfo info = Mesh::intersects(origin, dir);
  
  // assume 
  for (unsigned int i = 0; i < info.hits.size(); i++) {
	Point3D p = info.hits.at(i).intersection;
	Point3D bottomleft = m_verts.at(1);
	switch (m_type) {
      case XY: {
		info.hits.at(i).tex_coords[0] = (p[0] - bottomleft[0]) / 2.0;
		info.hits.at(i).tex_coords[1] = (p[1] - bottomleft[1]) / 2.0;
  	  }
      case YZ: {
		info.hits.at(i).tex_coords[0] = (p[2] - bottomleft[2]) / 2.0;
		info.hits.at(i).tex_coords[1] = (p[1] - bottomleft[1]) / 2.0;
	  }
      case XZ: {
		info.hits.at(i).tex_coords[0] = (p[2] - bottomleft[2]) / 2.0;
		info.hits.at(i).tex_coords[1] = (p[0] - bottomleft[0]) / 2.0;
	  }
	}
  }

  return info;
}


