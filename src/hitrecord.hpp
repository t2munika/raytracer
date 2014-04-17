#ifndef CS488_HITRECORD_HPP
#define CS488_HITRECORD_HPP

#include <list>
#include <vector>
#include "algebra.hpp"

class Material;

struct Hit {
  double t;
  Point3D intersection;

  // appearance parameters
  Vector3D normal;
  Vector3D b_normal;
  Material* material;
  bool in_shadow;
  Point2D tex_coords;
  Colour diffuse;
  Colour specular;

  Hit(const Hit &a) : t(a.t), intersection(a.intersection), normal(a.normal), 
					  b_normal(a.b_normal), material(a.material), in_shadow(a.in_shadow),
					  tex_coords(a.tex_coords), diffuse(a.diffuse), specular(a.specular) {}  
  Hit() : t(INFINITY), material(0), in_shadow(false), diffuse(0.0, 0.0, 0.0), 
		  specular(0.0, 0.0, 0.0) {}

  static std::vector<Hit> insert_hit_in_order(std::vector<Hit> hits, Hit h);
  static std::vector<Hit> merge_hits(std::vector<Hit> h1, std::vector<Hit> h2);
};


struct HitInfo {
  typedef std::pair<Hit, Hit> LineSegment;

  std::vector<Hit> hits;
  std::vector<LineSegment> lines;

  HitInfo(const HitInfo &a) : hits(a.hits), lines(a.lines) {}  
  HitInfo() {}  
  Hit get_closest();
  void clear();
  std::vector<Hit> get_all_hits();
  static std::vector<LineSegment> insert_line_in_order(std::vector<LineSegment> h1, 
													   LineSegment h2);
  bool empty() { return hits.empty() && lines.empty(); }
  static HitInfo merge_info(HitInfo h1, HitInfo h2);
};
#endif
