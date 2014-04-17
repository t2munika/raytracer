#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include <vector>
#include "algebra.hpp"
#include "hitrecord.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const = 0;
  static std::vector<double> determine_bounds(const std::vector<Point3D>& vertices);
  static HitInfo hits_sphere(Point3D origin, Vector3D dir, Point3D pos, double radius);
  static HitInfo hits_bounding_box(Point3D origin, Vector3D dir, std::vector<double> bounds);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;
};

/*
 * Assumes cylinder is y-axis aligned with height and radius of 1 in modelling coordinates
 */
class Cylinder : public Primitive {
public:
  virtual ~Cylinder();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;
};

/*
 * Assumes cone is y-axis aligned with height and radius of 1 in modelling coordinates, with
 * apex at (0, 1, 0)
 */
class Cone : public Primitive {
public:
  virtual ~Cone();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;

private:
  Point3D m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }  
  virtual ~NonhierBox();
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;

private:
  Point3D m_pos;
  double m_size;
};

#endif
