#include "Maze_Generator1.h"
#include <nclgl\NCLDebug.h>

#include <list>
#include <algorithm>

uint RandomGridCell(uint size)
{
	uint x = rand() % size;
	uint y = rand() % size;
	return y * size + x;
}

Maze_Generator1::Maze_Generator1()
	: allNodes(NULL)
	, allEdges(NULL)
{
	minfo.size = 0;
	minfo.start = NULL;
	minfo.end = NULL;
}

Maze_Generator1::Maze_Generator1(uint size, GraphNode *start, GraphNode *end)
{
	minfo.size = size;
	minfo.start = start;
	minfo.end = end;
	allNodes = NULL;
	allEdges = NULL;
}

Maze_Generator1::Maze_Generator1(uint size, GraphNode *start, GraphNode *end, GraphEdge *alledges)
{
	minfo.size = size;
	minfo.start = start;
	minfo.end = end;
	allNodes = NULL;
	allEdges = alledges;
}

Maze_Generator1::~Maze_Generator1()
{
	if (allNodes)
	{
		delete[] allNodes;
		delete[] allEdges;
		allNodes = NULL;
		allEdges = NULL;
	}
}

void Maze_Generator1::Generate(int grid_size, float maze_density)
{
	if (allNodes)
	{
		delete[] allNodes;
		delete[] allEdges;
	}

	minfo.size = grid_size;
	Initiate_Arrays();

	Generate_Sparse(maze_density);

	//Final step is de-randomise the neighbours list
	// - This isn't normally needed, but for the sake of the maze demonstration it is nice to see that
	//   breadth first always does '+x, -x, +y, -y' and depth first always searches all '+x'then all '-x' etc
	// Order of neighbours: 
	//   0: -x
	//   1: +x
	//   2: -y
	//   3: +y
#pragma omp parallel for
	for (int y = 0; y < (int)minfo.size; ++y)
	{
		GraphEdge* lookup[4];
		for (int x = 0; x < (int)minfo.size; ++x)
		{
			GraphNode* on = &allNodes[y * minfo.size + x];

			memset(lookup, 0, 4 * sizeof(GraphEdge*));

			for (GraphEdge* e : on->_neighbours)
			{
				GraphNode* nn = (e->_a == on) ? e->_b : e->_a;

				int xOffset = on->_pos.x > nn->_pos.x;
				int yOffset = (on->_pos.y > nn->_pos.y);
				if (on->_pos.x != nn->_pos.x)
					lookup[xOffset] = e;	 //0 or 1
				else
					lookup[yOffset + 2] = e; //2 or 3
			}

			on->_neighbours.clear();

			for (int i = 0; i < 4; ++i)
			{
				if (lookup[i])
					on->_neighbours.push_back(lookup[i]);
			}

		}
	}

	GetRandomStartEndNodes(minfo.size);
}

void Maze_Generator1::GetRandomStartEndNodes(uint size)
{
	//Traditional Maze one side to the other
	int edge = rand() % 2;
	int idxS = rand() % size;
	int idxE = rand() % size;
	switch (edge)
	{
	case 0: //x
		minfo.start = &allNodes[idxS * size];
		minfo.end = &allNodes[(idxE + 1) * size - 1];
		break;
	case 1: //y
		minfo.start = &allNodes[idxS];
		minfo.end = &allNodes[minfo.size * (size - 1) + idxE];
		break;
	}
}


void Maze_Generator1::Initiate_Arrays()
{
	allNodes = new GraphNode[minfo.size * minfo.size];
	for (uint y = 0; y < minfo.size; ++y)
	{
		for (uint x = 0; x < minfo.size; ++x)
		{
			allNodes[y * minfo.size + x]._pos = Vector3((float)x, (float)y, 0.0f);
			allNodes[y * minfo.size + x]._visited = false;
		}
	}

	uint base_offset = minfo.size * (minfo.size - 1);
	allEdges = new GraphEdge[base_offset * 2];

	for (uint y = 0; y < minfo.size; ++y)
	{
		for (uint x = 0; x < minfo.size - 1; ++x)
		{
			GraphEdge& edgeX = allEdges[(y * (minfo.size - 1) + x)];
			edgeX._a = &allNodes[y * minfo.size + x];
			edgeX._b = &allNodes[y * minfo.size + x + 1];
			edgeX.weighting = 1.0f;
			edgeX._connected = false;
			edgeX._iswall = true;
		}
	}
	for (uint y = 0; y < minfo.size - 1; ++y)
	{
		for (uint x = 0; x < minfo.size; ++x)
		{
			GraphEdge& edgeY = allEdges[base_offset + (x * (minfo.size - 1) + y)];
			edgeY._a = &allNodes[y * minfo.size + x];
			edgeY._b = &allNodes[(y + 1) * minfo.size + x];
			edgeY.weighting = 1.0f;
			edgeY._connected = false;
			edgeY._iswall = true;
		}
	}
}

