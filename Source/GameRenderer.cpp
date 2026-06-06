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


#include "GameRenderer.h"
#include "LayoutConstants.h"
#include "TilePiece.h"
#include "Board.h"
#include "Queue.h"
#include "Controller.h"


// ---- Class Implementation ----

GameRenderer::GameRenderer()
{
}

void GameRenderer::SetLayout(juce::Rectangle<int> windowBounds)
{
	m_windowBounds = windowBounds;

	int minDimension = std::min<int>(windowBounds.getWidth(), static_cast<int>(windowBounds.getHeight() * Layout::ASPECT_RATIO));
	m_tileSize = static_cast<int>(minDimension / Layout::TILE_SIZE_DIVISOR);

	m_fastForwardButtonRect = juce::Rectangle<float>(
		windowBounds.getWidth() / Layout::FF_BUTTON_H_DIVISOR,
		windowBounds.getHeight() / Layout::FF_BUTTON_V_DIVISOR,
		Layout::FF_BUTTON_W,
		Layout::FF_BUTTON_H).toNearestInt();
}

void GameRenderer::Render(juce::Graphics& g, int countDown)
{
	Board* board(Controller::GetInstance()->GetBoard());
	Queue* queue(Controller::GetInstance()->GetQueue());

	int boardHStartPos = static_cast<int>(m_windowBounds.getWidth() / Layout::BOARD_H_DIVISOR);
	int boardVStartPos = static_cast<int>(m_windowBounds.getHeight() / Layout::BOARD_V_DIVISOR);

	// Background colour
	g.fillAll(juce::Colour(67, 67, 67));

	// Draw countdown to ooze.
	DrawOozeMeter(juce::Point<int>(static_cast<int>(m_windowBounds.getWidth() / Layout::OOZE_METER_H_DIVISOR), 30), g, countDown);

	// Draw current level number and score
	DrawLevelAndScore(g);

	// Draw app info
	{
		juce::String infoText("Pipe Dreamer V");
		juce::String versionString(JUCE_STRINGIFY(JUCE_APP_VERSION));
		infoText << versionString;

		auto f = GetFont(LABEL_VERSION);
		auto textWidth = juce::GlyphArrangement::getStringWidthInt(f, infoText);

		juce::Rectangle<int> textRect(boardHStartPos, m_windowBounds.getHeight() - 50, textWidth, 40);

		g.setFont(f);
		g.setColour(juce::Colours::grey);
		g.drawText(infoText, textRect, juce::Justification::left, false);
	}

	// Draw bombs
	DrawBombs(juce::Point<int>(static_cast<int>(m_windowBounds.getWidth() / Layout::BOMBS_H_DIVISOR), 20), g);

	// Draw button to accelerate game speed.
	DrawFastForwardButton(g);

	// Draw tile queue: queueVStartPos is the origin of the bottommost tile in the queue
	int queueVStartPos = static_cast<int>(m_windowBounds.getHeight() / Layout::QUEUE_V_DIVISOR);
	int queueHStartPos = static_cast<int>(m_windowBounds.getWidth() / Layout::QUEUE_H_DIVISOR);
	for (int i = 0; i < queue->GetSize(); i++)
	{
		// Pipe shape
		juce::Point<int> p(queueHStartPos, queueVStartPos - i * (m_tileSize - 1));
		DrawTile((queue->GetTile(i)), p, g);
		DrawCrossSecondWay((queue->GetTile(i)), p, g);
		DrawTileDecoration((queue->GetTile(i)), p, g);

		if (i == 0)
		{
			// Extra frame for the tile at the start of the queue.
			g.setColour(juce::Colours::limegreen);
			g.drawRect(queueHStartPos - 4, queueVStartPos - 4, m_tileSize + 8, m_tileSize + 8, 2);
			g.setColour(juce::Colours::black);
			g.drawRect(queueHStartPos - 6, queueVStartPos - 6, m_tileSize + 12, m_tileSize + 12, 2);
		}
	}

	// Position of the oozing tile. Needed for displaying spills below.
	juce::Point<int> oozingTileOrigin;

	// Draw board.
	for (int i = 0; i < board->GetNumCols(); i++)
	{
		for (int j = 0; j < board->GetNumRows(); j++)
		{
			juce::Point<int> p(boardHStartPos + i * (m_tileSize - 1), boardVStartPos + j * (m_tileSize - 1));

			TilePiece* tile = board->GetTile(i, j);

			DrawTile(tile, p, g);
			DrawOoze(tile, p, g);
			DrawCrossSecondWay(tile, p, g);
			DrawTileDecoration(tile, p, g);

			if (board->GetOozingTile() == tile)
				oozingTileOrigin = p;
		}
	}

	// Draw ooze spillage, if any.
	DrawSpill(oozingTileOrigin, g);
}

