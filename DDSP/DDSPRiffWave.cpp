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
#include "windows.h"
#include "stdio.h"
#include ".\ddspriffwave.h"

CDDSPRiffWave::CDDSPRiffWave(void)
{
	psnd_buffer=NULL;
	psmpl_loop=NULL;
	snd_buffer_length=0;
}

CDDSPRiffWave::~CDDSPRiffWave(void)
{
	delete[] psnd_buffer;
	delete[] psmpl_loop;
}

int CDDSPRiffWave::ReadWave(HWND const hwnd, char const* pfilename)
{
	FILE* pfile=fopen(pfilename,"rb");

	// file not exist
	if(pfile == NULL){
		char tempString[256];
		sprintf(tempString, "File \"%s\" not available for reading",pfilename);
		MessageBox(hwnd,tempString,"discoDSP HighLife",MB_OK | MB_ICONERROR);
		return 0;
	}

	// delete previous wavedata and cue point array(s)
	delete[] psnd_buffer;
	delete[] psmpl_loop;

	// clean data
	psnd_buffer=NULL;
	psmpl_loop=NULL;
	snd_buffer_length=0;

	// chunk id holder
	unsigned long chk_id=0;
	unsigned long chk_size=0;

	// chunk flags
	has_smpl=false;
	has_inst=false;

	// read file
	while(!feof(pfile))
	{
		// read id four bytes
		fread(&chk_id,sizeof(unsigned long),1,pfile);
		
		// exit if end
		if(feof(pfile))
		{
			return 1;
			fclose(pfile);
		}

		// chunk scan
		switch(chk_id)
		{
			// riff chunk
		case 'FFIR':
			fread(&chk_size,sizeof(unsigned long),1,pfile);
			break;

			// wave
		case 'EVAW':
			break;

			// format chunk
		case ' tmf':
			fread(&chk_size,sizeof(unsigned long),1,pfile);
			fread(&chk_frmt,sizeof(DDSP_RIFF_WAVE_FRMT),1,pfile);
			fseek(pfile,chk_size-sizeof(DDSP_RIFF_WAVE_FRMT),SEEK_CUR);
			break;

			// data (sound) chunk
		case 'atad':
			fread(&chk_size,sizeof(unsigned long),1,pfile);
			snd_buffer_length=chk_size;
			psnd_buffer=new char[chk_size];
			fread(psnd_buffer,sizeof(char),chk_size,pfile);
			break;
		
			// sample chunk
		case 'lpms':
			has_smpl=true;
			fread(&chk_size,sizeof(unsigned long),1,pfile);
			fread(&chk_smpl,sizeof(DDSP_RIFF_WAVE_SMPL),1,pfile);
			
			// allocate and read loops
			if(chk_smpl.cSampleLoops)
			{
				psmpl_loop=new DDSP_RIFF_WAVE_LOOP[chk_smpl.cSampleLoops];
				fread(psmpl_loop,sizeof(DDSP_RIFF_WAVE_LOOP),chk_smpl.cSampleLoops,pfile);
			}
			
			// skip sampler data structures
			fseek(pfile,chk_smpl.cbSamplerData,SEEK_CUR);
			break;

			// instrument chunk
		case 'tsni':
			has_inst=true;
			fread(&chk_size,sizeof(unsigned long),1,pfile);
			fread(&chk_inst,sizeof(DDSP_RIFF_WAVE_INST),1,pfile);
			fseek(pfile,chk_size-sizeof(DDSP_RIFF_WAVE_INST),SEEK_CUR);
			break;

		// unknown chunk
		default:
			fread(&chk_size,sizeof(unsigned long),1,pfile);
			fseek(pfile,chk_size,SEEK_CUR);
			break;
		}
	}

	// unexpected end?
	fclose(pfile);

	return 0;
}

DDSP_RIFF_WAVE_FRMT* CDDSPRiffWave::GetFormat(void)
{
	return &chk_frmt;
}

DDSP_RIFF_WAVE_SMPL* CDDSPRiffWave::GetSample(void)
{
	return &chk_smpl;
}

DDSP_RIFF_WAVE_LOOP* CDDSPRiffWave::GetLoop(long const index)
{
	return &psmpl_loop[index];
}

DDSP_RIFF_WAVE_INST* CDDSPRiffWave::GetInstrument(void)
{
	return &chk_inst;
}

char* CDDSPRiffWave::GetData(void)
{
	return psnd_buffer;
}

unsigned long CDDSPRiffWave::GetDataLength(void)
{
	return snd_buffer_length;
}
