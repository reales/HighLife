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

#include ".\ddspenvelope.h"

CDDSPEnvelope::CDDSPEnvelope(void)
{
	stage=0;
}

CDDSPEnvelope::~CDDSPEnvelope(void)
{
}

void CDDSPEnvelope::Trigger(void)
{
	stage=1;
	accum=0.0;
}

void CDDSPEnvelope::Release(void)
{
	if(stage && stage!=6)
	{
		// release during attack delay
		if(stage==1)
		{
			accum=0.0;
			stage=0;
			return;
		}

		// release during hold
		if(stage==3)
			accum=1.0;

		stage=6;
	}
}

double CDDSPEnvelope::Run(double const delay_s,double const attack_s,double const hold_s,double const decay_s,double const sustain_l,double const release_s,double const sample_rate)
{
	// get inverse sr
	double const inv_sr=1.0/sample_rate;

	// off
	if(stage==0)
	{
		accum=0.0;
		return 0.0;
	}

	// delay stage
	if(stage==1)
	{
		accum+=inv_sr/delay_s;

		if(accum>=1.0)
		{
			accum=0.0;
			stage=2;
		}

		return 0.0;
	}

	// attack stage
	if(stage==2)
	{
		accum+=inv_sr/attack_s;

		if(accum>=1.0)
		{
			accum=0.0;
			stage=3;
			return 1.0;
		}

		return accum;
	}

	// hold stage
	if(stage==3)
	{
		accum+=inv_sr/hold_s;

		if(accum>=1.0)
		{
			accum=1.0;
			stage=4;
		}

		return 1.0;
	}

	// decay
	if(stage==4)
	{
		double const decay_distance=(1.0-sustain_l);
		accum-=(inv_sr/decay_s)*decay_distance;

		if(accum<=sustain_l)
		{
			accum=sustain_l;
			stage=5;
		}

		return accum;
	}

	// sustain
	if(stage==5)
	{
		accum=sustain_l;
		return sustain_l;
	}

	// release
	if(stage==6)
	{
		accum-=(inv_sr/release_s)*sustain_l;

		if(accum<=0.0)
		{
			accum=0.0;
			stage=0;
		}

		return accum;
	}

	// unhandled
	return 0.0;
}
	