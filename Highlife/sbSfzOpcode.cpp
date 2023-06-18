/*-
 * SFZ PARSER for DiscoDsp HighLife VST Sampler
 *   
 * file name: scSfzOpcode.cpp
 *   
 * Copyright (c) Andreas Sumerauer 2009, discoDSP
 * All rights reserved.
 *
 * the sfz format specification is provided by  Cakewalk Inc.
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

#include "sbSfzOpcode.h"
#include <cstdlib>


int  sbSfzOpcode::eval(char * testString)
{
	ccNum = -1;
	char tempStr[64];
	strcpy(tempStr, testString);
	strlwr(tempStr);
	
	// retrieve cc number
	char *pEnd, *pChr;
	pEnd = pChr = strchr ( tempStr, 0 ) -1;
	while ( pChr > tempStr ){
		if ( *pChr < '0' || *pChr > '9' )
			break;
		else
			--pChr;
	}
	if ( pEnd != pChr ){
		++pChr;
		ccNum = atoi( pChr );
		*pChr = 0;
	}

	// retrieve opcode
	for (int idx=0 ; idx<NUM_OPCODES ; idx++){
		if (strcmp( opcode[idx], tempStr ) == 0 ){
			return idx;
		}
	}
	return -1;
}



 sbSfzOpcode:: sbSfzOpcode() : ccNum(-1)
{
strcpy( opcode[ 0 ], "sample" );
strcpy( opcode[ 1 ], "lochan" );
strcpy( opcode[ 2 ], "hichan" );
strcpy( opcode[ 3 ], "lokey" );
strcpy( opcode[ 4 ], "hikey" );
strcpy( opcode[ 5 ], "key" );
strcpy( opcode[ 6 ], "lovel" );
strcpy( opcode[ 7 ], "hivel" );
strcpy( opcode[ 8 ], "locc" );
strcpy( opcode[ 9 ], "hicc" );
strcpy( opcode[ 10 ], "lobend" );
strcpy( opcode[ 11 ], "hibend" );
strcpy( opcode[ 12 ], "lochanaft" );
strcpy( opcode[ 13 ], "hichanaft" );
strcpy( opcode[ 14 ], "lopolyaft" );
strcpy( opcode[ 15 ], "hipolyaft" );
strcpy( opcode[ 16 ], "lorand" );
strcpy( opcode[ 17 ], "hirand" );
strcpy( opcode[ 18 ], "lobpm" );
strcpy( opcode[ 19 ], "hibpm" );
strcpy( opcode[ 20 ], "seq_length" );
strcpy( opcode[ 21 ], "seq_position" );
strcpy( opcode[ 22 ], "sw_lokey" );
strcpy( opcode[ 23 ], "sw_hikey" );
strcpy( opcode[ 24 ], "sw_last" );
strcpy( opcode[ 25 ], "sw_down" );
strcpy( opcode[ 26 ], "sw_up" );
strcpy( opcode[ 27 ], "sw_previous" );
strcpy( opcode[ 28 ], "sw_vel" );
strcpy( opcode[ 29 ], "trigger" );
strcpy( opcode[ 30 ], "group" );
strcpy( opcode[ 31 ], "off_by" );
strcpy( opcode[ 32 ], "off_mode" );
strcpy( opcode[ 33 ], "on_locc" );
strcpy( opcode[ 34 ], "on_hicc" );
strcpy( opcode[ 35 ], "delay" );
strcpy( opcode[ 36 ], "delay_random" );
strcpy( opcode[ 37 ], "delay_cc" );
strcpy( opcode[ 38 ], "offset" );
strcpy( opcode[ 39 ], "offset_random" );
strcpy( opcode[ 40 ], "offset_cc" );
strcpy( opcode[ 41 ], "end" );
strcpy( opcode[ 42 ], "count" );
strcpy( opcode[ 43 ], "loop_mode" );
strcpy( opcode[ 44 ], "loop_start" );
strcpy( opcode[ 45 ], "loop_end" );
strcpy( opcode[ 46 ], "sync_beats" );
strcpy( opcode[ 47 ], "sync_offset" );
strcpy( opcode[ 48 ], "transpose" );
strcpy( opcode[ 49 ], "tune" );
strcpy( opcode[ 50 ], "pitch_keycenter" );
strcpy( opcode[ 51 ], "pitch_keytrack" );
strcpy( opcode[ 52 ], "pitch_veltrack" );
strcpy( opcode[ 53 ], "pitch_random" );
strcpy( opcode[ 54 ], "bend_up" );
strcpy( opcode[ 55 ], "bend_down" );
strcpy( opcode[ 56 ], "bend_step" );
strcpy( opcode[ 57 ], "pitcheg_delay" );
strcpy( opcode[ 58 ], "pitcheg_start" );
strcpy( opcode[ 59 ], "pitcheg_attack" );
strcpy( opcode[ 60 ], "pitcheg_hold" );
strcpy( opcode[ 61 ], "pitcheg_decay" );
strcpy( opcode[ 62 ], "pitcheg_sustain" );
strcpy( opcode[ 63 ], "pitcheg_release" );
strcpy( opcode[ 64 ], "pitcheg_depth" );
strcpy( opcode[ 65 ], "pitcheg_vel2delay" );
strcpy( opcode[ 66 ], "pitcheg_vel2attack" );
strcpy( opcode[ 67 ], "pitcheg_vel2hold" );
strcpy( opcode[ 68 ], "pitcheg_vel2decay" );
strcpy( opcode[ 69 ], "pitcheg_vel2sustain" );
strcpy( opcode[ 70 ], "pitcheg_vel2release" );
strcpy( opcode[ 71 ], "pitcheg_vel2depth" );
strcpy( opcode[ 72 ], "pitchlfo_delay" );
strcpy( opcode[ 73 ], "pitchlfo_fade" );
strcpy( opcode[ 74 ], "pitchlfo_freq" );
strcpy( opcode[ 75 ], "pitchlfo_depth" );
strcpy( opcode[ 76 ], "pitchlfo_depthcc" );
strcpy( opcode[ 77 ], "pitchlfo_depthchanaft" );
strcpy( opcode[ 78 ], "pitchlfo_depthpolyaft" );
strcpy( opcode[ 79 ], "pitchlfo_freqcc" );
strcpy( opcode[ 80 ], "pitchlfo_freqchanaft" );
strcpy( opcode[ 81 ], "pitchlfo_freqpolyaft" );
strcpy( opcode[ 82 ], "fil_type" );
strcpy( opcode[ 83 ], "cutoff" );
strcpy( opcode[ 84 ], "cutoff_cc" );
strcpy( opcode[ 85 ], "cutoff_chanaft" );
strcpy( opcode[ 86 ], "cutoff_polyaft" );
strcpy( opcode[ 87 ], "resonance" );
strcpy( opcode[ 88 ], "fil_keytrack" );
strcpy( opcode[ 89 ], "fil_keycenter" );
strcpy( opcode[ 90 ], "fil_veltrack" );
strcpy( opcode[ 91 ], "fil_random" );
strcpy( opcode[ 92 ], "fileg_delay" );
strcpy( opcode[ 93 ], "fileg_start" );
strcpy( opcode[ 94 ], "fileg_attack" );
strcpy( opcode[ 95 ], "fileg_hold" );
strcpy( opcode[ 96 ], "fileg_decay" );
strcpy( opcode[ 97 ], "fileg_sustain" );
strcpy( opcode[ 98 ], "fileg_release" );
strcpy( opcode[ 99 ], "fileg_depth" );
strcpy( opcode[ 100 ], "fileg_vel2delay" );
strcpy( opcode[ 101 ], "fileg_vel2attack" );
strcpy( opcode[ 102 ], "fileg_vel2hold" );
strcpy( opcode[ 103 ], "fileg_vel2decay" );
strcpy( opcode[ 104 ], "fileg_vel2sustain" );
strcpy( opcode[ 105 ], "fileg_vel2release" );
strcpy( opcode[ 106 ], "fileg_vel2depth" );
strcpy( opcode[ 107 ], "fillfo_delay" );
strcpy( opcode[ 108 ], "fillfo_fade" );
strcpy( opcode[ 109 ], "fillfo_freq" );
strcpy( opcode[ 110 ], "fillfo_depth" );
strcpy( opcode[ 111 ], "fillfo_depthcc" );
strcpy( opcode[ 112 ], "fillfo_depthchanaft" );
strcpy( opcode[ 113 ], "fillfo_depthpolyaft" );
strcpy( opcode[ 114 ], "fillfo_freqcc" );
strcpy( opcode[ 115 ], "fillfo_freqchanaft" );
strcpy( opcode[ 116 ], "fillfo_freqpolyaft" );
strcpy( opcode[ 117 ], "volume" );
strcpy( opcode[ 118 ], "pan" );
strcpy( opcode[ 119 ], "width" );
strcpy( opcode[ 120 ], "position" );
strcpy( opcode[ 121 ], "amp_keytrack" );
strcpy( opcode[ 122 ], "amp_keycenter" );
strcpy( opcode[ 123 ], "amp_veltrack" );
strcpy( opcode[ 124 ], "amp_velcurve_1" );
strcpy( opcode[ 125 ], "amp_velcurve_127" );
strcpy( opcode[ 126 ], "amp_random" );
strcpy( opcode[ 127 ], "rt_decay" );
strcpy( opcode[ 128 ], "output" );
strcpy( opcode[ 129 ], "gain_cc" );
strcpy( opcode[ 130 ], "xfin_lokey" );
strcpy( opcode[ 131 ], "xfin_hikey" );
strcpy( opcode[ 132 ], "xfout_lokey" );
strcpy( opcode[ 133 ], "xfout_hikey" );
strcpy( opcode[ 134 ], "xf_keycurve" );
strcpy( opcode[ 135 ], "xfin_lovel" );
strcpy( opcode[ 136 ], "xfin_hivel" );
strcpy( opcode[ 137 ], "xfout_lovel" );
strcpy( opcode[ 138 ], "xfout_hivel" );
strcpy( opcode[ 139 ], "xf_velcurve" );
strcpy( opcode[ 140 ], "xfin_locc" );
strcpy( opcode[ 141 ], "xfin_hicc" );
strcpy( opcode[ 142 ], "xfout_locc" );
strcpy( opcode[ 143 ], "xfout_hicc" );
strcpy( opcode[ 144 ], "xf_cccurve" );
strcpy( opcode[ 145 ], "ampeg_delay" );
strcpy( opcode[ 146 ], "ampeg_start" );
strcpy( opcode[ 147 ], "ampeg_attack" );
strcpy( opcode[ 148 ], "ampeg_hold" );
strcpy( opcode[ 149 ], "ampeg_decay" );
strcpy( opcode[ 150 ], "ampeg_sustain" );
strcpy( opcode[ 151 ], "ampeg_release" );
strcpy( opcode[ 152 ], "ampeg_vel2delay" );
strcpy( opcode[ 153 ], "ampeg_vel2attack" );
strcpy( opcode[ 154 ], "ampeg_vel2hold" );
strcpy( opcode[ 155 ], "ampeg_vel2decay" );
strcpy( opcode[ 156 ], "ampeg_vel2sustain" );
strcpy( opcode[ 157 ], "ampeg_vel2release" );
strcpy( opcode[ 158 ], "ampeg_delaycc" );
strcpy( opcode[ 159 ], "ampeg_startcc" );
strcpy( opcode[ 160 ], "ampeg_attackcc" );
strcpy( opcode[ 161 ], "ampeg_holdcc" );
strcpy( opcode[ 162 ], "ampeg_decaycc" );
strcpy( opcode[ 163 ], "ampeg_sustaincc" );
strcpy( opcode[ 164 ], "ampeg_releasecc" );
strcpy( opcode[ 165 ], "amplfo_delay" );
strcpy( opcode[ 166 ], "amplfo_fade" );
strcpy( opcode[ 167 ], "amplfo_freq" );
strcpy( opcode[ 168 ], "amplfo_depth" );
strcpy( opcode[ 169 ], "amplfo_depthcc" );
strcpy( opcode[ 170 ], "amplfo_depthchanaft" );
strcpy( opcode[ 171 ], "amplfo_depthpolyaft" );
strcpy( opcode[ 172 ], "amplfo_freqcc" );
strcpy( opcode[ 173 ], "amplfo_freqchanaft" );
strcpy( opcode[ 174 ], "amplfo_freqpolyaft" );
strcpy( opcode[ 175 ], "eq1_freq" );
strcpy( opcode[ 176 ], "eq2_freq" );
strcpy( opcode[ 177 ], "eq3_freq" );
strcpy( opcode[ 178 ], "eq1_freqcc" );
strcpy( opcode[ 179 ], "eq2_freqcc" );
strcpy( opcode[ 180 ], "eq3_freqcc" );
strcpy( opcode[ 181 ], "eq1_vel2freq" );
strcpy( opcode[ 182 ], "eq2_vel2freq" );
strcpy( opcode[ 183 ], "eq3_vel2freq" );
strcpy( opcode[ 184 ], "eq1_bw" );
strcpy( opcode[ 185 ], "eq2_bw" );
strcpy( opcode[ 186 ], "eq3_bw" );
strcpy( opcode[ 187 ], "eq1_bwcc" );
strcpy( opcode[ 188 ], "eq2_bwcc" );
strcpy( opcode[ 189 ], "eq3_bwcc" );
strcpy( opcode[ 190 ], "eq1_gain" );
strcpy( opcode[ 191 ], "eq2_gain" );
strcpy( opcode[ 192 ], "eq3_gain" );
strcpy( opcode[ 193 ], "eq1_gaincc" );
strcpy( opcode[ 194 ], "eq2_gaincc" );
strcpy( opcode[ 195 ], "eq3_gaincc" );
strcpy( opcode[ 196 ], "eq1_vel2gain" );
strcpy( opcode[ 197 ], "eq2_vel2gain" );
strcpy( opcode[ 198 ], "eq3_vel2gain" );
strcpy( opcode[ 199 ], "effect1" );
strcpy( opcode[ 200 ], "effect2" );
}