#include "InfiniCRC.h"

namespace INFI {
  WORD calc_crc_half(const BYTE /*far*/ *pin, BYTE len) {
    WORD crc;

    BYTE da;
    const BYTE /*far*/ *ptr;
    BYTE bCRCHigh;
    BYTE bCRCLow;

    WORD crc_ta[16]=
    { 
      0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
      0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef
    };
    
    ptr=pin;
    crc=0;

    while(len--!=0)
    {
        da=((BYTE)(crc>>8))>>4;

        crc<<=4;

        crc^=crc_ta[da^(*ptr>>4)];

        da=((BYTE)(crc>>8))>>4;

        crc<<=4;

        crc^=crc_ta[da^(*ptr&0x0f)];

        ptr++;
    }
    // Make bCRCLow the bottom 8 bits of crc
    bCRCLow = crc;
    // Make bCRCHigh the top 8 bits of crc
    bCRCHigh= (BYTE)(crc>>8);

    // Apparently, we just increment (, \r and LineFeed (which I guess is \n)
    if(bCRCLow==0x28||bCRCLow==0x0d||bCRCLow==0x0a)
    {
        bCRCLow++;
    }
    // Apparently, we just increment (, \r and LineFeed (which I guess is \n)
    if(bCRCHigh==0x28||bCRCHigh==0x0d||bCRCHigh==0x0a)
    {
        bCRCHigh++;
    }
    // Reassemble crc as the (bCRCHigh,bCRCLow)
    crc = ((WORD)bCRCHigh)<<8;
    crc += bCRCLow;
    return(crc);
  }
}
