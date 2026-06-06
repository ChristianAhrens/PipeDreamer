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


// ---- Helper types and constants ----

static constexpr float MAX_OOZE_LEVEL = 100.0f;
static constexpr float MIN_OOZE_LEVEL = 0.0f;


// ---- Class definition ----

/**
 * Base class which represents all tiles, including Pipes as well as empty tiles on the Grid.
 */
class TilePiece
{
public:
	/**
	 * Score value of a regular pipe full of Ooze
	 */
	static const int PIPE_SCORE_VALUE;

	/**
	 * Additional score value of a cross-pipe full of Ooze on both ways.
	 */
	static const int CROSS_PIPE_SCORE_VALUE;


	/**
	 * Types of TilePiece.
	 */
	enum Type
	{
		TYPE_NONE = 0,
		TYPE_START_N,
		TYPE_START_S,
		TYPE_START_E,
		TYPE_START_W,
		TYPE_VERTICAL,
		TYPE_HORIZONTAL,
		TYPE_NW_ELBOW,
		TYPE_NE_ELBOW,
		TYPE_SE_ELBOW,
		TYPE_SW_ELBOW,
		TYPE_CROSS,
		TYPE_MAX
	};

	/** Default constructor. Creates an empty tile of type TYPE_NONE. */
	TilePiece();

	/**
	 * Constructor for a specific tile type.
	 * @param t The type to assign to this tile.
	 */
	TilePiece(Type t);

	/** Destructor. */
	virtual ~TilePiece();

	/**
	 * Factory method. Allocates and returns a TilePiece, Pipe, or Cross
	 * depending on t. The caller takes ownership of the returned object.
	 * @param t Tile type to create. Defaults to TYPE_NONE (empty tile).
	 * @return Pointer to the newly created tile.
	 */
	static TilePiece* CreateTile(Type t = TilePiece::TYPE_NONE);

	/**
	 * Get the tile's type enum value.
	 * @return This tile's Type.
	 */
	Type GetType() const;

	/**
	 * Check whether this tile is one of the four starter pipe types
	 * (TYPE_START_N, TYPE_START_S, TYPE_START_E, TYPE_START_W).
	 * @return True if this tile is a start tile.
	 */
	bool IsStart() const;

	/**
	 * Score awarded when this tile is fully filled with ooze.
	 * The base implementation returns 0 (empty tiles score nothing).
	 * @return Score points for this tile.
	 */
	virtual int GetScoreValue() const;

protected:
	/**
	 * Overwrite the tile type.
	 * @param t The new type to assign.
	 */
	void SetType(Type t);

	/** The type of this tile. */
	Type m_type;
};


/**
 * Class which represents Pipes through which Ooze can flow.
 * The member m_oozeLevel indicates how full of Ooze the pipe is (0 per default), 
 * and m_flowDirection indicates towards which opening the Ooze is flowing.
 */
class Pipe : public TilePiece
{
public:
	/** Cardinal directions used to describe ooze flow and pipe openings. */
	enum Direction
	{
		DIR_NONE = 0,
		DIR_N,
		DIR_S,
		DIR_E,
		DIR_W,
	};

	/**
	 * Constructor. Sets the flow direction for starter pipe types;
	 * leaves it as DIR_NONE for all other types until SetFlowEntry() is called.
	 * @param t Pipe type. Must be a pipe-compatible Type (not TYPE_NONE).
	 */
	Pipe(TilePiece::Type t);

	/** Destructor. */
	virtual ~Pipe();

	/**
	 * Increase the ooze fill level by the given amount.
	 * @param amount Ooze units to add. The level must be below MAX_OOZE_LEVEL before calling.
	 * @return The updated ooze level.
	 */
	virtual float Pump(float amount);

	/**
	 * Get the current ooze fill level.
	 * @return Ooze level in the range [MIN_OOZE_LEVEL, MAX_OOZE_LEVEL].
	 */
	float GetOozeLevel() const;

	/**
	 * Check whether the pipe is completely filled with ooze.
	 * @return True when ooze level >= MAX_OOZE_LEVEL.
	 */
	virtual bool IsFull() const;

	/**
	 * Check whether the pipe contains no ooze.
	 * @return True when ooze level == MIN_OOZE_LEVEL.
	 */
	virtual bool IsEmpty() const;

	/**
	 * Check whether this pipe shape has an opening facing the given direction.
	 * @param dir Direction to test.
	 * @return True if the pipe's geometry allows ooze to enter or exit in dir.
	 */
	bool HasOpening(Pipe::Direction dir) const;

