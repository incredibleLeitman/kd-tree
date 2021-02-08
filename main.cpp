#include "kdtree.h"
#ifdef _WIN32 // vis only supported on windows
	#include "vis.h"
#endif

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "ALGO assignment \"kd-tree\"\n";

	// TODO:
	// evaluate params
	// -p, performance
	// -f, filename

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

	#ifdef _WIN32
		Vis vis(tree);
		vis.display();
	#else
		std::cout << "visualization not supported in non-windows systems" << std::endl;
	#endif
}