int GameRenderer::GetTileSize() const
{
	return m_tileSize;
}

juce::Rectangle<int> GameRenderer::GetFastForwardButtonRect() const
{
	return m_fastForwardButtonRect;
}

juce::Colour GameRenderer::GetTileColourForLevel(int difficultyLevel)
{
	static const juce::Colour colorsPerLevel[] = {
		juce::Colour(125, 125, 125),	// Level 1
		juce::Colours::cadetblue,		// Level 2
		juce::Colours::darkkhaki,		// Level 3
		juce::Colour(140, 180, 90),		// Level 4
		juce::Colours::darkslategrey,	// Level 5
		juce::Colours::hotpink,			// Level 6
		juce::Colour(27, 122, 165),		// Level 7
		juce::Colours::coral,			// Level 8
		juce::Colours::blueviolet,		// Level 9
		juce::Colours::darkorange,		// Level 10
		juce::Colours::mediumseagreen,	// Level 11
		juce::Colours::orangered,		// Level 12
	};

	// difficultyLevel starts at 1
	int arraySize = static_cast<int>(sizeof(colorsPerLevel) / sizeof(*colorsPerLevel));
	difficultyLevel--;
	if (difficultyLevel >= arraySize)
		difficultyLevel = arraySize - 1;

	return colorsPerLevel[difficultyLevel];
}

juce::Font GameRenderer::GetFont(LabelID labelID) const
{
	// Scale the font according to the game window's both width and height.
	int minDimension = std::min<int>(static_cast<int>(m_windowBounds.getWidth() / Layout::ASPECT_RATIO), m_windowBounds.getHeight());
	switch (labelID)
	{
		case LABEL_VERSION:
			return { juce::FontOptions("consolas", (minDimension * Layout::FONT_VERSION_PT / Layout::FONT_REF_HEIGHT), juce::Font::plain) };
		case LABEL_SCORE:
			return { juce::FontOptions("consolas", (minDimension * Layout::FONT_SCORE_PT / Layout::FONT_REF_HEIGHT), juce::Font::plain) };
		case LABEL_BSCORE:
			return { juce::FontOptions("consolas", (minDimension * Layout::FONT_SCORE_PT / Layout::FONT_REF_HEIGHT), juce::Font::bold) };
	}

	return { juce::FontOptions() };
}

void GameRenderer::DrawLevelAndScore(juce::Graphics& g)
{
	Controller* controller(Controller::GetInstance());
	int playerScore = controller->GetBoard()->GetScoreValue();

	g.setFont(GetFont(LABEL_SCORE));
	g.setColour(juce::Colours::grey);

	int halfTile = m_tileSize / 2;
	juce::Rectangle<int> textRect(	static_cast<int>(m_windowBounds.getWidth() / Layout::LEVEL_LABEL_H_DIVISOR), Layout::SCORE_LABEL_V_OFFSET,
									static_cast<int>(m_windowBounds.getWidth() / Layout::SCORE_LABEL_WIDTH_DIVISOR), halfTile);
	g.drawText("Level:", textRect, juce::Justification::left, false);

	textRect = juce::Rectangle<int>(	static_cast<int>(m_windowBounds.getWidth() / Layout::SCORE_LABEL_H_DIVISOR), Layout::SCORE_LABEL_V_OFFSET,
										static_cast<int>(m_windowBounds.getWidth() / Layout::SCORE_LABEL_WIDTH_DIVISOR), halfTile);
	g.drawText("Score:", textRect, juce::Justification::left, false);

	// If score this round is high enough to advance to next difficulty level, highlight the number.
	if (playerScore >= Controller::MIN_SCORE_TO_ADVANCE)
	{
		g.setColour(juce::Colours::yellow);
		g.setFont(GetFont(LABEL_BSCORE));
	}
	textRect = juce::Rectangle<int>(static_cast<int>(m_windowBounds.getWidth() / Layout::SCORE_VALUE_H_DIVISOR), Layout::SCORE_LABEL_V_OFFSET,
									static_cast<int>(m_windowBounds.getWidth() / Layout::SCORE_VALUE_WIDTH_DIVISOR), halfTile);
	g.drawText(juce::String(playerScore), textRect, juce::Justification::left, false);

	// Show difficulty level number in this level's tile color.
	g.setColour(GetTileColourForLevel(controller->GetDifficultyLevel()));
	textRect = juce::Rectangle<int>(static_cast<int>(m_windowBounds.getWidth() / Layout::LEVEL_VALUE_H_DIVISOR), Layout::SCORE_LABEL_V_OFFSET,
									static_cast<int>(m_windowBounds.getWidth() / Layout::LEVEL_VALUE_WIDTH_DIVISOR), halfTile);
	g.drawText(juce::String(controller->GetDifficultyLevel()), textRect, juce::Justification::left, false);
}

