#include "StdAfx.h"
#include "windows.h"
#include "uclsCRC8.h"

#define COMMAND_LENGTH	8

CuclsCRC8::CuclsCRC8(void)
{
}

CuclsCRC8::~CuclsCRC8(void)
{
}

byte CuclsCRC8::Compute_CRC8(byte* bt, int Soffset, int Eoffset){
	byte crc = 0;
    byte b = 0;
    byte data = 0;

	for(int i = 0; i<=(Eoffset-Soffset); i++){
		b = bt[i+Soffset];
		data = (byte)(b ^ crc);
		crc = (byte)crc_table[data];
	}

	return crc;
}

byte CuclsCRC8::Compute_CRC8(byte bt0, byte bt1, byte bt2){
	byte crc = 0;
    byte b = 0;
    byte data = 0;

    b = bt0;
    data = (byte)(b ^ crc);
    crc = (byte)crc_table[data];

    b = bt1;
    data = (byte)(b ^ crc);
    crc = (byte)crc_table[data];

    b = bt2;
    data = (byte)(b ^ crc);
    crc = (byte)crc_table[data];

    return crc;
}

const byte* const CuclsCRC8::Read_Current_Data(){
	byte tmpBt = 0;
	byte arrRet[COMMAND_LENGTH];

	for (int i=0;i<COMMAND_LENGTH;i++){
		arrRet[i] = CMD1723B_READ_CURRENT_DATA[i];}

	tmpBt = Compute_CRC8(arrRet[2], arrRet[3], arrRet[4]);
	arrRet[5] = tmpBt;

	byte *arrRetPtr = arrRet;
	return arrRetPtr;
}

