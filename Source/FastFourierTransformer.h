/*
  ==============================================================================

    FastFourierTransformer.h
    Created: 3 Aug 2012 8:13:37am
    Author:  Martin Klang

  ==============================================================================
*/

#define USE_MATH_DEFINES

#include "fftw3.h"
#include "math.h"

#ifndef __FASTFOURIERTRANSFORMER_H_9F2E8A4F__
#define __FASTFOURIERTRANSFORMER_H_9F2E8A4F__

// fft class

class FastFourierTransformer {

// class methods
	
public:
	
	FastFourierTransformer (int bufSize);	//constructor
	~FastFourierTransformer();				//destructor
	
	//convert coordinates 
	
	double poltocarX	   (double angle, double radius);		
	double poltocarY	   (double angle, double radius);
	double cartopolAngle   (double Cx, double Cy);
	double cartopolRadius  (double Cx, double Cy);
	
	//fft conversion
	
	void processForward	   (float* channelData, fftw_complex* fftData, int bufSize);
	void processBackward   (fftw_complex* fftData, int nfft, float* channelData, int bufSize);
	
	//fft gain
	
	void gainFFT		   (fftw_complex* fftData, float oldGain, float gainStep, int bufSize);

	void compMult		   (fftw_complex* fftData_A, fftw_complex* fftData_B, 
											fftw_complex* fftData, int bufSize);

	
private:
	
	// member variables
	
	fftw_complex    *data, *fft_result, *ifft_result;	
	fftw_plan       plan_forward, plan_backward;
	double			Pangle[], Pmagnitude[];
	int             i;
	
};

class convert {
public:
	double mtof(int midinote);
};

#endif  // __FASTFOURIERTRANSFORMER_H_9F2E8A4F__