void GameRenderer::DrawTile(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g)
{
	if (tile->GetType() != TilePiece::TYPE_NONE)
	{
		Pipe* pipe = dynamic_cast<Pipe*>(tile);
		if (pipe != nullptr)
		{
			// Draw tile's Background color
			g.setColour(GetTileColourForLevel(Controller::GetInstance()->GetDifficultyLevel()));
			g.fillRect(origin.getX(), origin.getY(), m_tileSize, m_tileSize);

			juce::Line<int> line;
			float pipeThickness = m_tileSize / Layout::PIPE_THICKNESS_DIVISOR;
			g.setColour(juce::Colours::black);

			// Ellipse rect used to have nice rounded corners in the elbow pipes.
			int halfTile = m_tileSize / 2;
			juce::Rectangle<float> elbowJoint(	origin.getX() + halfTile - (pipeThickness / 2.0f),
												origin.getY() + halfTile - (pipeThickness / 2.0f),
												pipeThickness,
												pipeThickness);

			switch (pipe->GetType())
			{
			case TilePiece::TYPE_START_N:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY(),
											origin.getX() + halfTile,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_START_S:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX() + halfTile,
											origin.getY() + m_tileSize);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_START_E:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX() + m_tileSize,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_START_W:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX(),
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_VERTICAL:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY(),
											origin.getX() + halfTile,
											origin.getY() + m_tileSize);
					g.drawLine(line.toFloat(), pipeThickness);
				}
				break;

			case TilePiece::TYPE_HORIZONTAL:
				{
					line = juce::Line<int>(	origin.getX(),
											origin.getY() + halfTile,
											origin.getX() + m_tileSize,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
				}
				break;

			case TilePiece::TYPE_NW_ELBOW:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY(),
											origin.getX() + halfTile,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					line = juce::Line<int>(	origin.getX(),
											origin.getY() + halfTile,
											origin.getX() + halfTile,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_NE_ELBOW:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY(),
											origin.getX() + halfTile,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX() + m_tileSize,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_SE_ELBOW:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX() + halfTile,
											origin.getY() + m_tileSize);
					g.drawLine(line.toFloat(), pipeThickness);
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX() + m_tileSize,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_SW_ELBOW:
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + halfTile,
											origin.getX() + halfTile,
											origin.getY() + m_tileSize);
					g.drawLine(line.toFloat(), pipeThickness);
					line = juce::Line<int>(	origin.getX(),
											origin.getY() + halfTile,
											origin.getX() + halfTile,
											origin.getY() + halfTile);
					g.drawLine(line.toFloat(), pipeThickness);
					g.fillEllipse(elbowJoint);
				}
				break;

			case TilePiece::TYPE_CROSS:
				{
					Cross* crossTile = dynamic_cast<Cross*>(pipe);
					if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
					{
						// Draw horizontal pipe first.
						// Vertical pipe will be drawn in DrawCrossSecondWay()
						line = juce::Line<int>(	origin.getX(),
												origin.getY() + halfTile,
												origin.getX() + m_tileSize,
												origin.getY() + halfTile);
					}
					else
					{
						// Draw vertical pipe first.
						// Horizontal pipe will be drawn in DrawCrossSecondWay()
						line = juce::Line<int>(	origin.getX() + halfTile,
												origin.getY(),
												origin.getX() + halfTile,
												origin.getY() + m_tileSize);
					}

					g.drawLine(line.toFloat(), pipeThickness);
				}
				break;

			default:
				break;
			}
		}
	}
}

