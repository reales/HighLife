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
// HighLife Fx Header                                                                                                                  //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../freeverb/revmodel.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxSimpleFilter
{
public:
	CFxSimpleFilter()
	{
		f_memory=0.0f;
	}

	float process_sample(float const in,float const fc)
	{
		f_memory+=(in-f_memory)*fc;
		return f_memory;
	}

public:
	float f_memory;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxRbjFilter
{
public:

	CFxRbjFilter()
	{
		// reset filter coeffs
		b0a0=b1a0=b2a0=a1a0=a2a0=0.0;

		// reset in/out history
		ou1=ou2=in1=in2=0.0f;	
	};

	float filter(float in0)
	{
		// filter
		float const yn = b0a0*in0 + b1a0*in1 + b2a0*in2 - a1a0*ou1 - a2a0*ou2;

		// push in/out buffers
		in2=in1;
		in1=in0;
		ou2=ou1;
		ou1=yn;

		// return output
		return yn;
	};

	void calc_filter_coeffs(int const type,float const frequency,float const sample_rate,float const q,float const db_gain,bool q_is_bandwidth)
	{
		// temp pi
		float const temp_pi=3.1415926535897932384626433832795f;

		// temp coef vars
		float alpha,a0,a1,a2,b0,b1,b2;

		// peaking, lowshelf and hishelf
		if(type>=6)
		{
			float const A		=	powf(10.0f,(db_gain/40.0f));
			float const omega	=	2.0f*temp_pi*frequency/sample_rate;
			float const tsin	=	sinf(omega);
			float const tcos	=	cosf(omega);

			if(q_is_bandwidth)
				alpha=tsin*sinhf(logf(2.0f)/2.0f*q*omega/tsin);
			else
				alpha=tsin/(2.0f*q);

			float const beta	=	sqrtf(A)/q;

			// peaking
			if(type==6)
			{
				b0=1.0f+alpha*A;
				b1=-2.0f*tcos;
				b2=1.0f-alpha*A;
				a0=1.0f+alpha/A;
				a1=-2.0f*tcos;
				a2=1.0f-alpha/A;
			}

			// lowshelf
			if(type==7)
			{
				b0=A*((A+1.0f)-(A-1.0f)*tcos+beta*tsin);
				b1=2.0f*A*((A-1.0f)-(A+1.0f)*tcos);
				b2=A*((A+1.0f)-(A-1.0f)*tcos-beta*tsin);
				a0=(A+1.0f)+(A-1.0f)*tcos+beta*tsin;
				a1=-2.0f*((A-1.0f)+(A+1.0f)*tcos);
				a2=(A+1.0f)+(A-1.0f)*tcos-beta*tsin;
			}

			// hishelf
			if(type==8)
			{
				b0=A*((A+1.0f)+(A-1.0f)*tcos+beta*tsin);
				b1=-2.0f*A*((A-1.0f)+(A+1.0f)*tcos);
				b2=A*((A+1.0f)+(A-1.0f)*tcos-beta*tsin);
				a0=(A+1.0f)-(A-1.0f)*tcos+beta*tsin;
				a1=2.0f*((A-1.0f)-(A+1.0f)*tcos);
				a2=(A+1.0f)-(A-1.0f)*tcos-beta*tsin;
			}
		}
		else
		{
			// other filters
			float const omega	=	2.0f*temp_pi*frequency/sample_rate;
			float const tsin	=	sinf(omega);
			float const tcos	=	cosf(omega);

			if(q_is_bandwidth)
				alpha=tsin*sinhf(logf(2.0f)/2.0f*q*omega/tsin);
			else
				alpha=tsin/(2.0f*q);


			// lowpass
			if(type==0)
			{
				b0=(1.0f-tcos)/2.0f;
				b1=1.0f-tcos;
				b2=(1.0f-tcos)/2.0f;
				a0=1.0f+alpha;
				a1=-2.0f*tcos;
				a2=1.0f-alpha;
			}

			// hipass
			if(type==1)
			{
				b0=(1.0f+tcos)/2.0f;
				b1=-(1.0f+tcos);
				b2=(1.0f+tcos)/2.0f;
				a0=1.0f+ alpha;
				a1=-2.0f*tcos;
				a2=1.0f-alpha;
			}

			// bandpass csg
			if(type==2)
			{
				b0=tsin/2.0f;
				b1=0.0f;
				b2=-tsin/2;
				a0=1.0f+alpha;
				a1=-2.0f*tcos;
				a2=1.0f-alpha;
			}

			// bandpass czpg
			if(type==3)
			{
				b0=alpha;
				b1=0.0f;
				b2=-alpha;
				a0=1.0f+alpha;
				a1=-2.0f*tcos;
				a2=1.0f-alpha;
			}

			// notch
			if(type==4)
			{
				b0=1.0f;
				b1=-2.0f*tcos;
				b2=1.0f;
				a0=1.0f+alpha;
				a1=-2.0f*tcos;
				a2=1.0f-alpha;
			}

			// allpass
			if(type==5)
			{
				b0=1.0f-alpha;
				b1=-2.0f*tcos;
				b2=1.0f+alpha;
				a0=1.0f+alpha;
				a1=-2.0f*tcos;
				a2=1.0f-alpha;
			}
		}

		// set filter coeffs
		b0a0=b0/a0;
		b1a0=b1/a0;
		b2a0=b2/a0;
		a1a0=a1/a0;
		a2a0=a2/a0;
	};

private:

	// filter coeffs
	float b0a0,b1a0,b2a0,a1a0,a2a0;

	// in/out history
	float ou1,ou2,in1,in2;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxCompressor
{
public:
	CFxCompressor()
	{
		peak_c=0;
	};

	virtual ~CFxCompressor()
	{
	};

	float saturate(float x, float t)
	{
		if(fabsf(x)<t)
		{
			return x;
		}
		else
		{
			if(x>0.0f)
				return t+(1.0f-t)*tanhf((x-t)/(1.0f-t));
			else
				return -(t+(1.0f-t)*tanhf((-x-t)/(1.0f-t)));
		}
	}

	void ProcessBuffer(float* pbuffer,int const numsamples,float const sample_rate)
	{
		// HOUSE
		// eq set
		pf_lo.calc_filter_coeffs(6,100,sample_rate,1.0f,6.0f,false);
		pf_mi.calc_filter_coeffs(7,1000,sample_rate,1.0f,3.0f,false);
		pf_hi.calc_filter_coeffs(8,10000,sample_rate,1.0f,4.0f,false);

		float const f_ad=1.0e-14f;

		// compressor set
		float const srate_ms=sample_rate/1000.0f;

		// umbral (threshold)
		float const threshold_c=powf(10.0f,-12.0f/20.0f);

		// attack (ataque)
		float const c_attack_ms=3.6f;
		float const ga_fu=(float)expf(-1.0f/(srate_ms*c_attack_ms));

		// caida (release)
		float const c_release_ms=150.0f;
		float const gr_fu=(float)expf(-1.0f/(srate_ms*c_release_ms));

		// post gain
		float const post_gain=powf(10.0f,6.0f/20.0f);

		// zumbando
		for(int s=0;s<numsamples;s++)
		{	
			// eq process
			float const lo=pf_lo.filter(pf_mi.filter(pf_hi.filter(pbuffer[s]+(float)rand()*f_ad)));

			// computa picos maximo
			float const in_peak=fabsf(lo);

			// compute gain			
			float comp_gain;

			if(in_peak>threshold_c)
				comp_gain=threshold_c/in_peak;
			else
				comp_gain=1.0f;

			// envolvente
			if(peak_c>comp_gain)
			{
				peak_c*=ga_fu;
				peak_c+=(1.0f-ga_fu)*comp_gain;
			}
			else
			{
				peak_c*=gr_fu;
				peak_c+=(1.0f-gr_fu)*comp_gain;
			}

			// salida
			pbuffer[s]=saturate(lo*peak_c*post_gain,0.75f);
		}
	}

public:
	float peak_c;
	CFxRbjFilter pf_lo,pf_mi,pf_hi;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxFlanger
{
public:

	CFxFlanger()
	{
		flush();
	}

	virtual ~CFxFlanger()
	{
	}

	void flush()
	{
		fla_cnt=0;

		for(int s=0;s<MAX_CHO_SIZE;s++)
			fla_buf[s]=0;
	}

	inline float const update(float const in,float const del,float const fdb)
	{
		// calculate delay offset
		float back=(float)fla_cnt-del;

		// clip lookback buffer-bound
		if(back<0.0f)
			back=float(MAX_CHO_SIZE)+back;

		// get index
		int const index=(int)back;

		// 4 samples hermite
		float const y_1=fla_buf[(index+0)&(MAX_CHO_SIZE-1)];
		float const y0=fla_buf[(index+1)&(MAX_CHO_SIZE-1)];
		float const y1=fla_buf[(index+2)&(MAX_CHO_SIZE-1)];
		float const y2=fla_buf[(index+3)&(MAX_CHO_SIZE-1)];

		// csi calculate
		float const c0=y0;
		float const c1=0.5f*(y1-y_1);
		float const c2=y_1-2.5f*y0+2.0f*y1-0.5f*y2;
		float const c3=0.5f*(y2-y_1)+1.5f*(y0-y1);

		// compute interpolation x
		float const x=back-(float)floorf(back);

		// get output
		float const output=((c3*x+c2)*x+c1)*x+c0;

		// add to delay buffer
		fla_buf[(fla_cnt++)&(MAX_CHO_SIZE-1)]=in+output*fdb;

		// return output
		return output;
	}

	float	fla_buf[MAX_CHO_SIZE];
	int		fla_cnt;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxChorus
{
public:
	CFxChorus()
	{
		lfo_deg=0.0f;
	}

	virtual ~CFxChorus()
	{
	}

	void ProcessBuffer(float* pl,float* pr,int const numsamples,float const f_del,float const f_fdb,float const f_rat,float const f_mod,float const scale)
	{
		float const f_chr_del=f_del*float(MAX_CHO_SIZE)*0.5f;
		float const f_chr_fdb=f_fdb*0.95f;
		float const f_chr_rat=get_lfo_rate(f_rat)*scale;
		float const f_chr_amt=f_chr_del*f_mod*f_mod*0.99f;

		for(int s=0;s<numsamples;s++)
		{
			// get mod calc
			float const fp_sin_val=pseudo_sine(lfo_deg)*f_chr_amt;
			float const fp_delay_l=f_chr_del-fp_sin_val;
			float const fp_delay_r=f_chr_del-fp_sin_val*0.9f;

			// chorus mix
			pl[s]=fx_flanger_l.update(pl[s],fp_delay_l,f_chr_fdb);
			pr[s]=fx_flanger_r.update(pr[s],fp_delay_r,f_chr_fdb);

			// lfo advance
			lfo_deg+=f_chr_rat;

			// lfo wrap
			if(lfo_deg>=1.0f)
				lfo_deg-=2.0f;
		}
	}

	float pseudo_sine(float x)
	{
		// Compute 2*(x^2-1.0)^2-1.0
		x*=x; 
		x-=1.0f;
		x*=x;

		// return x
		return x;
	}

public:
	CFxFlanger	fx_flanger_l;
	CFxFlanger	fx_flanger_r;
	float		lfo_deg;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxDelay
{
public:

	CFxDelay()
	{
		flush();
	}

	virtual ~CFxDelay()
	{
	}

	void flush()
	{
		for(int s=0;s<MAX_DEL_SIZE;s++)
		{
			l_buffer[s]=0.0;
			r_buffer[s]=0.0;
		}

		l_out=r_out=0;

		counter=0;
	}

	void update(float const l_in,float const r_in,int const l_delay,int const r_delay, float const l_feedback,float const r_feedback)
	{
		int const masked_counter=counter&(MAX_DEL_SIZE-1);
		l_out=l_buffer[(masked_counter-l_delay)&(MAX_DEL_SIZE-1)];
		r_out=r_buffer[(masked_counter-r_delay)&(MAX_DEL_SIZE-1)];
		l_buffer[masked_counter]=l_in+l_out*l_feedback;
		r_buffer[masked_counter]=r_in+r_out*r_feedback;
		counter++;
	}

	void cross_update(float const l_in,float const r_in,int const l_delay,int const r_delay, float const l_feedback, float const r_feedback)
	{
		int const masked_counter=counter&(MAX_DEL_SIZE-1);
		l_out=l_buffer[(masked_counter-l_delay)&(MAX_DEL_SIZE-1)];
		r_out=r_buffer[(masked_counter-r_delay)&(MAX_DEL_SIZE-1)];	
		l_buffer[masked_counter]=l_in+r_out*l_feedback;
		r_buffer[masked_counter]=r_in+l_out*r_feedback;
		counter++;
	}

	void ProcessBuffer(float* pl,float* pr,int const numsamples,float const f_del,float const f_fdb,int const i_cross,int const i_sync,float const f_sam,float const f_bpm)
	{
		// some useful contants
		float const f_tic_sam=(f_sam*15.0f)/f_bpm;
		float const f_feedbck=f_fdb*0.95f;
		float const	f_max_del_tks=4.0f;

		// calculate delay time
		float f_del_tim;	
	
		if(i_sync)
			f_del_tim=f_del*f_tic_sam*f_max_del_tks;
		else
			f_del_tim=f_del*f_sam;

		// set integer delay
		int i_delay=(int)f_del_tim;

		// clamp
		if(i_delay<256)i_delay=256;
		if(i_delay>MAX_DEL_SIZE)i_delay=MAX_DEL_SIZE;

		// delay process
		for(int s=0;s<numsamples;s++)
		{
			if(i_cross)
				cross_update(0.0f,pr[s],i_delay,i_delay,f_feedbck,f_feedbck);
			else
				update(pl[s],pr[s],i_delay,i_delay,f_feedbck,f_feedbck);

			// output
			pl[s]=l_out;
			pr[s]=r_out;
		}
	}

	int		counter;
	float	l_out,r_out;
	float	l_buffer[MAX_DEL_SIZE];
	float	r_buffer[MAX_DEL_SIZE];
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFxReverb
{
public:

	CFxReverb()
	{
	}

	virtual ~CFxReverb()
	{
	}

	void ProcessBuffer(float* pl,float* pr,int const numsamples,float const f_roo,float const f_wid,float const f_dam)
	{
		reverb.setroomsize(f_roo);
		reverb.setwidth(f_wid);
		reverb.setdamp(f_dam);
		reverb.setwet(1.0f);
		reverb.processreplace(pl,pr,pl,pr,numsamples,1,0.0f,1.0f);
	}

	revmodel reverb;
};
