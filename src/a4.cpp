#include "a4.hpp"
#include "image.hpp"
#include <pthread.h>

// Image to write to.
Image img; 

struct ThreadInfo {
  int start_row;
  int num_rows;

  SceneNode* root;
  int width;
  int height;
  double w_world;
  double h_world;
  Vector3D u;
  Vector3D v;
  Vector3D w;
  Point3D eye;

  Colour ambient;
  std::list<Light*> lights;
  double focal_length;
  double aperture;

  Settings settings;

  ThreadInfo() : ambient(0.0, 0.0, 0.0) {}
};

void* render_rows(void* info);

Point3D view_to_world(Point3D pixel, const Point3D& eye, const Matrix4x4& rotation, 
					  double h_world, double w_world, int width, int height, 
					  double distance) {
  Matrix4x4 T1 = Matrix4x4(
    Vector4D(1.0, 0.0, 0.0, -width/2),
    Vector4D(0.0, 1.0, 0.0, -height/2),
    Vector4D(0.0, 0.0, 1.0, distance),
    Vector4D(0.0, 0.0, 0.0, 1.0));
  Matrix4x4 S2 = Matrix4x4(
    Vector4D(-w_world/width, 0.0, 0.0, 0.0),
    Vector4D(0.0, h_world/height, 0.0, 0.0),
    Vector4D(0.0, 0.0, 1.0, 0.0),
    Vector4D(0.0, 0.0, 0.0, 1.0));
  Matrix4x4 T4 = Matrix4x4(
    Vector4D(1.0, 0.0, 0.0, eye[0]),
	Vector4D(0.0, 1.0, 0.0, eye[1]),
    Vector4D(0.0, 0.0, 1.0, eye[2]),
    Vector4D(0.0, 0.0, 0.0, 1.0));

  pixel = T4 * rotation * S2 * T1 * pixel;
  return pixel;
}

Vector3D get_ray(Point3D pixel, const Point3D& eye, const Matrix4x4& rotation, double h_world, 
				 double w_world, int width, int height, double distance) {
  pixel = view_to_world(pixel, eye, rotation, h_world, w_world, width, height, distance);
  return pixel - eye;
}


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
			   // camera parameters
			   double focal_length,
			   double aperture,
			   Settings settings
               )
{
  // Fill in raytracing code here.
  double h_world = 2 * focal_length * tan((fov/2) * M_PI / 180);
  double w_world = (width / height) * h_world;

  // world coordinate system
  Vector3D w = view;
  w.normalize();
  Vector3D u = up.cross(w);
  u.normalize();
  Vector3D v = w.cross(u);

  img = Image(width, height, 3);
  pthread_t workers[settings.num_threads];
  ThreadInfo info[settings.num_threads];

  int rows_to_render = height;
  int start_row = 0;
  for (unsigned int i = 0; i < settings.num_threads; i++) {
	int num_rows = (i == settings.num_threads - 1) ? rows_to_render : 
	  height / settings.num_threads;
	info[i].root = root;
	info[i].height = height;
	info[i].width = width;
	info[i].h_world = h_world;
	info[i].w_world = w_world;
	info[i].u = u;
	info[i].v = v;
	info[i].w = w;
	info[i].focal_length = focal_length;
	info[i].aperture = aperture;
	info[i].ambient = ambient;
	info[i].lights = lights;
	info[i].eye = eye;
	info[i].start_row = start_row;
	info[i].settings = settings;
	info[i].num_rows = num_rows;
	std::cerr << "render " << num_rows << " rows starting from " << start_row << std::endl;
	pthread_create(&workers[i], NULL, render_rows, &info[i]);
	rows_to_render -= num_rows;
	start_row += num_rows;
  }

  for (unsigned int i = 0; i < settings.num_threads; i++) {
	pthread_join(workers[i], NULL);
  }

  std::cerr << "Stub: a4_render(" << root << ",\n     "
            << filename << ", " << width << ", " << height << ",\n     "
            << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
            << ambient << ",\n     {";

  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << "} ";

  std::cerr << focal_length << ", " << aperture << ");" << std::endl;
  
  img.savePng(filename);  
}

