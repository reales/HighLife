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
// HighLife WAV Implementation                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct WA24
{
	UCHAR byte_lsb;
	UCHAR byte_mib;
	char  byte_msb;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::wav_export(HWND const hwnd)
{
	// get program and wavetable pointers
	hlf_Program* pprog=&bank.program[user_program];

	// check for zones
	if(pprog->numZones)
	{
		CDDSPGuiFileDialog dlg;

		// filename holder
		char filename[_MAX_PATH];
		sprintf(filename,pprog->name);

		if(dlg.SaveFileDialog(hwnd,"discoDSP HighLife - Export Wav(s)",filename,NULL,NULL,NULL))
		{
			CreateDirectory(filename,NULL);
			SetCurrentDirectory(filename);

			// save zones into wav files
			for(int z=0;z<pprog->numZones;z++)
			{
				hlf_Zone* pz=&pprog->pZones[z];

				char wavpath[_MAX_PATH];
				sprintf(wavpath,"%s_%.4d_%s.wav",pprog->name,z,pz->name);
				wav_save(hwnd,&pprog->pZones[z],wavpath);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::wav_save(HWND const hwnd,hlf_Zone* pz,char* pfilename)
{
	FILE* pfile=fopen(pfilename,"wb");

	if(pfile!=NULL)
	{
		// chunk id and len holders
		unsigned long chunk_id;
		unsigned long chunk_len;

		// set RIFF header id
		chunk_id='FFIR';
		
		// determine RIFF length
		chunk_len=(4)+(8+sizeof(DDSP_RIFF_WAVE_FRMT))+(8+pz->pWave->num_channels*pz->pWave->num_samples*2)+(8+sizeof(DDSP_RIFF_WAVE_INST));

		// add loop length if any
		if(pz->pWave->loop_mode)
			chunk_len+=(8+sizeof(DDSP_RIFF_WAVE_SMPL)+sizeof(DDSP_RIFF_WAVE_LOOP));

		// write RIFF header
		fwrite(&chunk_id,sizeof(unsigned long),1,pfile);
		fwrite(&chunk_len,sizeof(unsigned long),1,pfile);

		// write WAVE tag
		chunk_id='EVAW';
		fwrite(&chunk_id,sizeof(unsigned long),1,pfile);
		
		// write FORMAT chunk
		chunk_id=' tmf';
		chunk_len=sizeof(DDSP_RIFF_WAVE_FRMT);
		fwrite(&chunk_id,sizeof(unsigned long),1,pfile);
		fwrite(&chunk_len,sizeof(unsigned long),1,pfile);
		DDSP_RIFF_WAVE_FRMT fmt;
		fmt.wBitsPerSample=16;
		fmt.wChannels=pz->pWave->num_channels;
		fmt.dwSamplesPerSec=pz->pWave->sample_rate;
		fmt.wFormatTag=1;
		fmt.wBlockAlign=pz->pWave->num_channels*(fmt.wBitsPerSample/8);
		fmt.dwAvgBytesPerSec=pz->pWave->sample_rate*fmt.wBlockAlign;
		fwrite(&fmt,sizeof(DDSP_RIFF_WAVE_FRMT),1,pfile);

		// write DATA (sound) chunk
		chunk_id='atad';
		chunk_len=pz->pWave->num_channels*pz->pWave->num_samples*2;
		fwrite(&chunk_id,sizeof(unsigned long),1,pfile);
		fwrite(&chunk_len,sizeof(unsigned long),1,pfile);
	
		// write 16bit converted sampledata
		for(int s=0;s<pz->pWave->num_samples;s++)
		{
			for(int c=0;c<pz->pWave->num_channels;c++)
			{
				float const sample_32bit=pz->pWave->data[c]->pBuf[s+WAVE_PAD]*32767.0f;
				short const sample_16bit=short(sample_32bit);
				fwrite(&sample_16bit,sizeof(short),1,pfile);
			}
		}

		// write sample if loop enabled
		if(pz->pWave->loop_mode)
		{
			chunk_id='lpms';
			chunk_len=sizeof(DDSP_RIFF_WAVE_SMPL);
			fwrite(&chunk_id,sizeof(unsigned long),1,pfile);
			fwrite(&chunk_len,sizeof(unsigned long),1,pfile);
			DDSP_RIFF_WAVE_SMPL smpl;
			smpl.cbSamplerData=0;
			smpl.cSampleLoops=1;
			smpl.dwManufacturer=0;
			smpl.dwMIDIPitchFraction=0;
			smpl.dwMIDIUnityNote=pz->midi_root_key;
			smpl.dwProduct=0;
			smpl.dwSamplePeriod=0;
			smpl.dwSMPTEFormat=0;
			smpl.dwSMPTEOffset=0;
			fwrite(&smpl,sizeof(DDSP_RIFF_WAVE_SMPL),1,pfile);

			// fill and write loop struct
			DDSP_RIFF_WAVE_LOOP loop;
			loop.dwStart=pz->pWave->loop_start;
			loop.dwEnd=(pz->pWave->loop_end-1);
			loop.dwFraction=0;
			loop.dwIdentifier=0;
			loop.dwPlayCount=0;
			loop.dwType=0;
			fwrite(&loop,sizeof(DDSP_RIFF_WAVE_LOOP),1,pfile);
		}

		// write inst chunk
		chunk_id='tsni';
		chunk_len=sizeof(DDSP_RIFF_WAVE_INST);
		fwrite(&chunk_id,sizeof(unsigned long),1,pfile);
		fwrite(&chunk_len,sizeof(unsigned long),1,pfile);
		DDSP_RIFF_WAVE_INST inst;
		inst.LowNote=pz->lo_input_range.midi_key;
		inst.HighNote=pz->hi_input_range.midi_key;
		inst.LowVelocity=pz->lo_input_range.midi_vel;
		inst.HighVelocity=pz->hi_input_range.midi_vel;
		inst.UnshiftedNote=pz->midi_root_key;
		inst.FineTune=pz->midi_fine_tune;
		inst.Gain=pz->mp_gain;
		fwrite(&inst,sizeof(DDSP_RIFF_WAVE_INST),1,pfile);

		// close file
		fclose(pfile);
	}
}
