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

#include ".\ddsposcillator.h"
#include "math.h"

CDDSPOscillator::CDDSPOscillator(void)
{
	// define constant
	k_osc_2pi=atan(1.0)*8.0;

	// reset phase
	phase=0.0;
	phase_speed=0.0;
}

CDDSPOscillator::~CDDSPOscillator(void)
{
}

void CDDSPOscillator::SetRate(double const frequency,double const sample_rate)
{
	phase_speed=(k_osc_2pi*frequency)/sample_rate;
}

void CDDSPOscillator::SetPhase(double const radians)
{
	phase=radians;
	PhaseWrap();
}

inline double CDDSPOscillator::Run(int const waveform)
{
	// output accumulator
	double result=0.0;

	// select waveform
	switch(waveform)
	{
	case 0: // sine
		result=sin(phase);
		break;
	case 1: // cosine
		result=cos(phase);
		break;
	}

	// phase increment
	phase+=phase_speed;

	// wrap phase
	PhaseWrap();

	// return sample
	return result;
}

void CDDSPOscillator::PhaseWrap(void)
{
	// positive phase wrap
	while(phase>=k_osc_2pi)
		phase-=k_osc_2pi;

	// negative phase wrap
	while(phase<0.0)
		phase+=k_osc_2pi;
}
