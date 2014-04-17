#include <utility>

#include "primitive.hpp"
#include "polyroots.hpp"


const int faces[6][4] = {
  {0, 1, 2, 3}, // xmin face
  {4, 5, 6, 7}, // xmax face
  {0, 1, 5, 4}, // ymin face
  {3, 7, 6, 2}, // ymax face
  {0, 3, 7, 4}, // zmin face
  {1, 2, 6, 5}  // zmax face
};

const Vector3D normals[6] = {
  Vector3D(-1.0,  0.0,  0.0),
  Vector3D( 1.0,  0.0,  0.0),
  Vector3D( 0.0, -1.0,  0.0),
  Vector3D( 0.0,  1.0,  0.0),
  Vector3D( 0.0,  0.0, -1.0),
  Vector3D( 0.0,  0.0,  1.0),
};

Primitive::~Primitive()
{
}

HitInfo Primitive::hits_sphere(Point3D origin, Vector3D dir, Point3D pos, double radius) {
  HitInfo info;
  Vector3D center_to_origin = origin - pos;
  double A = dir.dot(dir);
  double B = 2 * dir.dot(center_to_origin);
  double C = center_to_origin.dot(center_to_origin) - (radius * radius);
  double roots[2]; 
  int num_roots = quadraticRoots(A, B, C, roots);

  // if D has no roots, then it doesn't intersect.
  if (num_roots < 1) {
	return info;
  } else if (num_roots == 1 && roots[0] > 0) {
	Hit h;
	h.intersection = origin + roots[0] * dir;
	info.hits.push_back(h);
  } else {
	double t1 = roots[0];
	double t2 = roots[1];

	if (t1 <= 0.0 && t2 <= 0.0) {
	  return info;
	} 
	if (t1 >= 0.0) {
	  Hit h;
	  h.intersection = origin + t1 * dir;
	  h.t = t1;
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}	
	if (t2 >= 0.0) {
	  Hit h;
	  h.intersection = origin + t2 * dir;
	  h.t = t2;
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}
  }

  std::vector<Hit> final_hits;
  for (unsigned int i = 0; i < info.hits.size(); i++) {
	double len = (info.hits.at(i).intersection - origin).length();
	if (len > EPSILON) {
	  info.hits.at(i).in_shadow = (len > EPSILON) && (dir.length() > len);
	  info.hits.at(i).normal = info.hits.at(i).intersection - pos;
	  Vector3D Vn = Vector3D(0.0, 1.0, 0.0);
	  Vector3D Ve = Vector3D(1.0, 0.0, 0.0);
	  Vector3D Vp = info.hits.at(i).intersection - pos;
	  Vp.normalize();
	  double phi = acos(-Vn.dot(Vp)); 
	  double theta = (acos(Vp.dot(Ve)) / sin(phi)) / (2 * M_PI);
	  double u = (Vp.dot(Vn.cross(Ve)) > 0) ? theta : 1 - theta;
	  double v = phi / M_PI;
	  if (u < 0) {
		u = 0.0;
	  }
	  if (u > 1.0) {
		u = 1.0;
	  }
	  if (v < 0.0) {
		v = 0.0;
	  }
	  if (v > 1.0) {
		v = 1.0;
	  }
	  //std::cout <<"uv: " <<  u << " " << v << std::endl;
	  info.hits.at(i).tex_coords[0] = u;
	  info.hits.at(i).tex_coords[1] = v;
	  final_hits.push_back(info.hits.at(i));
	}
  }
  info.hits = final_hits;
  if (info.hits.size() == 2) {
	info.lines.push_back(std::make_pair(info.hits.at(0), info.hits.at(1)));
	info.hits.clear();
  }
  return info;
}

// returns bounds in (xmin, xmax, ymin, ymax, zmin, zmax) order.
std::vector<double> Primitive::determine_bounds(const std::vector<Point3D>& vertices) {
  std::vector<double> bounds;
  bounds.push_back(vertices.at(0)[0]);
  bounds.push_back(vertices.at(0)[0]);
  bounds.push_back(vertices.at(0)[1]);
  bounds.push_back(vertices.at(0)[1]);
  bounds.push_back(vertices.at(0)[2]);
  bounds.push_back(vertices.at(0)[2]);

  // get max and min values for (x,y,z) for p_int to be on face
  for(unsigned int i = 0; i < vertices.size(); i++) {
	if (bounds.at(0) > vertices.at(i)[0] + EPSILON) {
	  bounds.at(0) = vertices.at(i)[0];
	}
	if (bounds[2] > vertices.at(i)[1] + EPSILON) {
	  bounds.at(2) = vertices.at(i)[1];
	}
	if (bounds.at(4) > vertices.at(i)[2] + EPSILON) {
	  bounds.at(4) = vertices.at(i)[2];
	}
	if (bounds.at(1) + EPSILON < vertices.at(i)[0]) {
	  bounds.at(1) = vertices.at(i)[0];
	}
	if (bounds.at(3) + EPSILON < vertices.at(i)[1]) {
	  bounds.at(3) = vertices.at(i)[1];
	}
	if (bounds.at(5) + EPSILON < vertices.at(i)[2]) {
	  bounds.at(5) = vertices.at(i)[2];
	}
  }

  return bounds;
}

