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
// HighLife RAW Implementation                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "HighLifeDefines.h"
#include "HighLifeVoice.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int hlf_Zone::raw_load(HWND const hwnd, char* pfilename)
{
	char fileName[_MAX_PATH];
	sbFileName::getName(pfilename, fileName, sizeof(fileName)-1 );
	
	char buf[_MAX_PATH];
	sprintf(buf,"Load '%s' As Stereo",fileName);
	
	if(MessageBox(hwnd,buf,"discoDSP HighLife - RAW Loader",MB_YESNO | MB_ICONQUESTION)==IDYES)
		pWave->num_channels=2;
	else
		pWave->num_channels=1;

	// num samples holder
	pWave->num_samples=0;

	// get file length
	FILE* pfile=fopen(pfilename,"rb");
	if(pfile == 0){
		char tempString[256];
		sprintf(tempString, "File \"%s\" not available for reading",pfilename);
		MessageBox(hwnd,tempString,"discoDSP HighLife",MB_OK | MB_ICONERROR);
		return 0;
	}

	fseek(pfile,0,SEEK_END);
	pWave->num_samples=ftell(pfile)/(2*pWave->num_channels);
	fclose(pfile);

	// alloc wavedata in zone
	allocWave(pWave->num_channels,pWave->num_samples);

	// read file raw data
	pfile=fopen(pfilename,"rb");

	if(pfile)
	{
		for(int s=0;s<pWave->num_samples;s++)
		{
			for(int c=0;c<pWave->num_channels;c++)
			{
				short i_sample;
				fread(&i_sample,sizeof(short),1,pfile);
				pWave->data[c]->pBuf[s+WAVE_PAD]=float(i_sample)/32768.0f;
			}
		}
		
		fclose(pfile);
	}
	return 1;
}
