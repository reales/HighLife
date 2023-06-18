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
// HighLife Voice Implementation                                                                                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "HighLifeDefines.h"
#include "stdlib.h"
#include ".\highlifevoice.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int global_interpolation_mode=0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeVoice::CHighLifeVoice() // TODO: make sure rampzone gets properly initialized if needed
{
	f_old_pit_value=0.0;
	f_sampling_delta=1.0;
	playzone=0;
	d_wave_phase=0.0;
	parameter_ramp=0;
	ramp_not_coe=0.0f;
	not_coe_dif=0.0f;
	amp_env.stage=0;
	mod_env.stage=0;
	loop_flag=0;
	midi_curr_key=0x60;
	midi_curr_vel=0x40;
	midi_last_key=0x60;
	midi_last_vel=0x40;
	trigger_count=0;
	trigger_legato=0;
    counter=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeVoice::~CHighLifeVoice(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CHighLifeVoice::get_int_mode(void)
{
	return global_interpolation_mode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeVoice::set_int_mode(int const im)
{
	global_interpolation_mode=im;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeVoice::process(hlf_Program* pprg,float* psamplesl,float* psamplesr,float* pchol,float* pchor,float* pdell,float* pdelr, float* prevl,float* prevr,int const num_samples,float const f_pit_bnd,float const f_mod_whl,float const f_tempo,float const f_sample_rate)
{
	// voice busy
	if(is_working() && playzone<pprg->numZones)
	{
		// get playing zone pointer
		hlf_Zone* pplayzone=&pprg->pZones[playzone];

		// check phase
		if(d_wave_phase>pplayzone->pWave->num_samples)
		{
			amp_env.stage=0;
			return;
		}

		// constants
		float const f_scale=44100.0f/f_sample_rate;
		float const f_fmo=4.0f;
		float const f_2pi=atanf(1.0f)*8.0f;
	
		// wave zone constants
		int const	i_filt_mode=get_int_param(pplayzone->flt_type,NUM_FILTER_MODES);
		float const f_sys_pit_rng=2.0f*pplayzone->sys_pit_rng;

		// velocity mod
		float const f_mid_vel=(float)midi_curr_vel/127.0f;
		float const f_vel_sca=1.0f-(1.0f-f_mid_vel)*pplayzone->vel_amp;

		// morph x
		float morph_x=f_mod_whl+f_mid_vel*pplayzone->vel_mod;

		// clamp morph x
		if(morph_x<0.0f)morph_x=0.0f;
		if(morph_x>1.0f)morph_x=1.0f;

		// ramp parameter set
		if(parameter_ramp==0)
		{
			// set ramping pitchbend and modwheel values
			ramp_pit_bnd=f_pit_bnd;
			ramp_mod_whl=f_mod_whl;

			// set ramping zone values
			rampzone=*pplayzone;
		}

		// voice just triggered
		if(trigger_count==0)
		{
			// trigger lfo
			if(!(trigger_legato && pprg->ply_mode==1))
				mod_lfo.trigger(get_morph(&pplayzone->mod_lfo_phs,morph_x));
			
			// glide source
			float const f_gld=get_morph(&pplayzone->glide,morph_x);
			int const i_auto_gld=get_int_param(pplayzone->glide_auto,NUM_GLIDE_MODES);

			// get note shift(s)
			float const f_dst_not_coe_curr=float(midi_curr_key*pplayzone->midi_keycents)/1200.0f;
			float const f_dst_not_coe_last=float(midi_last_key*pplayzone->midi_keycents)/1200.0f;

			// glide compute
			if(f_gld>0.0f)
			{
				if(i_auto_gld)
				{
					if(trigger_legato)
						ramp_not_coe=f_dst_not_coe_last;
					else
						ramp_not_coe=f_dst_not_coe_curr;
				}
				else
				{
					ramp_not_coe=f_dst_not_coe_last;
				}
			}
			else
			{
				ramp_not_coe=f_dst_not_coe_curr;
			}

			// note difference
			not_coe_dif=float(midi_curr_key)-float(midi_last_key);
		}

		// compute amplitude envelope gain
		float const f_amp_env_amt=get_morph(&pplayzone->amp_env_amt,morph_x);

		// wavezone mod lfo constants
		float const f_mod_lfo_rat=get_morph(&pplayzone->mod_lfo_rat,morph_x);
		float const f_mod_lfo_del=mod_lfo.get_lfo_delta(f_tempo,f_sample_rate,pplayzone->mod_lfo_syn,f_mod_lfo_rat);

		// wavezone mod env constants
		float const f_mod_env_att=get_env_rate(get_morph(&pplayzone->mod_env_att,morph_x))*f_scale;
		float const f_mod_env_dec=get_env_rate(get_morph(&pplayzone->mod_env_dec,morph_x))*f_scale;
		float const f_mod_env_rel=get_env_rate(get_morph(&pplayzone->mod_env_rel,morph_x))*f_scale;
		float const	f_mod_env_sus=get_morph(&pplayzone->mod_env_sus,morph_x);

		// wavezone amp env constants
		float const f_amp_env_att=get_env_rate(get_morph(&pplayzone->amp_env_att,morph_x))*f_scale;
		float const f_amp_env_dec=get_env_rate(get_morph(&pplayzone->amp_env_dec,morph_x))*f_scale;
		float const f_amp_env_rel=get_env_rate(get_morph(&pplayzone->amp_env_rel,morph_x))*f_scale;
		float const f_amp_env_sus=get_morph(&pplayzone->amp_env_sus,morph_x);

		// zone glide rate
		float const f_gld_rat=not_coe_dif*get_env_rate(0.55f+get_morph(&pplayzone->glide,morph_x)*0.4f)*f_scale;

		// zone filter track amount
		float const f_flt_cut_kbd_trk=pplayzone->flt_kbd_trk*pplayzone->flt_kbd_trk*(float(midi_curr_key)/127.0f);
		float const f_flt_cut_vel_trk=pplayzone->flt_vel_trk*pplayzone->flt_vel_trk*(float(midi_curr_vel)/127.0f);
		float const f_flt_cut_trk=f_flt_cut_kbd_trk+f_flt_cut_vel_trk;

		// zone calculate gain amplitude
		float const f_wav_gain=powf(10.0f,float(pplayzone->mp_gain)/20.0f);
		float const f_amp_gain=f_vel_sca*f_vel_sca*f_amp_env_amt*f_amp_env_amt*f_wav_gain;
		
		// zone pan amplitude
		float const f_pan=float(pplayzone->mp_pan/50.0f)*0.5f+0.5f;
		float const f_amp_gain_l=sqrtf(1.0f-f_pan)*f_amp_gain;
		float const f_amp_gain_r=sqrtf(f_pan)*f_amp_gain;

		// zone fx sends
		float const f_fxcho_send=get_morph(&pplayzone->efx_chr_lev,morph_x);
		float const f_fxdel_send=get_morph(&pplayzone->efx_del_lev,morph_x);
		float const f_fxrev_send=get_morph(&pplayzone->efx_rev_lev,morph_x);

		// zone get pitch shift
		float const f_wav_kbd_shf=float(pplayzone->midi_coarse_tune)/12.0f+float(pplayzone->midi_fine_tune)/1200.0f-float(pplayzone->midi_root_key*pplayzone->midi_keycents)/1200.0f;

		// zone midi note destination pitch coef
		float const f_dst_not_coe=float(midi_curr_key*pplayzone->midi_keycents)/1200.0f;

		// zone get sampling scale
		float const f_wave_44k_ratio=float(pplayzone->pWave->sample_rate)/44100.0f;
		float f_sampling_scale=f_scale*f_wave_44k_ratio;

		// zone get synced speed
		if(pplayzone->mp_synchro && pplayzone->pWave->mp_num_ticks>0)
		{
			float const f_host_tick_samps=(15.0f*f_sample_rate)/f_tempo;
			float const f_wave_tick_samps=(float)pplayzone->pWave->num_samples/(float)pplayzone->pWave->mp_num_ticks;
			float const f_wave_sync_ratio=f_wave_tick_samps/f_host_tick_samps;
			f_sampling_scale=f_wave_sync_ratio;
		}

		// zone assign run wave pointers
		float* pwave_float_l=pplayzone?(pplayzone->pWave->data[0]->pBuf+WAVE_PAD):0;
		float* pwave_float_r=pplayzone?(pplayzone->pWave->data[pplayzone->pWave->num_channels==2]->pBuf+WAVE_PAD):0;

		// zone loop mode
		int i_lm=pplayzone->pWave->loop_mode;

		// zone loop points
		double d_loop_sta=(double)pplayzone->pWave->loop_start;
		double d_loop_end=(double)pplayzone->pWave->loop_end;

		// zone static loop phase (begin=end)
		if(i_lm && pplayzone->pWave->loop_start==pplayzone->pWave->loop_end)
			i_lm=255;

		// verify normal loop (begin<end)
		if(pplayzone->pWave->loop_start<pplayzone->pWave->loop_end)
		{
			// normal loop
			d_loop_sta=(double)pplayzone->pWave->loop_start;
			d_loop_end=(double)pplayzone->pWave->loop_end;
		}
		
		// verify swapped loop (begin>end)
		if(pplayzone->pWave->loop_start>pplayzone->pWave->loop_end)
		{
			// swapped loop
			d_loop_sta=(double)pplayzone->pWave->loop_end;
			d_loop_end=(double)pplayzone->pWave->loop_start;
		}

		// zone get loop lenght and end point (wave len)
		double const d_loop_len=d_loop_end-d_loop_sta;
		double const d_wave_len=double(pplayzone->pWave->num_samples);

		// zone sample accumms
		float sample_l=0.0f;
		float sample_r=0.0f;

		// zone pitch / amp mod amount holders
		float f_mod_lfo_pit=get_morph(&pplayzone->mod_lfo_pit,morph_x)-0.5f;
		float f_mod_env_pit=get_morph(&pplayzone->mod_env_pit,morph_x)-0.5f;;
		float f_mod_lfo_amp=get_morph(&pplayzone->mod_lfo_amp,morph_x)*0.5f;;

		// zone voice mix
		for(int s=0;s<num_samples;s++)
		{		
			// paramater ramping iteration
			if (parameter_ramp)
			{
				// pitchbend / modwheel
				ramp_pit_bnd+=(f_pit_bnd-ramp_pit_bnd)*RMP_COEF;
				ramp_mod_whl+=(f_mod_whl-ramp_mod_whl)*RMP_COEF;

				// vel mod / vel amp follow
				rampzone.vel_mod+=(pplayzone->vel_mod-rampzone.vel_mod)*RMP_COEF;
				rampzone.vel_amp+=(pplayzone->vel_amp-rampzone.vel_amp)*RMP_COEF;

				// follow glide
				val_follow(&rampzone.glide,&pplayzone->glide);

				// follow cutoff and reso
				val_follow(&rampzone.flt_cut_frq,&pplayzone->flt_cut_frq);
				val_follow(&rampzone.flt_res_amt,&pplayzone->flt_res_amt);

				// follow mod env amounts
				val_follow(&rampzone.mod_env_cut,&pplayzone->mod_env_cut);
				val_follow(&rampzone.mod_env_pit,&pplayzone->mod_env_pit);

				// follow mod lfo amounts
				val_follow(&rampzone.mod_lfo_amp,&pplayzone->mod_lfo_amp);
				val_follow(&rampzone.mod_lfo_cut,&pplayzone->mod_lfo_cut);
				val_follow(&rampzone.mod_lfo_res,&pplayzone->mod_lfo_res);
				val_follow(&rampzone.mod_lfo_pit,&pplayzone->mod_lfo_pit);

				// parameter ramp counter decrement (settle time)
				parameter_ramp--;

				// update morph x
				morph_x=ramp_mod_whl+f_mid_vel*pplayzone->vel_mod;

				// clamp morph x
				if(morph_x<0.0f)morph_x=0.0f;
				if(morph_x>1.0f)morph_x=1.0f;
			
				// pitch modulation
				f_mod_lfo_pit=get_morph(&rampzone.mod_lfo_pit,morph_x)-0.5f;
				f_mod_env_pit=get_morph(&rampzone.mod_env_pit,morph_x)-0.5f;

				// amp modulation
				f_mod_lfo_amp=get_morph(&rampzone.mod_lfo_amp,morph_x)*0.5f;
			}

			// mod lfo value
			float const f_mod_lfo_val=mod_lfo.work(f_mod_lfo_del);

			// mod env value
			float const f_mod_env_val=mod_env.work(f_mod_env_att,f_mod_env_dec,f_mod_env_sus,f_mod_env_rel);

			// pitch modulation value
			float const f_pit_mod_val=(f_mod_lfo_val*f_mod_lfo_pit+f_mod_env_val*f_mod_env_pit)*f_fmo;

			// glide up
			if(ramp_not_coe<f_dst_not_coe)
			{
				ramp_not_coe+=f_gld_rat;

				if(ramp_not_coe>f_dst_not_coe)
					ramp_not_coe=f_dst_not_coe;
			}

			// glide down
			if(ramp_not_coe>f_dst_not_coe)
			{
				ramp_not_coe+=f_gld_rat;

				if(ramp_not_coe<f_dst_not_coe)
					ramp_not_coe=f_dst_not_coe;
			}

			// compute pitch
			float const f_pit_value=ramp_not_coe+ramp_pit_bnd*f_sys_pit_rng+f_pit_mod_val+f_wav_kbd_shf;

			// update delta speed if pitch varied
			if(f_pit_value!=f_old_pit_value)
			{
				f_sampling_delta=powf(2.0f,f_pit_value);
				f_old_pit_value=f_pit_value;
			}

			// get double sampling delta
			double const d_sampling_delta=double(f_sampling_delta*f_sampling_scale);

			// hermite
			if ((pwave_float_l)&&(pwave_float_r))
			{
			if(global_interpolation_mode==0)
			{
				// compute intger sample indexes
				int const i_phase=(int)d_wave_phase;

				// left samples
				float const xm1l=pwave_float_l[i_phase-1];
				float const x0_l=pwave_float_l[i_phase+0];
				float const x1_l=pwave_float_l[i_phase+1];
				float const x2_l=pwave_float_l[i_phase+2];

				// right samples
				float const xm1r=pwave_float_r[i_phase-1];
				float const x0_r=pwave_float_r[i_phase+0];
				float const x1_r=pwave_float_r[i_phase+1];
				float const x2_r=pwave_float_r[i_phase+2];

				// left coeffs
				const float	c_l=(x1_l-xm1l)*0.5f;
				const float	v_l=x0_l-x1_l;
				const float	w_l=c_l+v_l;
				const float	a_l=w_l+v_l+(x2_l-x0_l)*0.5f;
				const float	b_neg_l=w_l+a_l;

				// right coeffs
				const float	c_r=(x1_r-xm1r)*0.5f;
				const float	v_r=x0_r-x1_r;
				const float	w_r=c_r+v_r;
				const float	a_r=w_r+v_r+(x2_r-x0_r)*0.5f;
				const float	b_neg_r=w_r+a_r;

				// get fract pos
				float const d_frac_pos=float(d_wave_phase-double(i_phase));

				// cubic interpolation compute
				sample_l=((((a_l*d_frac_pos)-b_neg_l)*d_frac_pos+c_l)*d_frac_pos+x0_l);
				sample_r=((((a_r*d_frac_pos)-b_neg_r)*d_frac_pos+c_r)*d_frac_pos+x0_r);
			}
			else if(global_interpolation_mode==1)
			{
				sample_l=sinc_interpol(d_wave_phase,pwave_float_l,d_sampling_delta,32);

				if(pplayzone->pWave->num_channels==2)
					sample_r=sinc_interpol(d_wave_phase,pwave_float_r,d_sampling_delta,32);
				else
					sample_r=sample_l;
			}
			else if(global_interpolation_mode==2)
			{
				sample_l=sinc_interpol(d_wave_phase,pwave_float_l,d_sampling_delta,256);

				if(pplayzone->pWave->num_channels==2)
					sample_r=sinc_interpol(d_wave_phase,pwave_float_r,d_sampling_delta,256);
				else
					sample_r=sample_l;
			}
			}else
			{
				sample_l=0;
				sample_r=0;
			}

			// one shoot
			if(i_lm==0)
			{
				// increment phase
				d_wave_phase+=d_sampling_delta;

				// raise wave end and shut off voice
				if(d_wave_phase>=d_wave_len)
				{
					amp_env.stage=0;
					return;
				}
			}

			// forward loop
			if(i_lm==1)
			{
				// increment phase
				d_wave_phase+=d_sampling_delta;

				// wrap phase to loop start
				while(d_wave_phase>=d_loop_end)
					d_wave_phase-=d_loop_len;
			}

			// bidirectional loop
			if(i_lm==2)
			{
				if(loop_flag==0)
				{
					// increment phase
					d_wave_phase+=d_sampling_delta;
				
					// change to backwards
					if(d_wave_phase>=d_loop_end)
					{
						double const d_outside=d_wave_phase-d_loop_end;
						d_wave_phase=d_loop_end-d_outside;
						loop_flag=1;
					}
				}
				else
				{
					// decrement phase
					d_wave_phase-=d_sampling_delta;
				
					// change to backwards
					if(d_wave_phase<=d_loop_sta)
					{
						double const d_outside=d_loop_sta-d_wave_phase;
						d_wave_phase=d_loop_sta+d_outside;
						loop_flag=0;
					}
				}
			}

			// backward loop
			if(i_lm==3)
			{
				if(loop_flag==0)
				{
					// increment phase
					d_wave_phase+=d_sampling_delta;
				
					// change to backwards
					if(d_wave_phase>=d_loop_end)
					{
						double const d_outside=d_wave_phase-d_loop_end;
						d_wave_phase=d_loop_end-d_outside;
						loop_flag=1;
					}
				}
				else
				{
					// decrement phase
					d_wave_phase-=d_sampling_delta;
				
					// wrap phase to loop end
					while(d_wave_phase<d_loop_sta)
						d_wave_phase+=d_loop_len;
				}
			}

			// sustained forward loop
			if(i_lm==4)
			{
				// increment phase
				d_wave_phase+=d_sampling_delta;

				// wrap phase to loop start while sustained
				if(amp_env.stage<4)
				{
					while(d_wave_phase>=d_loop_end)
						d_wave_phase-=d_loop_len;
				}
				else
				{
					// continue voice since is releaing, raise wave end and shut off voice
					if(d_wave_phase>=d_wave_len)
					{
						amp_env.stage=0;
						return;
					}
				}
			}

			// static loop
			if(i_lm==255)
			{
				// increment phase
				d_wave_phase+=d_sampling_delta;

				// static loop phase
				if(d_wave_phase>=d_loop_end)
					d_wave_phase=d_loop_end;
			}

			// program filter enabled
			if(i_filt_mode)
			{
				// program filter cutoff set
				float f_flt_cut_val=get_morph(&rampzone.flt_cut_frq,morph_x);
				
				// add mod lfo to cutoff modulation
				float const f_mod_lfo_cut=get_morph(&rampzone.mod_lfo_cut,morph_x)-0.5f;
				f_flt_cut_val+=f_mod_lfo_val*f_mod_lfo_cut;

				// add mod env to cutoff modulation
				float const f_mod_env_cut=get_morph(&rampzone.mod_env_cut,morph_x)-0.5f;
				f_flt_cut_val+=f_mod_env_val*f_mod_env_cut;
				
				// add key/vel tracking
				f_flt_cut_val+=f_flt_cut_trk;

				// cutoff clamp
				if(f_flt_cut_val<0.1f)f_flt_cut_val=0.1f;
				if(f_flt_cut_val>1.0f)f_flt_cut_val=1.0f;

				// make cutoff curve exponential
				f_flt_cut_val*=f_flt_cut_val;

				// program filter resonance set
				float f_flt_res_val=get_morph(&rampzone.flt_res_amt,morph_x);

				// add mod lfo to resonance modulation
				float const f_mod_lfo_res=get_morph(&rampzone.mod_lfo_res,morph_x)-0.5f;
				f_flt_res_val+=(f_mod_lfo_val*f_mod_lfo_res);

				// resonance clamp
				if(f_flt_res_val<0.1f)f_flt_res_val=0.1f;
				if(f_flt_res_val>1.0f)f_flt_res_val=1.0f;

				// program filter process
				sample_l=flt_l.work(sample_l,f_flt_cut_val,f_flt_res_val,i_filt_mode);
				sample_r=flt_r.work(sample_r,f_flt_cut_val,f_flt_res_val,i_filt_mode);
			}

			// program amp modulation
			float const f_mod_lfo_amp_val=(1.0f-f_mod_lfo_amp)-(f_mod_lfo_val*f_mod_lfo_amp);

			// envelope amp env work
			float const f_amp_env_val=amp_env.work(f_amp_env_att,f_amp_env_dec,f_amp_env_sus,f_amp_env_rel)*f_mod_lfo_amp_val;

			// apply amp gain
			sample_l*=f_amp_env_val;
			sample_l*=f_amp_gain_l;
			sample_r*=f_amp_env_val;
			sample_r*=f_amp_gain_r;
			
			// mix
			psamplesl[s]+=sample_l;
			psamplesr[s]+=sample_r;

			// efx chorus send
			pchol[s]+=sample_l*f_fxcho_send;
			pchor[s]+=sample_r*f_fxcho_send;

			// efx delay send
			pdell[s]+=sample_l*f_fxdel_send;
			pdelr[s]+=sample_r*f_fxdel_send;

			// efx reverb send
			prevl[s]+=sample_l*f_fxrev_send;
			prevr[s]+=sample_r*f_fxrev_send;
		}

		// increment trigger counter
		trigger_count+=num_samples;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeVoice::val_follow(RTPAR* pp,RTPAR* pt)
{
	pp->value+=(pt->value-pp->value)*RMP_COEF;
	pp->sense=pt->sense;
	pp->ctrln=pt->ctrln;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CHighLifeVoice::is_working(void)
{
	if(amp_env.stage)
		return 1;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeVoice::release(void)
{
	mod_env.release();
	amp_env.release();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLifeVoice::sinc_interpol(double const phase,float* psamples,double const phase_speed,int const num_taps)
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
		mix+=double(psamples[integer_part+s])*k_sinc*k_window;
	}

	// return
	return (float)mix;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeVoice::trigger(hlf_Program* pprg,int const zone_index,HIGHLIFE_INPUT_STATE* pis,int const pa_last_note,int const legato,float const f_mod_whl,int const i_playmode)
{
	// check if zone is available
	if(zone_index<pprg->numZones)
	{
		// get zone
		hlf_Zone* pz=&pprg->pZones[zone_index];

		// get note and velo
		midi_curr_key=pis->midi_key;
		midi_curr_vel=pis->midi_vel;
		midi_last_key=pa_last_note;
		midi_last_vel=pis->midi_vel;
		
		// get cue
		int cue_i=0;

		// determine cue index
		if(pz->num_cues>0)
			cue_i=(midi_curr_key-pz->midi_root_key)%pz->num_cues;

		// get trigger info
		trigger_count=0;
		trigger_legato=legato;

		// check voice trigger for busy or idle state
		if(is_working())
		{
			// mono or stealed poly (set attack stages)
			if(i_playmode==0 || i_playmode==2)
			{	
				// update rampers
				parameter_ramp=0;

				// update zone index
				playzone=zone_index;

				// reset loop flag
				loop_flag=0;

				// set initial phase offset
				// TODO: make it here without discontinuties due to voice retriggering
				d_wave_phase=get_fixed_wave_offset(pz,cue_i);

				// trigger voice
				amp_env.stage=1;
				mod_env.stage=1;
			}

			// legato (set attack stages for the voice only when releasing)
			if(i_playmode==1 && amp_env.stage==4)
			{
				// trigger voice without set phase
				amp_env.stage=1;
				mod_env.stage=1;
			}
		}
		else 
		{
			// trigger idle voice (free voice)
			loop_flag=0;

			// update zone index
			playzone=zone_index;

			// reset filter
			flt_l.clear_filter();
			flt_r.clear_filter();

			// init phase
			d_wave_phase=get_fixed_wave_offset(pz,cue_i);

			// trigger voice
			mod_env.trigger();
			amp_env.trigger();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double CHighLifeVoice::get_fixed_wave_offset(hlf_Zone* pz,int const cue_index)
{
	// no cues? always return start
	if(pz->num_cues<=0)
		return 0.0;

	// get offset pos of the cue
	int w_offset=pz->cue_pos[cue_index];

	if(pz->pWave->loop_mode)
	{
		if(pz->pWave->loop_end>pz->pWave->loop_start)
		{
			if(w_offset>pz->pWave->loop_end)
				w_offset=pz->pWave->loop_end;
		}
		else
		{
			if(w_offset>pz->pWave->loop_start)
				w_offset=pz->pWave->loop_start;
		}

		if(w_offset>=pz->pWave->num_samples)
			w_offset=pz->pWave->num_samples-1;
	}

	return double(w_offset);
}