HitInfo Primitive::hits_bounding_box(Point3D origin, Vector3D dir,std::vector<double> bounds) {
  HitInfo info;
  Point3D vertices[8] = {
	Point3D(bounds.at(0), bounds.at(2), bounds.at(4)),
	Point3D(bounds.at(0), bounds.at(2), bounds.at(5)),
	Point3D(bounds.at(0), bounds.at(3), bounds.at(5)),
	Point3D(bounds.at(0), bounds.at(3), bounds.at(4)),
	Point3D(bounds.at(1), bounds.at(2), bounds.at(4)),
	Point3D(bounds.at(1), bounds.at(2), bounds.at(5)),
	Point3D(bounds.at(1), bounds.at(3), bounds.at(5)),
	Point3D(bounds.at(1), bounds.at(3), bounds.at(4)),
  };

  for(int i = 0; i < 6; i++) {
	Hit hit;
	Point3D p0 = vertices[faces[i][0]];
	Vector3D N_face = normals[i];
	double num = -N_face.dot(origin - p0);
	double denom = N_face.dot(dir);

	if (denom > -EPSILON && denom < EPSILON) {
	  continue;
	}

	hit.t = num / denom;

	if (hit.t < 0.0) {
	  continue;
	}

	Point3D p_int = origin + hit.t * dir;
	std::vector<Point3D> verts;
	for (int j = 0; j < 4; j++) {
	  verts.push_back(vertices[faces[i][j]]);
	}
	std::vector<double> bounds = determine_bounds(verts);
	bool insert = false;
	hit.intersection = p_int;
	hit.normal = normals[i];
	hit.in_shadow = ((hit.intersection - origin).length() > EPSILON) && 
	  (dir.length() > (hit.intersection - origin).length());

	double len = (hit.intersection - origin).length();
	if (len <= EPSILON) {
	  continue;
	}
	  
	// check faces to determine texture coordinates
	switch(i) {
	case 0:
	case 1: {
	  if (bounds.at(2) < p_int[1] && p_int[1] <= bounds.at(3) &&
		  bounds.at(4) < p_int[2] && p_int[2] <= bounds.at(5)) {
		double width = bounds.at(3) - bounds.at(2);
		double height = bounds.at(5) - bounds.at(4);
		hit.tex_coords[0] = (p_int[1] - bounds.at(2)) / width;
		hit.tex_coords[1] = (p_int[2] - bounds.at(4)) / height;
		insert = true;
	  }
	  break;
	}
	case 2:
	case 3: {
	  if (bounds.at(0) < p_int[0] && p_int[0] <= bounds.at(1) &&
		  bounds.at(4) < p_int[2] && p_int[2] <= bounds.at(5)) {	
		double width = bounds.at(1) - bounds.at(0);
		double height = bounds.at(5) - bounds.at(4);
		hit.tex_coords[0] = (p_int[0] - bounds.at(0)) / width;
		hit.tex_coords[1] = (p_int[2] - bounds.at(4)) / height;
		insert = true;
	  }
	  break;
	}		  
	case 4:
	case 5: {
	  if (bounds.at(0) < p_int[0] && p_int[0] <= bounds.at(1) && 
		  bounds.at(2) < p_int[1] && p_int[1] <= bounds.at(3)) {
		double width = bounds.at(1) - bounds.at(0);
		double height = bounds.at(3) - bounds.at(2);
		hit.tex_coords[0] = (p_int[0] - bounds.at(0)) / width;
		hit.tex_coords[1] = (p_int[1] - bounds.at(2)) / height;
		insert = true;
	  }
	  break;
	}
	default:
	  std::cerr << "invalid face" << std::endl;
	}

	if (insert) {
	  if (hit.tex_coords[0] < 0.0) {
		hit.tex_coords[0] = 0.0;
	  }
	  if (hit.tex_coords[0] > 1.0) {
		hit.tex_coords[0] = 1.0;
	  }
	  if (hit.tex_coords[1] < 0.0) {
		hit.tex_coords[1] = 0.0;
	  }
	  if (hit.tex_coords[1] > 1.0) {
		hit.tex_coords[1] = 1.0;
	  }
	  
	  info.hits = Hit::insert_hit_in_order(info.hits, hit);
	}
  }
  // get line segment
  if (info.hits.size() == 2) {
	info.lines.push_back(std::make_pair(info.hits.at(0), info.hits.at(1)));
	info.hits.clear();
  }
  return info;
}

