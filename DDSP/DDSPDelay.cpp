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

#include ".\ddspdelay.h"
#include "stdlib.h"

CDDSPDelay::CDDSPDelay(void)
{
}

CDDSPDelay::CDDSPDelay(int const max_buffer)
{
	// counter to 0
	buffer_counter=0;

	// create buffer delay
	buffer_dat=new double[max_buffer];

	// flush buffer
	for(int s=0;s<max_buffer;s++)
		buffer_dat[s]=0.0;
	
	// set new delay size
	buffer_size=max_buffer;
}

CDDSPDelay::~CDDSPDelay(void)
{
	delete[] buffer_dat;
}

double CDDSPDelay::Run(double const input,double const delay,double const feedback,double const mod_speed,double const sample_rate,int const interpolation_sinc_taps,int const interpolation_mode)
{
	double const delayed_pos=double(buffer_counter)-delay*sample_rate;

	double const output=into.Interpolate(delayed_pos,buffer_dat,mod_speed,interpolation_sinc_taps,interpolation_mode);
	
	double const antidenormal_noise=double(rand())*1.0e-14;

	buffer_dat[buffer_counter]=filt.Run(input+output*feedback)+antidenormal_noise;

	if(++buffer_counter>=buffer_size)
		buffer_counter=0;

	return output;
}

void CDDSPDelay::UpdateFeedbackFilterCoeffs(int const type,double const frequency,double const sample_rate,double const q,double const db_gain,bool q_is_bandwidth)
{
	filt.UpdateFilterCoeffs(type,frequency,sample_rate,q,db_gain,false);
}
