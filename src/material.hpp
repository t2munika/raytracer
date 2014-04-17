#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "light.hpp"
#include "image.hpp"
#include "hitrecord.hpp"

class Material {
public:
  virtual ~Material();
  virtual Colour get_ambient(Colour ambient) const { return ambient; }

  virtual bool should_refract() const { return false; }
  virtual bool get_transmit_ray(Vector3D& refracted, Vector3D view, Hit h) const { 
	refracted = refracted; view = view; h = h;
	return false; 
  }
  virtual Colour get_refract(Colour refracted) const { return refracted; }

  virtual bool should_reflect() const { return false; }
  virtual Colour get_reflect(Colour c) const { return c; }

  virtual Hit apply_material(Vector3D view, Light* light, Hit h) const = 0;

  // bump-mapping stuff
  virtual void set_normal_map(const std::string filename);
  virtual Hit apply_normal_map(Hit h);
  virtual void set_bump_map(const std::string filename);
  virtual Hit apply_bump_map(Hit h);

  virtual double get_transparency() { return 0.0; }

protected:
  Material() : m_has_normal_map(false), m_has_bump_map(false)
  {
  }
  
  Image m_bump_map;
  Image m_normal_map;
  bool m_has_normal_map;
  bool m_has_bump_map;
};

class PhongMaterial : public Material {
public:
  // default value of refract index is not used of transparency is 0 anyway, but most solids have a 
  // refraction index between 1 and 2
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double transparency = 0.0,
				double n = 1.5);
  virtual ~PhongMaterial();
  virtual Colour get_ambient(Colour ambient) const;

  virtual double get_transparency() { return m_transparency; }

  // methods for refraction
  virtual bool should_refract() const;
  virtual bool get_transmit_ray(Vector3D& refracted, Vector3D view, Hit h) const;
  virtual Colour get_refract(Colour refracted) const;

  // methods for reflection
  virtual bool should_reflect() const;
  virtual Colour get_reflect(Colour c) const;

  virtual Hit apply_material(Vector3D view, Light* light, Hit h) const;
private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  double m_transparency;
  double m_refract_index;
  Colour phong_coefficient(Vector3D l, Vector3D r, Vector3D v, Vector3D N) const;
};

class TextureMaterial : public Material {
public:
  TextureMaterial(const std::string filename);
  virtual ~TextureMaterial();
  virtual Hit apply_material(Vector3D view, Light* light, Hit h) const;
  virtual Colour get_reflect(Colour c) const { return c; }

private:
  Colour get_colour(Point2D tex) const;
  Image m_img;
};

#endif
