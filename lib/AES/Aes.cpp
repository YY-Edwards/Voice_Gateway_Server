#include "StdAfx.h"
#include "Aes.h"

Aes1::Aes1(void)
: Nb(0)
, Nk(0)
, Nr(0)
{
}

Aes1::~Aes1(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
Aes1::Aes1(int keysize1,unsigned char* keyBytes)
{
	SetNbNkNr(keysize1);                         //设置密钥块数，轮数 
	memcpy(key,keyBytes,keysize1);				//字符串拷贝函数，把keyBytes的keysize个字符复制到key中
	KeyExpansion();								//密钥扩展，必须提前做的初始化
}
void Aes1::SetNbNkNr(int keySize1)
{
	Nb=4;
	if(keySize1==Bits128)
	{
		Nk=4;    //4*4字节，128位密钥，10轮加密
		Nr=10;
	}
	else if(keySize1==Bits192)
	{
		Nk=6;    //6*4字节，192位密钥，12轮加密
		Nr=12;
	}
	else if(keySize1==Bits256)
	{
		Nk=8;    //8*4字节，256位密钥，14轮加密
		Nr=14;
	}
}

void Aes1::Cipher(unsigned char* input, unsigned char* output)//加密函数
{
	memset(&State[0][0],0,16);
	for( int i=0;i<4*Nb;i++)                        //这里是先写列后写行的，因为输入是一列一列的进来的
	{
		State[i%4][i/4]=input[i];					//换成先写行后写列也是可以的，只要在输出时也是这样就可以了
	}
	AddRoundKey(0);									//读入轮密钥
	
	for (int round = 1; round <= (Nr - 1); round++)  
	{
        SubBytes();									//字节代换
        ShiftRows();								//行移位
        MixColumns();								//列混淆
        AddRoundKey(round);							//读入轮密钥
	}
	
	SubBytes();										//字节代换
	ShiftRows();									//行移位
	AddRoundKey(Nr);								//读入轮密钥
	
	// output = state
	for ( int i = 0; i < (4 * Nb); i++)
	{
        output[i] =  State[i % 4][ i / 4];
	}

}

void Aes1::InvCipher(unsigned char* input, unsigned char* output)//解密函数
{
	memset(&State[0][0],0,16);
	for (int  i = 0; i < (4 * Nb); i++)
	{
		State[i % 4][ i / 4] = input[i];
	}
	
	AddRoundKey(Nr);
	
	for (int round = Nr-1; round >= 1; round--)  // main round loop
	{
        InvShiftRows();
        InvSubBytes();
        AddRoundKey(round);
        InvMixColumns();
	}
	
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);
	
	// output = state
	for (int i = 0; i < (4 * Nb); i++)
	{
        output[i] =  State[i % 4][ i / 4];
	}
}

void Aes1::AddRoundKey(int round)//读入轮密钥
{
	int i,j;  //i行 j列           //因为密钥w是一列一列排列的，即 k0 k4 k8 k12
	for(j=0;j<4;j++)			  //							  k1 k5 k9 k13
	{							  //							  k2 k6 k10k14
		for(i=0;i<4;i++)		  //							  k3 k7 k11k15
		{						  // 所以i行j列的下标是4*((round*4)+j)+i即16*round+4*j+i
			State[i][j]=(unsigned char)((int)State[i][j]^(int)w[4*((round*4)+j)+i]);  
		}
	}
}

void Aes1::SubBytes(void)  //字节代换函数
{
	int i,j;
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			State[i][j]=Sbox[State[i][j]];
			//因为 16*(State[i][j]>>4)+State[i][j]&0x0f=State[i][j]


		}
	}
}

void Aes1::InvSubBytes(void)  //字节代换函数
{
	int i,j;
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			State[i][j]=iSbox[State[i][j]]; //因为 16*(State[i][j]>>4)+State[i][j]&0x0f=State[i][j]
		}
	}

}