void GameRenderer::DrawOoze(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g)
{
	if (tile->GetType() != TilePiece::TYPE_NONE)
	{
		Pipe* pipe = dynamic_cast<Pipe*>(tile);
		if ((pipe != nullptr) &&
			(!pipe->IsEmpty()))
		{
			static const float oozeHalfThickness = OOZE_THICKNESS / 2.0f;
			bool overHalf(pipe->GetOozeLevel() >= 50.0f);
			bool underHalf(pipe->GetOozeLevel() < 50.0f);
			int fill = static_cast<int>(m_tileSize * pipe->GetOozeLevel() / MAX_OOZE_LEVEL);

			juce::Line<int> line;
			g.setColour(juce::Colours::limegreen);

			// Ellipse rect used to have nice rounded corners in the elbow pipes.
			int halfTile = m_tileSize / 2;
			juce::Rectangle<float> elbowJoint(	origin.getX() + halfTile - oozeHalfThickness,
												origin.getY() + halfTile - oozeHalfThickness,
												OOZE_THICKNESS,
												OOZE_THICKNESS);

			switch (pipe->GetType())
			{
			case TilePiece::TYPE_START_N:
				{
					if (overHalf)
					{
						line = juce::Line<int>(	origin.getX() + halfTile,
												origin.getY() + m_tileSize - fill,
												origin.getX() + halfTile,
												origin.getY() + halfTile);
						g.drawLine(line.toFloat(), OOZE_THICKNESS);
						g.fillEllipse(elbowJoint);
					}
				}
				break;

			case TilePiece::TYPE_START_S:
				{
					if (overHalf)
					{
						line = juce::Line<int>(	origin.getX() + halfTile,
												origin.getY() + halfTile,
												origin.getX() + halfTile,
												origin.getY() + fill);
						g.drawLine(line.toFloat(), OOZE_THICKNESS);
						g.fillEllipse(elbowJoint);
					}
				}
				break;

			case TilePiece::TYPE_START_E:
				{
					if (overHalf)
					{
						line = juce::Line<int>(	origin.getX() + halfTile,
												origin.getY() + halfTile,
												origin.getX() + fill,
												origin.getY() + halfTile);
						g.drawLine(line.toFloat(), OOZE_THICKNESS);
						g.fillEllipse(elbowJoint);
					}
				}
				break;

			case TilePiece::TYPE_START_W:
				{
					if (overHalf)
					{
						line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
												origin.getY() + halfTile,
												origin.getX() + halfTile,
												origin.getY() + halfTile);
						g.drawLine(line.toFloat(), OOZE_THICKNESS);
						g.fillEllipse(elbowJoint);
					}
				}
				break;

			case TilePiece::TYPE_VERTICAL:
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_N)
						line = juce::Line<int>(	origin.getX() + halfTile,
												origin.getY() + m_tileSize - fill,
												origin.getX() + halfTile,
												origin.getY() + m_tileSize);
					else if (pipe->GetFlowDirection() == Pipe::DIR_S)
						line = juce::Line<int>(	origin.getX() + halfTile,
												origin.getY(),
												origin.getX() + halfTile,
												origin.getY() + fill);

					g.drawLine(line.toFloat(), OOZE_THICKNESS);
				}
				break;

			case TilePiece::TYPE_HORIZONTAL:
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_E)
						line = juce::Line<int>(	origin.getX(),
												origin.getY() + halfTile,
												origin.getX() + fill,
												origin.getY() + halfTile);
					else if (pipe->GetFlowDirection() == Pipe::DIR_W)
						line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
												origin.getY() + halfTile,
												origin.getX() + m_tileSize,
												origin.getY() + halfTile);

					g.drawLine(line.toFloat(), OOZE_THICKNESS);
				}
				break;

			case TilePiece::TYPE_NW_ELBOW:
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_N)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX(),
													origin.getY() + halfTile,
													origin.getX() + fill,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX(),
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + m_tileSize - fill,
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
					else if (pipe->GetFlowDirection() == Pipe::DIR_W)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY(),
													origin.getX() + halfTile,
													origin.getY() + fill);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY(),
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
				}
				break;

			case TilePiece::TYPE_NE_ELBOW:
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_N)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
													origin.getY() + halfTile,
													origin.getX() + m_tileSize,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + m_tileSize,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + m_tileSize - fill,
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
					else if (pipe->GetFlowDirection() == Pipe::DIR_E)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY(),
													origin.getX() + halfTile,
													origin.getY() + fill);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY(),
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + fill,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
				}
				break;

			case TilePiece::TYPE_SE_ELBOW:
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_S)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
													origin.getY() + halfTile,
													origin.getX() + m_tileSize,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + m_tileSize,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + fill);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
					else if (pipe->GetFlowDirection() == Pipe::DIR_E)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + m_tileSize - fill,
													origin.getX() + halfTile,
													origin.getY() + m_tileSize);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + m_tileSize);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + fill,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
				}
				break;

			case TilePiece::TYPE_SW_ELBOW:
				{
					if (pipe->GetFlowDirection() == Pipe::DIR_S)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX(),
													origin.getY() + halfTile,
													origin.getX() + fill,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX(),
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + fill);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
					else if (pipe->GetFlowDirection() == Pipe::DIR_W)
					{
						if (underHalf)
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + m_tileSize - fill,
													origin.getX() + halfTile,
													origin.getY() + m_tileSize);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
						else
						{
							line = juce::Line<int>(	origin.getX() + halfTile,
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + m_tileSize);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
													origin.getY() + halfTile,
													origin.getX() + halfTile,
													origin.getY() + halfTile);
							g.drawLine(line.toFloat(), OOZE_THICKNESS);
							g.fillEllipse(elbowJoint);
						}
					}
				}
				break;

			case TilePiece::TYPE_CROSS:
				{
					Cross* crossTile = dynamic_cast<Cross*>(pipe);
					if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
					{
						// Draw horizontally flowing ooze first.
						fill = static_cast<int>(m_tileSize * crossTile->GetOozeLevel(Cross::WAY_HORIZONTAL) / MAX_OOZE_LEVEL);
						if (fill > 0)
						{
							if (pipe->GetFlowDirection() == Pipe::DIR_E)
							{
								line = juce::Line<int>(	origin.getX(),
														origin.getY() + halfTile,
														origin.getX() + fill,
														origin.getY() + halfTile);
							}
							else
							{
								line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
														origin.getY() + halfTile,
														origin.getX() + m_tileSize,
														origin.getY() + halfTile);
							}

							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
					}

					else
					{
						// Draw vertically flowing ooze first.
						fill = static_cast<int>(m_tileSize * crossTile->GetOozeLevel(Cross::WAY_VERTICAL) / MAX_OOZE_LEVEL);
						if (fill > 0)
						{
							if (pipe->GetFlowDirection() == Pipe::DIR_N)
							{
								line = juce::Line<int>(	origin.getX() + halfTile,
														origin.getY() + m_tileSize - fill,
														origin.getX() + halfTile,
														origin.getY() + m_tileSize);
							}
							else
							{
								line = juce::Line<int>(	origin.getX() + halfTile,
														origin.getY(),
														origin.getX() + halfTile,
														origin.getY() + fill);
							}

							g.drawLine(line.toFloat(), OOZE_THICKNESS);
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}
}

void GameRenderer::DrawCrossSecondWay(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g)
{
	if (tile->GetType() == TilePiece::TYPE_CROSS)
	{
		Cross* crossTile = dynamic_cast<Cross*>(tile);

		juce::Line<int> line;
		int halfTile = m_tileSize / 2;
		float pipeThickness = m_tileSize / Layout::PIPE_THICKNESS_DIVISOR;
		int pipeHalfThickness = static_cast<int>(m_tileSize / Layout::PIPE_HALF_THICKNESS_DIVISOR);
		g.setColour(juce::Colours::black);

		// Draw pipe first
		if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
		{
			// Vertical pipe.
			line = juce::Line<int>(	origin.getX() + halfTile,
									origin.getY(),
									origin.getX() + halfTile,
									origin.getY() + m_tileSize);
		}
		else
		{
			// Horizontal pipe.
			line = juce::Line<int>(	origin.getX(),
									origin.getY() + halfTile,
									origin.getX() + m_tileSize,
									origin.getY() + halfTile);
		}
		g.drawLine(line.toFloat(), pipeThickness);

		// Little lines along the pipe, which make the separation between horizontal and vertical
		// components of the cross-pipe more visually obvious.
		float littleLineThickness = (m_tileSize * Layout::CROSS_SEP_NUM) / Layout::CROSS_SEP_DEN;
		g.setColour(GetTileColourForLevel(Controller::GetInstance()->GetDifficultyLevel()));
		if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
		{
			// Vertical little lines
			line = juce::Line<int>(	static_cast<int>(origin.getX() + halfTile - pipeHalfThickness - 1),
									origin.getY() + 1,
									static_cast<int>(origin.getX() + halfTile - pipeHalfThickness - 1),
									origin.getY() + m_tileSize - 1);
			g.drawLine(line.toFloat(), littleLineThickness);
			line = juce::Line<int>(	static_cast<int>(origin.getX() + halfTile + pipeHalfThickness + 1),
									origin.getY() + 1,
									static_cast<int>(origin.getX() + halfTile + pipeHalfThickness + 1),
									origin.getY() + m_tileSize - 1);
			g.drawLine(line.toFloat(), littleLineThickness);
		}
		else
		{
			// Horizontal little lines
			line = juce::Line<int>(	origin.getX() + 1,
									origin.getY() + halfTile - static_cast<int>(pipeHalfThickness) - 1,
									origin.getX() + m_tileSize - 1,
									origin.getY() + halfTile - static_cast<int>(pipeHalfThickness) - 1);
			g.drawLine(line.toFloat(), littleLineThickness);
			line = juce::Line<int>(	origin.getX() + 1,
									origin.getY() + halfTile + static_cast<int>(pipeHalfThickness) + 1,
									origin.getX() + m_tileSize - 1,
									origin.getY() + halfTile + static_cast<int>(pipeHalfThickness) + 1);
			g.drawLine(line.toFloat(), littleLineThickness);
		}

		// Draw ooze
		g.setColour(juce::Colours::limegreen);
		int fill;

		// Vertically flowing ooze
		if (crossTile->GetBackgroundWay() == Cross::WAY_HORIZONTAL)
		{
			fill = static_cast<int>(m_tileSize * crossTile->GetOozeLevel(Cross::WAY_VERTICAL) / MAX_OOZE_LEVEL);
			if (fill > 0)
			{
				if (crossTile->GetFlowDirection() == Pipe::DIR_N)
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY() + m_tileSize - fill,
											origin.getX() + halfTile,
											origin.getY() + m_tileSize);
				}
				else
				{
					line = juce::Line<int>(	origin.getX() + halfTile,
											origin.getY(),
											origin.getX() + halfTile,
											origin.getY() + fill);
				}

				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
		}

		// Horizontally flowing ooze
		else
		{
			fill = static_cast<int>(m_tileSize * crossTile->GetOozeLevel(Cross::WAY_HORIZONTAL) / MAX_OOZE_LEVEL);
			if (fill > 0)
			{
				if (crossTile->GetFlowDirection() == Pipe::DIR_E)
				{
					line = juce::Line<int>(	origin.getX(),
											origin.getY() + halfTile,
											origin.getX() + fill,
											origin.getY() + halfTile);
				}
				else
				{
					line = juce::Line<int>(	origin.getX() + m_tileSize - fill,
											origin.getY() + halfTile,
											origin.getX() + m_tileSize,
											origin.getY() + halfTile);
				}

				g.drawLine(line.toFloat(), OOZE_THICKNESS);
			}
		}
	}
}

