#include "material.hpp"

Material::~Material()
{
}

void Material::set_normal_map(const std::string filename) {
  m_has_normal_map = m_normal_map.loadPng(filename);
  if (!m_has_normal_map) {
	std::cerr << "error loading texture" << std::endl;
  }
}

Hit Material::apply_normal_map(Hit h) {
  if (!m_has_normal_map) {
	return h;
  }
  int x = h.tex_coords[0] * m_normal_map.width();
  int y = (1 - h.tex_coords[1]) * m_normal_map.height();
  Colour c = Colour(m_normal_map(x, y, 0), m_normal_map(x, y, 1), m_normal_map(x, y, 2));

  // convert to surface normal;
  h.normal = Vector3D(2 * c.R() - 1.0, 2 * c.G() - 1.0, 2 * c.B() - 1.0);
  return h;
}

void Material::set_bump_map(const std::string filename) {
  m_has_bump_map = m_bump_map.loadPng(filename);
  if (!m_has_bump_map) {
	std::cerr << "error loading texture" << std::endl;
  }
}

Hit Material::apply_bump_map(Hit h) {
  Vector3D N = h.normal;
  if (!m_has_bump_map) {
	return h;
  }
  int x = h.tex_coords[0] * m_bump_map.width();
  int y = (1 - h.tex_coords[1]) * m_bump_map.height();
  double dx, dy;

  if (x == 0) {
	dx = (m_bump_map(x+2, y, 0) - m_bump_map(x, y, 0));	
  } else if (x == m_bump_map.width()) {
	dx = (m_bump_map(x, y, 0) - m_bump_map(x-1, y, 0)) / 0.5;
  } else {
	dx = m_bump_map(x+1, y, 0) - m_bump_map(x-1, y, 0);
  }

  if (y == 0) {
	dy = (m_bump_map(x, y+2, 0) - m_bump_map(x, y, 0));
  } else if (y == m_bump_map.height()) {
	dy = (m_bump_map(x, y, 0) - m_bump_map(x, y-2, 0));
  } else {
	dy = m_bump_map(x, y+1, 0) - m_bump_map(x, y-1, 0);
  }

  dx = dx * 100;
  dy = dy * 100;

  Vector3D w = h.normal;
  w.normalize();
  Vector3D u = w.cross(Vector3D(1.0, 0.0, 0.0));
  u.normalize();
  Vector3D v = w.cross(Vector3D(0.0, 1.0, 0.0));
  v.normalize();

  // convert to surface normal;
  h.normal = N + dx * u + dy * v;
  return h;
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double transparency,
							 double n)
  : m_kd(kd), m_ks(ks), m_shininess(shininess), m_transparency(transparency), m_refract_index(n)
{
}

PhongMaterial::~PhongMaterial()
{
}

Colour PhongMaterial::phong_coefficient(Vector3D l, Vector3D reflected, Vector3D v, Vector3D N) const {
  return (1 / N.dot(l)) * pow(reflected.dot(v), m_shininess);
}

bool PhongMaterial::should_reflect() const {
  return (m_ks.R() > 0.0 || m_ks.G() > 0.0 || m_ks.B() > 0.0);
}

Colour PhongMaterial::get_reflect(Colour c) const {
  return m_ks * c;
}

bool PhongMaterial::should_refract() const {
  return m_transparency > 0.0;
}

bool PhongMaterial::get_transmit_ray(Vector3D& refracted, Vector3D view, Hit h) const {
  view.normalize();
  h.normal.normalize();
  if (view.dot(h.normal) < 0) {
	// the transmitted ray is going into the object, assume for now that 
	// incident refract index is same as  air (n = 1)
	double vdotN = h.normal.dot(view);
	double coeff = 1.0 / m_refract_index;
	double root = 1 - (coeff * coeff) * (1 - vdotN * vdotN);
	if (root < 0) {
	  return false;
	}
	refracted = (-coeff * vdotN - sqrt(root)) * h.normal + (coeff * view);
  } else {
	double vdotN = (-h.normal).dot(view);
	double coeff = m_refract_index / 1.0;
	double root = 1 - ((coeff * coeff) * (1 - vdotN * vdotN));
	if (root < 0) {
	  return false;
	}
	refracted = (-coeff * vdotN - sqrt(root)) * -h.normal + (coeff * view);
  }
  return true;  
}

Colour PhongMaterial::get_refract(Colour refracted) const {
  return m_transparency * refracted;
}

Colour PhongMaterial::get_ambient(Colour ambient) const {
  return m_kd * ambient;
}

Hit PhongMaterial::apply_material(Vector3D view, Light* light, Hit h) const {
  h.diffuse = Colour(0.0, 0.0, 0.0); 
  h.specular = Colour(0.0, 0.0, 0.0); 
  Vector3D l = light->position - h.intersection;
  Vector3D N = h.normal;
  double r = l.length();
  l.normalize(); N.normalize(); view.normalize();
  double ldotN = l.dot(N);
  Vector3D reflected = -l + (2 * ldotN * N);
  reflected.normalize();
  double attenuation = light->falloff[0] + light->falloff[1] * r + 
	light->falloff[2] * r * r;

  if ((m_kd.R() > 0.0 || m_kd.G() > 0.0 || m_kd.B() > 0.0) && l.dot(N) > 0.0) {
	attenuation = 1 / attenuation;
	h.diffuse = (1.0 - m_transparency) * ldotN * attenuation * light->colour * m_kd;
  }

  if ((m_ks.R() > 0.0 || m_ks.G() > 0.0 || m_ks.B() > 0.0) && l.dot(N) > 0.0) {
	Colour phong = phong_coefficient(l, reflected, view, N);
	h.specular = ldotN * attenuation * light->colour * phong * m_ks;
  }

  return h;
}

TextureMaterial::TextureMaterial(const std::string filename) {
  bool loaded = m_img.loadPng(filename);
  if (!loaded) {
	std::cout << "error loading texture" << std::endl;
  }
}

TextureMaterial::~TextureMaterial() 
{
}

Colour TextureMaterial::get_colour(Point2D tex_coords) const {
  int x = tex_coords[0] * m_img.width();
  int y = (1 - tex_coords[1]) * m_img.height();
  return Colour(m_img(x, y, 0), m_img(x, y, 1), m_img(x, y, 2));
}

Hit TextureMaterial::apply_material(Vector3D view, Light* light, Hit h) const {
  Vector3D l = light->position - h.intersection;
  Vector3D N = h.normal;
  double r = l.length();
  l.normalize();
  N.normalize();
  view.normalize();

  double ldotN = l.dot(N);
  if (ldotN > 0.0) {
	Vector3D reflected = -l + (2 * ldotN * N);
	reflected.normalize();
	double attenuation = light->falloff[0] + light->falloff[1] * r + 
	  light->falloff[2] * r * r;
	attenuation = 1 / attenuation;
	h.diffuse = ldotN * attenuation * light->colour * get_colour(h.tex_coords);
  } else {
	h.diffuse = Colour(0.0, 0.0, 0.0); 
  }

  return h;
}
