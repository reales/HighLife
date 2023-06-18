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
// HighLife Tools Implementation                                                                                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::tool_sample_import_dlg(HWND const hwnd)
{
	CDDSPGuiFileDialog dlg;

	if(dlg.OpenFileDialog(hwnd,"discoDSP HighLife - Import Sample File(s)","*.wav;*.mp3","RIFF Wave Files\0*.wav\0RAW Files\0*.*\0\0","wav",NULL,true))
	{
		// get num files
		int const num_files=dlg.GetOpenFileCount();

		// check all files
		bool check=true;

		for(int f=0;f<num_files;f++)
		{
			FILE* pf=fopen(dlg.GetFileName(f),"rb");

			if(pf)
				fclose(pf);
			else
				check=false;
		}

		// open if some files selected
		if(num_files && check)
		{
			// wave spare flag
			bool dm_wave_spare=false;

			// ask for autospread
			if(num_files>1 && MessageBox(hwnd,"Use Auto-Drum-Machine Spread?","discoDSP HighLife",MB_YESNO | MB_ICONQUESTION)==IDYES)
				dm_wave_spare=true;
 
			// enter critical section
			crs_lock();

			// get program and wavetable pointers
			hlf_Program* pprog=&bank.program[user_program];

			// format program title
			sbFileName::getTitle( dlg.GetFileName(0), pprog->name, sizeof(pprog->name)-1 );

			// load each wavefile
			for(int wf=0;wf<num_files;wf++)
			{
				// append new zones to program
				int zone_index = 0;
				hlf_Zone* pz=pprog->allocZone(&zone_index);
				if (pz->loadWave(hwnd, dlg.GetFileName(wf)) == 0){
					// TODO: let user locate Sample on HD. if no wave is found reset to default wave
					// TODO: put repeated code into a single module
					pprog->deleteZone(zone_index);
					continue;
				}

				// drum machine spare
				if(dm_wave_spare)
				{
					// compute mapping keystart
					int spare_start=120-num_files;

					// clamp
					if(spare_start>60)spare_start=60;
					if(spare_start<0)spare_start=0;

					int const i_keys_per_drumhit=1; // user_kbd_splits;
					int const dm_key=spare_start+wf*i_keys_per_drumhit;
					pz->lo_input_range.midi_key=dm_key;
					pz->hi_input_range.midi_key=dm_key+(i_keys_per_drumhit-1);
					pz->midi_root_key=dm_key+(i_keys_per_drumhit/2);
				}
			}

			// sample editor should adapt
			// TODO: don't adapt sample editor if sample load failed

			user_sed_zone=pprog->numZones-1;
			user_sed_adapt=1;

			// leave critical setion
			crs_unlock();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::tool_sample_browse_dlg(HWND const hwnd,hlf_Zone* pz)
{
	CDDSPGuiFileDialog dlg;

	if(dlg.OpenFileDialog(hwnd,"discoDSP HighLife - Browse Sample File","*.wav","RIFF Wave Files\0*.wav\0RAW Files\0*.*\0\0","wav",NULL,false))
	{
		// open file (check existence)
		FILE* pf=fopen(dlg.GetFileName(0),"rb");

		if(pf!=NULL)
		{
			// close file
			fclose(pf);

			// enter critical section
			crs_lock();

			// load new wave in zone
			pz->clearWave();
			// TODO: what happens if no sample is found? can this happen?
			pz->loadWave(hwnd, dlg.GetFileName(0));

			// sample editor should adapt
			user_sed_adapt=1;

			// leave critical section
			crs_unlock();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::tool_init_dsp_buffer(float* pbuf,int const numsamples,int dfix)
{
	if(dfix)
	{
		float const f_ad=1.0e-14f;

		for(int s=0;s<numsamples;s++)
			pbuf[s]=rand()*f_ad;
	}
	else
	{
		for(int s=0;s<numsamples;s++)
			pbuf[s]=0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::tool_mix_dsp_buffer(float* pbuf_src,float* pbuf_dest,int const numsamples)
{
	for(int s=0;s<numsamples;s++)
		pbuf_dest[s]+=pbuf_src[s];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::tool_copy_dsp_buffer(float* pbuf_src,float* pbuf_dest,int const numsamples)
{
	for(int s=0;s<numsamples;s++)
		pbuf_dest[s]=pbuf_src[s];
}
