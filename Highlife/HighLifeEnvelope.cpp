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
// HighLife Envelope Implementation                                                                                                    //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlifeenvelope.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeEnvelope::CHighLifeEnvelope(void)
{
	value=0.0f;
	stage=0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeEnvelope::~CHighLifeEnvelope(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLifeEnvelope::work(float const atr,float const der,float const sul,float const rer)
{
	// attack
	if(stage==1)
	{
		value+=atr;

		if(value>=1.0f)
		{
			value=1.0f;
			stage=2;
		}

		return value*value*value*value;
	}

	// decay 1
	if(stage==2)
	{
		value-=der;

		if(value<=sul)
		{
			value=sul;
			stage=3;
		}

		return value*value*value*value;
	}

	// sustain
	if(stage==3)
		return value*value*value*value;

	// release
	if(stage==4)
	{
		value-=rer;

		if(value<=0.0f)
		{
			value=0.0f;
			stage=0;
		}

		return value*value*value*value;
	}

	// return none
	return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeEnvelope::trigger(void)
{
	value=0.0f;
	stage=1;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeEnvelope::release(void)
{
	if(stage)
		stage=4;
}