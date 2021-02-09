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

	// test values
	std::vector<Point*> points
	{
		new Point(0.0f, 0.0f, 0.0f),
		new Point(2.0f, 0.0f, 0.0f),
		new Point(1.0f, 2.0f, 0.0f),
		new Point(2.0f, 0.0f, 0.0f),
		new Point(3.5f, 1.0f, 0.0f),
		new Point(2.5f, 3.0f, 0.0f)
	};

	//KDTree *tree = new KDTree(points);
	KDTree tree(points);
	tree.print();

	if (vis)
	{
#ifdef _WIN32
		Vis vis(tree);
		vis.display();
#else
		std::cout << "visualization not supported in non-windows systems" << std::endl;
#endif
	}
}