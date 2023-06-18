/*-
 * SFZ PARSER for DiscoDsp HighLife VST Sampler
 *   
 * file name: scSfzOpcode.h
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

#include <cstring>


class  sbSfzOpcode
{
public:
	 sbSfzOpcode();
	int ccNum;
	int eval( char* testString);
private:
	char opcode[201][24];
};

#define NUM_OPCODES 201

#define OP_INVALID		-1

#define OP_SAMPLE		0
#define OP_LOCHAN		1
#define OP_HICHAN		2
#define OP_LOKEY		3
#define OP_HIKEY		4
#define OP_KEY			5
#define OP_LOVEL		6
#define OP_HIVEL		7
#define OP_LOCCN		8
#define OP_HICCN		9
#define OP_LOBEND		10
#define OP_HIBEND		11
#define OP_LOCHANAFT	12
#define OP_HICHANAFT	13
#define OP_LOPOLYAFT	14
#define OP_HIPOLYAFT	15
#define OP_LORAND		16
#define OP_HIRAND		17
#define OP_LOBPM		18
#define OP_HIBPM		19
#define OP_SEQ_LENGTH	20
#define OP_SEQ_POSITION	21
#define OP_SW_LOKEY		22
#define OP_SW_HIKEY		23
#define OP_SW_LAST		24
#define OP_SW_DOWN		25
#define OP_SW_UP		26
#define OP_SW_PREVIOUS	27
#define OP_SW_VEL		28
#define OP_TRIGGER		29
#define OP_GROUP		30
#define OP_OFF_BY		31
#define OP_OFF_MODE		32
#define OP_ON_LOCCN		33
#define OP_ON_HICCN		34
#define OP_DELAY		35
#define OP_DELAY_RANDOM	36
#define OP_DELAY_CCN	37
#define OP_OFFSET	38
#define OP_OFFSET_RANDOM	39
#define OP_OFFSET_CCN	40
#define OP_END			41
#define OP_COUNT		42
#define OP_LOOP_MODE	43
#define OP_LOOP_START	44
#define OP_LOOP_END		45
#define OP_SYNC_BEATS	46
#define OP_SYNC_OFFSET	47
#define OP_TRANSPOSE	48
#define OP_TUNE			49
#define OP_PITCH_KEYCENTER	50
#define OP_PITCH_KEYTRACK	51
#define OP_PITCH_VELTRACK	52
#define OP_PITCH_RANDOM	53
#define OP_BEND_UP		54
#define OP_BEND_DOWN	55
#define OP_BEND_STEP	56
#define OP_PITCHEG_DELAY	57
#define OP_PITCHEG_START	58
#define OP_PITCHEG_ATTACK	59
#define OP_PITCHEG_HOLD	60
#define OP_PITCHEG_DECAY	61
#define OP_PITCHEG_SUSTAIN	62
#define OP_PITCHEG_RELEASE	63
#define OP_PITCHEG_DEPTH	64
#define OP_PITCHEG_VEL2DELAY	65
#define OP_PITCHEG_VEL2ATTACK	66
#define OP_PITCHEG_VEL2HOLD	67
#define OP_PITCHEG_VEL2DECAY	68
#define OP_PITCHEG_VEL2SUSTAIN	69
#define OP_PITCHEG_VEL2RELEASE	70
#define OP_PITCHEG_VEL2DEPTH	71
#define OP_PITCHLFO_DELAY	72
#define OP_PITCHLFO_FADE	73
#define OP_PITCHLFO_FREQ	74
#define OP_PITCHLFO_DEPTH	75
#define OP_PITCHLFO_DEPTHCCN	76
#define OP_PITCHLFO_DEPTHCHANAFT	77
#define OP_PITCHLFO_DEPTHPOLYAFT	78
#define OP_PITCHLFO_FREQCCN	79
#define OP_PITCHLFO_FREQCHANAFT	80
#define OP_PITCHLFO_FREQPOLYAFT	81
#define OP_FIL_TYPE		82
#define OP_CUTOFF		83
#define OP_CUTOFF_CCN	84
#define OP_CUTOFF_CHANAFT	85
#define OP_CUTOFF_POLYAFT	86
#define OP_RESONANCE	87
#define OP_FIL_KEYTRACK	88
#define OP_FIL_KEYCENTER	89
#define OP_FIL_VELTRACK	90
#define OP_FIL_RANDOM	91
#define OP_FILEG_DELAY	92
#define OP_FILEG_START	93
#define OP_FILEG_ATTACK	94
#define OP_FILEG_HOLD	95
#define OP_FILEG_DECAY	96
#define OP_FILEG_SUSTAIN	97
#define OP_FILEG_RELEASE	98
#define OP_FILEG_DEPTH	99
#define OP_FILEG_VEL2DELAY	100
#define OP_FILEG_VEL2ATTACK	101
#define OP_FILEG_VEL2HOLD	102
#define OP_FILEG_VEL2DECAY	103
#define OP_FILEG_VEL2SUSTAIN	104
#define OP_FILEG_VEL2RELEASE	105
#define OP_FILEG_VEL2DEPTH	106
#define OP_FILLFO_DELAY	107
#define OP_FILLFO_FADE	108
#define OP_FILLFO_FREQ	109
#define OP_FILLFO_DEPTH	110
#define OP_FILLFO_DEPTHCCN	111
#define OP_FILLFO_DEPTHCHANAFT	112
#define OP_FILLFO_DEPTHPOLYAFT	113
#define OP_FILLFO_FREQCCN	114
#define OP_FILLFO_FREQCHANAFT	115
#define OP_FILLFO_FREQPOLYAFT	116
#define OP_VOLUME		117
#define OP_PAN			118
#define OP_WIDTH		119
#define OP_POSITION		120
#define OP_AMP_KEYTRACK	121
#define OP_AMP_KEYCENTER	122
#define OP_AMP_VELTRACK	123
#define OP_AMP_VELCURVE_1	124
#define OP_AMP_VELCURVE_127	125
#define OP_AMP_RANDOM	126
#define OP_RT_DECAY		127
#define OP_OUTPUT		128
#define OP_GAIN_CCN		129
#define OP_XFIN_LOKEY	130
#define OP_XFIN_HIKEY	131
#define OP_XFOUT_LOKEY	132
#define OP_XFOUT_HIKEY	133
#define OP_XF_KEYCURVE	134
#define OP_XFIN_LOVEL	135
#define OP_XFIN_HIVEL	136
#define OP_XFOUT_LOVEL	137
#define OP_XFOUT_HIVEL	138
#define OP_XF_VELCURVE	139
#define OP_XFIN_LOCCN	140
#define OP_XFIN_HICCN	141
#define OP_XFOUT_LOCCN	142
#define OP_XFOUT_HICCN	143
#define OP_XF_CCCURVE	144
#define OP_AMPEG_DELAY	145
#define OP_AMPEG_START	146
#define OP_AMPEG_ATTACK	147
#define OP_AMPEG_HOLD	148
#define OP_AMPEG_DECAY	149
#define OP_AMPEG_SUSTAIN	150
#define OP_AMPEG_RELEASE	151
#define OP_AMPEG_VEL2DELAY	152
#define OP_AMPEG_VEL2ATTACK	153
#define OP_AMPEG_VEL2HOLD	154
#define OP_AMPEG_VEL2DECAY	155
#define OP_AMPEG_VEL2SUSTAIN	156
#define OP_AMPEG_VEL2RELEASE	157
#define OP_AMPEG_DELAYCCN	158
#define OP_AMPEG_STARTCCN	159
#define OP_AMPEG_ATTACKCCN	160
#define OP_AMPEG_HOLDCCN	161
#define OP_AMPEG_DECAYCCN	162
#define OP_AMPEG_SUSTAINCCN	163
#define OP_AMPEG_RELEASECCN	164
#define OP_AMPLFO_DELAY	165
#define OP_AMPLFO_FADE	166
#define OP_AMPLFO_FREQ	167
#define OP_AMPLFO_DEPTH	168
#define OP_AMPLFO_DEPTHCCN	169
#define OP_AMPLFO_DEPTHCHANAFT	170
#define OP_AMPLFO_DEPTHPOLYAFT	171
#define OP_AMPLFO_FREQCCN	172
#define OP_AMPLFO_FREQCHANAFT	173
#define OP_AMPLFO_FREQPOLYAFT	174
#define OP_EQ1_FREQ		175
#define OP_EQ2_FREQ		176
#define OP_EQ3_FREQ		177
#define OP_EQ1_FREQCCN	178
#define OP_EQ2_FREQCCN	179
#define OP_EQ3_FREQCCN	180
#define OP_EQ1_VEL2FREQ	181
#define OP_EQ2_VEL2FREQ	182
#define OP_EQ3_VEL2FREQ	183
#define OP_EQ1_BW		184
#define OP_EQ2_BW		185
#define OP_EQ3_BW		186
#define OP_EQ1_BWCCN	187
#define OP_EQ2_BWCCN	188
#define OP_EQ3_BWCCN	189
#define OP_EQ1_GAIN		190
#define OP_EQ2_GAIN		191
#define OP_EQ3_GAIN		192
#define OP_EQ1_GAINCCN	193
#define OP_EQ2_GAINCCN	194
#define OP_EQ3_GAINCCN	195
#define OP_EQ1_VEL2GAIN	196
#define OP_EQ2_VEL2GAIN	197
#define OP_EQ3_VEL2GAIN	198
#define OP_EFFECT1	199
#define OP_EFFECT2	200

#define TRIG_ATTACK 0
#define TRIG_RELEASE 1
#define TRIG_FIRST 2
#define TRIG_LEGATO 3


