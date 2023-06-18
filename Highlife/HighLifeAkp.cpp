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
// HighLife AKP Implementation                                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AKP_KLOC
{
	unsigned long	kloc_id;			// 00A6 - 00A9:	'kloc'
	unsigned long	kloc_len;			// 00AA - 00AD:	10 00 00 00 (16)
	unsigned long	res0;				// 00AE - 00B1: 01,03,01,04
	unsigned char	low_note;			// 00B2:	15	low note (21) 21 -> 127
	unsigned char	hig_note;			// 00B3:	7F	high note (127) 21 -> 127
	char			semitone_tune;		// 00B4:	00	Semitone Tune (0) -36 -> 36
	char			fine_tune;			// 00B5:	00	Fine Tune (0) -50 -> 50
	unsigned char	override_fx;		// 00B6:	00	Overide FX (0) 0 = OFF, 1 = FX1, 2 = FX2, 3 = RV3, 4 = RV4
	unsigned char	fx_send_level;		// 00B7:	00	FX Send Level (0) 0 -> 100
	char			pitch_mod_1;		// 00B8:	64	Pitch Mod 1 (100) -100 -> 100
	char			pitch_mod_2;		// 00B9:	64	Pitch Mod 2 (100) -100 -> 100
	char			amp_mod;			// 00BA:	00	Amp Mod (0) -100 -> 100
	unsigned char	zone_xfade;			// 00BB:	00	Zone Xfade (0) 0 = OFF, 1 = ON
	unsigned char	mute_group;			// 00BC:	00	Mute Group
	unsigned char	res1;				// 00BD:	00	.
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AKP_ENV
{
	unsigned long	env_id;				// 00BE - 00C1:	'env ' (Akp Env)
	unsigned long	env_len;			// 00C2 - 00C5:	12 00 00 00 (18)
	unsigned char	res0;				// 00C6:	01	.
	unsigned char	attack;				// 00C7:	00	Attack (0) 0 -> 100
	unsigned char	res1;				// 00C8:	00	.
	unsigned char	decay;				// 00C9:	32	Decay (50) 0 -> 100
	unsigned char	release;			// 00CA:	0F	Release (15) 0 -> 100
	unsigned char	res2;				// 00CB:	00	.
	unsigned char	res3;				// 00CC:	00	.
	unsigned char	sustain;			// 00CD:	64	Sustain (100) 0 -> 100
	unsigned char	res4;				// 00CE:	00	.
	unsigned char	res5;				// 00CF:	00	.
	char			velo_attack;		// 00D0:	00	Velo->Attack (0) -100 -> 100
	unsigned char	res6;				// 00D1:	00	.
	char			key_scale;			// 00D2:	00	Keyscale (0) -100 -> 100
	unsigned char	res7;				// 00D3:	00	.
	char			on_vel_rel;			// 00D4:	00	On Vel->Rel (0) -100 -> 100
	char			off_vel_rel;		// 00D5:	00	Off Vel->Rel (0) -100 -> 100
	unsigned char	res8;				// 00D6:	00	.
	unsigned char	res9;				// 00D7:	00	.
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AKP_FILT
{
	unsigned long	filt_id;			// 010C - 010F:	'filt'
	unsigned long	filt_len;			// 0110 - 0113:	0A 00 00 00 (10)
	unsigned char	res0;				// 0114:	01	.
	unsigned char	filter_mode;		// 0115:	00	Filter Mode (0)
	unsigned char	cutoff_freq;		// 0116:	64	Cutoff Freq (100) 0 -> 100
	unsigned char	resonance;			// 0117:	00	Resonance (0) 0 -> 12
	char			kbd_track;			// 0118:	00	Keyboard Track (0) -36 -> 36
	char			mod_input1;			// 0119:	00	Mod Input 1 (0) -100 -> 100
	char			mod_input2;			// 011A:	00	Mod Input 2 (0) -100 -> 100
	char			mod_input3;			// 011B:	00	Mod Input 3 (0) -100 -> 100
	unsigned char	headroom;			// 011C:	00	Headroom (0) 0 = 0db, 1 = 6db, 2 = 12db, 3 = 18db, 4 = 24db, 5 = 30db
	unsigned char	reserved;			// 011D:	00	.
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AKP_ZONE
{
	unsigned long	zone_id;			// 011E - 0121:	'zone' (Zone 1)
	unsigned long	zone_len;			// 0122 - 0125:	2E 00 00 00 (46)
	unsigned char	res0;				// 0126:	01	.
	unsigned char	num_chars;			// 0127:	nn	number of chars in Sample Name
	char			sample_name[20];	// 0128 - 013B:	Sample Name (pad with 00h) (if first character = 00h then no sample assigned)
	char			res1;				// 013C:	00	.
	char			res2;				// 013D:	00	.
	char			res3;				// 013E:	00	.
	char			res4;				// 013F:	00	.
	char			res5;				// 0140:	00	.
	char			res6;				// 0141:	00	.
	char			res7;				// 0142:	00	.
	char			res8;				// 0143:	00	.
	char			res9;				// 0144:	00	.
	char			res10;				// 0145:	00	.
	char			res11;				// 0146:	00	.
	char			res12;				// 0147:	00	.
	unsigned char	low_vel;			// 0148:	00	Low Velocity (0) 0 -> 127
	unsigned char	hig_vel;			// 0149:	7F	High Velocity (127) 0 -> 127
	char			fine_tune;			// 014A:	00	Fine Tune (0) -50 -> 50
	char			semitone_tune;		// 014B:	00	Semitone Tune (0) -36 -> 36
	char			filter;				// 014C:	00	Filter (0) -100 -> 100
	char			pan_balance;		// 014D:	00	Pan/Balance (0) -50 -> 50 = L50 -> R50
	unsigned char	loop_mode;			// 014E:	04	Playback (4) 0 = NO LOOPING, 1 = ONE SHOT, 2 = LOOP IN REL, 3 = LOOP UNTIL REL, 4 = AS SAMPLE
	unsigned char	output;				// 014F:	00	Output (0)
	unsigned char	zone_level;			// 0150:	00	Zone Level (0) -100 -> 100
	unsigned char	kbd_track;			// 0151:	01	Keyboard Track (1) 0 = OFF, 1 = ON
	unsigned char	vel_start_msb;		// 0152:	00	Velocity->Start LSB (0) }
	unsigned char	vel_start_lsb;		// 0153:	00	Velocity->Start MSB (0) } -9999 -> 9999
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct AKP_KGRP
{
	AKP_KLOC		kloc;
	AKP_ENV			amp_env;
	AKP_ENV			flt_env;
	AKP_ENV			aux_env;
	AKP_FILT		filt;
	AKP_ZONE		zone[4];
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::akp_import(HWND const hwnd)
{
	CDDSPGuiFileDialog dlg;

	AKP_KGRP	akp_kgrp[128];

	if(dlg.OpenFileDialog(hwnd,"discoDSP HighLife - Import Akai Program(s)","*.akp","Akai S5000/S6000 Program Files\0*.akp\0\0","akp",NULL,true))
	{
		// enter critical section
		crs_lock();
		
		// get num files
		int const num_files=dlg.GetOpenFileCount();

		// open each file
		for(int p=0;p<num_files;p++)
		{
			int const c_prg=user_program+p;
			
			if(c_prg<NUM_PROGRAMS)
			{
				// num keygroups holder
				int	akp_num_keygroups=0;

				// open the file
				FILE* pfile=fopen(dlg.GetFileName(p),"rb");

				if(pfile)
				{
					// chunk id and length holders
					unsigned long chunk_id;
					unsigned long chunk_le;

					// read riff header
					fread(&chunk_id,sizeof(unsigned long),1,pfile);
					fread(&chunk_le,sizeof(unsigned long),1,pfile);
	
					// read aprg id
					fread(&chunk_id,sizeof(unsigned long),1,pfile);
					
					// process akp chunks
					while(!feof(pfile))
					{
						// read chunk id and length
						fread(&chunk_id,sizeof(unsigned long),1,pfile);
						fread(&chunk_le,sizeof(unsigned long),1,pfile);

						// check for file end
						if(feof(pfile))
							break;

						// process chunk
						switch(chunk_id)
						{

						case 'prgk':
							fread(&akp_kgrp[akp_num_keygroups++],sizeof(AKP_KGRP),1,pfile);
							break;
						
						default:
							fseek(pfile,chunk_le,SEEK_CUR);
							break;
						}
					}

					fclose(pfile);

					// get program pointer and initialize
					hlf_Program* pprg=&bank.program[c_prg];
					pprg->init();

					// init program name
					sbFileName::getTitle(dlg.GetFileName(p), pprg->name, sizeof(pprg->name)-1);

					for(int kgrp=0;kgrp<akp_num_keygroups;kgrp++)
					{
						// get keygroup pointer
						AKP_KGRP* pak=&akp_kgrp[kgrp];

						// parse keyzones
						for(int z=0;z<4;z++)
						{
							// get keygroup zone pointer
							AKP_ZONE* paz=&pak->zone[z];

							// existant wave
							if(paz->sample_name[0])
							{
								// set filename
								char wavpath[_MAX_PATH];
								sprintf(wavpath,"%s.wav",paz->sample_name);

								// alloc new zone and load the sample in it
								int zone_index = 0;
								hlf_Zone* phz=pprg->allocZone(&zone_index);
								if (phz->loadWave(hwnd, wavpath) == 0){
									// TODO: let user locate Sample on HD. if no wave is found reset to default wave
									// TODO: put repeated code into a single module
									pprg->deleteZone(zone_index);
									continue;
								}

								// parse kloc and kzone info
								phz->lo_input_range.midi_key=pak->kloc.low_note;
								phz->hi_input_range.midi_key=pak->kloc.hig_note;
								phz->lo_input_range.midi_vel=paz->low_vel;
								phz->hi_input_range.midi_vel=paz->hig_vel;
								phz->midi_coarse_tune=int(paz->semitone_tune)+int(pak->kloc.semitone_tune);
								phz->midi_fine_tune=int(paz->fine_tune)+int(pak->kloc.fine_tune);
								phz->mp_gain=paz->zone_level;
								phz->mp_pan=paz->pan_balance;
								
								// keyboard track patch
								if(!paz->kbd_track)
									phz->midi_keycents=0;
							}
						}
					}
				}
			}
		}
	
		// sample editor should adapt
		// TODO: don't adapt sample editor if Program load failed
		user_sed_adapt=1;
		user_sed_zone=0;
	
		// leave critical section
		crs_unlock();
	}
}