#include <iostream>
#include <cstdlib>
#include "scene_lua.hpp"
#include "a4.hpp"

int main(int argc, char** argv)
{
  std::string filename = "scene.lua";
  if (argc >= 2) {
    filename = argv[argc-1];
  }

  Settings settings;
  int i = 1;

  while(i < argc - 1) {
	std::string arg(argv[i]);
	if (arg == "-NUM_SUPERSAMPLE") {
	  settings.num_supersample = std::atoi(argv[i+1]);
	  i += 2;
	} else if (arg == "-NUM_GLOSSY_RAYS") {
	  settings.num_glossy_rays = std::atoi(argv[i+1]);
	  i += 2;
	} else if (arg == "-NUM_DOF_RAYS") {
	  settings.num_dof_rays = std::atoi(argv[i+1]);
	  i += 2;
	} else if (arg == "-NUM_THREADS") {
	  settings.num_threads = std::atoi(argv[i+1]);
	  i += 2;
	} else {
	  std::cerr << "Invalid option!" << std::endl;
	  return 1;
	}
  }

  if (!run_lua(filename, settings)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}

