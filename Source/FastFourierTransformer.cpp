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

//MIDI note to frequency conversion array

double mtofarray[129]={0, 8.661957, 9.177024, 9.722718, 10.3, 10.913383, 11.562325, 12.25, 12.978271, 13.75, 14.567617, 15.433853, 16.351599, 17.323914, 18.354048, 19.445436, 20.601723, 21.826765, 23.124651, 24.5, 25.956543, 27.5, 29.135235, 30.867706, 32.703197, 34.647827, 36.708096, 38.890873, 41.203445, 43.65353, 46.249302, 49., 51.913086, 55., 58.27047, 61.735413, 65.406395, 69.295654, 73.416191, 77.781746, 82.406891, 87.30706, 92.498604, 97.998856, 103.826172, 110., 116.540939, 123.470825, 130.81279, 138.591309, 146.832382, 155.563492, 164.813782, 174.61412, 184.997208, 195.997711, 207.652344, 220., 233.081879, 246.94165, 261.62558, 277.182617,293.664764, 311.126984, 329.627563, 349.228241, 369.994415, 391.995422, 415.304688, 440., 466.163757, 493.883301, 523.25116, 554.365234, 587.329529, 622.253967, 659.255127, 698.456482, 739.988831, 783.990845, 830.609375, 880., 932.327515, 987.766602, 1046.502319, 1108.730469, 1174.659058, 1244.507935, 1318.510254, 1396.912964, 1479.977661, 1567.981689, 1661.21875, 1760., 1864.655029, 1975.533203, 2093.004639, 2217.460938, 2349.318115, 2489.015869, 2637.020508, 2793.825928, 2959.955322, 3135.963379, 3322.4375, 3520., 3729.31, 3951.066406, 4186.009277, 4434.921875, 4698.63623, 4978.031738, 5274.041016, 5587.651855, 5919.910645, 6271.926758, 6644.875, 7040., 7458.620117, 7902.132812, 8372.018555, 8869.84375, 9397.272461, 9956.063477, 10548.082031, 11175.303711, 11839.821289, 12543.853516, 13289.75};

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

void FastFourierTransformer::processBackward (fftw_complex* fftData, int nfft, 
					      float* channelData, int bufSize) {
			
	for(i = 0; i < nfft; i++) {
		data[i][0] = fftData[i][0];        // stick your fft data in here!
		data[i][1] = fftData[i][1];        // use this if your data is complex valued
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

double convert::mtof(int midinote) {
	
	return mtofarray[midinote];
}

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
