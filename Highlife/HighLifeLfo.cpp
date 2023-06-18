/*-
 * Copyright (c) discoDSP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by discoDSP
 *        http://www.discodsp.com/ and contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HighLife Lfo Implementation                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "HighLifeDefines.h"
#include ".\highlifelfo.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeLfo::CHighLifeLfo(void)
{
	// reset phase
	x=0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeLfo::~CHighLifeLfo(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeLfo::trigger(float const phase)
{
	// set phase or free phase
	if(phase<0.95f)
		x=phase*2.0f-1.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLifeLfo::work(float const speed)
{
	// store result
	float const f_result=pseudo_sine(x);

	// iterate angle increment
	x+=speed;

	// phase wrap
	if(x>=1.0)
		x-=2.0f;

	// return result
	return f_result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLifeLfo::get_lfo_delta(float const f_tempo,float const f_sample_rate,int const sync,float const f_mod_lfo_rat)
{
	// delta storer
	float f_lfo_delta=0.0f;

	// lfo sync
	if(sync)
	{	
		int const i_num_ticks=int(1.0f+(1.0f-f_mod_lfo_rat)*255.0f);
		float const f_tic_sam=(15.0f*f_sample_rate)/f_tempo;
		f_lfo_delta=2.0f/(float(i_num_ticks)*f_tic_sam);
	}
	else
	{
		float const f_scale=44100.0f/f_sample_rate;
		f_lfo_delta=get_lfo_rate(f_mod_lfo_rat)*f_scale;
	}

	return f_lfo_delta;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeLfo::render(float* plfobuf,int const num_samples,float const f_tempo,float const f_sample_rate,int const sync,float const f_mod_lfo_rat)
{
	// constants
	float const f_lfo_delta=get_lfo_delta(f_tempo,f_sample_rate,sync,f_mod_lfo_rat);

	// lfo render
	for(int s=0;s<num_samples;s++)
	{
		plfobuf[s]=pseudo_sine(x);
		
		x+=f_lfo_delta;

		if(x>=1.0f)
			x-=2.0f;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLifeLfo::pseudo_sine(float x)
{
	// Compute 2*(x^2-1.0)^2-1.0
	x*=x; 
	x-=1.0f;
	x*=x;

	// The following lines modify the range to lie between -1.0 and 1.0.
	// If a range of between 0.0 and 1.0 is acceptable or preferable
	// (as in a modulated delay line) then you can save some cycles.
	x*=2.0f;
	x-=1.0f;

	// return x
	return x;
}