void GameRenderer::DrawTileDecoration(TilePiece* tile, juce::Point<int> origin, juce::Graphics& g)
{
	// Frame around tile
	g.setColour(juce::Colours::white);
	g.drawRect(origin.getX(), origin.getY(), m_tileSize, m_tileSize, 1);

	if (tile->GetType() != TilePiece::TYPE_NONE)
	{
		Pipe* pipe = dynamic_cast<Pipe*>(tile);
		if (pipe != nullptr)
		{
			// If this tile has an explosion on it, draw it.
			int exp = pipe->PopExplosion();
			if (exp > 0)
			{
				juce::Path starPath;
				int halfTile = m_tileSize / 2;
				starPath.addStar(juce::Point<float>(static_cast<float>(origin.getX() + halfTile),
													static_cast<float>(origin.getY() + halfTile)),
								 7,								// Number of peaks
								 static_cast<float>(exp * 4),	// Inner radius
								 static_cast<float>(exp * 8),	// Outer radius
								 static_cast<float>(exp * 2));	// Rotation angle
				g.setColour(juce::Colours::orangered);
				g.fillPath(starPath);
			}
		}
	}
}

void GameRenderer::DrawSpill(juce::Point<int> origin, juce::Graphics& g)
{
	if (Controller::GetInstance()->GetState() == Controller::STATE_STOPPED)
	{
		Pipe* oozingPipe = dynamic_cast<Pipe*>(Controller::GetInstance()->GetBoard()->GetOozingTile());
		if (oozingPipe != nullptr)
		{
			int halfTile = m_tileSize / 2;
			int qt(m_tileSize / 4);
			Pipe::Direction spillDir = oozingPipe->GetFlowDirection();
			juce::Rectangle<int> bigRec;
			juce::Rectangle<int> smlRec;
			switch (spillDir)
			{
			case Pipe::DIR_N:
				bigRec = juce::Rectangle<int>(origin.getX(), origin.getY() - m_tileSize, m_tileSize, m_tileSize);
				smlRec = juce::Rectangle<int>(origin.getX() + qt, origin.getY() - halfTile, halfTile, halfTile);
				break;
			case Pipe::DIR_S:
				bigRec = juce::Rectangle<int>(origin.getX(), origin.getY() + m_tileSize, m_tileSize, m_tileSize);
				smlRec = juce::Rectangle<int>(origin.getX() + qt, origin.getY() + m_tileSize, halfTile, halfTile);
				break;
			case Pipe::DIR_E:
				bigRec = juce::Rectangle<int>(origin.getX() + m_tileSize, origin.getY(), m_tileSize, m_tileSize);
				smlRec = juce::Rectangle<int>(origin.getX() + m_tileSize, origin.getY() + qt, halfTile, halfTile);
				break;
			case Pipe::DIR_W:
				bigRec = juce::Rectangle<int>(origin.getX() - m_tileSize, origin.getY(), m_tileSize, m_tileSize);
				smlRec = juce::Rectangle<int>(origin.getX() - halfTile, origin.getY() + qt, halfTile, halfTile);
				break;
			case Pipe::DIR_NONE:
			default:
				break;
			}

			g.setColour(juce::Colour(0x88008000)); // transparent green
			g.fillEllipse(bigRec.toFloat());
			g.setColour(juce::Colours::limegreen);
			g.fillEllipse(smlRec.toFloat());
		}
	}
}

