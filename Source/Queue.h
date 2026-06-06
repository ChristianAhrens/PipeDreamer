/*
===============================================================================

Copyright (C) 2021 Bernardo Escalona. All Rights Reserved.

  This file is part of Pipe Dreamer, found at:
  https://github.com/escalonely/PipeDreamer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

===============================================================================
*/


#pragma once

#include "TilePiece.h"
#include <vector>


// ---- Class Definition ----

/**
 * Class which represents the pipe queue. Pipe tiles can be conceptually popped 
 * from the end of the queue, which will generate a new random pipe at the start of the queue.
 */
class Queue
{
public:
	/**
	 * Constructor. Fills the queue with size randomly chosen pipe tiles.
	 * @param size Number of tiles the queue should hold.
	 */
	Queue(int size);

	/** Destructor. Frees all tile objects owned by the queue. */
	~Queue();

	/** Replace all tiles in the queue with freshly randomised pipe types. */
	void Reset();

	/**
	 * Get the number of tiles in the queue.
	 * @return Current queue size (fixed after construction).
	 */
	int GetSize() const;

	/**
	 * Get a tile at a logical position without removing it.
	 * Position 0 is the next tile to be placed on the board (the "active" tile).
	 * @param pos Logical queue position (0 = front / next to place).
	 * @return Pointer to the Pipe at that position. Ownership remains with the Queue.
	 */
	Pipe* GetTile(int pos) const;

	/**
	 * Get the type of the tile at a logical position without removing it.
	 * @param pos Logical queue position (0 = front).
	 * @return TilePiece::Type of the tile at that position.
	 */
	TilePiece::Type GetTileType(int pos) const;

	/**
	 * Remove the front tile from the queue, generate a new random tile at the back,
	 * and return the type that was at the front.
	 * @return The TilePiece::Type that was removed from the front.
	 */
	TilePiece::Type Pop();

protected:
	/**
	 * Underlying vector containing the Pipe objects.
	 */
	std::vector<Pipe*> m_buff;

	/**
	 * Index pointing to the end of the queue.
	 */
	int m_readPos;
};