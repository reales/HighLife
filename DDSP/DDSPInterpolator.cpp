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

#include ".\ddspinterpolator.h"
#include "math.h"

CDDSPInterpolator::CDDSPInterpolator(void)
{
}

CDDSPInterpolator::~CDDSPInterpolator(void)
{
}

double CDDSPInterpolator::InterpolateNearest(double const phase,double* psamples)
{
	// get integer and fractional part
	int const integer_part=int(phase);
	double const fractional_part=phase-integer_part;
	
	// return next
	if(fractional_part>0.5)
		return psamples[integer_part+1];

	// return floor
	return psamples[integer_part];
}

double CDDSPInterpolator::InterpolateLinear(double const phase,double* psamples)
{
	// get integer and fractional part
	int const integer_part=int(phase);
	double const fractional_part=phase-integer_part;

	// get samples
	double const a=psamples[integer_part+0];
	double const b=psamples[integer_part+1];
	
	// compute linear interpolation and send result
	return a+(b-a)*fractional_part;
}

double CDDSPInterpolator::InterpolateHermite(double const phase,double* psamples)
{
	// get integer and fractional part
	int const integer_part=int(phase);
	double const fractional_part=phase-integer_part;

	// get samples
	double const xm1=psamples[integer_part-1];
	double const x_0=psamples[integer_part+0];
	double const x_1=psamples[integer_part+1];
	double const x_2=psamples[integer_part+2];
	
	// compute hermite
	double const c=(x_1-xm1)*0.5;
	double const v=x_0-x_1;
	double const w=c+v;
	double const a=w+v+(x_2-x_0)*0.5;
	double const b=w+a;
	
	// send result
	return ((((a*fractional_part)-b)*fractional_part+c)*fractional_part+x_0);
}

double CDDSPInterpolator::InterpolateSinc(double const phase,double* psamples,double const phase_speed,int const num_taps)
{
	// get integer and fractional part
	int const integer_part=int(phase);
	double fractional_part=phase-integer_part;

	// pi constant
	double const k_pi=atan(1.0)*4.0;

	// mix accumulator
	double mix=0.0;

	// avoid sinc fractional part division by 0
	if(fractional_part==0.0)
		fractional_part=1.0e-25;

	// get absolute phase speed
	double phase_speed_absolute=fabs(phase_speed);

	// avoid fractional part division by 0
	if(phase_speed_absolute==0.0)
		phase_speed_absolute=1.0e-25;

	// compute antialiasing filtering coefficient
	double c_cutoff=1.0/phase_speed_absolute;

	// clamp cutoff
	if(c_cutoff>1.0)
		c_cutoff=1.0;

	// FIR convolution
	for(int s=-num_taps;s<=num_taps;s++)
	{
		// get x pos
		double const x=double(s)-fractional_part;
		
		// window value
		double const k_window=0.5+0.5*cos(x/double(num_taps)*k_pi);
		
		// sinc value
		double const k_sinc=sin(c_cutoff*k_pi*x)/(k_pi*x);

		// add mix convolution
		mix+=psamples[integer_part+s]*k_sinc*k_window;
	}

	// return
	return mix;
}

double CDDSPInterpolator::Interpolate(double const phase,double* psamples,double const phase_speed,int const num_taps,int const mode)
{
	switch(mode)
	{
	case 0:
		return InterpolateNearest(phase,psamples);
		break;

	case 1:
		return InterpolateLinear(phase,psamples);
		break;

	case 2:
		return InterpolateHermite(phase,psamples);
		break;
	
	case 3:
		return InterpolateSinc(phase,psamples,phase_speed,num_taps);
		break;
	};

	return 0.0;
}
