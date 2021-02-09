#include "kdtree.h"
#ifdef _WIN32 // vis only supported on windows
	#include "vis.h"
#endif
#include "pointGenerator.h"

#include <cstring> // stoi
#include <iostream>

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
	int count = 10;

	// TODO: add params:
	// -runs <count>        runs performance mode multiple times
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-v") == 0) vis = true;
		else if (strcmp(argv[i], "-f") == 0) fileRnd = argv[i + 1];
		else if (strcmp(argv[i], "--rngcount") == 0) count = std::stoi(argv[i + 1]);
		//else if (strcmp(argv[i], "--runs") == 0) runs = std::stoi(argv[i + 1]);
	}

	/*std::vector<Point*> points;
	if (fileRnd.empty() == false) // read random points from given file
	{
		std::cout << "reading random numbers from file: " << fileRnd << "..." << std::endl;
		points = PointGenerator::read_file(fileRnd);
	}
	else // generate random points
	{
		std::cout << "generating " << count << " random numbers" << std::endl;
		//points = PointGenerator::generate_points(count, 0.0, 1000.0);
	}*/

	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left
		 0.5f, -0.5f, 0.0f, // right
		 0.0f,  0.5f, 0.0f, // top

		 -0.5f, -5.5f, 0.0f, // left
		 0.5f, -5.5f, 0.0f, // right
		 0.0f, -4.5f, 0.0f, // top

		 /*
		 - 0.5f, 5.5f, 0.0f, // left
		 0.5f, 5.5f, 0.0f, // right
		 0.0f, 6.5f, 0.0f, // top

		 4.5f, -5.5f, 0.0f, // left
		 5.5f, -5.5f, 0.0f, // right
		 5.0f, -4.5f, 0.0f, // top

		 -5.5f, -5.5f, 0.0f, // left
		 -4.5f, -5.5f, 0.0f, // right
		 -5.0f, -4.5f, 0.0f // top
		 */

	};
	uint32_t countValues = 18; //45;

	//KDTree *tree = new KDTree(points);
	//KDTree tree(points);
	KDTree tree(vertices, countValues);
	tree.print();

	if (vis)
	{
#ifdef _WIN32
		Vis vis(tree, vertices, countValues);
		vis.display();
#else
		std::cout << "visualization not supported in non-windows systems" << std::endl;
#endif
	}
}