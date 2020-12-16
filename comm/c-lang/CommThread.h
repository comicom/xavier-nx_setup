// CommThread.h: interface for the CCommThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMTHREAD_H__E90CA7C3_844A_433E_88C0_6B6348B0ADDB__INCLUDED_)
#define AFX_COMMTHREAD_H__E90CA7C3_844A_433E_88C0_6B6348B0ADDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFF_SIZE			4096
#define WM_COMM_READ (WM_USER +1)


class CQueue
{
public:
	unsigned char buff[BUFF_SIZE];//ť ���� ������� 4192�� ���� 
	int m_iHead, m_iTail;//ť ���ۿ� �����͸� �ְ� �� �ö� ����� ���� 
	CQueue();
	void Clear();//���۸� �ʱ�ȭ ��Ŵ
	int GetSize();//���� ���ۿ� ����ִ� �������� size�� ����
	BOOL PutByte(unsigned char b);//ť���ۿ� 1����Ʈ�� ����
	BOOL GetByte(unsigned char *pb);//ť���ۿ��� 1����Ʈ�� �� ��
	int nowdata ;
	int GetNow(){return nowdata;}
};

class CCommThread  
{
public:
	CCommThread();
	virtual ~CCommThread();
	BOOL		OpenPort(CString PortName,DWORD BaudRate);
#if 1	// gyj :: com3(rs-485 ����) ��� �߰�..	:: send�� ���� �ϴ� ���μ�����  recv���� thread�� �����ϸ� �ȵ�.   ��  data�� �����Ŀ�,  ����mode�� ��ȯ�Ŀ�,  ������ ��ٷ��� �Ѵ�.
	BOOL		m_testRecv;
#endif
	BOOL        check;
	HANDLE		m_hComm;				// ��� ��Ʈ ���� �ڵ�
	CString		m_sPortName;			// ��Ʈ �̸� (COM1 ..)
	BOOL		m_bConnected;			// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	OVERLAPPED	m_osRead, m_osWrite;	// ��Ʈ ���� Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch�Լ� Thread �ڵ�.
	WORD		m_wPortID;				// WM_COMM_READ�� �Բ� ������ �μ�.
	CQueue      m_QueueRead;			//ť����
	//--------- �ܺ� ��� �Լ� ------------------------------------//
	void	ClosePort();				//��Ʈ �ݱ�
	DWORD	WriteComm( BYTE *pBuff, DWORD nToWrite);//��Ʈ�� ������ ����

	//--------- ���� ��� �Լ� ------------------------------------//
	DWORD	ReadComm( BYTE *pBuff, DWORD nToRead);//��Ʈ���� ������ �о����

};

// Thread�� ����� �Լ� 
DWORD	ThreadWatchComm(CCommThread* pComm);

#endif // !defined(AFX_COMMTHREAD_H__E90CA7C3_844A_433E_88C0_6B6348B0ADDB__INCLUDED_)
