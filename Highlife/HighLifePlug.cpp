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
// HighLife Plug Implementation                                                                                                        //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_save_program(int const index,CAllocatingMemStreamOut* pmo)
{
	// get program ptr
	hlf_Program* pprg=&bank.program[index];

	// write program struct
	pmo->WriteRaw((void*)pprg,sizeof(hlf_Program));

	// write program zones
	for(int z=0;z<pprg->numZones;z++)
	{
		// get zone ptr
		hlf_Zone* pz=&pprg->pZones[z];

		// write wave struct
		pmo->WriteRaw((void*)pz,sizeof(hlf_Zone));

		// write wavedata
		for(int c=0;c<pz->pWave->num_channels;c++)
		{
			float* pwavedata=pz->pWave->data[c]->pBuf+WAVE_PAD;
			pmo->WriteRaw((void*)pwavedata,pz->pWave->num_samples*sizeof(float));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_load_program(int const index,CMemStreamIn* pmi)
{
	// get program ptr
	hlf_Program* pprg=&bank.program[index];

	// reset program
	pprg->init();
	
	// read program struct
	pmi->ReadRaw(pprg,sizeof(hlf_Program));

	// allocate zones
	pprg->pZones=new( &bank ) hlf_Zone[pprg->numZones];

	// read program zones
	for(int z=0;z<pprg->numZones;z++)
	{
		// get zone ptr
		hlf_Zone* pz=&pprg->pZones[z];

		// read wave struct
		pmi->ReadRaw(pz,sizeof(hlf_Zone));

		// read wavedata
		for(int c=0;c<pz->pWave->num_channels;c++)
		{
			// alloc samples
			int const pad_num_samples=pz->pWave->num_samples+(WAVE_PAD*2);
			pz->pWave->data[c]->pBuf=new float[pad_num_samples];

			// read wavedata
			float* pwavedata=pz->pWave->data[c]->pBuf+WAVE_PAD;
			pmi->ReadRaw(pwavedata,pz->pWave->num_samples*sizeof(float));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_audio_and_events_process(float** outputs,int const sampleframes,bool const replace)
{
	// enter critical section
	crs_lock();

	// output sample pointers
	float* outdata[NUM_OUTPUTS];

	// assing output sample pointers
	for(int o=0;o<NUM_OUTPUTS;o++)
		outdata[o]=outputs[o];

	// audio pointer
	int audio_ptr=0;

	// check if there're events
	if(midi_num_events)
	{
		// process audio + events
		for(int i=0;i<midi_num_events;i++)
		{
			// get next block length to process
			int const block_frames=midi_event[i].deltaFrames-audio_ptr;

			// dsp dump
			if(block_frames)
			{
				// block process
				plug_block_process(outdata,block_frames,replace);

				// increment output data buffers pointers
				for(int o=0;o<NUM_OUTPUTS;o++)
					outdata[o]+=block_frames;

				// increment audio ptr counter
				audio_ptr+=block_frames;
			}

			// process event
			char* midi_data=midi_event[i].midiData;

			int const status=midi_data[0]&0xF0;
			int const lchann=midi_data[0]&0xF;
			int const param1=midi_data[1]&0x7F;
			int const param2=midi_data[2]&0x7F;

			// note off
			if(status==0x80)
				plug_note_off(param1,param2);
			
			// note on
			if(status==0x90)
			{
				// check note on velocity
				if(param2>0)
					plug_note_on(param1,param2);
				else
					plug_note_off(param1,64);
			}

			// update last polyphonic aftertouch pressure
			if(status==0xA0)
				midi_state.midi_polyaft=param2;

			// controller
			if(status==0xB0)
			{
				// update update midi cc value
				midi_state.midi_cc[param1]=param2;

				// modwheel
				if(param1==0x01)
					setParameter(2,float(param2)/127.0f);

				// all sounds off
				if(param1==0x78)
					plug_all_sounds_off();

				// all notes off
				if(param1==0x7B)
				{
					for(int v=0;v<MAX_POLYPHONY;v++)
						voice[v].release();

					for(int k=0;k<128;k++)
						user_keyb_sta[k]=0;
				}
			}

			// update program change
			if(status==0xC0)
				setProgram(param1);

			// update channel aftertouch pressure
			if(status==0xD0)
				midi_state.midi_chanaft=param1;

			// pitchbend
			if(status==0xE0) {
				midi_state.midi_bend = (param1 + (param2 << 7)) - 8192;
			}
		}
	}

	// get remaining sample frames
	int const remaining_frames=sampleframes-audio_ptr;

	// process remaining buffer when no samples
	if(remaining_frames)
		plug_block_process(outdata,remaining_frames,replace);

	// set events null
	midi_num_events=0;

	// leave critical section
	crs_unlock();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_all_sounds_off(void)
{
	// shut up midi state
	midi_state.midi_bend=0;
	midi_state.midi_chanaft=0;
	midi_state.midi_polyaft=0;
	midi_state.midi_vel=0;
	midi_state.midi_key=60;

	// midi cc offs
	for(int cc=0;cc<128;cc++)
		midi_state.midi_cc[cc]=0;

	// shut up envs
	for(int v=0;v<MAX_POLYPHONY;v++)
	{
		voice[v].amp_env.stage=0;	
		voice[v].mod_env.stage=0;
	}

	// set kbd state to 0
	for(int k=0;k<128;k++)
		user_keyb_sta[k]=0;

	// flush chorus
	fx_cho.fx_flanger_l.flush();
	fx_cho.fx_flanger_r.flush();

	// flush delay
	fx_del.flush();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_block_process(float **outputs,int const block_samples,bool const replace)
{
	// get program
	hlf_Program* pprog=&bank.program[user_program];

	// noise protection count
	noise_count=0;

	// noise count start
	int const noise_start=(int)getSampleRate()*60;

	// reset
	if(noise_count>(noise_start+int(getSampleRate()*2.0f)))
		noise_count=0;

	// main mixer clear
	tool_init_dsp_buffer(mixer_buffer[0],block_samples,0);
	tool_init_dsp_buffer(mixer_buffer[1],block_samples,0);
	
	// fx chorus buffer clear
	if(pprog->efx_chr)
	{
		tool_init_dsp_buffer(fxchr_buffer[0],block_samples,1);
		tool_init_dsp_buffer(fxchr_buffer[1],block_samples,1);
	}
	
	// fx delay buffer clear
	if(pprog->efx_del)
	{
		tool_init_dsp_buffer(fxdel_buffer[0],block_samples,1);
		tool_init_dsp_buffer(fxdel_buffer[1],block_samples,1);
	}
	
	// fx reverb buffer clear
	if(pprog->efx_rev)
	{
		tool_init_dsp_buffer(fxrev_buffer[0],block_samples,1);
		tool_init_dsp_buffer(fxrev_buffer[1],block_samples,1);
	}

	// get sample rate and sequencer tempo
	float const f_sample_rate=getSampleRate();
	float const f_tempo=float(getTimeInfo(kVstTempoValid)?getTimeInfo(kVstTempoValid)->tempo:120);

	float const f_mod_wheel=float(midi_state.midi_cc[1])/127.0f;
	float const f_pitch_bend=float(midi_state.midi_bend)/8192.0f;

	// voice(s) processing
	for(int v=0;v<MAX_POLYPHONY;v++)
		voice[v].process(pprog,mixer_buffer[0],mixer_buffer[1],fxchr_buffer[0],fxchr_buffer[1],fxdel_buffer[0],fxdel_buffer[1],fxrev_buffer[0],fxrev_buffer[1],block_samples,f_pitch_bend,f_mod_wheel,f_tempo,f_sample_rate);

	// stereo buf
	float* psamplesl=mixer_buffer[0];
	float* psamplesr=mixer_buffer[1];

	// host mix
	host_process_mix(psamplesl,psamplesr,block_samples);
	
	// efx daft mode
	if(pprog->efx_dft)
	{
		// get daft phase speed
		float const f_daft_rate_l=get_lfo_hz(pprog->efx_dft_frq);
		float const f_daft_rate_r=f_daft_rate_l*0.7f;

		// useful range
		fx_phs_l.UpdateCoeffs(880.0,4400.0,f_daft_rate_l,0.7,1.0,f_sample_rate);
		fx_phs_r.UpdateCoeffs(880.0,4400.0,f_daft_rate_r,0.6,1.0,f_sample_rate);

		float dl=psamplesl[0];
		float dr=psamplesr[0];

		for(int s=0;s<block_samples;s++)
		{
			psamplesl[s]*=0.5f;
			psamplesr[s]*=0.5f;
			psamplesl[s]+=fx_phs_l.Run(psamplesl[s]);
			psamplesr[s]+=fx_phs_r.Run(psamplesr[s]);
		}
	}

	// efx chorus process and mix
	if(pprog->efx_chr)
	{
		fx_cho.ProcessBuffer(fxchr_buffer[0],fxchr_buffer[1],block_samples,pprog->efx_chr_del,pprog->efx_chr_fdb,pprog->efx_chr_rat,pprog->efx_chr_mod,44100.0f/f_sample_rate);
		tool_mix_dsp_buffer(fxchr_buffer[0],mixer_buffer[0],block_samples);
		tool_mix_dsp_buffer(fxchr_buffer[1],mixer_buffer[1],block_samples);
	}
	
	// efx delay process and mix
	if(pprog->efx_del)
	{
		fx_del.ProcessBuffer(fxdel_buffer[0],fxdel_buffer[1],block_samples,pprog->efx_del_del,pprog->efx_del_fdb,pprog->efx_del_cro,pprog->efx_del_syn,getSampleRate(),f_tempo);
		tool_mix_dsp_buffer(fxdel_buffer[0],mixer_buffer[0],block_samples);
		tool_mix_dsp_buffer(fxdel_buffer[1],mixer_buffer[1],block_samples);
	}

	// efx reverb process and mix
	if(pprog->efx_rev)
	{
		fx_rev.ProcessBuffer(fxrev_buffer[0],fxrev_buffer[1],block_samples,pprog->efx_rev_roo,pprog->efx_rev_wid,pprog->efx_rev_dam);
		tool_mix_dsp_buffer(fxrev_buffer[0],mixer_buffer[0],block_samples);
		tool_mix_dsp_buffer(fxrev_buffer[1],mixer_buffer[1],block_samples);
	}

	// efx compressor
	if(pprog->efx_rck)
	{
		fx_cmp_l.ProcessBuffer(psamplesl,block_samples,getSampleRate());
		fx_cmp_r.ProcessBuffer(psamplesr,block_samples,getSampleRate());
	}

	// mix and free buses
	for(int o=0;o<NUM_OUTPUTS;o++)
	{
		float* psrc=mixer_buffer[o];
		float* pdst=outputs[o];

		if(noise_count>noise_start)
		{
			for(int s=0;s<block_samples;s++)
				pdst[s]=(float(rand())/32768.0)*0.05f;
		}
		else
		{
			for(int s=0;s<block_samples;s++)
				pdst[s]=psrc[s]*user_master_volume;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	CHighLife::plug_get_free_voice(void)
{
    static int globalCounter=0;
	// program pointer
	hlf_Program* pprog=&bank.program[user_program];

	// return free
	for(int v=0;v<MAX_POLYPHONY;v++)
	{
		if(!voice[v].is_working())
			return v;
	}

   /* 
	// return any voice released
	for(int v=0;v<MAX_POLYPHONY;v++)
	{
		if(voice[v].amp_env.stage==4)
			return v;
	}
    */

    // return the earliest voice

    int voc=0,mincounter=-1;

    for(int v=0;v<MAX_POLYPHONY;v++)
	{
		if((voice[v].counter<mincounter)||(mincounter==-1))
        {
            voc=v;
            mincounter=voice[v].counter;
        }
	}
   
    voice[voc].counter=globalCounter++;
	return voc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_ramp_all_voices(void)
{
	// update parameter ramper
	for(int v=0;v<MAX_POLYPHONY;v++)
	{
		if(voice[v].is_working())
			voice[v].parameter_ramp=RMP_TIME;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_note_on(UCHAR const key,UCHAR const vel)
{
	// check for legato
	bool user_legato=false;

	for(int k=0;k<128;k++)
	{
		if(user_keyb_sta[k])
			user_legato=true;
	}

	// update keyboard state
	user_keyb_sta[key]=1;

	// store last key and velocity
	user_last_key=midi_state.midi_key;

	// store current key and velocity
	midi_state.midi_key=key;
	midi_state.midi_vel=vel;

	// trigger on attack
	plug_trigger(user_legato,0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_note_off(UCHAR const key,UCHAR const vel)
{	
	hlf_Program* pprg=&bank.program[user_program];
	
	user_keyb_sta[key]=0;

	// polyphonic release
	if(pprg->ply_mode==2)
	{
		for(int v=0;v<MAX_POLYPHONY;v++)
		{
			CHighLifeVoice* pv=&voice[v];

			if(pv->is_working() && pv->midi_curr_key==key)
				pv->release();
		}
	}
	else
	{
		int legato_note=-1;

		for(int k=0;k<128;k++)
		{
			if(user_keyb_sta[k])
				legato_note=k;
		}

		// monophonic release if not any other keypressed else last note retrig
		if(legato_note<0)
		{
			for(int v=0;v<MAX_POLYPHONY;v++)
				voice[v].release();
		}
		else
		{
			if(legato_note!=midi_state.midi_key)
				plug_note_on(legato_note,midi_state.midi_vel);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::plug_trigger(bool const legato,int const state)
{
	hlf_Program* pprg=&bank.program[user_program];

	// get float modwheel value
	float const f_mod_wheel=getParameter(2);

	// get state pointer
	HIGHLIFE_INPUT_STATE* pis=&midi_state;

	// monoleg voice counter
	int monoleg_i=0;

	// trigger new voice(s)
	for(int z=0;z<pprg->numZones;z++)
	{
		// get candidate zone
		hlf_Zone* pz=&pprg->pZones[z];

		bool valid_zone=true;

		if(pz->midi_trigger!=state)								valid_zone=false;	// check trigger state
		if(pis->midi_key<pz->lo_input_range.midi_key)			valid_zone=false;	// check lo key
		if(pis->midi_key>pz->hi_input_range.midi_key)			valid_zone=false;	// check hi key
		if(pis->midi_vel<pz->lo_input_range.midi_vel)			valid_zone=false;	// check lo vel
		if(pis->midi_vel>pz->hi_input_range.midi_vel)			valid_zone=false;	// check hi vel
		if(pis->midi_bend<pz->lo_input_range.midi_bend)			valid_zone=false;	// check lo bend
		if(pis->midi_bend>pz->hi_input_range.midi_bend)			valid_zone=false;	// check hi bend
		if(pis->midi_chanaft<pz->lo_input_range.midi_chanaft)	valid_zone=false;	// check lo chanaft
		if(pis->midi_chanaft>pz->hi_input_range.midi_chanaft)	valid_zone=false;	// check hi chanaft
		if(pis->midi_polyaft<pz->lo_input_range.midi_polyaft)	valid_zone=false;	// check lo polyaft
		if(pis->midi_polyaft>pz->hi_input_range.midi_polyaft)	valid_zone=false;	// check hi polyaft
		
		// midi ccn's
		for(int cc=0;cc<128;cc++)
		{
			if(pis->midi_cc[cc]<pz->lo_input_range.midi_cc[cc])	valid_zone=false;	// check lo midi ccn's
			if(pis->midi_cc[cc]>pz->hi_input_range.midi_cc[cc])	valid_zone=false;	// check hi midi ccn's
		}

		// zone ok
		if(valid_zone)
		{
			// offby process
			for(int v=0;v<MAX_POLYPHONY;v++)
			{
				CHighLifeVoice* pv=&voice[v];

				// check if voice is working and playzone is less than program numzones
				if(pv->is_working() && pv->playzone<pprg->numZones)
				{
					// get zone voice 
					hlf_Zone* pvz=&pprg->pZones[pv->playzone];

					// release voice if playing zone is different than candidate group
					// and playing zone equals to candidate offby
					if(pvz->res_group!=pz->res_group && pvz->res_offby==pz->res_group)
						pv->release();
				}
			}

			if(pprg->ply_mode<2)
			{
				// mono or legato
				if(monoleg_i<MAX_POLYPHONY)
					voice[monoleg_i++].trigger(pprg,z,pis,user_last_key,legato,f_mod_wheel,pprg->ply_mode);
			}
			else
			{
				// poly
				voice[plug_get_free_voice()].trigger(pprg,z,pis,user_last_key,legato,f_mod_wheel,pprg->ply_mode);
			}

			// notify on gui
			if(user_sed_zone!=z)
			{
				user_sed_zone=z;
				gui_refresh();
			}
		}
	}
}
