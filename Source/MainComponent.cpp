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


#include "MainComponent.h"
#include "LayoutConstants.h"
#include "AudioManager.h"
#include "Controller.h"
#include "Board.h"
#include "Queue.h"
#include "TilePiece.h"
#include "ScoreWindow.h"


// ---- Class Implementation ----

MainComponent::MainComponent()
	:	m_blockInteraction(0)
{
	m_controller = std::make_unique<Controller>();

	// Create GUI component which will work as a clickable hyperlink to our github.
	m_hyperlink = std::make_unique<juce::HyperlinkButton>(	juce::String("https://github.com/escalonely/PipeDreamer"),
															juce::URL("https://github.com/escalonely/PipeDreamer"));
	m_hyperlink->setColour(juce::HyperlinkButton::textColourId, juce::Colours::grey);
	addAndMakeVisible(m_hyperlink.get());

	setSize(Layout::WINDOW_DEFAULT_W, Layout::WINDOW_DEFAULT_H);

	// Reset the countdown to the start of the round
	// (before ooze starts pumping out)
	m_countDown = m_controller->GetCurrentCountdown();

	// GUI-refresh rate
	startTimer(Layout::GUI_REFRESH_RATE);
}

MainComponent::~MainComponent()
{
}

int MainComponent::GetTileSize() const
{
	return m_renderer.GetTileSize();
}

void MainComponent::resized()
{
	m_renderer.SetLayout(getLocalBounds());

	// Position the hyperlink using the renderer's font computation.
	auto f = m_renderer.GetFont(GameRenderer::LABEL_VERSION);
	auto textWidth = juce::GlyphArrangement::getStringWidthInt(f, m_hyperlink->getButtonText());
	m_hyperlink->setFont(f, false /* do not resize */);
	m_hyperlink->setBounds(	getLocalBounds().getWidth() - (textWidth + 40),
							getLocalBounds().getHeight() - 50,
							textWidth + 40,
							40);

	// Resize the ScoreWindow, if any.
	if (m_scoreWindow)
		m_scoreWindow->resized();
}

void MainComponent::paint(juce::Graphics& g)
{
	m_renderer.Render(g, m_countDown);
}

void MainComponent::timerCallback()
{
	const juce::ScopedLock lock(m_lock);

	Controller* controller(m_controller.get());
	Controller::GameState state(controller->GetState());

	if (state == Controller::STATE_RUNNING)
	{
		// When it reaches 0, clicks are enabled again.
		if (m_blockInteraction > 0)
			m_blockInteraction--;

		// Countdown to start pumping ooze.
		if (m_countDown > 0)
		{
			// If fast-forward button is currently toggled on, decrease countdown faster.
			if (controller->GetFastForward())
				m_countDown -= 5;
			else
				m_countDown -= 1;
		}

		else
		{
			bool contained = controller->Pump();
			if (!contained)
			{
				// Ooze spill!
				// Give the player a moment to see where the spill took place,
				// before covering up the board with the score window.
				startTimer(2000);
			}
		}
	}

	else if (state == Controller::STATE_STOPPED)
	{
		// Stop refreshing GUI.
		stopTimer();

		// Position the small AdvanceWindow in the middle of the window,
		// while the HighScoreWindow will take up the whole window.
		Controller::ScoreDetails details(controller->GetScoreDetails());
		juce::Point<int> windowOrigin(0, 0);
		if (details.advance)
			windowOrigin = juce::Point<int>(getLocalBounds().getWidth() / 3, getLocalBounds().getHeight() / 4);

		// Show scoreboard overlay.
		m_scoreWindow.reset(ScoreWindow::CreateScoreWindow(details));
		m_scoreWindow->addChangeListener(this);
		addAndMakeVisible(m_scoreWindow.get());
		m_scoreWindow->setTopLeftPosition(windowOrigin);
		m_scoreWindow->resized();
	}

	this->repaint();
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
	const juce::ScopedLock lock(m_lock);

	Controller* controller(m_controller.get());

	if ((controller->GetState() == Controller::STATE_RUNNING) &&
		(m_blockInteraction == 0))
	{
		juce::Point<int> clickPos = event.getMouseDownPosition();

		// If user clicked on the fast-forward button, toggle fast-forward state.
		// This increases the ooze amount in GetCurrentOozePerPump().
		if (m_renderer.GetFastForwardButtonRect().contains(clickPos))
		{
			controller->SetFastForward(!controller->GetFastForward());
		}

		else
		{
			Board* board(controller->GetBoard());
			bool replace(false);
			int tileSize = m_renderer.GetTileSize();
			int boardHStartPos = static_cast<int>(getLocalBounds().getWidth() / Layout::BOARD_H_DIVISOR);
			int boardVStartPos = static_cast<int>(getLocalBounds().getHeight() / Layout::BOARD_V_DIVISOR);

			for (int i = 0; (i < board->GetNumCols()) && !replace; i++)
			{
				for (int j = 0; (j < board->GetNumRows()) && !replace; j++)
				{
					juce::Rectangle<int> tileRect(	boardHStartPos + i * (tileSize - 1),
													boardVStartPos + j * (tileSize - 1),
													tileSize, tileSize);
					if (tileRect.contains(clickPos))
					{
						// Default sound effect for placing pipes on the grid.
						AudioManager::SoundID soundID(AudioManager::SOUND_CLICK);

						TilePiece* clickedTile = board->GetTile(i, j);
						replace = (clickedTile->GetType() == TilePiece::TYPE_NONE);

						if (!replace)
						{
							Pipe* clickedPipe = dynamic_cast<Pipe*>(clickedTile);
							if ((clickedPipe != nullptr) &&
								(clickedPipe->IsEmpty()) &&		// Only empty tiles can be replaced.
								(!clickedPipe->IsStart()) &&	// Cannot replace starter tiles.
								(board->PopBomb()))				// Need bombs to replace existing pipe tiles.
							{
								replace = true;

								// Sound effect should be explosive instead.
								soundID = AudioManager::SOUND_EXPLODE;
							}
						}

						if (replace)
						{
							// Trigger appropriate sound effect.
							controller->QueueSound(soundID);

							// Grab the next piece in the queue, and...
							TilePiece::Type newType = controller->GetQueue()->Pop();

							// ... place it on the board.
							board->ReplaceTile(i, j, newType);

							// To prevent accidental double-clicking disable actions for a few frames.
							static constexpr int framesInteractionBlocked = 5;
							m_blockInteraction += framesInteractionBlocked;
						}
					}
				}
			}
		}
	}
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	(void)source;

	const juce::ScopedLock lock(m_lock);

	if (m_scoreWindow != nullptr)
	{
		switch (m_scoreWindow->GetCommand())
		{
			case Controller::CMD_RESTART:
			case Controller::CMD_CONTINUE:
				{
					m_controller->Reset(m_scoreWindow->GetCommand());

					// Countdown to ooze pumping.
					m_countDown = m_controller->GetCurrentCountdown();

					// Restart GUI
					m_blockInteraction = 0;
					startTimer(Layout::GUI_REFRESH_RATE);
				}
				break;

			case Controller::CMD_QUIT:
				{
					juce::JUCEApplicationBase::quit();
				}
				break;

			default:
				break;
		}

		// This deletes the unique_ptr.
		m_scoreWindow = nullptr;
	}
}
