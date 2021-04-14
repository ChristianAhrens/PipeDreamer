/*
===============================================================================

Copyright (C) 2021 Bernardo Escalona. All Rights Reserved.

  This file is part of the Pipe Dream clone found at:
  https://github.com/escalonely/PipeDream

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

===============================================================================
*/


#include "TilePiece.h"
#include <assert.h>



TilePiece::TilePiece()
	: m_type(TYPE_NONE)
{

}

TilePiece::TilePiece(Type t)
	: m_type(t)
{

}

TilePiece::~TilePiece()
{

}

TilePiece* TilePiece::CreateTile(TilePiece::Type t)
{
	TilePiece* ret(nullptr);

	switch (t)
	{
		case TYPE_NONE:
			ret = new TilePiece();
			break;
		case TYPE_START_N:
		case TYPE_START_S:
		case TYPE_START_E:
		case TYPE_START_W:
		case TYPE_VERTICAL:
		case TYPE_HORIZONTAL:
		case TYPE_NW_ELBOW:
		case TYPE_NE_ELBOW:
		case TYPE_SE_ELBOW:
		case TYPE_SW_ELBOW:
			ret = new Pipe(t);
			break;
		case TYPE_CROSS:
			ret = new Cross();
			break;
		default:
			break;
	}

	return ret;
}

TilePiece::Type TilePiece::GetType() const
{
	return m_type;
}

void TilePiece::SetType(Type t)
{
	m_type = t;
}

bool TilePiece::IsStart() const
{
	switch (m_type)
	{
		case TYPE_START_N:
		case TYPE_START_S:
		case TYPE_START_E:
		case TYPE_START_W:
			return true;
		default: break;
	}

	return false;
}

int TilePiece::GetScoreValue() const
{
	return 0;
}


/****************************************************************
* Pipe
*****************************************************************/


Pipe::Pipe(TilePiece::Type t)
	: TilePiece(t),
	m_oozeLevel(0.0f),
	m_exploding(0)
{
	// Starter pipes have only one possible
	// flow direction. Set it from the start.
	switch (t)
	{
	case TYPE_START_N:
		m_flowDirection = DIR_N;
		break;
	case TYPE_START_S:
		m_flowDirection = DIR_S;
		break;
	case TYPE_START_E:
		m_flowDirection = DIR_E;
		break;
	case TYPE_START_W:
		m_flowDirection = DIR_W;
		break;
	default:
		m_flowDirection = DIR_NONE;
		break;
	}
}

Pipe::~Pipe()
{

}

float Pipe::Pump(float amount)
{
	assert(m_oozeLevel < MAX_OOZE_LEVEL);

	m_oozeLevel += amount;

	return m_oozeLevel;
}

float Pipe::GetOozeLevel() const
{
	return m_oozeLevel;
}

bool Pipe::IsFull() const
{
	return (m_oozeLevel >= MAX_OOZE_LEVEL);
}

bool Pipe::IsEmpty() const
{
	return (m_oozeLevel == MIN_OOZE_LEVEL);
}

bool Pipe::HasOpening(Pipe::Direction dir) const
{
	switch (m_type)
	{
	case TYPE_START_N:
		return (dir == DIR_N);
	case TYPE_START_S:
		return (dir == DIR_S);
	case TYPE_START_E:
		return (dir == DIR_E);
	case TYPE_START_W:
		return (dir == DIR_W);
	case TYPE_VERTICAL:
		return ((dir == DIR_N) || (dir == DIR_S));
	case TYPE_HORIZONTAL:
		return ((dir == DIR_E) || (dir == DIR_W));
	case TYPE_NW_ELBOW:
		return ((dir == DIR_N) || (dir == DIR_W));
	case TYPE_NE_ELBOW:
		return ((dir == DIR_N) || (dir == DIR_E));
	case TYPE_SE_ELBOW:
		return ((dir == DIR_S) || (dir == DIR_E));
	case TYPE_SW_ELBOW:
		return ((dir == DIR_S) || (dir == DIR_W));
	case TYPE_CROSS:
		return true;
	default:
		break;
	}

	return false;
}

Pipe::Direction Pipe::GetOppositeDirection(Pipe::Direction dir)
{
	switch (dir)
	{
		case DIR_N: 
			return DIR_S;
		case DIR_S: 
			return DIR_N;
		case DIR_E:
			return DIR_W;
		case DIR_W:
			return DIR_E;
	}

	return DIR_NONE;
}

