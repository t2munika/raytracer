#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh() {}
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;
  HitInfo intersects(Point3D origin, Vector3D dir) const;
  
protected:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  std::vector<Vector3D> m_normals;
  static double det3x3(Vector3D x, Vector3D y, Vector3D z);
  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

// Assumes that plane vertex listing start at the top left corner
class Plane : public Mesh {
public:
  enum Type {
	XY,
	XZ,
	YZ
  };

  Plane(Type t);
  HitInfo intersects(Point3D origin, Vector3D dir) const;

private:
  Type m_type;
};

#endif