void GameRenderer::DrawOozeMeter(juce::Point<int> origin, juce::Graphics& g, int countDown)
{
	Board* board(Controller::GetInstance()->GetBoard());

	// Draw empty vial (background)
	int vialHeight = static_cast<int>(m_windowBounds.getHeight() / Layout::OOZE_METER_HEIGHT_DIVISOR);
	juce::Rectangle<int> vialRect(origin.getX(), origin.getY(), Layout::OOZE_VIAL_WIDTH, vialHeight);
	g.setColour(juce::Colours::black);
	g.fillRect(vialRect);

	// Ooze inside the vial.
	g.setColour(juce::Colours::limegreen);
	int oozeMaxHeight = vialHeight - (Layout::OOZE_VIAL_PADDING * 2);
	int oozeHeight;
	if (countDown > 0)
	{
		// Starts at 0, goes to vialHeight
		oozeHeight = static_cast<int>(oozeMaxHeight - ((countDown * oozeMaxHeight) / Controller::GetInstance()->GetCurrentCountdown()));
	}
	else if (board->GetScoreValue() < Controller::MIN_SCORE_TO_ADVANCE)
	{
		// Starts at vialHeight, goes to 0.
		oozeHeight = static_cast<int>(((Controller::MIN_SCORE_TO_ADVANCE - (board->GetScoreValue())) * oozeMaxHeight) / Controller::MIN_SCORE_TO_ADVANCE);
	}
	else
	{
		// Full yellow vial.
		g.setColour(juce::Colours::yellow);
		oozeHeight = oozeMaxHeight;
	}
	g.fillRect(juce::Rectangle<int>(origin.getX() + Layout::OOZE_VIAL_PADDING, origin.getY() + Layout::OOZE_VIAL_PADDING + oozeMaxHeight - oozeHeight, Layout::OOZE_FILL_WIDTH, oozeHeight));

	// Vial outline and markings.
	g.setColour(juce::Colours::black);
	g.drawLine(origin.getX() + 15.0f, 1.0f + origin.getY() + vialHeight * 0.25f,	origin.getX() + 22.0f, 1.0f + origin.getY() + vialHeight * 0.25f,	2.0f);
	g.drawLine(origin.getX() + 10.0f, 1.0f + origin.getY() + vialHeight * 0.5f,	origin.getX() + 22.0f, 1.0f + origin.getY() + vialHeight * 0.5f,	2.0f);
	g.drawLine(origin.getX() + 15.0f, 1.0f + origin.getY() + vialHeight * 0.75f,	origin.getX() + 22.0f, 1.0f + origin.getY() + vialHeight * 0.75f,	2.0f);
	g.setColour(juce::Colours::white);
	g.drawLine(origin.getX() + 15.0f, origin.getY() + vialHeight * 0.25f,	origin.getX() + 22.0f,	origin.getY() + vialHeight * 0.25f,	1.0f);
	g.drawLine(origin.getX() + 10.0f, origin.getY() + vialHeight * 0.5f,	origin.getX() + 22.0f,	origin.getY() + vialHeight * 0.5f,	1.0f);
	g.drawLine(origin.getX() + 15.0f, origin.getY() + vialHeight * 0.75f,	origin.getX() + 22.0f,	origin.getY() + vialHeight * 0.75f,	1.0f);
	g.drawRect(vialRect);
}

