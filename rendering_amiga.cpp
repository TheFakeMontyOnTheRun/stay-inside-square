#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <intuition/intuition.h>



struct IntuitionBase *IntuitionBase;
struct Window *my_window;

struct NewWindow my_new_window= {
  50,            /* LeftEdge    pencerenin X pozisyonu */
  30,            /* TopEdge     pencerenin Y pozisyonu */
  300,           /* Width       pencerenin genisligi */
  200,            /* Height      pencerenin yuksekligi */
  0,             /* DetailPen   colour reg. 0 ile text cizilir */
  1,             /* BlockPen    colour reg. 1 ile block cizilir */
  ACTIVEWINDOW|
  VANILLAKEY|
  CLOSEWINDOW|
  RAWKEY,        /* IDCMPFlags  */
  SMART_REFRESH| /* Flags       */
  WINDOWDRAG|    /*             */
  WINDOWDEPTH|   /*             */
  ACTIVATE,      /*             acildiginda pencereyi aktif hale getir*/
  NULL,          /* FirstGadget */
  NULL,          /* CheckMark   */
  (UBYTE*)"Squares",	 /* Title       pencere basligi */
  NULL,          /* Screen      */
  NULL,          /* BitMap      */
  320,             /* MinWidth    */
  200,             /* MinHeight   */
  320,             /* MaxWidth    */
  200,             /* MaxHeight   */
  WBENCHSCREEN   /* Type        Workbench Screen. */
};

SHORT my_points[]= {
  0, 0,
  1, 1
};

SHORT quadPoints[]= {
  0,0, 50,0, 50,30, 0,30, 0,0,  
};

struct Border my_border=   {
  0, 0,        /* LeftEdge, TopEdge. */
  3,           /* FrontPen, colour register 3. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 3. */
  2,           /* mypoint 6 noktanın koordinatlari cizilecek */
  my_points,   /* koordinatlara ait dizinin pointer'i */
  NULL,        /* NextBorder */
};


struct Border quad =   {
  0, 0,        /* LeftEdge, TopEdge. */
  3,           /* FrontPen, colour register 3. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 3. */
  5,           /* mypoint 6 noktanın koordinatlari cizilecek */
  quadPoints,   /* koordinatlara ait dizinin pointer'i */
  NULL,        /* NextBorder */
};

void put( int x, int y, int value ) {
  WritePixel(my_window->RPort, x, y);
}

void clear() {
  RectFill(my_window->RPort, 0, 0, 320, 200);
}

drawSquare(int x0, int y0, int x1, int y1, int index) {
  
  quadPoints[0] = 2 * x0;
  quadPoints[1] = 2 * y0;  
  quadPoints[2] = 2 * x1;
  quadPoints[3] = 2 * y0;  
  quadPoints[4] = 2 * x1;
  quadPoints[5] = 2 * y1;
  quadPoints[6] = 2 * x0;
  quadPoints[7] = 2 * y1;  
  quadPoints[8] = 2 * x0;
  quadPoints[9] = 2 * y0;

  quad.FrontPen = index;
  
  DrawBorder(my_window->RPort, &quad, 0, 0);  
}


void putBuffer( int x, int y, int index )  {
  put( 2 * x, 2 * y, index );
  put( 2*  x, 2 * y + 1, index );
  put( 2*  x + 1, 2 * y, index );
  put( 2*  x + 1, 2 * y + 1, index );
}

unsigned char getPaletteEntry( int origin );

void printString(const char* pStr) {
}

void shutdown();

int getKey() {
  
  int toReturn = 255;

  IntuiMessage *my_message;
  BOOL close_me;
  ULONG messageClass; 
  USHORT code;
  SHORT x, y; 
  BOOL mouse_moved;
  USHORT qualifier; 
  ULONG seconds;
  ULONG micros; 
	
  //  Wait( 1 << my_window->UserPort->mp_SigBit );

  if ( my_message = (IntuiMessage *) GetMsg(my_window->UserPort)) {
    messageClass = my_message->Class;
    code = my_message->Code;
    qualifier = my_message->Qualifier; 
    x = my_message->MouseX; 
    y = my_message->MouseY; 
    seconds = my_message->Seconds;
    micros = my_message->Micros;  
    ReplyMsg(my_message);
    if (messageClass == VANILLAKEY) {
      return code;
    }
      
  } else {
    return 255;
  }
  

  return 255;
}

void gameOver() {
  puts("Thank you for playing!");
  shutdown();
  exit(0);
}


void initMode13h() {
  IntuitionBase = (struct IntuitionBase *) OpenLibrary( "intuition.library", 0 );

  if( IntuitionBase == NULL ) {
    puts("nope 1!");
    exit(0);
  }
  
  my_window = (struct Window *) OpenWindow( &my_new_window );
  
  if(my_window == NULL) {
    puts("nope 2!");
    CloseLibrary( IntuitionBase );
    exit(0);
  }
}

unsigned char getPaletteEntry( int origin ) {
  return origin;
}

void copyImageBufferToVideoMemory() {
  Delay(1);
}

void shutdown() {
  CloseWindow( my_window );
  CloseLibrary( IntuitionBase );
}
