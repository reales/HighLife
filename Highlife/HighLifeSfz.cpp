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
// HighLife SFZ Implementation                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "sbSfzParser.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CHighLife::ins_import(HWND const hwnd)
{
	CDDSPGuiFileDialog dlg;

	if(dlg.OpenFileDialog(hwnd,"discoDSP HighLife - Import HighLife Instrument","*.hilife","HighLife Instrument\0*.hilife\0\0","hilife",NULL,false))
	{
		// enter critical section
		crs_lock();
   		// open the file
   		FILE* pfile=fopen(dlg.GetFileName(0),"rb");

        if (pfile)
        {
            FILE *f=pfile;
            char sig[16];
            memset(sig,0,sizeof(sig));

            fread(sig,1,16,f);

            if (strcmp(sig,"HIGHLIFE_INS")==0)
            {
                int sz=0;
                fread(&sz,4,1,f);
                if ((sz<0)||(sz>1000000000)) goto bailout;
                std::vector<char> res;
                res.resize(sz);
                memset(&res[0],0,sz);
                fread(&res[0],sz,1,f);
                setChunk(&res[0],sz,true);
            }

        bailout:
            fclose(f);
        }

        crs_unlock();
    }
}

void CHighLife::ins_export(HWND const hwnd)
{

	// get program and wavetable pointers
	hlf_Program* pprog=&bank.program[user_program];

	CDDSPGuiFileDialog dlg;

	// filename holder
	char dirpath[_MAX_PATH];
	sprintf(dirpath,"%s.hilife",pprog->name);

	if(dlg.SaveFileDialog(hwnd,"discoDSP HighLife - Export Highlife Instrument",dirpath,"HighLife Instrument\0*.hilife\0\0","hilife",NULL))
	{
		// create and save sfz file
		char sfz_file[_MAX_PATH];
        strcpy(sfz_file,dirpath);

		FILE* pfile=fopen(sfz_file,"wb");

		// stream out file
		if(pfile)
		{
            FILE *f=pfile;
            char sig[16];
            memset(sig,0,sizeof(sig));
            strcpy(sig,"HIGHLIFE_INS");

            fwrite(sig,1,16,f);

			hlf_Program* pprg=&bank.program[user_program];
            char tmpbuf[64];
            memset(tmpbuf,0,sizeof(tmpbuf));
			sbFileName::getName(sfz_file, tmpbuf, sizeof(tmpbuf)-1);
            
            if (strstr(tmpbuf,".hilife")) 
                *strstr(tmpbuf,".hilife")=0;
                           
            memset(pprg->name,0,sizeof(pprg->name));
            strncpy(pprg->name,tmpbuf,31);



            void *data=0;
            int sz=getChunk(&data,true);

            fwrite(&sz,4,1,f);
            if (sz>0) fwrite(data,sz,1,f);
            ms_out.Reset();

            fclose(pfile);
        }
    }
}

