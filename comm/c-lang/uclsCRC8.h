#pragma once
#define COMMAND_LENGTH	8

class CuclsCRC8
{
public:
	CuclsCRC8(void);
	~CuclsCRC8(void);

	int Command_length(){return COMMAND_LENGTH;};

	byte				Compute_CRC8(byte bt0, byte bt1, byte bt2);
	byte				Compute_CRC8(byte bt0, byte bt1, byte bt2, byte bt3, byte bt4, byte bt5, byte bt6, byte bt7);
	byte				Compute_CRC8(byte* bt, int Soffset, int Eoffset);
	const byte* const	Read_Current_Data();
};
