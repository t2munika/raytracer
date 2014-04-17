#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

// structure for command-line settings
struct Settings {
  unsigned int num_glossy_rays;
  unsigned int num_supersample;
  unsigned int num_dof_rays;
  unsigned int num_threads;

  Settings(const Settings &a) : num_glossy_rays(a.num_glossy_rays), 
								num_supersample(a.num_supersample),
								num_dof_rays(a.num_dof_rays),
								num_threads(a.num_threads) {}  
  Settings() : num_glossy_rays(0), num_supersample(0), num_dof_rays(2), num_threads(6) {}
  
};

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights,
			   double focal_length,
			   double aperture,
			   Settings settings
               );

#endif
