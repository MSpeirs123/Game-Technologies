#pragma once
#include <ncltech\GameObject.h>
#include <ncltech\Scene.h>
#include "Search_Algorithm1.h"



class Maze_Generator1
{
public:
	Maze_Generator1();
	Maze_Generator1(uint size, GraphNode *start, GraphNode *end);
	Maze_Generator1(uint size, GraphNode *start, GraphNode *end, GraphEdge *alledges); //Maze_density goes from 1 (single path to exit) to 0 (no walls at all)
	virtual ~Maze_Generator1();

	void Generate(int size, float maze_density);

	//All points on the maze grid are connected in some shape or form
	// so any two nodes picked randomly /will/ have a path between them
	GraphNode* GetStartNode() const { return minfo.start; }
	GraphNode* GetGoalNode()  const { return minfo.end; }
	uint GetSize() const { return minfo.size; }


	//Used as a hack for the MazeRenderer to generate the walls more effeciently
	GraphNode* GetAllNodesArr() { return allNodes; }

protected:
	void Initiate_Arrays();

	void Generate_Prims();
	void Generate_Sparse(float density);
	void GetRandomStartEndNodes(uint size);



public:
	struct mazeinfo {
		uint size;
		GraphNode *start, *end;
	} minfo;

	GraphNode* allNodes;
	GraphEdge* allEdges;
};