void Aes1::ShiftRows(void)//行移位
{
	unsigned char temp[4*4];                                       
	int i,j;
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			temp[4*i+j]=State[i][j];
		}
	}
	for(i=1;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(i==1)State[i][j]=temp[4*i+(j+1)%4];					//第一行左移1位
			else if(i==2)State[i][j]=temp[4*i+(j+2)%4];				//第二行左移2位
			else if(i==3)State[i][j]=temp[4*i+(j+3)%4];				//第三行左移3位
		}
}
}

	void Aes1::MixColumns(void)
	{
			unsigned char temp[4*4];
	int i,j;
	for(j=0;j<4;j++)                                    //2 3 1 1  列混淆矩阵
	{													//1 2 3 1
		for(i=0;i<4;i++)								//1 1 2 3
		{												//3 1 1 2
			temp[4*i+j]=State[i][j];
		}
	}
	for(j=0;j<4;j++)
	{
		State[0][j] = (unsigned char) ( (int)gfmultby02(temp[0+j]) ^ (int)gfmultby03(temp[4*1+j]) ^
			(int)gfmultby01(temp[4*2+j]) ^ (int)gfmultby01(temp[4*3+j]) );
		State[1][j] = (unsigned char) ( (int)gfmultby01(temp[0+j]) ^ (int)gfmultby02(temp[4*1+j]) ^
			(int)gfmultby03(temp[4*2+j]) ^ (int)gfmultby01(temp[4*3+j]) );
		State[2][j] = (unsigned char) ( (int)gfmultby01(temp[0+j]) ^ (int)gfmultby01(temp[4*1+j]) ^
			(int)gfmultby02(temp[4*2+j]) ^ (int)gfmultby03(temp[4*3+j]) );
		State[3][j] = (unsigned char) ( (int)gfmultby03(temp[0+j]) ^ (int)gfmultby01(temp[4*1+j]) ^
			(int)gfmultby01(temp[4*2+j]) ^ (int)gfmultby02(temp[4*3+j]) );
	}
	

	}

	void Aes1::InvShiftRows(void)
	{
		unsigned char temp[4*4];
	int i,j;
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			temp[4*i+j]=State[i][j];
		}
	}
	for(i=1;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			//if(i==1)State[i][j]=temp[4*i+(j-1)%4];    在此犯了一个错误 -1%4=-1 而不是3，所以采用了下面再加一个4的做法
			if(i==1)State[i][j]=temp[4*i+(j+3)%4];			//第一行右移1位 j-1+4=j+3
			else if(i==2)State[i][j]=temp[4*i+(j+2)%4];		//第二行右移2位 j-2+4=j+2
			else if(i==3)State[i][j]=temp[4*i+(j+1)%4];		//第三行右移3位 j-3+4=j+2
		}
	}
	
	}

	void Aes1::InvMixColumns(void)
	{
		
	unsigned char temp[4*4];
	int i,j;
	for (i = 0; i < 4; i++)  // copy State into temp[]
	{
        for (j = 0; j < 4; j++)                         //0e 0b 0d 09   逆变换矩阵 
        {												//09 0e 0b 0d
			temp[4*i+j] =  State[i][j];					//0d 09 0e 0b
        }												//0b 0d 09 0e
	}
	
	for (j = 0; j < 4; j++)
	{
		State[0][j] = (unsigned char) ( (int)gfmultby0e(temp[j]) ^ (int)gfmultby0b(temp[4+j]) ^
			(int)gfmultby0d(temp[4*2+j]) ^ (int)gfmultby09(temp[4*3+j]) );
		State[1][j] = (unsigned char) ( (int)gfmultby09(temp[j]) ^ (int)gfmultby0e(temp[4+j]) ^
			(int)gfmultby0b(temp[4*2+j]) ^ (int)gfmultby0d(temp[4*3+j]) );
		State[2][j] = (unsigned char) ( (int)gfmultby0d(temp[j]) ^ (int)gfmultby09(temp[4+j]) ^
			(int)gfmultby0e(temp[4*2+j]) ^ (int)gfmultby0b(temp[4*3+j]) );
		State[3][j] = (unsigned char) ( (int)gfmultby0b(temp[j]) ^ (int)gfmultby0d(temp[4+j]) ^
			(int)gfmultby09(temp[4*2+j]) ^ (int)gfmultby0e(temp[4*3+j]) );
	}

	}

	unsigned char Aes1::gfmultby01(unsigned char b)
	{
		return b;
	}

	unsigned char Aes1::gfmultby02(unsigned char b)
	{
		if (b < 0x80)
        return (unsigned char)(int)(b <<1);
	else
        return (unsigned char)( (int)(b << 1) ^ (int)(0x1b) );
	
	}

	unsigned char Aes1::gfmultby03(unsigned char b)
	{
		return (unsigned char) ( (int)gfmultby02(b) ^ (int)b );
	}

	unsigned char Aes1::gfmultby09(unsigned char b)
	{
		return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^ (int)b );
	}

	unsigned char Aes1::gfmultby0b(unsigned char b)
	{
		return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)gfmultby02(b) ^ (int)b );
	}

	unsigned char Aes1::gfmultby0d(unsigned char b)
	{
	return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)gfmultby02(gfmultby02(b)) ^ (int)(b) );
	}

	unsigned char Aes1::gfmultby0e(unsigned char b)
	{
		return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)gfmultby02(gfmultby02(b)) ^(int)gfmultby02(b) );
	}


	//密钥扩展，负责把输入的密钥转化成所需的Nr+1个128为轮密钥
    //先移位，再代换，最后和轮常量异或

	void Aes1::KeyExpansion(void)
	{//void *memset( void *dest, int c, size_t count );
	//The memset function sets the first count bytes of dest to the character c.
	memset(w,0,16*15);
	for( int row=0;row<Nk;row++)       //拷贝密钥
	{
		w[4*row+0] =  key[4*row];
		w[4*row+1] =  key[4*row+1];
		w[4*row+2] =  key[4*row+2];
		w[4*row+3] =  key[4*row+3];
	}
	byte* temp = new byte[4];
	for( int row=Nk;row<4*(Nr+1);row++)
	{
		temp[0]=w[4*row-4];     //temp装入当前列的前一列，当前列待填入  
		temp[1]=w[4*row-3];
		temp[2]=w[4*row-2];
		temp[3]=w[4*row-1];
		if(row%Nk==0)           //逢nk时，对当前列的前一列作特殊处理
		{
			temp=SubWord(RotWord(temp));  //先移位，再S代换
			//与轮常量异或
			temp[0] = (byte)( (int)temp[0] ^ (int) Rcon[4*(row/Nk)+0] );   
			temp[1] = (byte)( (int)temp[1] ^ (int) Rcon[4*(row/Nk)+1] );
			temp[2] = (byte)( (int)temp[2] ^ (int) Rcon[4*(row/Nk)+2] );
			temp[3] = (byte)( (int)temp[3] ^ (int) Rcon[4*(row/Nk)+3] );
        }
		else if ( Nk > 6 && (row % Nk == 4) )   //课本135页
        {
			temp = SubWord(temp);
        }
        
        // w[row] = w[row-Nk] xor temp
        w[4*row+0] = (byte) ( (int) w[4*(row-Nk)+0] ^ (int)temp[0] );
		w[4*row+1] = (byte) ( (int) w[4*(row-Nk)+1] ^ (int)temp[1] );
		w[4*row+2] = (byte) ( (int) w[4*(row-Nk)+2] ^ (int)temp[2] );
		w[4*row+3] = (byte) ( (int) w[4*(row-Nk)+3] ^ (int)temp[3] );
	}  // for loop
	

	}

	unsigned char* Aes1::RotWord(unsigned char* word) //密钥移位函数
	{
		byte* temp = new byte[4];
	temp[0] = word[1];
	temp[1] = word[2];
	temp[2] = word[3];
	temp[3] = word[0];
	return temp;

	}

	unsigned char* Aes1::SubWord(unsigned char* word) //密钥字代换函数
	{
		byte* temp = new byte[4];
	for(int j=0;j<4;j++)
	{
		temp[j] = Sbox[16*(word[j] >> 4)+(word[j] & 0x0f)];  //实际上也可以写成Sbox[[j]];因为两者相等
	}
	return temp;

	}
