#ifndef TOwenH
#define TOwenH
//-------------------------------------------------------------
// Протокол Owen
//-------------------------------------------------------------
#include "TSerial.h"
#include "Strutils.hpp"   // для reverse
#include <vcl.h>

class TOwen: public TSerial
{
public:
	TOwen(): TSerial(){degree_=2;};
	~TOwen(){};
	void Init();
	bool Poll(int min, int max){for(min;min!=max; min++){Poll(min);}} // опрос устройств от min до max
	bool Poll(int nPort);  // опрос nPort устройства
	bool Check(int nPort, bool in_out);      // если true, то закрыть, иначе открыть
	void Toggle(int nPort);
	int Set_Status(int nPort);
    AnsiString adrr(int nPort);
	int padrr(AnsiString adr);
	float result;
	AnsiString as;
	AnsiString GET_res(int nPort, AnsiString comand);
	short int degree_; //кол-во символов после запятой


private:
	AnsiString End(int digit); // преобразует контрольную сумму в строку
	unsigned short CRC16(unsigned char* packet, size_t length); //подсчёт контрольной суммы

};
//----------------------------------------------------------------------------
AnsiString TOwen::GET_res(int nPort, AnsiString comand)     // возвращает строку CRC команды
{
  AnsiString temp, res;
  unsigned short int data[20]={0}, count=0;
  char pname[20]={0};
  for(int i = 0, j = 0; i<comand.Length() ; i+=2, j++ )
  {
	temp = comand.SubString(i,2);
	data[j] = padrr(temp);
	count++;
	temp.Delete(1, temp.Length());
	if(i == 0){i = 1;}
  }
  pname[0] = nPort;
  for(int i = 1; i < (count+1); i++)
  {
	pname[i] = data[i-1];
  }
  unsigned short int r = CRC16(pname, (count+1));
  return End(r);
}
//----------------------------------------------------------------------------
AnsiString TOwen::End(int digit)
{
 AnsiString temp, res = "", as1, tm ="" ;
 int i, j=0; // для циклов
 while(1)
    {
		tm += digit%2; // десятичное число делют по модулю на 2 (т.е. в num_2[i] записывается сам остаток от деления на 2)
		if(!(digit/2)) // если деление без модуля (без остатка) на 2 стало равнятся 0, то...
		{
			if(tm.Length()!=16){tm += AnsiString::StringOfChar('0',(16-tm.Length()));} ;      // добавляем нули, сколько не хватает, вначале слова
			temp = ReverseString(tm);    // переворачиваем строку
			break; // цикл завершается
		}
		digit/=2; // десятичное число делют без модуля (без остатка)
        i++;
	}
	for(int k =0; k<16; k+=4)
	{
		as1 = temp.SubString(k,4);
        if(as1 == "0000"){res += "G";}
		if(as1 == "0001"){res += "H";}
		if(as1 == "0010"){res += "I";}
		if(as1 == "0011"){res += "J";}
		if(as1 == "0100"){res += "K";}
		if(as1 == "0101"){res += "L";}
		if(as1 == "0110"){res += "M";}
		if(as1 == "0111"){res += "N";}
		if(as1 == "1000"){res += "O";}
		if(as1 == "1001"){res += "P";}
		if(as1 == "1010"){res += "Q";}
		if(as1 == "1011"){res += "R";}
		if(as1 == "1100"){res += "S";}
		if(as1 == "1101"){res += "T";}
		if(as1 == "1110"){res += "U";}
		if(as1 == "1111"){res += "V";}
		as1.Delete(1, as1.Length());
		if(k==0){k =1;}
	}
	return res;
}
//----------------------------------------------------------------------------
unsigned short TOwen::CRC16(unsigned char* packet, size_t length)   // вводим тело пакета, без маркера
{
	size_t i, j;
	unsigned short crc;

	assert(packet);

	crc = 0;
	for (i = 0; i < length; ++i)
	{
		unsigned char b = packet[i];
		for (j = 0; j < 8; ++j, b <<= 1)
		{
			if ((b ^ (crc >> 8)) & 0x80)
			{
				crc <<= 1;
				crc ^= 0x8F57;
			}
			else
				crc <<= 1;
		}
	}
	return crc;
}
//----------------------------------------------------------------------------
int TOwen::padrr(AnsiString adr)
{
 return (byte(adr[2])-0x47)+(((byte(adr[1])-0x47))<<4);
}
//----------------------------------------------------------------------------
void TOwen::Init()
{
//

}
//----------------------------------------------------------------------------
AnsiString TOwen::adrr(int nPort)
{
AnsiString  tm;
tm = "#";
tm += char((nPort >> 4) + 0x47);
tm += char((nPort & 0xf) + 0x47);
return tm;
}
//----------------------------------------------------------------------------
bool TOwen::Poll(int nPort)
{
  int n,i;
 unsigned long t1,t2;
 unsigned short v;
 char *mes;
 if(Device==NULL){ ShowMessage("Не установлена связь с прибором!"); return(false);}

 AnsiString adr = adrr(nPort) ;
 adr += "HGROTV";	 // запрос на показания прибора
 adr += GET_res(nPort, "HGROTV");
 adr += "\r";
 mes = adr.c_str();

 Purge();
 WriteData(mes,adr.Length());
 BYTE reciv[100];
 AnsiString as1;
 short int ex;       // разряд
 short int tm;

	//************ ожидаем **********(
 t1=GetTickCount();
 do{
  t2=GetTickCount();
  Sleep(21);
  n=Get_NumBytesInQue();
  result = n;
  as = "";
  as1 ="";
  if(n>=1) {
	ReadData(&reciv[0],n);
	for(int i = n-11; i<n-5; i++)        //читаем байты поля данных, переносим в двоичную систему
	{
		switch (reciv[i]) {
			case 71:  as1+= "0000";
					  break;
			case 72:  as1+= "0001";
					  break;
			case 73:  as1+= "0010";
					  break;
			case 74:  as1+= "0011";
					  break;
			case 75:  as1+= "0100";
					  break;
			case 76:  as1+= "0101";
					  break;
			case 77:  as1+= "0110";
					  break;
			case 78:  as1+= "0111";
					  break;
			case 79:  as1+= "1000";
					  break;
			case 80:  as1+= "1001";
					  break;
			case 81:  as1+= "1010";
					  break;
			case 82:  as1+= "1011";
					  break;
			case 83:  as1+= "1100";
					  break;
			case 84:  as1+= "1101";
					  break;
			case 85:  as1+= "1110";
					  break;
			case 86:  as1+= "1111";
					  break;

		default:
			ShowMessage("Неверное значение");
			break;
		}
	 }                                      //считаем порядок числа
	 if(as1[1] == '1'){as = "-";}
	 if(as1[2] == '1'){ex = 128;}
	 else{ex = 0;}
	 if(as1[3] == '1'){ex += 64;}
	 else{ex += 0;}
	 if(as1[4] == '1'){ex += 32;}
	 else{ex += 0;}
	 if(as1[5] == '1'){ex += 16;}
	 else{ex += 0;}
	 if(as1[6] == '1'){ex += 8;}
	 else{ex += 0;}
	 if(as1[7] == '1'){ex += 4;}
	 else{ex += 0;}
	 if(as1[8] == '1'){ex += 2;}
	 else{ex += 0;}
	 if(as1[9] == '1'){ex += 1;}
	 else{ex += 0;}
	 ex -=127;   // поправить алгоритм, если получиться отрицательное число
	 if((as1[1]=='1')&& ex==0){as = "0"; return(true);}    // по стандарту "0" это минус 127
	 tm = ex-1;
	 short int tr = pow(2,(ex));
	 for (int i = 10; i < (10+ex); i++, tm--)     // находим целую часть числа
	 {
	  if(as1[i] == '1'){tr += pow(2,(tm));}
	 else{tr += 0;}
	 }
   //---------------
	 as+=tr;

	 float dev = 0.0;
	 short int k =1;
	 for (int i = 10+ex; i < 10+ex+degree_+4; i++, k++)     // находим дробную часть числа
	 {
		if(as1[i] == '1'){dev += pow(2,(k*(-1)));}
		else{dev += 0.0;}
	 }
	 AnsiString s= "";
	 s = dev;
	 s.Delete(1,1);
	 s.Delete(degree_+1, (s.Length()-degree_-1));
	 as+=s;
	 return(true);}
  Sleep(1);
 }while(t2-t1<500);
   //*******************************)
   {ShowMessage("Нет байт");}
 return(false);
}
//----------------------------------------------------------------------------
bool TOwen::Check(int nPort, bool in_out)
{

 int n,i;
 unsigned long t1,t2;
 unsigned short v;
 char *mes;
 if(Device==NULL){ ShowMessage("Не установлена связь с прибором!"); return(false);}

 AnsiString adr = adrr(nPort) ;
 if(in_out)
 {
	adr += "GJUOPKJVOGGG";     // замыкание выхода
	adr += GET_res(nPort, "GJUOPKJVOGGG");
	adr += "\r";
 }
 else
 {
	adr += "GJUOPKGGGGGG";     // размыкание выхода
	adr += GET_res(nPort, "GJUOPKGGGGGG");
	adr += "\r";
 }
 mes = adr.c_str();

 Purge();
 WriteData(adr.c_str(),adr.Length());
 BYTE reciv[100];
 AnsiString as1;

	//************ ожидаем **********(
 t1=GetTickCount();
 do{
  t2=GetTickCount();
  Sleep(1);
  n=Get_NumBytesInQue();
  result = n;
  as = "";
  as1 ="";
  if(n>=1){ return true;}
  else return false;
  Sleep(1);
 }while(t2-t1<500);
}

//----------------------------------------------------------------------------
void TOwen::Toggle(int nPort)
{
if(Set_Status(nPort)){Check(nPort,false);}
else {Check(nPort,true);}
}
//----------------------------------------------------------------------------
int TOwen::Set_Status(int nPort)      // если 1 значит на замке единица
{
 int n,i;
 unsigned long t1,t2;
 unsigned short v;
 char *mes;
 if(Device==NULL){ ShowMessage("Не установлена связь с прибором!"); return(0);}

 AnsiString adr = adrr(nPort) ;
 adr += "HGUOPK";     // просмотр значения на замке
 adr += GET_res(nPort, "HGUOPK");
 adr += "\r";
 mes = adr.c_str();

 Purge();
 WriteData(adr.c_str(),adr.Length());
 BYTE reciv[100];
 AnsiString as1;

	//************ ожидаем **********(
 t1=GetTickCount();
 do{
  t2=GetTickCount();
  Sleep(1);
  n=Get_NumBytesInQue();
  result = n;
  as = "";
  as1 ="";
  if(n>=1)
  {
	  ReadData(&reciv[0],n);
	  if(reciv[9]== 74) return 1;
	  else return 0;
  }
  Sleep(1);
 }while(t2-t1<500);
}

#endif