	/**
	 * Return the direction 180 degrees opposite to dir.
	 * @param dir Input direction.
	 * @return The opposite of dir, or DIR_NONE when dir is DIR_NONE.
	 */
	static Pipe::Direction GetOppositeDirection(Pipe::Direction dir);

	/**
	 * Set the ooze entry point for this pipe (and derive the exit direction).
	 * Can only succeed on an empty pipe whose shape supports the given entry direction.
	 * @param dir Direction from which ooze enters this pipe.
	 * @return True if the entry point was set successfully.
	 */
	virtual bool SetFlowEntry(Pipe::Direction dir);

	/**
	 * Get the direction in which ooze currently exits this pipe.
	 * @return The exit direction, or DIR_NONE if flow has not been set yet.
	 */
	virtual Pipe::Direction GetFlowDirection() const;

	/**
	 * Trigger an explosion animation on this tile.
	 * Sets the internal frame counter to its maximum value.
	 */
	void Explode();

	/**
	 * Decrement and return the explosion animation frame counter.
	 * Should be called once per rendered frame; returns 0 when the animation is finished.
	 * @return Remaining animation frames (0 when done).
	 */
	int PopExplosion();

	/** @return Score points awarded when this pipe is fully filled with ooze. */
	int GetScoreValue() const override;

protected:
	/** Current ooze fill level, in the range [MIN_OOZE_LEVEL, MAX_OOZE_LEVEL]. */
	float m_oozeLevel;

	/** Remaining frames of the explosion animation; 0 when no explosion is active. */
	int m_exploding;

	/** Direction in which ooze exits this pipe. DIR_NONE until SetFlowEntry() is called. */
	Pipe::Direction m_flowDirection;
};

/**
 * Class which specifically represents Cross-Pipes. This type of Pipe requires special handling, 
 * because Ooze can flow through it twice: once vertically and once horizontally.
 */
class Cross : public Pipe
{
public:
	/** Identifies which of the two pipe channels (horizontal or vertical) is being referenced. */
	enum Way
	{
		WAY_NONE = 0,
		WAY_VERTICAL,
		WAY_HORIZONTAL
	};

	/**
	 * Constructor. Randomly decides which way (horizontal or vertical) renders
	 * on the background so the two channels look visually distinct.
	 */
	Cross();

	/**
	 * Increase the ooze level for the currently active flow direction.
	 * @param amount Ooze units to add to the active way.
	 * @return The updated ooze level for the active way.
	 */
	float Pump(float amount) override;

	/**
	 * Get the ooze fill level for a specific crossing channel.
	 * @param w The channel to query (WAY_HORIZONTAL or WAY_VERTICAL).
	 * @return Ooze level for that channel.
	 */
	float GetOozeLevel(Way w) const;

	/**
	 * Check whether the currently active channel is completely filled.
	 * @return True when the active channel's ooze level >= MAX_OOZE_LEVEL.
	 */
	bool IsFull() const override;

	/**
	 * Check whether both channels contain no ooze.
	 * @return True when both horizontal and vertical ooze levels are at MIN_OOZE_LEVEL.
	 */
	bool IsEmpty() const override;

	/**
	 * Set the ooze entry direction, occupying whichever channel is still free.
	 * Can be called twice — once for each channel (horizontal and vertical).
	 * @param dir Direction from which ooze enters this cross-pipe.
	 * @return True if a free channel accepts the given direction.
	 */
	bool SetFlowEntry(Pipe::Direction dir) override;

	/**
	 * Score awarded depends on how many channels have been fully filled:
	 * one full channel = PIPE_SCORE_VALUE; both full = CROSS_PIPE_SCORE_VALUE (bonus).
	 * @return Score points for this cross-pipe.
	 */
	int GetScoreValue() const override;

	/**
	 * Get which channel was designated to render behind the other.
	 * @return WAY_HORIZONTAL or WAY_VERTICAL.
	 */
	Way GetBackgroundWay() const;

protected:
	/**
	 * Replaces Pipe::m_oozeLevel, and keeps track of the Ooze fill level within
	 * the horizontal part of the Cross-Pipe.
	 */
	float m_horizOozeLevel;

	/**
	 * Replaces Pipe::m_oozeLevel, and keeps track of the Ooze fill level within
	 * the vertical part of the Cross-Pipe.
	 */
	float m_vertOozeLevel;

	bool m_horizWayFree;
	bool m_vertWayFree;

	/**
	 * Which way, horizontal or vertical, is drawn first and thus ends up 
	 * on the background. The second way, will then appear on the foreground. 
	 */
	Way m_backgroundWay;
};