#define SAVE_SCREEN         // this directive forces saving/restoring the screen

#define USE_TI89            // produce all types of files

#include <stdio.h>          // standard ANSI C input/output support
#include <kbd.h>            // keyboard handling support, needed for ngetchx
#include <tigcclib.h>

#define TRUE 1
#define FALSE 0
#define SPEED 1
#define DELAY 800
#define MAX_WORM 128
#define PLAY_X 18
#define PLAY_Y 11
#define BORDER 2
#define WBORDER 4

// textures
unsigned char head[] = {0xFF,0xFF,0xC3,0xDB,0xDB,0xC3,0xFF,0xFF};
unsigned char body[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
unsigned char treat[] = {0x18,0x08,0x76,0xFF,0xFF,0xFF,0x7A,0x34};
struct section{
	unsigned char x;
	unsigned char y;
};
// data
struct section worm[MAX_WORM];

int tx = 0;
int ty = 0;

int vx = SPEED;
int vy = 0;
int score = 1;
int alive=TRUE;
int game=TRUE;
int quit=FALSE;
int pause=FALSE;

void delay(short int value){
	value = 1000-5* (value-1);
	if(value < 250)
		value = 250;
	value <<= 3;

	while (value-- > 0){
		rand();
	}
}

void draw(void){
	clrscr();
	printf("SCORE:%d\n",score);
	//BORDER
	//TOP
	DrawLine(3,8,LCD_WIDTH-3,8,A_THICK1);
	//BOTTOM
	DrawLine(3,LCD_HEIGHT-2,LCD_WIDTH-3,LCD_HEIGHT-2,A_THICK1);
	DrawLine(2,LCD_HEIGHT-1,LCD_WIDTH-3,LCD_HEIGHT-1,A_THICK1);
	//LEFT
	DrawLine(2,8,2,LCD_HEIGHT-2,A_THICK1);
	//RIGHT
	DrawLine(LCD_WIDTH-4,8,LCD_WIDTH-4,LCD_HEIGHT-2,A_THICK1);
	//DrawLine(0,LCD_HEIGHT-2,LCD_WIDTH,LCD_HEIGHT-2,A_THICK1);
	//delay(100);
	int i=0;
	while(i++<score-1){
		Sprite8(WBORDER+worm[i].x*8,BORDER+worm[i].y*8,8,body, LCD_MEM, SPRT_OR);
	//	printf("%d\n", i);
	}
	Sprite8(WBORDER+worm[0].x*8,BORDER+worm[0].y*8,8,head, LCD_MEM, SPRT_OR);
	// jutalomfalat
	Sprite8(WBORDER+tx*8,BORDER+ty*8,8,treat, LCD_MEM, SPRT_OR);
}

void update(void){
	int x = worm[0].x + vx;
	int y = worm[0].y + vy;

	if(x==tx && y == ty){
		score++;
		int insnake = TRUE;
		while(insnake){
			insnake = FALSE;
			tx = (rand() % PLAY_X);
			ty = (rand() % (PLAY_Y-1))+1;
			int i=score;
			while(i-->0){
				if(tx==worm[i].x && ty==worm[i].y){
					insnake = TRUE;
				}
			}
		}
	}

	int i=score;
	while(i-->0){
		if(x == worm[i].x && y == worm[i].y)
			alive = FALSE;

		worm[i].x=worm[i-1].x;
		worm[i].y=worm[i-1].y;
	}
	//check for collision
	if(x < 0 || x > PLAY_X)
		alive = FALSE;
	if(y <= 0 || y >= PLAY_Y)
		alive = FALSE;
	worm[0].x = x;
	worm[0].y = y;
}

void input(){
	if(kbhit()){
		int key = ngetchx();
		int back = FALSE;
		if(score == 1)
			back = TRUE;
		switch(key){
			case 338:
				if(back || vx != SPEED){
					vx = -SPEED;
					vy = 0;
				}
				//bal
				break;
			case 340:
				if(back || vy != -SPEED){
					vx = 0;
					vy = SPEED;
				}
				//le
				break;
			case 344:
				if(back || vx != -SPEED){
					vx = SPEED;
					vy = 0;
				}
				//jobb
				break;
			case 337:
				if(back || vy != SPEED){
					vx = 0;
					vy = -SPEED;
				}
				//fel
				break;
			case 264:
				pause=TRUE;
				break;
		}
		if(pause){
			DrawStr(5,12,"PAUSED",A_XOR);
			if(ngetchx()==264){
				quit=TRUE;
			}
			pause=FALSE;
		}
	}
}

void _main(void){
	randomize();
	clrscr();
	// game loop
	while(game){
		vx = SPEED;
		vy = 0;
		tx = (rand() % (PLAY_X));
		ty = (rand() % (PLAY_Y-1))+1;
		worm[0].x = (rand() % 5)+2;
		worm[0].y = (rand() % 5)+2;
		draw();
		while(alive){
			input();
			if(quit)
				break;
			update();
			draw();
			delay(score);
		}
		if(quit)
		       	break;
		DrawStr(50,36,"GAME OVER",A_XOR);
		DrawStr(55,50,"Retry?",A_XOR);
		score=1;
		delay(0);
		if(ngetchx()==264||!game)
			break;
		game=TRUE;
		alive=TRUE;
	}
	game=TRUE;
       	alive=TRUE;
	quit=FALSE;
}
