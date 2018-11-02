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

unsigned char getPaletteEntry( int origin );
int level = 0;
int px = 80;
int py = 50;
int vx = 0;
int vy = 0;
int score = 0;
int counter = 0;

int isInside() {
  
  if ( currentSquare.first - ticksUntilClosure <= px && px <= currentSquare.first + ticksUntilClosure ) {
    if ( currentSquare.second - ticksUntilClosure <= py && py <= currentSquare.second + ticksUntilClosure ) {
      return 1;
    }
  }

  return 0;
}

void makeNextSquare() {
  
  ++level;
  int tries = 10;
  ticksUntilClosure = 40;
  /*
  do {
    currentSquare.first = (rand() + 20) % 40;
    currentSquare.second = (rand() + 20) % 40;
  }  while( isInside() && --tries > 0 );
  */
}

void shutdown();

void printString(const char* pStr);

int getKey();

void gameOver();

void clear();

void put( int x, int y, int value );

void initMode13h();

unsigned char getPaletteEntry( int origin );

void copyImageBufferToVideoMemory();

void putBuffer( int x, int y, int index );

void drawSquare( int x0, int y0, int x1, int y1, int index );

void render() {  
  
  int y0 = -ticksUntilClosure + currentSquare.second;
  int y1 = currentSquare.second + ticksUntilClosure;
  int x0 = -ticksUntilClosure + currentSquare.first;
  int x1 = currentSquare.first + ticksUntilClosure;

  int isInsideTarget = isInside();
  clear();
  drawSquare( x0, y0, x1, y1, isInsideTarget ? 1 : 2 );
  drawSquare( px - 5, py - 5, px + 6, py + 6, isInsideTarget ? 1 : 2 );
  drawSquare( px, py, px + 1, py + 1, isInsideTarget ? 1 : 2 );
    
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
  
  copyImageBufferToVideoMemory();
}

int main( int argc, char **argv ) {
  initMode13h();
  puts("init");

  //  srand(time(NULL));
  makeNextSquare();
  int done = 0;

  char lastKey = 0;
  puts("set screen");


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

    

    lastKey = getKey();

    switch (lastKey){
    case 'q':
      done = 1;
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
      
    

    render();
  }


  puts("get out!");
  shutdown();
  return 0;
}
