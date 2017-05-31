#include <conio.h>
#include <dpmi.h>
#include <go32.h>
#include <pc.h>
#include <bios.h>
#include <algorithm>
#include <array>
#include <random>
#include <iostream>
#include <time.h>
#include <unistd.h>

std::pair<int, int> currentSquare = {1,1};
int ticksUntilClosure = 100;

unsigned char getPaletteEntry( int origin );
int level = 0;
int px = 160;
int py = 100;
int vx = 0;
int vy = 0;
int score = 0;
int counter = 0;
std::array< unsigned int, 320 * 200> imageBuffer;
std::array< unsigned char, 320 * 200 >  buffer;

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
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> random(50, 150);

  ++level;
  int tries = 10;
  ticksUntilClosure = 100;

  do {
    currentSquare = { random(gen), random(gen) };
  } while( isInside() && --tries > 0 );

}

void gameOver() {
  textmode(C80);
  clrscr();
  std::cout << "Game Over!" << std::endl << "Your score: " << score << std::endl;
  exit(0);
}

void initMode13h() {
  union REGS regs;

  regs.h.ah = 0x00;
  regs.h.al = 0x4;
  int86(0x10,&regs,&regs);
}

unsigned char getPaletteEntry( int origin ) {
  return origin;
  
  unsigned char shade = 0;
  
  if ( origin > 0 ) {
    return 3;
  } else {
    return 0;
  }
  
  int r = (((((origin & 0x0000FF)      ) << 2  ) >> 8 ) );
  int g = (((((origin & 0x00FF00) >> 8 ) << 2  ) >> 8 ) );
  int b = (((((origin & 0xFF0000) >> 16) << 2  ) >> 8 ) );
  
  if ( r > g && r > b ) {
    return 2;
  }
  
  if ( g > b && g > r ) {
    return 1;
  }

  if ( b > r && b > g ) {
    return 1;
  }
  
  if ( b == g && b == r && r > 192 ) {
    return 3;
  }

  return 0;
  
  if ( r > g && r > b ) {
    shade = 2;
  } else if ( r < g && r < b && ( r > 128 || g > 128 ) ) {
    shade = 1;
  }

  if ( ( r == g && r ==  b ) && r > 128 ) {
    shade = 3;
  }
  
  return shade;
}

void copyImageBufferToVideoMemory() {
  
  int origin = 0;
  int lastOrigin = -1;
  int value = -2;
  int offset = 0;
  
  for ( int y = 0; y < 200; ++y ) {
    
    if ( y < 0 || y >= 200 ) {
      continue;
    }
    
    for ( int x = 0; x < 320; ++x ) {
      
      if ( x < 0 || x >= 320 ) {
	continue;
      }
      
      offset = (y * 320) + x;
      origin = imageBuffer[ offset ];
      
      if ( lastOrigin != origin ) {
	value = getPaletteEntry( origin );
        lastOrigin = origin;
      }
      
      
      if ( buffer[ offset ] != value ) {
	union REGS regs;
	regs.h.ah = 0x0C;
	regs.h.al = value;
	regs.h.bh = 0;
	regs.x.cx = x;
	regs.x.dx = y;
	int86(0x10,&regs,&regs);
      }
      
      buffer[ offset ] = value;
    }
  }
}

void render() {  
  std::fill( std::begin( imageBuffer), std::end( imageBuffer ), 0 );

  int y0 = -ticksUntilClosure + currentSquare.second;
  int y1 = currentSquare.second + ticksUntilClosure;
  int x0 = -ticksUntilClosure + currentSquare.first;
  int x1 = currentSquare.first + ticksUntilClosure;

  bool isInsideTarget = isInside();
  
  for ( int y = y0; y < y1; ++y ) {
    if ( y < 0 || y >= 200 ) {
      continue;
    }
    
    for ( int x = x0; x < x1; ++x ) {      
      if ( x < 0 || x >= 320 ) {
	continue;
      }
      
      if ( x == x0 || x == ( x1 - 1 ) || y == y0 || y == (y1 - 1) ) {
	imageBuffer[ ( 320 * y ) + x ] = isInsideTarget ? 1 : 2;
      }
    }
  }
  

  y0 = -5 + py;
  y1 = 6 + py;
  x0 = -5 + px;
  x1 = 6 + px;
  
  for ( int y = y0; y < y1; ++y ) {
    if ( y < 0 || y >= 200 ) {
      continue;
    }
    
    for ( int x = x0; x < x1; ++x ) {      
      if ( x < 0 || x >= 320 ) {
	continue;
      }
      
      if ( x == x0 || x == ( x1 - 1 ) || y == y0 || y == (y1 - 1) ) {
	imageBuffer[ ( 320 * y ) + x ] = isInsideTarget ? 1 : 3;
      }
    }
  }

  imageBuffer[ ( 320 * py ) + px ] = isInsideTarget ? 1 : 3;
  
  copyImageBufferToVideoMemory();
  
  gotoxy(1,1);
  std::cout << "Score " << score << " Steps remaining: " << ticksUntilClosure - 10 << std::endl;
  
  usleep(  20000 );
  
  ticksUntilClosure -= level;
  
  if ( ticksUntilClosure <= 10 ) {
    if ( isInside() ) {
      makeNextSquare();
      level = std::min( 5, level + 1);
      ++score;
    } else {
      gameOver();
    }
  }
}

int main( int argc, char **argv ) {
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
    
    while( kbhit() ) {
      lastKey = getch();
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
  }
 
  return 0;
}
