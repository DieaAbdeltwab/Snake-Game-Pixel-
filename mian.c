#include "STD_TYPES.h"
#include "BIT_MATH.h"
#define F_CPU 8000000UL
#include <util/delay.h>
#include "DIO_interface.h"
#include "LCD_interface.h"
#include "KPD_interface.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
/******************************************************************************************************************************************/
/************************************************* Global *********************************************************************************/
enum Snake_enumDirection {SNAKE_LEFT,SNAKE_UP,SNAKE_RIGHT,SNAKE_DOWN} ;
typedef struct part
{
  s16 Row,Column;
  enum Snake_enumDirection Direction;
} part;

#define  Snake_LCD_HEIGHT                  16
#define  Snake_LCD_WIDTH                   80
#define  Snake_WIN_LENGTH                  34
#define  Snake_START_POSITION_ROW          0
#define  Snake_START_POSITION_COLUMN       45
#define  Snake_START_DIRECTION             SNAKE_RIGHT
#define  Snake_GAME_SPEED_MS               50
#define  Snake_START_LENGTH                3

bool Snake_AboolLCDMatrix[Snake_LCD_HEIGHT][Snake_LCD_WIDTH];
part Snake_ApartInfo[Snake_WIN_LENGTH];
part Snake_partApplePosition;
u8 Snake_u8Length;
bool Snake_boolGameOver=true,Snake_boolWin=true;
u8 Snake_u8Collected;
u8 Snake_u8SnakeBodyChar[8];
u8 Snake_u8NULLChar[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
bool Snake_boolCheckBody;
/******************************************************* Initialisation ********************************************************************/
void Snake_voidInitSnake()
{
	/* Initialize all element Snake_AboolLCDMatrix= false--> we used Snake_AboolLCDMatrix to draw Snake in it After print on LCD  */
	  for (u8 Local_u8CounterRow=0;Local_u8CounterRow<Snake_LCD_HEIGHT;Local_u8CounterRow++)
	  {
	     for (u8 Local_u8CounterColumn=0;Local_u8CounterColumn<Snake_LCD_WIDTH;Local_u8CounterColumn++)
	     {
	        Snake_AboolLCDMatrix[Local_u8CounterRow][Local_u8CounterColumn] = false;
	     }
	  }
	  /* Initialize all element Snake_partInfo= 0--> we used Snake_partInfo to save information of Snake for each part  */
	  for (u8 Local_u8Counter=0;Local_u8Counter<(Snake_WIN_LENGTH);Local_u8Counter++)
	  {
		  Snake_ApartInfo[Local_u8Counter].Row=0;
		  Snake_ApartInfo[Local_u8Counter].Column=0;
	  }
	  /* Initialize Snake start parts --> First element in Snake_partInfo array is Head last element in Array is Tail(Snake_u8Length-1) */
	  for (u8 Local_u8Counter=0;Local_u8Counter<(Snake_START_LENGTH);Local_u8Counter++)
	  {
		  Snake_ApartInfo[Local_u8Counter].Row=Snake_START_POSITION_ROW;
		  Snake_ApartInfo[Local_u8Counter].Column=Snake_START_POSITION_COLUMN;
		  Snake_ApartInfo[Local_u8Counter].Direction=Snake_START_DIRECTION;
	     Snake_AboolLCDMatrix[Snake_ApartInfo[Local_u8Counter].Row][Snake_ApartInfo[Local_u8Counter].Column] = true;
	  }
}
void Snake_voidInitGame()
{
	/* Start Snake Game*/
  LCD_vidGoToXY(3,0);
  LCD_voidSendString("Snake Game");
  _delay_ms(3000);
  LCD_voidClear();
  /* We used Snake_boolGameOver to check is GameOver Or not  */
  Snake_boolGameOver = false;
  /* We used Snake_boolWin to check is Win Or not  */
  Snake_boolWin=false;
  /* Score =0  */
  Snake_u8Collected = 0;
  /* Initialize Snake start parts  */
  Snake_voidInitSnake();
  /* Start Length*/
  Snake_u8Length=Snake_START_LENGTH;
}
/**************************************************** Eat Apple and Grow *******************************************************************/
void Snake_voidGrowSnake()
{
	/* Add to Snake Length  1 */
  Snake_u8Length++;
  /* Add to Snake Length  New Part in Tail--> Copy info from old tail to new tail */
  Snake_ApartInfo[Snake_u8Length-1].Row=Snake_ApartInfo[Snake_u8Length-2].Row;
  Snake_ApartInfo[Snake_u8Length-1].Column=Snake_ApartInfo[Snake_u8Length-2].Column;
  Snake_ApartInfo[Snake_u8Length-1].Direction=Snake_ApartInfo[Snake_u8Length-2].Direction;
}
void Sanke_vidNewApple()
{
	  bool Local_boolCheckApplePosition = true;
	  while (Local_boolCheckApplePosition)
	  {
		  /* Using rand() function to draw new apple in random position */
		  Snake_partApplePosition.Row = rand()%Snake_LCD_HEIGHT;
		  Snake_partApplePosition.Column = rand()%Snake_LCD_WIDTH;
	      Local_boolCheckApplePosition = false;
	      /* Check apple not Draw on any part of snake */
		    for (u8 Local_u8Counter=1;Local_u8Counter<Snake_u8Length && Local_boolCheckApplePosition==false;Local_u8Counter++)
		    {
		    	if (Snake_ApartInfo[Snake_u8Length-Local_u8Counter].Row == Snake_partApplePosition.Row && Snake_ApartInfo[Snake_u8Length-Local_u8Counter].Column == Snake_partApplePosition.Column)
		    	{
		    		 Local_boolCheckApplePosition = true;
		    	}
		    }
	  }
}
/******************************************************** GameOver and Win *****************************************************************/
void Snake_voidDeleteInfo()
{
	/* Delete all info of Snake to Start again*/
	for (u8 Local_u8Counter=0;Local_u8Counter<(Snake_WIN_LENGTH);Local_u8Counter++)
	{
		Snake_ApartInfo[Local_u8Counter].Row=0;
		Snake_ApartInfo[Local_u8Counter].Column=0;
	}
}
void Snake_voidGameOver()
{
  _delay_ms(1000);
  LCD_voidClear();
  /* We used Snake_boolGameOver to check is GameOver Or not-->GameOver  */
  Snake_boolGameOver = true;
  /* Delete all info of Snake to Start again*/
  Snake_voidDeleteInfo();
  LCD_vidGoToXY(3,0);
  LCD_voidSendString("Game Over!");
  LCD_vidGoToXY(4,1);
  LCD_voidSendString("Score: ");
  LCD_voidSendNumber(Snake_u8Collected);
  _delay_ms(3000);
  LCD_voidClear();
}
void Snake_voidWin()
{
  _delay_ms(1000);
  LCD_voidClear();
  /* We used Snake_boolWin to check is Win Or not-->player is win  */
  Snake_boolWin=true;
  /* Delete all info of Snake to Start again*/
  Snake_voidDeleteInfo();
  LCD_vidGoToXY(2,0);
  LCD_voidSendString("Congratulations");
  LCD_vidGoToXY(4,1);
  LCD_voidSendString("Score: ");
  LCD_voidSendNumber(Snake_u8Collected);
  _delay_ms(3000);
  LCD_voidClear();

}
/************************************************* The Movement And Movement Logic *********************************************************/
void Snake_voidHeadLogic()
{
  /* Head take step-->Element 0 in Array Snake_partInfo is Head every cycle Head Will move Step then All part of Snake move --> Direction of head is last Direction saved  */
  switch(Snake_ApartInfo[0].Direction)
  {
    case SNAKE_UP   : Snake_ApartInfo[0].Row--;    break;
    case SNAKE_DOWN : Snake_ApartInfo[0].Row++;    break;
    case SNAKE_RIGHT: Snake_ApartInfo[0].Column++; break;
    case SNAKE_LEFT : Snake_ApartInfo[0].Column--; break;
    default : break;
  }
  /* When snake up to the edge of LCD ,Start from opposite position */
  if (Snake_ApartInfo[0].Column >= Snake_LCD_WIDTH)
  {
	  Snake_ApartInfo[0].Column = 0;
  }
  else if (Snake_ApartInfo[0].Column < 0)
  {
	  Snake_ApartInfo[0].Column = Snake_LCD_WIDTH-1;
  }
  else if (Snake_ApartInfo[0].Row >= Snake_LCD_HEIGHT)
  {
	  Snake_ApartInfo[0].Row = 0;
  }
  else if (Snake_ApartInfo[0].Row < 0)
  {
	  Snake_ApartInfo[0].Row = Snake_LCD_HEIGHT-1;
  }
  /* Check if Head of Snake eat any other part from snake--> GameOver if snake eat himself */
  for(u8 Local_u8Counter=1; Local_u8Counter<Snake_u8Length; Local_u8Counter++)
  {
    if(Snake_ApartInfo[0].Row==Snake_ApartInfo[Local_u8Counter].Row && Snake_ApartInfo[0].Column==Snake_ApartInfo[Local_u8Counter].Column)
	{
        	Snake_voidGameOver();
    }
  }
  /* Win if Length of Snake == max Length */
  if (Snake_u8Length==Snake_WIN_LENGTH)
  {
      Snake_voidWin();
  }
  else
  {
    /* To snake move we draw in  Snake_AboolLCDMatrix new head position  -->For each cycle */
    Snake_AboolLCDMatrix[Snake_ApartInfo[0].Row][Snake_ApartInfo[0].Column] = true;
    /* Check if head eat Apple or not  */
    if ((Snake_ApartInfo[0].Row == Snake_partApplePosition.Row) && (Snake_ApartInfo[0].Column == Snake_partApplePosition.Column))
    {
      /* Add one for Score */
      Snake_u8Collected++;
      /* Draw a new Apple */
      Sanke_vidNewApple();
      /* Add new part for Snake */
      Snake_voidGrowSnake();
    }
  }
}
void Snake_voidMoveSnake()
{
  /* To snake move we delete from  Snake_AboolLCDMatrix old tail position  -->For each cycle */
  Snake_AboolLCDMatrix[Snake_ApartInfo[Snake_u8Length-1].Row][Snake_ApartInfo[Snake_u8Length-1].Column] = false;
  for (u8 Local_u8Counter=1;Local_u8Counter<Snake_u8Length;Local_u8Counter++)
  {
	  /* Shifting info for all Parts Snake --> To move we should change all tails position */
	  Snake_ApartInfo[Snake_u8Length-Local_u8Counter].Row=Snake_ApartInfo[Snake_u8Length-Local_u8Counter-1].Row;
	  Snake_ApartInfo[Snake_u8Length-Local_u8Counter].Column=Snake_ApartInfo[Snake_u8Length-Local_u8Counter-1].Column;
	  Snake_ApartInfo[Snake_u8Length-Local_u8Counter].Direction=Snake_ApartInfo[Snake_u8Length-Local_u8Counter-1].Direction;
  }
  /* Head Move and Logic */
  Snake_voidHeadLogic();
}
/******************************************** Build And Print ******************************************************************************/
void Snake_voidPrintSnake()
{
	/* This function build snake in GCRAM than print in LCD by using Snake_AboolLCDMatrix which Snake drawen in it */
  u8 Local_u8NumberSpicalCaracter=0;
  if (!Snake_boolGameOver)
  {
	  /* Draw Apple in Snake_AboolLCDMatrix */
      Snake_AboolLCDMatrix[Snake_partApplePosition.Row][Snake_partApplePosition.Column] = true;
	  for (u8 Local_u8Counter=0;Local_u8Counter<8;Local_u8Counter++)
      {
		   /* Save NULL character in GCRAM */
           LCD_voidWriteSpicalCaracter(Local_u8Counter, Snake_u8NULLChar);
      }
	  /* Move in Snake_AboolLCDMatrix character then byte then bit and check if this bit Drawen or not */
      for(u8 Local_u8CounterRow=0;Local_u8CounterRow<2;Local_u8CounterRow++)
      {
          for(u8 Local_u8CounterColumn=0;Local_u8CounterColumn<16;Local_u8CounterColumn++)
          {
              /* We using  Snake_boolCheckBody to Check if this byte have Snake Parts or not */
              Snake_boolCheckBody = false;
              for(u8 Local_u8CounterByte=0;Local_u8CounterByte<8;Local_u8CounterByte++)
              {
            	  /* We will build character to save it in GCRAM*/
                 u8 Local_u8BuildSnakeBodyChar=0b00000000;
                 /*Check in bit then bit ,if it Drawen set bit in this position  */
                 if (Snake_AboolLCDMatrix[Local_u8CounterRow*8+Local_u8CounterByte][Local_u8CounterColumn*5+0])
                 {
                	 /* Every time We add new edit to old edit in this byte */
                    Local_u8BuildSnakeBodyChar+=0b00010000;
                    Snake_boolCheckBody = true;
                 }
                 if (Snake_AboolLCDMatrix[Local_u8CounterRow*8+Local_u8CounterByte][Local_u8CounterColumn*5+1])
		         {
		             Local_u8BuildSnakeBodyChar+=0b00001000;
		             Snake_boolCheckBody = true;
                 }
                 if (Snake_AboolLCDMatrix[Local_u8CounterRow*8+Local_u8CounterByte][Local_u8CounterColumn*5+2])
		         {
		             Local_u8BuildSnakeBodyChar+=0b00000100;
		             Snake_boolCheckBody = true;
                 }
                 if (Snake_AboolLCDMatrix[Local_u8CounterRow*8+Local_u8CounterByte][Local_u8CounterColumn*5+3])
		         {
		             Local_u8BuildSnakeBodyChar+=0b00000010;
		             Snake_boolCheckBody = true;
                 }
                 if (Snake_AboolLCDMatrix[Local_u8CounterRow*8+Local_u8CounterByte][Local_u8CounterColumn*5+4])
                 {
                     Local_u8BuildSnakeBodyChar+=0b00000001;
                     Snake_boolCheckBody = true;
                 }
                 /* Save byte in this Character */
                 Snake_u8SnakeBodyChar[Local_u8CounterByte] = Local_u8BuildSnakeBodyChar;
              }
              /* If snake have parts in this character */
              if (Snake_boolCheckBody)
              {
            	  /* Save this Character in GCRAM*/
                 LCD_voidWriteSpicalCaracter(Local_u8NumberSpicalCaracter, Snake_u8SnakeBodyChar);
                 /* Print this Character in LCD in his position */
                 LCD_vidGoToXY(Local_u8CounterColumn,Local_u8CounterRow);
    	         LCD_voidSendChar(Local_u8NumberSpicalCaracter);
    	         /* Go to used next character in GCRAM -->this is useful when snake length taller than byte*/
                 Local_u8NumberSpicalCaracter++;
              }
              else
              {
            	  /* Print NULL */
    	          LCD_vidGoToXY(Local_u8CounterColumn,Local_u8CounterRow);
                  LCD_voidSendChar(' ');
              }
         }
      }
  }

}
/******************************************************************************************************************************************/
int main(void)
{
    u8 Local_Key;
	LCD_voidInit();
	LCD_voidHiddenCursor();
	KPD_voidInit();
	while(1)
	{
		/* Initialize first time and if player win or GameOver */
		if (Snake_boolGameOver || Snake_boolWin)
		{
			Snake_voidInitGame();
		}
		/* Print Snake */
		Snake_voidPrintSnake();
		/* Move one step */
		Snake_voidMoveSnake();
		_delay_ms(Snake_GAME_SPEED_MS);


        Local_Key=KPD_u8GetKeyState_HC_543();
        /* Check KEY pressed  or not */
		if(Local_Key!=KPD_u8_KEY_NOT_PRESSED)
		{
			/* If player pressed check Direction and edit on head snake direction  */
			/* If player pressed direction opposite to the direction of the head snake, do not change the direction */
			if ('2'==Local_Key && Snake_ApartInfo[0].Direction!=SNAKE_DOWN)
			{
				Snake_ApartInfo[0].Direction = SNAKE_UP   ;
			}
			else if ('8'==Local_Key && Snake_ApartInfo[0].Direction!=SNAKE_UP)
			{
				Snake_ApartInfo[0].Direction = SNAKE_DOWN   ;
			}
			else if ('6'==Local_Key && Snake_ApartInfo[0].Direction!=SNAKE_LEFT)
			{
				Snake_ApartInfo[0].Direction = SNAKE_RIGHT   ;
			}
			else if ('4'==Local_Key && Snake_ApartInfo[0].Direction!=SNAKE_RIGHT)
			{
				Snake_ApartInfo[0].Direction = SNAKE_LEFT   ;
			}
		}
	 }

return 0;
}
