#include "kdtree.h"
#ifdef _WIN32 // vis only supported on windows
	#include "vis.h"
#endif
#include "pointGenerator.h"

#include <cstring> // stoi
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
    std::cout << "ALGO assignment \"kd-tree\"\n";

	// read program arguments
	// TODO: care a little more <3 --> use getopt
	// -v                   mode with enabled visualization
	// -f <filename>        read triangle points from file
	// --rngcount <count>   generates count random float values
	bool vis = true;
	std::string fileRnd = "";
	uint32_t count = 0;

	// TODO: add params:
	// -runs <count>        runs performance mode multiple times
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-v") == 0) vis = true;
		else if (strcmp(argv[i], "-f") == 0) fileRnd = argv[i + 1];
		else if (strcmp(argv[i], "--rngcount") == 0) count = std::stoi(argv[i + 1]);
		//else if (strcmp(argv[i], "--runs") == 0) runs = std::stoi(argv[i + 1]);
	}

	//fileRnd = "Monkey.obj";
	//fileRnd = "MonkeySimple.obj";
	//fileRnd = "nubian_complex.obj";
	//fileRnd = "icosphere.obj";
	//fileRnd = "sphere.obj";
	fileRnd = "noobPot.obj";
	//count = 100000;

	std::vector<float> triangleVertices;
	if (fileRnd.empty() == false) // read points from given file
	{
		std::cout << "reading vertices from file: " << fileRnd << "..." << std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		triangleVertices = PointGenerator::read_file(fileRnd);
		std::cout << " took " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << " microseconds" << std::endl;
	}
	else if (count != 0) // generate random points
	{
		std::cout << "generating " << count << " random triangles" << std::endl;
		triangleVertices = PointGenerator::generate_triangles(count);
	}
	else
	{
		std::cout << "using simple test setup" << std::endl;
		triangleVertices =
		{
			#if defined (AXIS_X)
				// center
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f,

				// left
				-5.5f, -0.5f, 0.0f,
				-4.5f, -0.5f, 0.0f,
				-5.0f,  0.5f, 0.0f,

				// right
				 4.5f, -0.5f, 0.0f,
				 5.5f, -0.5f, 0.0f,
				 5.0f,  0.5f, 0.0f
			#elif defined (AXIS_Y_Z)
				// top
				-0.5f,  4.5f, 0.0f, // left
				 0.5f,  4.5f, 0.0f, // right
				 0.0f,  5.5f, 0.0f, // top

				// back
				-0.5f, -0.5f, -5.0f,
				 0.5f, -0.5f, -5.0f,
				 0.0f,  0.5f, -5.0f,

				// center
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f,

				// front
				-0.5f, -0.5f, 5.0f,
				 0.5f, -0.5f, 5.0f,
				 0.0f,  0.5f, 5.0f,

				// bottom
				-0.5f, -5.5f, 0.0f,
				 0.5f, -5.5f, 0.0f,
				 0.0f, -4.5f, 0.0f
			#elif defined (AXIS_X_Y_Z)
				// top
				-0.5f,  4.5f, 0.0f, // left
				 0.5f,  4.5f, 0.0f, // right
				 0.0f,  5.5f, 0.0f, // top

				// center
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f,

				// left
				-5.5f, -0.5f, 0.0f,
				-4.5f, -0.5f, 0.0f,
				-5.0f,  0.5f, 0.0f,

				// right
				 4.5f, -0.5f, 0.0f,
				 5.5f, -0.5f, 0.0f,
				 5.0f,  0.5f, 0.0f,

				// front
				-0.5f, -0.5f, 5.0f,
				 0.5f, -0.5f, 5.0f,
				 0.0f,  0.5f, 5.0f,

				// back
				-0.5f, -0.5f, -5.0f,
				 0.5f, -0.5f, -5.0f,
				 0.0f,  0.5f, -5.0f,

				// bottom
				-0.5f, -5.5f, 0.0f,
				 0.5f, -5.5f, 0.0f,
				 0.0f, -4.5f, 0.0f
			#endif
		};
	}

	KDTree tree(&triangleVertices[0], triangleVertices.size());
	//tree.print();

	if (vis)
	{
#ifdef _WIN32
		Vis vis(tree, &triangleVertices[0], triangleVertices.size());
		vis.display();
#else
		std::cout << "visualization not supported in non-windows systems" << std::endl;
#endif
	}
	else
	{
		// while read.input != esc
		// 'r' - create random raycast
		// or enter points
	}
}