void* render_rows(void* info) {
  ThreadInfo* ti = (ThreadInfo*) info;
  SceneNode* root = ti->root;
  int start_row = ti->start_row;
  int end_row = ti->start_row + ti->num_rows;
  int height = ti->height;
  int width = ti->width;
  double h_world = ti->h_world;
  double w_world = ti->w_world;
  Vector3D u = ti->u;
  Vector3D v = ti->v;
  Vector3D w = ti->w;
  double focal_length = ti->focal_length;
  double aperture = ti->aperture;
  Colour ambient = ti->ambient;
  Settings s = ti->settings;
  std::list<Light*> lights = ti->lights;
  Point3D eye = ti->eye;  

  Matrix4x4 rot = Matrix4x4(
	Vector4D(u[0], v[0], w[0], 0.0),
	Vector4D(u[1], v[1], w[1], 0.0),
	Vector4D(u[2], v[2], w[2], 0.0),
	Vector4D(0.0, 0.0, 0.0, 1.0));

  for (int j = start_row; j < end_row; j++) {
	for (int i = 0; i < width; i++) {
	  // for each pixel, get the point (x,y) and convert to world coordinates
	  Colour bg(0.0, 0.0, (double) j / height);
	  Colour pixel_colour(0.0, 0.0, 0.0);
	  Point3D pixel = Point3D(i, height - j, 0.0);
	  Vector3D ray = get_ray(pixel, eye, rot, h_world, w_world, width, height, focal_length);

	  if (s.num_supersample == 0) {
		if (aperture > 0.0 && s.num_dof_rays > 1) {
		  ray.normalize();
		  Point3D focalPoint = eye + focal_length * ray;
		  for (unsigned int k = 0; k < s.num_dof_rays; k++) {
			double dx = ((double)(rand() % 100) / 100.0) * aperture; 
			double dy = ((double)(rand() % 100) / 100.0) * aperture; 
			Point3D n_eye = eye + dx * u + dy * v;
			Vector3D fray = focalPoint - n_eye;
			Colour c = root->get_colour(n_eye, fray, ambient, bg, lights, s.num_glossy_rays);
			pixel_colour = pixel_colour + c;
		  }
		  pixel_colour = (1.0 / s.num_dof_rays) * pixel_colour;
		} else {
		  pixel_colour = root->get_colour(eye, ray, ambient, bg, lights, s.num_glossy_rays);
		}
	  } else {
		// supersampling with 4 sample points using jitter algorithm
		for (unsigned int x = 0; x < s.num_supersample; x++) {
		  for(unsigned int y = 0; y < s.num_supersample; y++) {
			double subpix_side = 1.0 / s.num_supersample;
			double r1 = (double) (rand() % 100) / 100.0;
			double r2 = (double) (rand() % 100) / 100.0;
			Point3D subpix = Point3D((double) i + subpix_side * (x + r1), 
									 (double) (height - j) + subpix_side* (y + r2), 0.0);
			Vector3D ray = get_ray(subpix, eye, rot, h_world, w_world, width, height, focal_length);

			if (aperture > 0.0 && s.num_dof_rays > 1) {
			  ray.normalize();
			  Point3D focalPoint = eye + focal_length * ray;
			  Colour dc(0.0, 0.0, 0.0);
			  for (unsigned int k = 0; k < s.num_dof_rays; k++) {
				double dx = ((double)(rand() % 100) / 100.0) * aperture; 
				double dy = ((double)(rand() % 100) / 100.0) * aperture; 
				Point3D n_eye = eye + dx * u + dy * v;
				Vector3D fray = focalPoint - n_eye;
				Colour c = root->get_colour(n_eye, fray, ambient, bg, lights, s.num_glossy_rays);
				dc = dc + c;
			  }
			  dc = (1.0 / s.num_dof_rays) * dc;
			  pixel_colour = pixel_colour + dc;
			} else {
			  Colour c = root->get_colour(eye, ray, ambient, bg, lights, s.num_glossy_rays);
			  pixel_colour = pixel_colour + c;
			}
		  }
		}
		pixel_colour = (1.0 / (s.num_supersample * s.num_supersample)) * pixel_colour;
	  }

	  img(i, j, 0) = pixel_colour.R();
	  img(i, j, 1) = pixel_colour.G();
	  img(i, j, 2) = pixel_colour.B();
	}
  }
  std::cerr << "rows " << start_row << " to " << end_row-1 << " completed" << std::endl;
}
