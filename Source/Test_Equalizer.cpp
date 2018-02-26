/*
==============================================================================

Test_Equalizer.cpp

==============================================================================
*/

#include "catch.hpp"

#include "Equalizer.h"
#include "Test_Util.h"
#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"


/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/


TEST_CASE("Equalizer class is tested", "[equalizer]") {
	//==============================================================================
	/**
	* Processor preparation
	*/
	constexpr int sampleRate = 44100;
	constexpr int channelNumber = 1;
	constexpr std::chrono::milliseconds BLOCK_DURATION_MS(20);
	constexpr double NUM_SAMPLES_PER_BLOCK = (BLOCK_DURATION_MS.count() / 1000.0) * sampleRate;


	reverb::AudioProcessor processor;
	processor.setPlayConfigDetails(channelNumber, channelNumber, sampleRate, std::ceil(NUM_SAMPLES_PER_BLOCK));

	REQUIRE(processor.getSampleRate() == sampleRate);

	//==============================================================================
	/**
	* Unit impulse construction
	*/

	int numSamples = 44100;

	juce::AudioBuffer<float> sampleBuffer(channelNumber, numSamples);

	float * const buffer = sampleBuffer.getWritePointer(0);

	memset(buffer, 0, sampleBuffer.getNumSamples() * sizeof(*buffer));

	buffer[0] = 1.0f;



	//==============================================================================
	/**
	* FFT Preparation
	*/

	//Prepare a buffer of proper size for FFT

	// compute the next highest power of 2 of sample number
	int order = std::ceil((log(sampleBuffer.getNumSamples()) / log(2)));
	juce::dsp::FFT forwardFFT(order);
	float * fftBuffer = new float[2 * forwardFFT.getSize()];

	memset(fftBuffer, 0, 2 * forwardFFT.getSize() * sizeof(*fftBuffer));

	//Compute frequency resolution
	float freqRes = (float)sampleRate / (float)forwardFFT.getSize();


	SECTION("Testing equalizer") {

		reverb::Equalizer EQ(&processor);

		EQ.exec(sampleBuffer);

		memcpy(fftBuffer, sampleBuffer.getReadPointer(0), sampleBuffer.getNumSamples() * sizeof(*sampleBuffer.getReadPointer(0)));
		forwardFFT.performFrequencyOnlyForwardTransform(fftBuffer);



		REQUIRE(compareValues(reverb::Filter::todB(fftBuffer[(int)(1000 / freqRes)]), EQ.getdBAmplitude(1000)));

	}
}