//Takes in a size parameter and generates a random square maze of dimensions: [sizexsize] 
// Uses Prims algorithm to generate the maze: https://en.wikipedia.org/wiki/Prim%27s_algorithm
void  Maze_Generator1::Generate_Prims()
{
	uint startIdx = RandomGridCell(minfo.size);
	GraphNode* start = &allNodes[startIdx];
	start->_visited = true;

	uint base_offset = minfo.size * (minfo.size - 1);

	std::list<GraphEdge*> walls;


	auto add_potential_wall = [&](uint x1, uint y1, uint x2, uint y2)
	{
		GraphEdge* edge;

		if (x1 != x2)
		{
			//It's edge on the x-axis!
			edge = &allEdges[y1 * (minfo.size - 1) + min(x1, x2)];
		}
		else
		{
			//It's edge on the y-axis!	
			edge = &allEdges[base_offset + x1 * (minfo.size - 1) + min(y1, y2)];
		}

		if (!edge->_connected)
		{
			edge->_connected = true;
			walls.push_back(edge);
		}
	};

	auto add_walls = [&](uint node_idx)
	{
		uint x = node_idx % minfo.size;
		uint y = node_idx / minfo.size;

		if (y > 0) add_potential_wall(x, y, x, y - 1);
		if (x > 0) add_potential_wall(x, y, x - 1, y);

		if (y < minfo.size - 1) add_potential_wall(x, y, x, y + 1);
		if (x < minfo.size - 1) add_potential_wall(x, y, x + 1, y);
	};


	add_walls(startIdx);

	while (walls.size() > 0)
	{
		//Pick random wall in list
		uint idx = rand() % walls.size();
		auto itr = walls.begin();
		std::advance(itr, idx);
		GraphEdge* edge = *itr;
		walls.erase(itr);

		if (edge->_a->_visited ^ edge->_b->_visited)
		{
			edge->_a->_neighbours.push_back(edge);
			edge->_b->_neighbours.push_back(edge);
			edge->_iswall = false;

			if (!edge->_a->_visited)
			{
				edge->_a->_visited = true;
				add_walls((edge->_a - allNodes));
			}
			else
			{
				edge->_b->_visited = true;
				add_walls((edge->_b - allNodes));
			}
		}
	}
}

void Maze_Generator1::Generate_Sparse(float density)
{
	//Making a sparse maze is not so easy, as we still need to ensure
	// any node in the graph /can/ reach any other node. So to make
	// everything simpler, this just generates a complete maze and 
	// knocks out some walls at the end.
	Generate_Prims();

	//Build list of un-used edges
	std::deque<GraphEdge*> edges;

	uint base_offset = minfo.size * (minfo.size - 1);
	for (uint y = 0; y < minfo.size; ++y)
	{
		for (uint x = 0; x < minfo.size - 1; ++x)
		{
			GraphEdge* edgeX = &allEdges[(y * (minfo.size - 1) + x)];
			if (edgeX->_iswall) edges.push_back(edgeX);
		}
	}
	for (uint y = 0; y < minfo.size - 1; ++y)
	{
		for (uint x = 0; x < minfo.size; ++x)
		{
			GraphEdge* edgeY = &allEdges[base_offset + (x * (minfo.size - 1) + y)];
			if (edgeY->_iswall) edges.push_back(edgeY);
		}
	}


	std::random_shuffle(edges.begin(), edges.end());

	//Remove half the walls/edges in the maze
	int total_to_remove = (int)(floor((float)(edges.size() * (1.0f - density))));
	for (int i = 0; i < total_to_remove; ++i)
	{
		GraphEdge* e = edges.back();
		e->_a->_neighbours.push_back(e);
		e->_b->_neighbours.push_back(e);

		e->_iswall = false;
		edges.pop_back();
	}
}


