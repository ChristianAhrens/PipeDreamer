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


#include "AudioManager.h"


// ---- AudioManager class implementation ----

AudioManager::AudioManager()
	: m_audioThread(*this)
{
	Init();
}

AudioManager::~AudioManager()
{
	Shutdown();
}

void AudioManager::QueueSound(SoundID soundID)
{
	m_audioThread.QueueSound(soundID);
}

void AudioManager::Init()
{
	// Enable support for WAV files and other common formats.
	juce::AudioFormatManager audioFormatManager;
	audioFormatManager.registerBasicFormats();

	// Stereo output.
	m_audioDeviceManager.initialiseWithDefaultDevices(0, 2);

	// Registers audio callback to be used.
	m_audioDeviceManager.addAudioCallback(&m_audioPlayer);

	juce::AudioIODevice* audioDevice = m_audioDeviceManager.getCurrentAudioDevice();
	if (audioDevice)
	{
		for (int i = SOUND_CLICK; i < SoundID::SOUND_MAX; i++)
		{
			std::unique_ptr<juce::InputStream> inputStream;

			SoundID sId = static_cast<SoundID>(i);
			switch (sId)
			{
				case SOUND_CLICK:
					inputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::tap_wav, BinaryData::tap_wavSize, true);
					break;
				case SOUND_EXPLODE:
					inputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::explode_wav, BinaryData::explode_wavSize, true);
					break;
				case SOUND_NOTIFY:
					inputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::notify_wav, BinaryData::notify_wavSize, true);
					break;
				case SOUND_LEVEL_UP:
					inputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::win_wav, BinaryData::win_wavSize, true);
					break;
				case SOUND_GAME_OVER:
					inputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::lose_wav, BinaryData::lose_wavSize, true);
					break;
				default:
					break;
			}

			// Create a new AudioSource which gets its data from the binary stream above.
			SoundSource source(new juce::AudioFormatReaderSource(audioFormatManager.createReaderFor(std::move(inputStream)), true));

			// Store this source in a map to be easily found later, in PlaySound().
			m_soundSources.insert(std::make_pair(sId, std::move(source)));
		}

		// Audio is to be mixed by m_audioMixer and passed on to
		// the AudioSourcePlayer, which then streams it to the AudioIODevice.
		m_audioPlayer.setSource(&m_audioMixer);

		// Start the AudioThread, which will lay dormant
		// until woken up by calls to QueueSound().
		m_audioThread.startThread();
	}
}

void AudioManager::Shutdown()
{
	// Release resources held by each sound source.
	for (auto& entry : m_soundSources)
		entry.second->releaseResources();

	m_audioMixer.releaseResources();
	m_audioMixer.removeAllInputs();

	// Exception in CriticalSection::enter() without this.
	m_audioPlayer.setSource(nullptr);

	// Attempts to stop the thread running. The threadShouldExit() method will return true,
	// and notify() will be called in case the thread is currently waiting.
	m_audioThread.stopThread(2000);
}

void AudioManager::PlaySound(SoundID soundID)
{
	juce::AudioIODevice* audioDevice = m_audioDeviceManager.getCurrentAudioDevice();
	if (audioDevice && (m_soundSources.count(soundID) != 0))
	{
		// Set playhead back to the start of the sample,
		// and add it to the mixer if not already added before.
		m_soundSources.at(soundID)->setNextReadPosition(0);
		m_audioMixer.addInputSource(m_soundSources.at(soundID).get(), false);
	}
}


// ---- AudioThread class implementation ----

AudioManager::AudioThread::AudioThread(AudioManager& owner)
	: juce::Thread("AudioThread"),
	  m_owner(owner)
{
}

void AudioManager::AudioThread::run()
{
	while (!threadShouldExit())
	{
		if (m_soundID != SOUND_NONE)
		{
			m_owner.PlaySound(m_soundID);
			m_soundID = SOUND_NONE;
		}

		// Stop the AudioThread until another thread calls notify().
		wait(-1);
	}
}

void AudioManager::AudioThread::QueueSound(SoundID soundID)
{
	// Wake up the AudioThread and let it know which sound to play.
	m_soundID = soundID;
	notify();
}
