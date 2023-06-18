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
// HighLife Filter Implementation                                                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdlib.h"
#include ".\highlifefilter.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeFilter::CHighLifeFilter(void)
{
	clear_filter();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeFilter::~CHighLifeFilter(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeFilter::clear_filter(void)
{

	b0=b1=b2=b3=b4=0.0f;
	t1=t2=t3=0.0f;
	f=p=q=0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLifeFilter::work(float in,float const frequency,float const resonance,int const mode)
{
	// add antidenormalization noise
	in+=(float)rand()*1.0e-14f;

	// filter coeffs
	q=1.0f-frequency;
	p=frequency+0.8f*frequency*q;
	f=p+p-1.0f;
	q=0.89f*resonance*(1.0f+0.5f*q*(1.0f-q+5.6f*q*q)); 

	// filter work
	in-=q*b4;
	t1=b1;
	b1=(in+b0)*p-b1*f;
	t2=b2;
	b2=(b1+t1)*p-b2*f;
	t1=b3;
	b3=(b2+t2)*p-b3*f;
	b4=(b3+t1)*p-b4*f;

	// update
	b0=in;

	if(mode==1)return b4;				// lowpass output
	if(mode==2)return in-b4;			// hipass output
	if(mode==3)return 3.0f*(b3-b4);		// bandpass output
	if(mode==4)return 3.0f*(b3-b4)-in;	// band reject

	// return 0.0
	return 0.0f;
}
