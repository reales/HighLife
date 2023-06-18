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

#include ".\ddspbiquadfilter.h"
#include "math.h"
#include "stdlib.h"

CDDSPBiquadFilter::CDDSPBiquadFilter(void)
{
	// flush IIR
	Flush();

	// set default filter
	UpdateFilterCoeffs(0,20000,44100,0.1,0.0,false);
}

CDDSPBiquadFilter::~CDDSPBiquadFilter(void)
{
}

void CDDSPBiquadFilter::Flush(void)
{
	// reset filter coeffs
	b0a0=b1a0=b2a0=a1a0=a2a0=0.0;

	// reset in/out history
	ou1=ou2=in1=in2=0.0;
}

double CDDSPBiquadFilter::Run(double const input)
{
	// antidenormal
	double const antidenormal_noise=double(rand())*1.0e-14;
	
	// set in0
	double const in0=input+antidenormal_noise;
	
	// filter
	double const yn=b0a0*in0+b1a0*in1+b2a0*in2-a1a0*ou1-a2a0*ou2;

	// push in/out buffers
	in2=in1;
	in1=in0;
	ou2=ou1;
	ou1=yn;

	// return output
	return yn;
}

void CDDSPBiquadFilter::UpdateFilterCoeffs(int const type,double const frequency,double const sample_rate,double const q,double const db_gain,bool q_is_bandwidth)
{
	// temp pi
	double const k_pi=atan(1.0)*4.0;

	// temp coef vars
	double alpha,a0,a1,a2,b0,b1,b2;

	// peaking, lowshelf and hishelf
	if(type>=6)
	{
		double const A		=	pow(10.0,(db_gain/40.0));
		double const omega	=	2.0*k_pi*frequency/sample_rate;
		double const tsin	=	sin(omega);
		double const tcos	=	cos(omega);

		if(q_is_bandwidth)
			alpha=tsin*sinh(log(2.0)/2.0*q*omega/tsin);
		else
			alpha=tsin/(2.0*q);

		double const beta	=	sqrt(A)/q;

		// peaking
		if(type==6)
		{
			b0=1.0+alpha*A;
			b1=-2.0*tcos;
			b2=1.0-alpha*A;
			a0=1.0+alpha/A;
			a1=-2.0*tcos;
			a2=1.0-alpha/A;
		}

		// lowshelf
		if(type==7)
		{
			b0=A*((A+1.0)-(A-1.0)*tcos+beta*tsin);
			b1=2.0*A*((A-1.0)-(A+1.0)*tcos);
			b2=A*((A+1.0)-(A-1.0)*tcos-beta*tsin);
			a0=(A+1.0)+(A-1.0)*tcos+beta*tsin;
			a1=-2.0*((A-1.0)+(A+1.0)*tcos);
			a2=(A+1.0)+(A-1.0)*tcos-beta*tsin;
		}

		// hishelf
		if(type==8)
		{
			b0=A*((A+1.0)+(A-1.0)*tcos+beta*tsin);
			b1=-2.0*A*((A-1.0)+(A+1.0)*tcos);
			b2=A*((A+1.0)+(A-1.0)*tcos-beta*tsin);
			a0=(A+1.0)-(A-1.0)*tcos+beta*tsin;
			a1=2.0*((A-1.0)-(A+1.0)*tcos);
			a2=(A+1.0)-(A-1.0)*tcos-beta*tsin;
		}
	}
	else
	{
		// other filters
		double const omega	=	2.0*k_pi*frequency/sample_rate;
		double const tsin	=	sin(omega);
		double const tcos	=	cos(omega);

		if(q_is_bandwidth)
			alpha=tsin*sinh(log(2.0)/2.0*q*omega/tsin);
		else
			alpha=tsin/(2.0*q);


		// lowpass
		if(type==0)
		{
			b0=(1.0-tcos)/2.0;
			b1=1.0-tcos;
			b2=(1.0-tcos)/2.0;
			a0=1.0+alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}

		// hipass
		if(type==1)
		{
			b0=(1.0+tcos)/2.0;
			b1=-(1.0+tcos);
			b2=(1.0+tcos)/2.0;
			a0=1.0+ alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}

		// bandpass csg
		if(type==2)
		{
			b0=tsin/2.0;
			b1=0.0;
			b2=-tsin/2;
			a0=1.0+alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}

		// bandpass czpg
		if(type==3)
		{
			b0=alpha;
			b1=0.0;
			b2=-alpha;
			a0=1.0+alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}

		// notch
		if(type==4)
		{
			b0=1.0;
			b1=-2.0*tcos;
			b2=1.0;
			a0=1.0+alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}

		// allpass
		if(type==5)
		{
			b0=1.0-alpha;
			b1=-2.0*tcos;
			b2=1.0+alpha;
			a0=1.0+alpha;
			a1=-2.0*tcos;
			a2=1.0-alpha;
		}
	}

	// set filter coeffs
	b0a0=b0/a0;
	b1a0=b1/a0;
	b2a0=b2/a0;
	a1a0=a1/a0;
	a2a0=a2/a0;
};