Sphere::~Sphere()
{
}

HitInfo Sphere::intersects(Point3D origin, Vector3D dir) const {
  return hits_sphere(origin, dir, Point3D(0.0, 0.0, 0.0), 1.0); 
}

Cube::~Cube()
{
}

HitInfo Cube::intersects(Point3D origin, Vector3D dir) const {
  std::vector<double> bounds;
  bounds.push_back(-0.5);
  bounds.push_back(0.5);
  bounds.push_back(-0.5);
  bounds.push_back(0.5);
  bounds.push_back(-0.5);
  bounds.push_back(0.5);
  return hits_bounding_box(origin, dir, bounds);
}

Cylinder::~Cylinder() 
{
}

HitInfo Cylinder::intersects(Point3D origin, Vector3D dir) const {
  HitInfo info;
  double A = (dir[0] * dir[0]) + (dir[2] * dir[2]);
  double B = 2 * (origin[0] * dir[0] + origin[2] * dir[2]);
  double C = origin[0] * origin[0]  + origin[2] * origin[2] - 1.0;
  double roots[2]; 
  int num_roots = quadraticRoots(A, B, C, roots);

  // check for cap intersections
  if (dir[1] != 0.0) {
	Point3D cmin = Point3D(0.0, -1.0, 0.0), cmax = Point3D(0.0, 1.0, 0.0);
	double t3min = (-1.0 - origin[1]) / dir[1];
	double t3max = (1.0 - origin[1]) / dir[1];
	Point3D mincap = origin + t3min * dir;
	Point3D maxcap = origin + t3max * dir;

	if (t3min > 0.0 && (mincap - cmin).length() <= 1.0) {
	  Hit h;
	  h.t = t3min;
	  h.intersection = mincap;
	  h.normal = Vector3D(0.0, -1.0, 0.0);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}

	if (t3max > 0.0 && (maxcap - cmax).length() <= 1.0) {
	  Hit h;
	  h.t = t3max;
	  h.intersection = maxcap;
	  h.normal = Vector3D(0.0, 1.0, 0.0);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}
  }

  if (num_roots == 1 && roots[0] > 0.0) {
	Point3D p = origin + roots[0] * dir;
	if (p[1] >= -1.0 && p[1] <= 1.0) {
	  Hit h;
	  h.t = roots[0];
	  h.intersection = p;
	  h.normal = Vector3D(p[0], 0.0, p[2]);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}
  } else if (num_roots == 2) {
	Point3D p0 = origin + roots[0] * dir;
	if (roots[0] > 0.0 && p0[1] >= -1.0 && p0[1] <= 1.0) {
	  Hit h;
	  h.t = roots[0];
	  h.intersection = p0;
	  h.normal = Vector3D(p0[0], 0.0, p0[2]);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}

	Point3D p1 = origin + roots[1] * dir;
	if (roots[1] > 0.0 && p1[1] >= -1.0 && p1[1] <= 1.0) {
	  Hit h;
	  h.t = roots[1];
	  h.intersection = p1;
	  h.normal = Vector3D(p1[0], 0.0, p1[2]);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}
  }

  std::vector<Hit> final_hits;
  for (unsigned int i = 0; i < info.hits.size(); i++) {
	double len = (info.hits.at(i).intersection - origin).length();
	if (len > EPSILON) {
	  Vector3D Vn = Vector3D(0.0, 1.0, 0.0);
	  Vector3D Ve = Vector3D(1.0, 0.0, 0.0);
	  Vector3D Vp = info.hits.at(i).intersection - Point3D(0.0, 0.5, 0.0);
	  Vp.normalize();
	  double phi = acos(-Vn.dot(Vp)); 
	  double theta = (acos(Vp.dot(Ve)) / sin(phi)) / (2 * M_PI);
	  double u = (Vp.dot(Vn.cross(Ve)) > 0) ? theta : 1 - theta;
	  double v = phi / M_PI;
	  if (u < 0) {
		u = 0.0;
	  }
	  if (u > 1.0) {
		u = 1.0;
	  }
	  if (v < 0.0) {
		v = 0.0;
	  }
	  if (v > 1.0) {
		v = 1.0;
	  }
	  info.hits.at(i).tex_coords[0] = u;
	  info.hits.at(i).tex_coords[1] = v;
	  info.hits.at(i).in_shadow = (len > EPSILON) && (dir.length() > len);
	  final_hits.push_back(info.hits.at(i));
	}
  }
  info.hits = final_hits;
  if (info.hits.size() == 2) {
	info.lines.push_back(std::make_pair(info.hits.at(0), info.hits.at(1)));
	info.hits.clear();
  }
  return info;
}