void CHighLife::sfz_import(HWND const hwnd)
{
	CDDSPGuiFileDialog dlg;
	sbSfzParser sfzParser( &bank );

	if(dlg.OpenFileDialog(hwnd,"discoDSP HighLife - Import Sfz Program(s)","*.sfz","SFZ Description\0*.sfz\0\0","sfz",NULL,true))
	{
		// enter critical section
		crs_lock();

		// get num files
		int const num_files=dlg.GetOpenFileCount();

		// open each file
		for(int p=0;p<num_files;p++)
		{
			int const c_prg=user_program+p;

			if(c_prg<NUM_PROGRAMS){
				sfzParser.readFile(dlg.GetFileName(p), hwnd, &bank.program[c_prg] );
			}
		}
	
		// sample editor should adapt
		user_sed_adapt=1;
		user_sed_zone=0;

		// enter critical section
		crs_unlock();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::sfz_export(HWND const hwnd)
{

	// get program and wavetable pointers
	hlf_Program* pprog=&bank.program[user_program];

	CDDSPGuiFileDialog dlg;

	// filename holder
	char dirpath[_MAX_PATH];
	sprintf(dirpath,pprog->name);

	if(dlg.SaveFileDialog(hwnd,"discoDSP HighLife - Export Sfz Program",dirpath,NULL,NULL,NULL))
	{
		// create directory
		CreateDirectory(dirpath,NULL);
		SetCurrentDirectory(dirpath);

		// create and save sfz file
		char sfz_file[_MAX_PATH];
		sprintf(sfz_file,"%s.sfz",pprog->name);
		FILE* pfile=fopen(sfz_file,"w");

		// stream out file
		if(pfile)
		{
			// comment file
			fprintf(pfile,"/////////////////////////////////////////////////////////////////////////////\n");
			fprintf(pfile,"/// sfz definition file \n");
			fprintf(pfile,"/// copyright rgc:audio 2004 \n");
			fprintf(pfile,"/// -------------------------------------------------------------------------\n");
			fprintf(pfile,"/// Application: HighLife \n");
			fprintf(pfile,"/// Vendor: discoDSP \n");
			fprintf(pfile,"/// -------------------------------------------------------------------------\n");
			fprintf(pfile,"/// Program Name: %s \n",pprog->name);
			fprintf(pfile,"/// Num. Regions: %d \n",pprog->numZones);
			
			// comment end
			fprintf(pfile,"\n");
			fprintf(pfile,"\n");
			fprintf(pfile,"\n");

			// write zone (regions in sfz)
			for(int z=0;z<pprog->numZones;z++)
			{
				hlf_Zone* pz=&pprog->pZones[z];

				// region comments
				fprintf(pfile,"/////////////////////////////////////////////////////////////////////////////\n");
				fprintf(pfile,"// Region Name: %s \n",pz->name);
				fprintf(pfile,"\n");
				
				// region opcodes
				fprintf(pfile,"<region>\n");
				fprintf(pfile,"sample=%s_%.4d_%s.wav\n",pprog->name,z,pz->name);

				// write key range
				if(pz->lo_input_range.midi_key>0 || pz->hi_input_range.midi_key<127)
					fprintf(pfile,"lokey=%d hikey=%d\n",pz->lo_input_range.midi_key,pz->hi_input_range.midi_key);

				// write vel range
				if(pz->lo_input_range.midi_vel>0 || pz->hi_input_range.midi_vel<127)
					fprintf(pfile,"lovel=%d hivel=%d\n",pz->lo_input_range.midi_vel,pz->hi_input_range.midi_vel);
				
				// write controllers
				for(int cc=0;cc<128;cc++)
				{
					int const locc=pz->lo_input_range.midi_cc[cc];
					int const hicc=pz->hi_input_range.midi_cc[cc];

					// only if change
					if(locc>0 || hicc<127)
						fprintf(pfile,"locc%d=%d hicc%d=%d\n",cc,locc,cc,hicc);
				}
	
				// write bend range
				if(pz->lo_input_range.midi_bend>-8192 || pz->hi_input_range.midi_bend<8192)
					fprintf(pfile,"lobend=%d hibend=%d\n",pz->lo_input_range.midi_bend,pz->hi_input_range.midi_bend);
	
				// write chanaft range
				if(pz->lo_input_range.midi_chanaft>0 || pz->hi_input_range.midi_chanaft<127)
					fprintf(pfile,"lochanaft=%d hichanaft=%d\n",pz->lo_input_range.midi_chanaft,pz->hi_input_range.midi_chanaft);
				
				// write polyaft range
				if(pz->lo_input_range.midi_polyaft>0 || pz->hi_input_range.midi_polyaft<127)
					fprintf(pfile,"lopolyaft=%d hipolyaft=%d\n",pz->lo_input_range.midi_polyaft,pz->hi_input_range.midi_polyaft);
	
				// write trigger mode
				if(pz->midi_trigger==1)
					fprintf(pfile,"trigger=release");
	
				// write group
				if(pz->res_group)
					fprintf(pfile,"group=%d\n",pz->res_group);
		
				// write off_by
				if(pz->res_offby)
					fprintf(pfile,"off_by=%d\n",pz->res_offby);
	
				// performance separators
				fprintf(pfile,"\n");

				// write offset
				if(pz->cue_pos[0] && pz->num_cues>0)
					fprintf(pfile,"offset=%d\n",pz->cue_pos[0]);

				// write loop mode
				if(pz->pWave->loop_mode)
				{
					if(pz->pWave->loop_mode==0)fprintf(pfile,"loopmode=no_loop\n");
					if(pz->pWave->loop_mode==1)fprintf(pfile,"loopmode=loop_continuous\n");
					if(pz->pWave->loop_mode==2)fprintf(pfile,"loopmode=loop_bidirectional\n");
					if(pz->pWave->loop_mode==3)fprintf(pfile,"loopmode=loop_backward\n");
					if(pz->pWave->loop_mode==4)fprintf(pfile,"loopmode=loop_sustain\n");

					// write loop start and loop end
					fprintf(pfile,"loop_start=%d loop_end=%d\n",pz->pWave->loop_start,pz->pWave->loop_end);
				}

				// write tune
				if(pz->midi_fine_tune)
					fprintf(pfile,"tune=%d\n",pz->midi_fine_tune);

				// write root key
				if(pz->midi_root_key!=60)
					fprintf(pfile,"pitch_keycenter=%d\n",pz->midi_root_key);

				// write keycents
				if(pz->midi_keycents!=100)
					fprintf(pfile,"pitch_keytrack=%d\n",pz->midi_keycents);

				// write transpose
				if(pz->midi_coarse_tune)
					fprintf(pfile,"transpose=%d\n",pz->midi_coarse_tune);
	
				// write volume
				if(pz->mp_gain)
					fprintf(pfile,"volume=%.1f\n",float(pz->mp_gain));

				// write pan
				if(pz->mp_pan)
					fprintf(pfile,"pan=%.1f\n",float(pz->mp_pan)*2.0f);

				// region end
				fprintf(pfile,"\n");
				fprintf(pfile,"\n");
				fprintf(pfile,"\n");
			}

			fclose(pfile);
		}

		// save zones (sfz regions) wav files
		for(int z=0;z<pprog->numZones;z++)
		{
			hlf_Zone* pz=&pprog->pZones[z];

			char wavpath[_MAX_PATH];
			sprintf(wavpath,"%s_%.4d_%s.wav",pprog->name,z,pz->name);
			wav_save(hwnd,&pprog->pZones[z],wavpath);
		}
	}
}



