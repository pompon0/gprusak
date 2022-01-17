#ifndef GPRUSAK_UTF_H_
#define GPRUSAK_UTF_H_

#include "gprusak/types.h"
#include "gprusak/errors.h"

namespace gprusak {

inline Error<>::Or<str32> utf8_to_utf32(const str &s8) {
	str32 s32;
  for(size_t i=0; i<s8.size();) {
		uint8_t c8 = s8[i++];
		char32_t c32 = 0;
		size_t tail = 0;
		if(c8>=0xF8) {
			return Err::New("invalid utf8 leading byte '%'",int(c8));
		} else if(c8>=0xF0) {
			c32 = c8^0xF0;	
      tail = 3;
		} else if(c8>=0xE0) {
      c32 = c8^0xE0;
      tail = 2;
		} else if(c8>=0xC0) {
      c32 = c8^0xC0;
      tail = 1;
		} else if(c8>=0xA0) {
      return Err::New("invalid utf8 head byte %",int(c8));
    } else {
			c32 = c8;
			tail = 0; 
		}
    if(i+tail>s8.size()) return Err::New("truncated utf8 codepoint");
		while(tail--){
      uint8_t c8 = s8[i++];
      if((c8&0xC0)!=0x80) return Err::New("invalid utf8 tail byte %",int(c8));
      c32 = (c32<<6)|(c8^0x80);
    }
    s32 += c32;
	}
  return s32;
}

inline opt<str> utf32_to_utf8(char32_t c32) {
	char b[5]={};
  if(c32 <= 0x7F) {
		b[0] = c32;
  } else if(c32 <= 0x7FF) {
		b[0] = 0xC0 | (c32 >> 6);            /* 110xxxxx */
		b[1] = 0x80 | (c32 & 0x3F);          /* 10xxxxxx */
	} else if(c32 <= 0xFFFF) {
		b[0] = 0xE0 | (c32 >> 12);           /* 1110xxxx */
		b[1] = 0x80 | ((c32 >> 6) & 0x3F);   /* 10xxxxxx */
		b[2] = 0x80 | (c32 & 0x3F);          /* 10xxxxxx */
  } else if(c32 <= 0x10FFFF) {
		b[0] = 0xF0 | (c32 >> 18);           /* 11110xxx */
		b[1] = 0x80 | ((c32 >> 12) & 0x3F);  /* 10xxxxxx */
		b[2] = 0x80 | ((c32 >> 6) & 0x3F);   /* 10xxxxxx */
		b[3] = 0x80 | (c32 & 0x3F);          /* 10xxxxxx */
	} else return {};
	return str(b);
}


inline Error<>::Or<str> utf32_to_utf8(const str32 &s32) {
  str s8;
  for(auto c32 : s32) {
		auto mc8 = utf32_to_utf8(c32); 
		if(!mc8) return Err::New("invalid utf32 char %",c32);    
    s8 += *mc8;
  }
  return s8;
}

}  // namespace gprusak

#endif  // GPRUSAK_UTF_H_