Cone::~Cone() 
{
}

HitInfo Cone::intersects(Point3D origin, Vector3D dir) const {
  HitInfo info;
  double A = dir[0] * dir[0] + dir[2] * dir[2] - dir[1] * dir[1];
  double B = 2 * (origin[0] * dir[0] + origin[2] * dir[2] - (origin[1] - 1) * dir[1]);
  double C = origin[0] * origin[0]  + origin[2] * origin[2] - (origin[1] - 1.0) * 
	(origin[1] - 1.0);
  double roots[2]; 
  int num_roots = quadraticRoots(A, B, C, roots);

  if (dir[1] != 0.0) {
	Hit hit;
	double t3 = (0.0 - origin[1]) / dir[1];
	Point3D p = origin + t3 * dir;
	if (t3 > 0.0 && (p - Point3D(0.0, 0.0, 0.0)).length() <= 1.0) {
	  hit.normal = Vector3D(0.0, -1.0, 0.0);
	  hit.intersection = p;
	  hit.t = t3;
	  info.hits.push_back(hit);
	}
  }

  if (num_roots == 1 && roots[0] > 0.0) {
	// one hit on the cone
	Point3D p = origin + roots[0] * dir;
	if (p[1] >= 0.0 && p[1] <= 1.0) {
	  Hit hit;
	  hit.normal = Vector3D(p[0], 0.5, p[2]);
	  hit.intersection = p;
	  hit.t = roots[0];
	  info.hits = Hit::insert_hit_in_order(info.hits, hit);
	}
  } else if (num_roots == 2) {
	// two hits on cone
	Point3D p0 = origin + roots[0] * dir;
	if (roots[0] > 0.0 && p0[1] >= 0.0 && p0[1] <= 1.0) {
	  Hit h;
	  h.t = roots[0];
	  h.intersection = p0;
	  h.normal = Vector3D(p0[0], 0.0, p0[2]);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}

	Point3D p1 = origin + roots[1] * dir;
	if (roots[1] > 0.0 && p1[1] >= 0.0 && p1[1] <= 1.0) {
	  Hit h;
	  h.t = roots[1];
	  h.intersection = p1;
	  h.normal = Vector3D(p1[0], 0.0, p1[2]);
	  info.hits = Hit::insert_hit_in_order(info.hits, h);
	}
  }

  std::vector<Hit> final_hits;
  for (unsigned int i = 0; i < info.hits.size(); i++) {
	double len = (info.hits.at(i).intersection - origin).length();
	if (len > EPSILON) {
	  Vector3D Vn = Vector3D(0.0, 1.0, 0.0);
	  Vector3D Ve = Vector3D(1.0, 0.0, 0.0);
	  Vector3D Vp = info.hits.at(i).intersection - Point3D(0.0, 0.5, 0.0);
	  Vp.normalize();
	  double phi = acos(-Vn.dot(Vp)); 
	  double theta = (acos(Vp.dot(Ve)) / sin(phi)) / (2 * M_PI);
	  double u = (Vp.dot(Vn.cross(Ve)) > 0) ? theta : 1 - theta;
	  double v = phi / M_PI;
	  if (u < 0) {
		u = 0.0;
	  }
	  if (u > 1.0) {
		u = 1.0;
	  }
	  if (v < 0.0) {
		v = 0.0;
	  }
	  if (v > 1.0) {
		v = 1.0;
	  }
	  info.hits.at(i).tex_coords[0] = u;
	  info.hits.at(i).tex_coords[1] = v;
	  info.hits.at(i).in_shadow = (len > EPSILON) && (dir.length() > len);
	  final_hits.push_back(info.hits.at(i));
	}
  }
  info.hits = final_hits;

  if (info.hits.size() == 2) {
	info.lines.push_back(std::make_pair(info.hits.at(0), info.hits.at(1)));
	info.hits.clear();
  }
  return info;
}

NonhierSphere::~NonhierSphere()
{
}

HitInfo NonhierSphere::intersects(Point3D origin, Vector3D dir) const {
  return hits_sphere(origin, dir, m_pos, m_radius);
}

NonhierBox::~NonhierBox()
{
}

HitInfo NonhierBox::intersects(Point3D origin, Vector3D dir) const {
  std::vector<double> bounds;
  bounds.push_back(m_pos[0] - (double)m_size/2.0);
  bounds.push_back(m_pos[0] + (double)m_size/2.0);
  bounds.push_back(m_pos[1] - (double)m_size/2.0);
  bounds.push_back(m_pos[1] + (double)m_size/2.0);
  bounds.push_back(m_pos[2] - (double)m_size/2.0);
  bounds.push_back(m_pos[2] + (double)m_size/2.0);
  return hits_bounding_box(origin, dir, bounds);
}