bool Pipe::SetFlowEntry(Pipe::Direction dir)
{
	bool ret(false);

	if (IsEmpty())
	{
		switch (m_type)
		{
		case TYPE_VERTICAL:
			if (dir == DIR_N)
			{
				m_flowDirection = DIR_S;
				ret = true;

			}
			else if (dir == DIR_S)
			{
				m_flowDirection = DIR_N;
				ret = true;
			}
			break;

		case TYPE_HORIZONTAL:
			if (dir == DIR_E)
			{
				m_flowDirection = DIR_W;
				ret = true;

			}
			else if (dir == DIR_W)
			{
				m_flowDirection = DIR_E;
				ret = true;
			}
			break;

		case TYPE_NW_ELBOW:
			if (dir == DIR_N)
			{
				m_flowDirection = DIR_W;
				ret = true;

			}
			else if (dir == DIR_W)
			{
				m_flowDirection = DIR_N;
				ret = true;
			}
			break;

		case TYPE_NE_ELBOW:
			if (dir == DIR_N)
			{
				m_flowDirection = DIR_E;
				ret = true;

			}
			else if (dir == DIR_E)
			{
				m_flowDirection = DIR_N;
				ret = true;
			}
			break;

		case TYPE_SE_ELBOW:
			if (dir == DIR_S)
			{
				m_flowDirection = DIR_E;
				ret = true;

			}
			else if (dir == DIR_E)
			{
				m_flowDirection = DIR_S;
				ret = true;
			}
			break;

		case TYPE_SW_ELBOW:
			if (dir == DIR_S)
			{
				m_flowDirection = DIR_W;
				ret = true;

			}
			else if (dir == DIR_W)
			{
				m_flowDirection = DIR_S;
				ret = true;
			}
			break;

		case TYPE_CROSS:
			assert(false); 
			break;

		default:
			break;
		}
	}

	return ret;
}

Pipe::Direction Pipe::GetFlowDirection() const
{
	return m_flowDirection;
}

void Pipe::Explode()
{
	// Duration of a tile explosion, in number of frames.
	static constexpr int maximumExplosiveness = 8;

	m_exploding = maximumExplosiveness;
}

int Pipe::PopExplosion()
{
	if (m_exploding > 0)
		m_exploding--;

	return m_exploding;
}

int Pipe::GetScoreValue() const
{
	switch (m_type)
	{
	case TYPE_VERTICAL:
	case TYPE_HORIZONTAL:
	case TYPE_NW_ELBOW:
	case TYPE_NE_ELBOW:
	case TYPE_SE_ELBOW:
	case TYPE_SW_ELBOW:
		{
			// All pipes, except for starter pipes, give 
			// normal score value if full.
			if (IsFull())
			{
				return 100;
			}
		}
		break;

	default:
		break;
	}

	return 0;
}



/****************************************************************
* Cross
*****************************************************************/


Cross::Cross()
	: Pipe(TilePiece::TYPE_CROSS),
	m_horizOozeLevel(0.0f),
	m_vertOozeLevel(0.0f),
	m_horizWayFree(true),
	m_vertWayFree(true)
{

}

float Cross::Pump(float amount)
{
	if ((m_flowDirection == DIR_E) ||
		(m_flowDirection == DIR_W))
	{
		assert(m_horizOozeLevel < MAX_OOZE_LEVEL);

		m_horizOozeLevel += amount;
		if (m_horizOozeLevel >= MAX_OOZE_LEVEL)
			m_horizWayFree = false;

		return m_horizOozeLevel;
	}

	if ((m_flowDirection == DIR_N) ||
		(m_flowDirection == DIR_S))
	{
		assert(m_vertOozeLevel < MAX_OOZE_LEVEL);

		m_vertOozeLevel += amount;
		if (m_vertOozeLevel >= MAX_OOZE_LEVEL)
			m_vertWayFree = false;

		return m_vertOozeLevel;
	}

	assert(false);
	return 0.0f;
}

float Cross::GetOozeLevel(Cross::Way w) const
{
	if (w == WAY_HORIZONTAL)
		return m_horizOozeLevel;

	return m_vertOozeLevel;
}

bool Cross::IsFull() const
{
	if ((m_flowDirection == DIR_E) ||
		(m_flowDirection == DIR_W))
		return (m_horizOozeLevel >= MAX_OOZE_LEVEL);

	if ((m_flowDirection == DIR_N) ||
		(m_flowDirection == DIR_S))
		return (m_vertOozeLevel >= MAX_OOZE_LEVEL);

	// Flow direction hasn't been set, tile unused.
	return false;
}

bool Cross::IsEmpty() const
{
	return ((m_horizOozeLevel == MIN_OOZE_LEVEL) &&
		(m_vertOozeLevel == MIN_OOZE_LEVEL));
}

bool Cross::SetFlowEntry(Pipe::Direction dir)
{
	bool ret(false);

	switch (dir)
	{
	case Pipe::DIR_E:
	case Pipe::DIR_W:
		{
			if (m_horizWayFree)
				ret = true;
		}
		break;
	case Pipe::DIR_N:
	case Pipe::DIR_S:
		{
			if (m_vertWayFree)
				ret = true;
		}
		break;
	default:
		break;
	}

	// Cross tiles can have flow entry set twice, once for each way (horiz vs. vert).
	if (ret)
		m_flowDirection = Pipe::GetOppositeDirection(dir);

	return ret;
}

int Cross::GetScoreValue() const
{
	// TODO explain
	if ((m_horizOozeLevel >= MAX_OOZE_LEVEL) &&
		(m_vertOozeLevel >= MAX_OOZE_LEVEL))
		return 200;

	if ((m_horizOozeLevel >= MAX_OOZE_LEVEL) ||
		(m_vertOozeLevel >= MAX_OOZE_LEVEL))
		return 100;

	return 0;
}