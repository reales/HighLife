
#ifndef __JX_STREAM_H
#define __JX_STREAM_H


#include <assert.h>
#include <string.h>
#include "jxdef.h"

class CStreamIn
{
public:
	virtual void ReadRaw(void *pbuf, dword const numbytes) = 0;
	virtual ~CStreamIn() {}
	
	virtual void SeekAbs(qword const pos) = 0;
	virtual void Seek(__int64 const ofs) = 0;
	virtual qword GetSize() = 0;
	virtual qword GetPos() = 0;
	
	

	void Read(int &d) { ReadRaw(&d, sizeof(int)); }
	void Read(qword &d) { ReadRaw(&d, sizeof(qword)); }
	void Read(dword &d) { ReadRaw(&d, sizeof(dword)); }
	void Read(short &d) { ReadRaw(&d, sizeof(short)); }
	void Read(word &d) { ReadRaw(&d, sizeof(word)); }
	void Read(char &d) { ReadRaw(&d, sizeof(char)); }
	void Read(byte &d) { ReadRaw(&d, sizeof(byte)); }
	void Read(float &d) { ReadRaw(&d, sizeof(float)); }
	void Read(double &d) { ReadRaw(&d, sizeof(double)); }
	void Read(bool &d) { ReadRaw(&d, sizeof(bool)); }


};

class CStreamOut
{
public:
	virtual void WriteRaw(void const *pbuf, dword const numbytes) = 0;
	virtual ~CStreamOut() {}
	
	void Write(int d) { WriteRaw(&d, sizeof(int)); }
	void Write(qword d) { WriteRaw(&d, sizeof(qword)); }
	void Write(dword d) { WriteRaw(&d, sizeof(dword)); }
	void Write(short d) { WriteRaw(&d, sizeof(short)); }
	void Write(word d) { WriteRaw(&d, sizeof(word)); }
	void Write(char d) { WriteRaw(&d, sizeof(char)); }
	void Write(byte d) { WriteRaw(&d, sizeof(byte)); }
	void Write(float d) { WriteRaw(&d, sizeof(float)); }
	void Write(double d) { WriteRaw(&d, sizeof(double)); }
	void Write(bool d) { WriteRaw(&d, sizeof(bool)); }
	void Write(char const *str) { WriteRaw((void *)str, strlen(str)+1); }

};


class CMemStreamIn : public CStreamIn
{

public:
	CMemStreamIn(void *p, dword const s) { assert(p != NULL); Data = (byte *)p; Size = s; Pos = 0; }
	virtual ~CMemStreamIn() {}

	virtual void ReadRaw(void *pbuf, dword const numbytes)
	{
		memcpy(pbuf, Data + Pos, numbytes);
		Pos += numbytes;
	}

	virtual void SeekAbs(qword const p) { Pos = (dword)p; }
	virtual void Seek(__int64 const o) { Pos += (int)o; }
	
	virtual qword GetSize() { return Size; }
	virtual qword GetPos() { return Pos; }

	byte *GetData() { return Data; }

protected:
	byte *Data;
	dword Pos;
	dword Size;

};

class CMemStreamOut : public CStreamOut
{
public:
	CMemStreamOut(void *p) { SetPtr(p); }
	void SetPtr(void *p) { assert(p != NULL); ptr = (byte *)p; }

	virtual void WriteRaw(void const *pbuf, dword const numbytes)
	{
		memcpy(ptr, pbuf, numbytes);
		ptr += numbytes;
	}

protected:
	byte *ptr;

};



class CAllocatingMemStreamOut : public CStreamOut
{
public:
	CAllocatingMemStreamOut() 
	{ 
		Size = 0;
		Capacity = 4;
		Data = new byte [Capacity];

	}
	
	virtual ~CAllocatingMemStreamOut()
	{
		delete[] Data;
	}
	
	void Reset()
	{
		delete[] Data;
		Size = 0;
		Capacity = 4;
		Data = new byte [Capacity];
	}
	
	virtual void WriteRaw(void const *pbuf, dword const numbytes)
	{
		while(Size + numbytes > Capacity)
		{
			dword const newcap = Capacity * 2;
			byte *newdata = new byte [newcap];
			memcpy(newdata, Data, Size);
			delete[] Data;
			
			Data = newdata;
			Capacity = newcap;
		}
		
		memcpy(Data + Size, pbuf, numbytes);
		Size += numbytes;
	}
	
	byte *GetData() const { return Data; }
	dword GetSize() const { return Size; }
	
	
protected:
	byte *Data;
	dword Size;
	dword Capacity;
	
};


#endif