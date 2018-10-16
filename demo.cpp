#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct pair {
  int first;
  int second;
};

pair currentSquare = {1,1};
int ticksUntilClosure = 100;
uint8_t imageBuffer[160 * 100];
uint8_t buffer[160 * 100];

unsigned char getPaletteEntry( int origin );
int level = 0;
int px = 80;
int py = 50;
int vx = 0;
int vy = 0;
int score = 0;
int counter = 0;

static const unsigned char p = getPaletteEntry( 0xFF );

bool isInside() {
  
  if ( currentSquare.first - ticksUntilClosure <= px && px <= currentSquare.first + ticksUntilClosure ) {
    if ( currentSquare.second - ticksUntilClosure <= py && py <= currentSquare.second + ticksUntilClosure ) {
      return true;
    }
  }

  return false;
}

void makeNextSquare() {
  
  ++level;
  int tries = 10;
  ticksUntilClosure = 40;

  do {
    currentSquare = { (rand() + 20) % 40, (rand() + 20) % 40 };
  }  while( isInside() && --tries > 0 );

}

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

int16_t getKey() {
  
  int8_t toReturn = -1;

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

void put( uint16_t x, uint16_t y, uint8_t value ) {
  int8_t pixel = value;
  int16_t px = x;
  int16_t py = y;
  
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
}

void render() {  
  memset( imageBuffer, 0, 160 * 100 );

  
  int y0 = -ticksUntilClosure + currentSquare.second;
  int y1 = currentSquare.second + ticksUntilClosure;
  int x0 = -ticksUntilClosure + currentSquare.first;
  int x1 = currentSquare.first + ticksUntilClosure;

  bool isInsideTarget = isInside();
  
  for ( int y = y0; y < y1; ++y ) {
    if ( y < 0 || y >= 100 ) {
      continue;
    }
    
    for ( int x = x0; x < x1; ++x ) {      
      if ( x < 0 || x >= 160 ) {
	continue;
      }
      
      if ( x == x0 || x == ( x1 - 1 ) || y == y0 || y == (y1 - 1) ) {
	imageBuffer[ ( 160 * y ) + x ] = isInsideTarget ? 1 : 2;
      }
    }
  }
  

  y0 = -5 + py;
  y1 = 6 + py;
  x0 = -5 + px;
  x1 = 6 + px;
  
  for ( int y = y0; y < y1; ++y ) {
    if ( y < 0 || y >= 100 ) {
      continue;
    }
    
    for ( int x = x0; x < x1; ++x ) {      
      if ( x < 0 || x >= 160 ) {
	continue;
      }
      
      if ( x == x0 || x == ( x1 - 1 ) || y == y0 || y == (y1 - 1) ) {
	imageBuffer[ ( 160 * y ) + x ] = isInsideTarget ? 1 : 3;
      }
    }
  }

  imageBuffer[ ( 160 * py ) + px ] = isInsideTarget ? 1 : 3;
  
  copyImageBufferToVideoMemory();


  asm volatile("movb $0x02, %ah\n\t"
	       "movb $0x00, %bh\n\t"
	       "movb $0x01, %dl\n\t"
	       "movb $0x10, %dh\n\t"	       
	       "int $0x10");

  printString("topo");
  //  std::cout << "Score " << score << " Steps remaining: " << ticksUntilClosure - 10 << std::endl;
  
  //  usleep(  20000 );
  
  ticksUntilClosure -= level;
  
  if ( ticksUntilClosure <= 10 ) {
    if ( isInside() ) {
      makeNextSquare();
      level = ( 5 > ( level + 1 ) ) ? (level + 1 ) : 5;
      ++score;
    } else {
      gameOver();
    }
  }
}

int main( int argc, char **argv ) {

  srand(time(NULL));
  makeNextSquare();
  bool done = false;

  char lastKey = 0;
  
  initMode13h();

  while( !done ) {

    px += vx;
    py += vy;

    //    vx = vy = 0;
    
    if ( px < 0 ) {
      px = 0;
    }

    if ( py < 0 ) {
      py = 0;
    }

    if ( px >= 315 ) {
      px = 315;
    }

    if ( py >= 195 ) {
      py = 195;
    }
    
    
    ++counter;
    render();
    

    lastKey = getKey();

    switch (lastKey){
    case 'q':
      done = true;
      break;
    case 'w':
      vy = -5 + level;
      break;
    case 's':
      vy = 5 + level;
      break;
    case 'a':
      vx = -5 + level;
	break;
    case 'd':
      vx = 5 + level;
      break;
    }
      
    

    
  }
 
  return 0;
}
