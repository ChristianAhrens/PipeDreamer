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


#include "QueueComponent.h"
#include "BoardComponent.h"
#include "Queue.h"
#include "Controller.h"


// ---- QueueComponent class implementation ----

QueueComponent::QueueComponent()
{
}

void QueueComponent::paint(juce::Graphics& g)
{
	Queue* queue = Controller::GetInstance()->GetQueue();
	int queueSize = queue->GetSize();

	// Bounds are set to the exact drawn pixel area: (N*T - (N-1)) in the tile direction.
	// Recover T with the inverse formula (width + N - 1) / N.
	bool horizontal = (getWidth() > getHeight());
	int T = horizontal ? std::min((getWidth() + queueSize - 1) / queueSize, getHeight())
	                   : std::min(getWidth(), (getHeight() + queueSize - 1) / queueSize);

	for (int i = 0; i < queueSize; i++)
	{
		juce::Point<int> p;
		if (horizontal)
		{
			// tile[0] (next to place) at left; tile[queueSize-1] at right.
			p = juce::Point<int>(i * (T - 1), 0);
		}
		else
		{
			// tile[0] (next to place) at bottom; tile[queueSize-1] at top.
			p = juce::Point<int>(0, (queueSize - 1 - i) * (T - 1));
		}

		BoardComponent::DrawTile(queue->GetTile(i), p, T, g);
		BoardComponent::DrawCrossSecondWay(queue->GetTile(i), p, T, g);
		BoardComponent::DrawTileDecoration(queue->GetTile(i), p, T, g);

		if (i == 0)
		{
			// Highlight frame around the next-to-place tile.
			g.setColour(juce::Colours::limegreen);
			g.drawRect(p.getX() - 4, p.getY() - 4, T + 8, T + 8, 2);
			g.setColour(juce::Colours::black);
			g.drawRect(p.getX() - 6, p.getY() - 6, T + 12, T + 12, 2);
		}
	}
}
