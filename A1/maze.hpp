// Termm--Fall 2023

#pragma once
#include <tuple>

class Maze
{
public:
	Maze( size_t dim );
	~Maze();

	void reset();

	size_t getDim() const;

	int getValue( int x, int y ) const;

	void setValue( int x, int y, int h );

	int getBlockNum() const;

	void decreBlockNum();

	std::tuple<float, float> getEntry();

	void digMaze();
	void printMaze(); // for debugging
private:
	size_t m_dim;
	int *m_values;
	int block_num;
	void recDigMaze(int r, int c);
	int numNeighbors(int r, int c);
};
