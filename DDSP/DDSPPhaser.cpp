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

#include ".\ddspphaser.h"
#include "stdlib.h"
#include "math.h"

CDDSPPhaser::CDDSPPhaser(void)
{
	//initialise accum vars
    _lfoPhase=0.0;
	_zm1=0.0;

	// useful range
	UpdateCoeffs(880.0,4400.0,0.5,0.7,1.0,44100.0);
}

CDDSPPhaser::~CDDSPPhaser(void)
{
}

void CDDSPPhaser::UpdateCoeffs(double const fMin,double const fMax,double const lfoRate,double const Feedback,double const Depth,double const sample_rate)
{
	// pi
	double const k_pi=atan(1.0)*4.0;

	// hz
	_dmin=fMin/(sample_rate*0.5);
	_dmax=fMax/(sample_rate*0.5);

	// lfo
	_lfoInc=2.0*k_pi*(lfoRate/sample_rate);

	// feedback
	_fb=Feedback;

	// depth
	_depth=Depth;
}

double CDDSPPhaser::Run(double const inSamp)
{
	// pi
	double const k_2pi=atan(1.0)*8.0;

	//calculate and update phaser sweep lfo...
	double d=_dmin+(_dmax-_dmin)*((sin(_lfoPhase)+1.0)/2.0);

	// increment phase
	_lfoPhase+=_lfoInc;

	// positive wrap phase
	if(_lfoPhase>=k_2pi)
		_lfoPhase-=k_2pi;

	// antidenormal
	double const antidenormal_noise=double(rand())*1.0e-14;

	// get input
	double const input=inSamp+antidenormal_noise;

	// accum
	double y=input+_zm1*_fb;

	//update filter coeffs and calculate output
	for(int i=0;i<DDSP_PHASER_TAPS;i++)
	{
		_alps[i].SetDelay(d);
		y=_alps[i].Run(y);
	}
	
	// set feedback z
	_zm1=y;

	// return output
	return input+y*_depth;
}
