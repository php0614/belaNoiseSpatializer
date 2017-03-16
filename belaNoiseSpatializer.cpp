#include <Bela.h>
#include <cmath>


float gFrequency = 3.0;
float audioFrequency = 600.0;
float audioPhase;
float gInverseSampleRate;
int gAudioFramesPerAnalogFrame;

float linCongVal = 0.0;
bool linCongInitFlag = false;
float lcMul = 1.04, lcAdd = 0.1, lcMod = 1;

float panX = 0.5;
float panY = 0.5;
bool button = false;

float amp0 = 0.5;
float amp1 = 0.5;
float amp2 = 0.5;
float amp3 = 0.5;

float pOut = 0;
float ppOut = 0;

bool soundOnFlag = false;


float linCong(float mul, float add, float mod){
	if(!linCongInitFlag){
		linCongInitFlag = true;
		return(0.0);
	}else{
		linCongVal = fmod(mul*linCongVal + add, mod);
		return(linCongVal);
	}
}

bool setup(BelaContext *context, void *userData)
{

	// Check if analog channels are enabled
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return false;
	}

	// Check that we have the same number of inputs and outputs.
	if(context->audioInChannels != context->audioOutChannels ||
			context->analogInChannels != context-> analogOutChannels){
		printf("Error: for this project, you need the same number of input and output channels.\n");
		return false;
	}

	gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;

	gInverseSampleRate = 1.0 / context->analogSampleRate;
	audioPhase = 0.0;

	return true;
}

void render(BelaContext *context, void *userData)
{
		if(button){
			lcMul = lcMul+((pOut+ppOut)/20);
			lcAdd = lcAdd+(pOut/10);
			lcMod = lcMod+(ppOut/10);
			
			soundOnFlag = true;
		}
		
		if(lcMul > 1.7 || lcMul <1.01)
			lcMul = 1.02;
			
		if(lcAdd > 1.0 || lcAdd <0.01)
			lcAdd = 0.05;
			
		if(lcMod > 1.0 || lcAdd <0.5)
			lcAdd = 1.0;
			
			
	for(unsigned int n = 0; n < context->analogFrames; n++) {

		if(!(n % gAudioFramesPerAnalogFrame)) {
			panX = analogRead(context, n/gAudioFramesPerAnalogFrame, 0);
			panY = analogRead(context, n/gAudioFramesPerAnalogFrame, 1);
			if(analogRead(context, n/gAudioFramesPerAnalogFrame, 2) > 0.75){
				button = true;
			} else {
				button = false;
			}
		}


		for(unsigned int channel = 0; channel < 4; channel++) {
			float ledBrightness = 0;

			if(channel == 0){
				amp0 = ledBrightness = (((0.5 - fabs(0.5-panX)) + (1-panY)) /1.5); 
		}
			if(channel == 1){
				amp3 = ledBrightness = ((1-panX) + (0.5-fabs(0.5-panY)) /1.5); 
		}
			if(channel == 2){
				amp1 = ledBrightness = (((0.5 - fabs(0.5-panX)) + (1.0-fabs(1.0-panY))) /1.5); 
		}
			if(channel == 3){
				amp2 = ledBrightness = ((1-fabs(1.0-panX)) + (0.5 - fabs(0.5-panY)) /1.5); 
		}
			analogWrite(context, n, channel, ledBrightness);
		}
	}
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float out = (linCong(lcMul, lcAdd, lcMod) -0.5) * 2.0 ;
		
		if(soundOnFlag){
		audioWrite(context, n, 0, out * 0.5*amp0);
		audioWrite(context, n, 1, out * 0.5*amp1);
		
		analogWrite(context, n, 6, (out+1.0) *0.46 * 2.0 * amp2);
		analogWrite(context, n, 7, (out+1.0) *0.46 * 2.0 * amp3);
		}
		
		audioPhase += 2.0 * M_PI * audioFrequency * gInverseSampleRate;
		if(audioPhase > 2.0 * M_PI)
			audioPhase -= 2.0 * M_PI;
	
	ppOut = pOut;
	pOut = out;
	}
}

void cleanup(BelaContext *context, void *userData)
{

}