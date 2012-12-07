/*
  ==============================================================================

    FastFourierTransformer.cpp
    Created: 3 Aug 2012 8:13:37am
    Author:  Martin Klang

  ==============================================================================
*/

#include "fftw3.h"
#include "FastFourierTransformer.h"

//--------------------------------------------------------------

// simple fft class implementation

//--------------------------------------------------------------

// constructor

FastFourierTransformer::FastFourierTransformer (int bufSize) {
	
	data          = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufSize);
	fft_result    = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufSize);
	ifft_result   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufSize);
	
	plan_forward  = fftw_plan_dft_1d(bufSize, data, fft_result, FFTW_FORWARD, FFTW_MEASURE);
	plan_backward = fftw_plan_dft_1d(bufSize, data, ifft_result, FFTW_BACKWARD, FFTW_MEASURE);	
	
}

//-------------------------------------------------------------

// destructor

FastFourierTransformer::~FastFourierTransformer() {
	
	fftw_free(data);
	fftw_free(fft_result);
	fftw_free(ifft_result);
		
	fftw_destroy_plan(plan_forward);	
	fftw_destroy_plan(plan_backward);
	
}

//-------------------------------------------------------------

//cartesian to polar conversion functions

double FastFourierTransformer::cartopolRadius (double x, double y) {
	
	return sqrt(y * y + x * x);	
}

//--------------------------------------------------------------

double FastFourierTransformer::cartopolAngle (double x, double y)  { 
	
    if (x > 0) { return atan(y/x); }
	if (x < 0 && y >= 0) {return atan(y/x) + M_PI; }
	if (x < 0 && y < 0) {return atan(y/x) - M_PI; }
	if (y > 0) { return M_PI / 2; }
	if (y < 0) { return -M_PI / 2; }
	
	return 0;	
}

//--------------------------------------------------------------

//polar to cartesian conversion functions

double FastFourierTransformer::poltocarX (double angle, double radius) { 
	
	return cos(angle) * radius;
}

//--------------------------------------------------------------

double FastFourierTransformer::poltocarY (double angle, double radius) {
	
	return sin(angle) * radius;	
}


//--------------------------------------------------------------

// fft conversion

void FastFourierTransformer::processForward (float* channelData, fftw_complex* fftData, int bufSize) {
	
	for(i = 0; i < bufSize; i++) {
		
		data[i][0] = channelData[i];       // stick your audio samples in here   
		data[i][1] = 0.0;                  // use this if your data is complex valued
	}
		
	fftw_execute(plan_forward);
	
	for (i = 0; i < bufSize; i++) {
		
		fftData[i][0] = fft_result[i][0];
		fftData[i][1] = fft_result[i][1];				
	}	
}

//--------------------------------------------------------------

// inverse fft conversion

void FastFourierTransformer::processBackward (fftw_complex* fftData, float* channelData, int bufSize) {
			
	for(i = 0; i < bufSize; i++) {
		
		data[i][0] = fftData[i][0];        // stick your fft data in here!
		data[i][1] = 0.0;        // use this if your data is complex valued
	}	
	
	fftw_execute(plan_backward);
		
	for(i = 0; i < bufSize; i++) {
	
		channelData[i] = ifft_result[i][0] / bufSize; 
		
		//it was distorting because we weren't dividing ifft_result[0] by bufSize - see demo code	
	}		
}

//--------------------------------------------------------------

// fft gain control 

void FastFourierTransformer::gainFFT (fftw_complex* fftData, float oldGain, float gainStep, int bufSize) {
		
	double Pmagnitude[bufSize];		//local arrays for storing polar coordinates
	double Pangle[bufSize];
		
	for(int i = 0; i < bufSize; i++) {
		
		//cartesian to polar conversion
		
		Pmagnitude[i] = this->cartopolRadius(fftData[i][0], fftData[i][1]);
		Pangle[i]	  =	this->cartopolAngle(fftData[i][0], fftData[i][1]);
		
		//change polar magnitude
		
		oldGain += gainStep;
		Pmagnitude[i] = Pmagnitude[i] * oldGain ;	
		
		//polar to cartesian conversion
		
		fftData[i][0] = this->poltocarX(Pangle[i], Pmagnitude[i]);
		fftData[i][1] = this->poltocarY(Pangle[i], Pmagnitude[i]);
		
	}		
}

//--------------------------------------------------------------

// fft convolution 

void FastFourierTransformer::compMult (fftw_complex* fftData_A, fftw_complex* fftData_B, fftw_complex* fftData, int bufSize) {
	
	for (int i = 0; i < bufSize; i++) {
		
		fftData[i][0] = fftData_A[i][0] * fftData_B[i][0] - fftData_A[i][1] * fftData_B[i][1];
		fftData[i][1] = fftData_A[i][0] * fftData_B[i][1] + fftData_A[i][1] * fftData_B[i][1];
	
		
		}

}
 

//--------------------------------------------------------------


//==============================================================================


/*  Demo code

void transform(){

int SIZE = 4;

fftw_complex    *data, *fft_result, *ifft_result;
fftw_plan       plan_forward, plan_backward;
int             i;

data        = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SIZE);
fft_result  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SIZE);
ifft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * SIZE);

plan_forward  = fftw_plan_dft_1d(SIZE, data, fft_result,
                                 FFTW_FORWARD, FFTW_ESTIMATE);
plan_backward = fftw_plan_dft_1d(SIZE, fft_result, ifft_result,
                                 FFTW_BACKWARD, FFTW_ESTIMATE);


for( i = 0 ; i < SIZE ; i++ ) {
    data[i][0] = 1.0; // stick your audio samples in here
    data[i][1] = 0.0; // use this if your data is complex valued
}


for( i = 0 ; i < SIZE ; i++ ) {
    fprintf( stdout, "data[%d] = { %2.2f, %2.2f }\n",
			i, data[i][0], data[i][1] );
}

fftw_execute( plan_forward );


for( i = 0 ; i < SIZE ; i++ ) {
    fprintf( stdout, "fft_result[%d] = { %2.2f, %2.2f }\n",
			i, fft_result[i][0], fft_result[i][1] );
}

fftw_execute( plan_backward );


for( i = 0 ; i < SIZE ; i++ ) {
    fprintf( stdout, "ifft_result[%d] = { %2.2f, %2.2f }\n",
			i, ifft_result[i][0] / SIZE, ifft_result[i][1] / SIZE );
}


fftw_destroy_plan( plan_forward );
fftw_destroy_plan( plan_backward );

fftw_free( data );
fftw_free( fft_result );
fftw_free( ifft_result );
}

*/