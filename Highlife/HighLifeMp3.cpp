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
// HighLife MP3 Implementation                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "HighLifeDefines.h"
#include "HighLifeVoice.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#include "../mpglib/interface.c"
#include "../Mpglib/mpg123.h"
#include "../Mpglib/mpglib.h"

extern "C" {
BOOL __declspec(dllexport) InitMP3(struct mpstr *mp);
void __declspec(dllexport) ExitMP3(struct mpstr *mp);
int __declspec(dllexport) decodeMP3(struct mpstr *mp, char *in, int isize, char *out, int osize, int *done);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char mp3_buf[16384];
char mp3_out[8192];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int hlf_Zone::mp3_load(HWND const hwnd, char* pfilename)
{
	// mp3 strcture
	mpstr mp;

	// open mp3 file
	FILE* pfile=fopen(pfilename,"rb");
	if(pfile == 0){
		char tempString[256];
		sprintf(tempString, "File \"%s\" not available for reading",pfilename);
		MessageBox(hwnd,tempString,"discoDSP HighLife",MB_OK | MB_ICONERROR);
		return 0;
	}

	// finding first frame for MPEG 1 LAYER III
	int flag1=0;

	// mega-hack first mp3 frame seek algorithm
	do
	{
		if(fgetc(pfile)==255)
		{
			if(tolower(fgetc(pfile)/16)==15)
				flag1=1;
		}
	}
	while(flag1==0);

	fseek(pfile,ftell(pfile)-2,SEEK_SET);

	// open temporal raw file for decode samples
	FILE* pfout=fopen("highlifetemp.raw","wb");

	// init mp3 decoder
	InitMP3(&mp);

	// decoder vars
	int size;
	int len,ret;
	int num_bytes=0;
	int num_channels=2;

	// decoder loop
	while(!feof(pfile))
	{
		len=fread(mp3_buf,1,16384,pfile);

		if(len<=0)		
			break;

		ret=decodeMP3(&mp,mp3_buf,len,mp3_out,8192,&size);

		while(ret==MP3_OK)
		{
			// first frame
			if(num_bytes==0)
			{
				// stereo/mono
				if(mp.fr.stereo<3)
					num_channels=2;
				else
					num_channels=1;
			}

			// write out raw
			fwrite(mp3_out,1,size,pfout);
			num_bytes+=size;
			ret=decodeMP3(&mp,NULL,0,mp3_out,8192,&size);
		}
	}

	// exit mp3 decoder
	ExitMP3(&mp);

	// close files
	fclose(pfout);
	fclose(pfile);

	// alloc wavedata in zone
	int const num_samples=num_bytes/(num_channels*sizeof(short));
	allocWave(num_channels,num_samples);

	// open temporal file and convert samples
	pfile=fopen("highlifetemp.raw","rb");

	for(int s=0;s<num_samples;s++)
	{
		for(int c=0;c<num_channels;c++)
		{
			short i_sample=0;
			fread(&i_sample,1,sizeof(short),pfile);
			pWave->data[c]->pBuf[s+WAVE_PAD]=float(i_sample)/32768.0f;
		}
	}

	// close file
	fclose(pfile);

	// delete temporal file
	DeleteFile("highlifetemp.raw");
	return 1;
}

	/*
	// check for id3 v2 tag
	char id3[3];
	fread(id3,1,3,pfile);

	// find first mp3 frame
	while(fgetc(pfile)!=255)
	{
		if(feof(pfile))
		{
			// no mp3 frame found
			return;
		}
	}

	// back up one byte
	fseek(pfile,ftell(pfile)-1,SEEK_SET);

	return;
*/