void GameRenderer::DrawBombs(juce::Point<int> p, juce::Graphics& g)
{
	Board* board(Controller::GetInstance()->GetBoard());
	int halfTile = m_tileSize / 2;

	for (int i = 0; i < Board::MAX_NUM_BOMBS; i++)
	{
		// Draw the bombs that are still available
		if (i < board->GetNumBombs())
		{
			g.setColour(juce::Colours::red);
			g.fillEllipse(juce::Rectangle<int>(p.getX() + i * (m_tileSize - 1), p.getY(), halfTile, halfTile).toFloat());
		}

		// Draw the one used up bomb, which will soon become available again.
		else if (i == board->GetNumBombs())
		{
			g.setColour(juce::Colour(static_cast<juce::uint8>(67 + board->GetPercentUntilFreeBomb()), 67, 67));
			g.fillEllipse(juce::Rectangle<int>(p.getX() + i * (m_tileSize - 1), p.getY(), halfTile, halfTile).toFloat());
		}

		g.setColour(juce::Colours::white);
		g.drawEllipse(juce::Rectangle<int>(p.getX() + i * (m_tileSize - 1), p.getY(), halfTile, halfTile).toFloat(), 1.0f);
	}
}

void GameRenderer::DrawFastForwardButton(juce::Graphics& g)
{
	float radius = Layout::FF_ICON_RADIUS;
	juce::Point<float> origin(m_fastForwardButtonRect.getX() + Layout::FF_ICON_CENTER_X, m_fastForwardButtonRect.getY() + Layout::FF_ICON_CENTER_Y);

	juce::Path ffwdPath;
	ffwdPath.addPolygon(juce::Point<float>(static_cast<float>(origin.getX()), static_cast<float>(origin.getY())), 3, radius, Layout::FF_ICON_ROTATION);
	ffwdPath.addPolygon(juce::Point<float>(origin.getX() + Layout::FF_ICON_TRIANGLE_SPACING, static_cast<float>(origin.getY())), 3, radius, Layout::FF_ICON_ROTATION);

	float thickness = 1.5f;
	juce::Colour iconColour(juce::Colours::grey);
	juce::Colour frameColour(juce::Colour(27, 27, 27));
	if (Controller::GetInstance()->GetFastForward())
	{
		thickness = 2.5f;
		iconColour = juce::Colours::red;
		frameColour = juce::Colours::black;
	}

	// Draw ff icon (two little triangles)
	g.setColour(iconColour);
	g.fillPath(ffwdPath);
	g.setColour(juce::Colours::white);
	g.strokePath(ffwdPath, juce::PathStrokeType(thickness, juce::PathStrokeType::curved));

	// Frame around button
	g.setColour(frameColour);
	g.drawRect(m_fastForwardButtonRect.toFloat(), thickness);
}
