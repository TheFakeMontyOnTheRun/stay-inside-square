#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

unsigned char imageBuffer[160 * 100];
unsigned char buffer[160 * 100];

void putBuffer( int x, int y, int index )  {
  imageBuffer[ (160 * y ) + x ] = index;
}

unsigned char getPaletteEntry( int origin );

void printString(const char* pStr) {
  while(*pStr) {
    __asm__ __volatile__ (
			  "movb 0x0e, %%ah\n"
			  "movb %[c], %%al\n"
			  "int $0x10\n"
			  : 
			  : [c] "r" (*pStr)
			  : "ax"
			  );
    ++pStr;
  }
}

void clear() {
  memset(imageBuffer, 0, 160 * 100 );
}

void drawSquare(int x0, int y0, int x1, int y1, int index ) {
  for ( int y = y0; y < y1; ++y ) {
    if ( y < 0 || y >= 100 ) {
      continue;
    }
    
    for ( int x = x0; x < x1; ++x ) {      
      if ( x < 0 || x >= 160 ) {
	continue;
      }
      
      if ( x == x0 || x == ( x1 - 1 ) || y == y0 || y == (y1 - 1) ) {
	putBuffer( x, y, index );
      }
    }
  }
}

int getKey() {
  
  unsigned char toReturn = 255;

  asm volatile ("movb $0x01, %%ah\n\t"
		"movb $0x00, %%al\n\t"
		"int $0x16       \n\t"
		"movb %%al, %0 "
		: "=rm"(toReturn)
		);
  
  asm volatile("movb $0x0C, %ah\n\t"
	       "movb $0x00, %al\n\t"
	       "int $0x21"
	       );

  return toReturn;
}

void gameOver() {
  asm("movb $0x0, %ah\n\t"
      "movb $0x3, %al\n\t"
      "int $0x10\n\t");
  
  puts("Thanks for playing!");
  exit(0);
}

void put( int x, int y, int value ) {
  int pixel = value;
  int px = x;
  int py = y;
  
  asm volatile ("movb $0x0C, %%ah\n\t"
		"movb %0,    %%al\n\t"
		"movb $0x0,  %%bh\n\t"
		"movw %1,    %%cx\n\t"
		"movw %2,    %%dx\n\t"
		"int $0x10"
		:
		:"rm" (pixel), "rm" (px), "rm" (py)		      
		);  
}

void initMode13h() {
  asm("movb $0x0, %ah\n\t"
      "movb $0x4, %al\n\t"
      "int $0x10\n\t");  
}

unsigned char getPaletteEntry( int origin ) {
  return origin;
  
  unsigned char shade = 0;
  
  if ( origin > 0 ) {
    return 3;
  } else {
    return 0;
  }
}

void copyImageBufferToVideoMemory() {
  
  int origin = 0;
  int lastOrigin = -1;
  int value = -2;
  int offset = 0;
  
  for ( int y = 0; y < 100; ++y ) {
    
    if ( y < 0 || y >= 100 ) {
      continue;
    }
    
    for ( int x = 0; x < 160; ++x ) {
      
      if ( x < 0 || x >= 160 ) {
	continue;
      }
      
      offset = (y * 160) + x;
      origin = imageBuffer[ offset ];
      
      if ( lastOrigin != origin ) {
	value = getPaletteEntry( origin );
        lastOrigin = origin;
      }
      
      
      if ( buffer[ offset ] != value ) {
	put( (2 * x), (2 * y), value);
	put( (2 * x) + 1, (2 * y), value);
	put( (2 * x) + 1, (2 * y) + 1, value);	
	put( (2 * x), (2 * y) + 1, value);
      }
      
      buffer[ offset ] = value;
    }
  }

  memset( imageBuffer, 0, 160 * 100 );
}

void shutdown() {
}
