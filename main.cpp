#include "kdtree.h"

#include <iostream>

int main()
{
    std::cout << "kd-tree\n";

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
	//KDTree tree = KDTree(points);
	KDTree tree(points);
}