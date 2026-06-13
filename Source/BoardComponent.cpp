/*
===============================================================================

Copyright (C) 2026 Christian Ahrens. All Rights Reserved.

  This file is part of the PipeDreamer fork maintained by Christian Ahrens, found at:
  https://github.com/christianahrens/PipeDreamer

  Original project by Bernardo Escalona:
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


#include "BoardComponent.h"
#include "LayoutConstants.h"
#include "TilePiece.h"
#include "Board.h"
#include "Queue.h"
#include "Controller.h"
#include "GameRenderer.h"


// ---- BoardComponent class implementation ----

BoardComponent::BoardComponent()
{
}

void BoardComponent::resized()
{
	// Bounds are set to (10T-9) × (7T-6) — the exact drawn pixel area for a
	// 10×7 grid where adjacent tiles share a 1px border. Recover T accordingly.
	m_tileSize = std::min((getWidth() + 9) / 10, (getHeight() + 6) / 7);
}

void BoardComponent::Tick()
{
	if (m_blockInteraction > 0)
		m_blockInteraction--;
}

void BoardComponent::ResetInteraction()
{
	m_blockInteraction = 0;
}

int BoardComponent::GetTileSize() const
{
	return m_tileSize;
}

void BoardComponent::paint(juce::Graphics& g)
{
	int T = std::min((getWidth() + 9) / 10, (getHeight() + 6) / 7);
	m_tileSize = T;

	Board* board = Controller::GetInstance()->GetBoard();

	juce::Point<int> oozingTileOrigin;

	for (int col = 0; col < board->GetNumCols(); col++)
	{
		for (int row = 0; row < board->GetNumRows(); row++)
		{
			// Tiles overlap by 1 pixel to share borders cleanly.
			juce::Point<int> p(col * (T - 1), row * (T - 1));
			TilePiece* tile = board->GetTile(col, row);

			DrawTile(tile, p, T, g);
			DrawOoze(tile, p, T, g);
			DrawCrossSecondWay(tile, p, T, g);
			DrawTileDecoration(tile, p, T, g);

			if (board->GetOozingTile() == tile)
				oozingTileOrigin = p;
		}
	}

	DrawSpill(oozingTileOrigin, T, g);
}

void BoardComponent::mouseDown(const juce::MouseEvent& e)
{
	Controller* controller = Controller::GetInstance();

	if (controller->GetState() != Controller::STATE_RUNNING || m_blockInteraction > 0)
		return;

	// JUCE delivers mouse events in the component's local (unrotated) coordinate space
	// even when a transform is active — no manual coordinate mapping needed.
	juce::Point<int> clickPos = e.getMouseDownPosition();
	Board* board = controller->GetBoard();
	int T = m_tileSize;
	bool replace = false;

	for (int col = 0; (col < board->GetNumCols()) && !replace; col++)
	{
		for (int row = 0; (row < board->GetNumRows()) && !replace; row++)
		{
			juce::Rectangle<int> tileRect(col * (T - 1), row * (T - 1), T, T);
			if (tileRect.contains(clickPos))
			{
				AudioManager::SoundID soundID(AudioManager::SOUND_CLICK);
				TilePiece* clickedTile = board->GetTile(col, row);
				replace = (clickedTile->GetType() == TilePiece::TYPE_NONE);

				if (!replace)
				{
					Pipe* clickedPipe = dynamic_cast<Pipe*>(clickedTile);
					if ((clickedPipe != nullptr) &&
						(clickedPipe->IsEmpty()) &&
						(!clickedPipe->IsStart()) &&
						(board->PopBomb()))
					{
						replace = true;
						soundID = AudioManager::SOUND_EXPLODE;
					}
				}

				if (replace)
				{
					controller->QueueSound(soundID);
					TilePiece::Type newType = controller->GetQueue()->Pop();
					board->ReplaceTile(col, row, newType);
					static constexpr int framesBlocked = 5;
					m_blockInteraction += framesBlocked;
				}
			}
		}
	}
}


// ---- Static tile-drawing helpers ----

// static
void BoardComponent::DrawTile(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g)
{
	if (tile->GetType() == TilePiece::TYPE_NONE)
		return;

	Pipe* pipe = dynamic_cast<Pipe*>(tile);
	if (pipe == nullptr)
		return;

	auto tileColour = GameRenderer::GetTileColourForLevel(Controller::GetInstance()->GetDifficultyLevel());
	// Shift tiles toward the window background in each mode for better contrast.
	if (juce::LookAndFeel::getDefaultLookAndFeel()
	        .findColour(juce::ResizableWindow::backgroundColourId).getBrightness() > 0.5f)
	    tileColour = tileColour.brighter(0.3f);  // light mode → brighter tiles
	else
	    tileColour = tileColour.darker(0.2f);    // dark mode  → darker tiles
	g.setColour(tileColour);
	g.fillRect(origin.getX(), origin.getY(), tileSize, tileSize);

	juce::Line<int> line;
	float pipeThickness = tileSize / Layout::PIPE_THICKNESS_DIVISOR;
	g.setColour(juce::Colours::black);

	int halfTile = tileSize / 2;
	juce::Rectangle<float> elbowJoint(
		origin.getX() + halfTile - (pipeThickness / 2.0f),
		origin.getY() + halfTile - (pipeThickness / 2.0f),
		pipeThickness, pipeThickness);

	switch (pipe->GetType())
	{
	case TilePiece::TYPE_START_N:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
		                        origin.getX() + halfTile, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_START_S:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX() + halfTile, origin.getY() + tileSize);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_START_E:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX() + tileSize, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_START_W:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX(), origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_VERTICAL:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
		                        origin.getX() + halfTile, origin.getY() + tileSize);
		g.drawLine(line.toFloat(), pipeThickness);
		break;

	case TilePiece::TYPE_HORIZONTAL:
		line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
		                        origin.getX() + tileSize, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		break;

	case TilePiece::TYPE_NW_ELBOW:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
		                        origin.getX() + halfTile, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
		                        origin.getX() + halfTile, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_NE_ELBOW:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
		                        origin.getX() + halfTile, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX() + tileSize, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_SE_ELBOW:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX() + halfTile, origin.getY() + tileSize);
		g.drawLine(line.toFloat(), pipeThickness);
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX() + tileSize, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_SW_ELBOW:
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
		                        origin.getX() + halfTile, origin.getY() + tileSize);
		g.drawLine(line.toFloat(), pipeThickness);
		line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
		                        origin.getX() + halfTile, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), pipeThickness);
		g.fillEllipse(elbowJoint);
		break;

	case TilePiece::TYPE_CROSS:
		{
			Cross* crossTile = dynamic_cast<Cross*>(pipe);
			if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
				line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
				                       origin.getX() + tileSize, origin.getY() + halfTile);
			else
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
				                       origin.getX() + halfTile, origin.getY() + tileSize);
			g.drawLine(line.toFloat(), pipeThickness);
		}
		break;

	default:
		break;
	}
}

// static
void BoardComponent::DrawCrossSecondWay(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g)
{
	if (tile->GetType() != TilePiece::TYPE_CROSS)
		return;

	Cross* crossTile = dynamic_cast<Cross*>(tile);

	juce::Line<int> line;
	int halfTile = tileSize / 2;
	float pipeThickness = tileSize / Layout::PIPE_THICKNESS_DIVISOR;
	int pipeHalfThickness = static_cast<int>(tileSize / Layout::PIPE_HALF_THICKNESS_DIVISOR);
	g.setColour(juce::Colours::black);

	if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
		line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
		                       origin.getX() + halfTile, origin.getY() + tileSize);
	else
		line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
		                       origin.getX() + tileSize, origin.getY() + halfTile);
	g.drawLine(line.toFloat(), pipeThickness);

	float littleLineThickness = (tileSize * Layout::CROSS_SEP_NUM) / Layout::CROSS_SEP_DEN;
	{
	    auto sepColour = GameRenderer::GetTileColourForLevel(Controller::GetInstance()->GetDifficultyLevel());
	    if (juce::LookAndFeel::getDefaultLookAndFeel()
	            .findColour(juce::ResizableWindow::backgroundColourId).getBrightness() > 0.5f)
	        sepColour = sepColour.brighter(0.3f);
	    else
	        sepColour = sepColour.darker(0.2f);
	    g.setColour(sepColour);
	}

	if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
	{
		line = juce::Line<int>(
			static_cast<int>(origin.getX() + halfTile - pipeHalfThickness - 1), origin.getY() + 1,
			static_cast<int>(origin.getX() + halfTile - pipeHalfThickness - 1), origin.getY() + tileSize - 1);
		g.drawLine(line.toFloat(), littleLineThickness);
		line = juce::Line<int>(
			static_cast<int>(origin.getX() + halfTile + pipeHalfThickness + 1), origin.getY() + 1,
			static_cast<int>(origin.getX() + halfTile + pipeHalfThickness + 1), origin.getY() + tileSize - 1);
		g.drawLine(line.toFloat(), littleLineThickness);
	}
	else
	{
		line = juce::Line<int>(
			origin.getX() + 1, origin.getY() + halfTile - pipeHalfThickness - 1,
			origin.getX() + tileSize - 1, origin.getY() + halfTile - pipeHalfThickness - 1);
		g.drawLine(line.toFloat(), littleLineThickness);
		line = juce::Line<int>(
			origin.getX() + 1, origin.getY() + halfTile + pipeHalfThickness + 1,
			origin.getX() + tileSize - 1, origin.getY() + halfTile + pipeHalfThickness + 1);
		g.drawLine(line.toFloat(), littleLineThickness);
	}

	// Ooze on the second (foreground) channel.
	g.setColour(juce::LookAndFeel::getDefaultLookAndFeel().findColour(GameRenderer::pipeOozeColourId));
	int fill;

	if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
	{
		fill = static_cast<int>(tileSize * crossTile->GetOozeLevel(Cross::WAY_VERTICAL) / MAX_OOZE_LEVEL);
		if (fill > 0)
		{
			if (crossTile->GetFlowDirection() == Pipe::DIR_N)
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
				                       origin.getX() + halfTile, origin.getY() + tileSize);
			else
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
				                       origin.getX() + halfTile, origin.getY() + fill);
			g.drawLine(line.toFloat(), OOZE_THICKNESS);
		}
	}
	else
	{
		fill = static_cast<int>(tileSize * crossTile->GetOozeLevel(Cross::WAY_HORIZONTAL) / MAX_OOZE_LEVEL);
		if (fill > 0)
		{
			if (crossTile->GetFlowDirection() == Pipe::DIR_E)
				line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
				                       origin.getX() + fill, origin.getY() + halfTile);
			else
				line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
				                       origin.getX() + tileSize, origin.getY() + halfTile);
			g.drawLine(line.toFloat(), OOZE_THICKNESS);
		}
	}
}

// static
void BoardComponent::DrawTileDecoration(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g)
{
	// Border colour adapts to LookAndFeel so it remains visible in both dark and light mode.
	g.setColour(juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::Label::textColourId));
	g.drawRect(origin.getX(), origin.getY(), tileSize, tileSize, 1);

	if (tile->GetType() == TilePiece::TYPE_NONE)
		return;

	Pipe* pipe = dynamic_cast<Pipe*>(tile);
	if (pipe == nullptr)
		return;

	int exp = pipe->PopExplosion();
	if (exp > 0)
	{
		juce::Path starPath;
		int halfTile = tileSize / 2;
		starPath.addStar(juce::Point<float>(static_cast<float>(origin.getX() + halfTile),
		                                    static_cast<float>(origin.getY() + halfTile)),
		                 7,
		                 static_cast<float>(exp * 4),
		                 static_cast<float>(exp * 8),
		                 static_cast<float>(exp * 2));
		{
		    auto starColour = juce::LookAndFeel::getDefaultLookAndFeel()
		                          .findColour(GameRenderer::pipeOozeColourId);
		    if (starColour.isTransparent() || starColour == juce::Colours::black)
		        starColour = juce::Colours::white; // fallback if colour ID not registered
		    g.setColour(starColour);
		    g.fillPath(starPath);
		}
	}
}


// ---- Private drawing helpers ----

void BoardComponent::DrawOoze(TilePiece* tile, juce::Point<int> origin, int tileSize, juce::Graphics& g)
{
	if (tile->GetType() == TilePiece::TYPE_NONE)
		return;

	Pipe* pipe = dynamic_cast<Pipe*>(tile);
	if ((pipe == nullptr) || pipe->IsEmpty())
		return;

	static const float oozeHalfThickness = OOZE_THICKNESS / 2.0f;
	bool overHalf(pipe->GetOozeLevel() >= 50.0f);
	bool underHalf(pipe->GetOozeLevel() < 50.0f);
	int fill = static_cast<int>(tileSize * pipe->GetOozeLevel() / MAX_OOZE_LEVEL);

	juce::Line<int> line;
	g.setColour(juce::LookAndFeel::getDefaultLookAndFeel().findColour(GameRenderer::pipeOozeColourId));

	int halfTile = tileSize / 2;
	juce::Rectangle<float> elbowJoint(
		origin.getX() + halfTile - oozeHalfThickness,
		origin.getY() + halfTile - oozeHalfThickness,
		OOZE_THICKNESS, OOZE_THICKNESS);

	switch (pipe->GetType())
	{
	case TilePiece::TYPE_START_N:
		if (overHalf)
		{
			line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
			                       origin.getX() + halfTile, origin.getY() + halfTile);
			g.drawLine(line.toFloat(), OOZE_THICKNESS);
			g.fillEllipse(elbowJoint);
		}
		break;

	case TilePiece::TYPE_START_S:
		if (overHalf)
		{
			line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
			                       origin.getX() + halfTile, origin.getY() + fill);
			g.drawLine(line.toFloat(), OOZE_THICKNESS);
			g.fillEllipse(elbowJoint);
		}
		break;

	case TilePiece::TYPE_START_E:
		if (overHalf)
		{
			line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
			                       origin.getX() + fill, origin.getY() + halfTile);
			g.drawLine(line.toFloat(), OOZE_THICKNESS);
			g.fillEllipse(elbowJoint);
		}
		break;

	case TilePiece::TYPE_START_W:
		if (overHalf)
		{
			line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
			                       origin.getX() + halfTile, origin.getY() + halfTile);
			g.drawLine(line.toFloat(), OOZE_THICKNESS);
			g.fillEllipse(elbowJoint);
		}
		break;

	case TilePiece::TYPE_VERTICAL:
		if (pipe->GetFlowDirection() == Pipe::DIR_N)
			line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
			                       origin.getX() + halfTile, origin.getY() + tileSize);
		else if (pipe->GetFlowDirection() == Pipe::DIR_S)
			line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
			                       origin.getX() + halfTile, origin.getY() + fill);
		g.drawLine(line.toFloat(), OOZE_THICKNESS);
		break;

	case TilePiece::TYPE_HORIZONTAL:
		if (pipe->GetFlowDirection() == Pipe::DIR_E)
			line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
			                       origin.getX() + fill, origin.getY() + halfTile);
		else if (pipe->GetFlowDirection() == Pipe::DIR_W)
			line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
			                       origin.getX() + tileSize, origin.getY() + halfTile);
		g.drawLine(line.toFloat(), OOZE_THICKNESS);
		break;

	case TilePiece::TYPE_NW_ELBOW:
		if (pipe->GetFlowDirection() == Pipe::DIR_N)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
				                       origin.getX() + fill, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		else if (pipe->GetFlowDirection() == Pipe::DIR_W)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
				                       origin.getX() + halfTile, origin.getY() + fill);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		break;

	case TilePiece::TYPE_NE_ELBOW:
		if (pipe->GetFlowDirection() == Pipe::DIR_N)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
				                       origin.getX() + tileSize, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + tileSize, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		else if (pipe->GetFlowDirection() == Pipe::DIR_E)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
				                       origin.getX() + halfTile, origin.getY() + fill);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + fill, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		break;

	case TilePiece::TYPE_SE_ELBOW:
		if (pipe->GetFlowDirection() == Pipe::DIR_S)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
				                       origin.getX() + tileSize, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + tileSize, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + fill);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		else if (pipe->GetFlowDirection() == Pipe::DIR_E)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
				                       origin.getX() + halfTile, origin.getY() + tileSize);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + tileSize);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + fill, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		break;

	case TilePiece::TYPE_SW_ELBOW:
		if (pipe->GetFlowDirection() == Pipe::DIR_S)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
				                       origin.getX() + fill, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + fill);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		else if (pipe->GetFlowDirection() == Pipe::DIR_W)
		{
			if (underHalf)
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
				                       origin.getX() + halfTile, origin.getY() + tileSize);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
			else
			{
				line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + tileSize);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
				                       origin.getX() + halfTile, origin.getY() + halfTile);
				g.drawLine(line.toFloat(), OOZE_THICKNESS);
				g.fillEllipse(elbowJoint);
			}
		}
		break;

	case TilePiece::TYPE_CROSS:
		{
			Cross* crossTile = dynamic_cast<Cross*>(pipe);
			if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
			{
				fill = static_cast<int>(tileSize * crossTile->GetOozeLevel(Cross::WAY_HORIZONTAL) / MAX_OOZE_LEVEL);
				if (fill > 0)
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_E)
						line = juce::Line<int>(origin.getX(), origin.getY() + halfTile,
						                       origin.getX() + fill, origin.getY() + halfTile);
					else
						line = juce::Line<int>(origin.getX() + tileSize - fill, origin.getY() + halfTile,
						                       origin.getX() + tileSize, origin.getY() + halfTile);
					g.drawLine(line.toFloat(), OOZE_THICKNESS);
				}
			}
			else
			{
				fill = static_cast<int>(tileSize * crossTile->GetOozeLevel(Cross::WAY_VERTICAL) / MAX_OOZE_LEVEL);
				if (fill > 0)
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_N)
						line = juce::Line<int>(origin.getX() + halfTile, origin.getY() + tileSize - fill,
						                       origin.getX() + halfTile, origin.getY() + tileSize);
					else
						line = juce::Line<int>(origin.getX() + halfTile, origin.getY(),
						                       origin.getX() + halfTile, origin.getY() + fill);
					g.drawLine(line.toFloat(), OOZE_THICKNESS);
				}
			}
		}
		break;

	default:
		break;
	}
}

void BoardComponent::DrawSpill(juce::Point<int> origin, int tileSize, juce::Graphics& g)
{
	if (Controller::GetInstance()->GetState() != Controller::STATE_STOPPED)
		return;

	Pipe* oozingPipe = dynamic_cast<Pipe*>(Controller::GetInstance()->GetBoard()->GetOozingTile());
	if (oozingPipe == nullptr)
		return;

	int halfTile = tileSize / 2;
	int qt = tileSize / 4;
	Pipe::Direction spillDir = oozingPipe->GetFlowDirection();
	juce::Rectangle<int> bigRec;
	juce::Rectangle<int> smlRec;

	switch (spillDir)
	{
	case Pipe::DIR_N:
		bigRec = juce::Rectangle<int>(origin.getX(), origin.getY() - tileSize, tileSize, tileSize);
		smlRec = juce::Rectangle<int>(origin.getX() + qt, origin.getY() - halfTile, halfTile, halfTile);
		break;
	case Pipe::DIR_S:
		bigRec = juce::Rectangle<int>(origin.getX(), origin.getY() + tileSize, tileSize, tileSize);
		smlRec = juce::Rectangle<int>(origin.getX() + qt, origin.getY() + tileSize, halfTile, halfTile);
		break;
	case Pipe::DIR_E:
		bigRec = juce::Rectangle<int>(origin.getX() + tileSize, origin.getY(), tileSize, tileSize);
		smlRec = juce::Rectangle<int>(origin.getX() + tileSize, origin.getY() + qt, halfTile, halfTile);
		break;
	case Pipe::DIR_W:
		bigRec = juce::Rectangle<int>(origin.getX() - tileSize, origin.getY(), tileSize, tileSize);
		smlRec = juce::Rectangle<int>(origin.getX() - halfTile, origin.getY() + qt, halfTile, halfTile);
		break;
	default:
		return;
	}

	const auto oozeColour = juce::LookAndFeel::getDefaultLookAndFeel()
	                            .findColour(GameRenderer::pipeOozeColourId);
	g.setColour(oozeColour.withAlpha(0.5f));
	g.fillEllipse(bigRec.toFloat());
	g.setColour(oozeColour);
	g.fillEllipse(smlRec.toFloat());
}
