#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>


/*Screen dim constantes.*/
#define SCREEN_WIDTH 	  800
#define SCREEN_HEIGHT 	800
#define SCREEN_MID_X	  (SCREEN_WIDTH / 2)
#define SCREEN_MID_Y	  (SCREEN_HEIGHT / 2)

#define CLOCK_D 	    	SCREEN_MID_X
#define CLOCK_R 	    	(CLOCK_D / 2)
#define CLOCK_R_IN	  	(CLOCK_R - 15)		
#define CLOCK_R_OUT	  	(CLOCK_R + 15)
#define CLOCK_HOURS_R 	(CLOCK_R * 0.5)
#define CLOCK_MIN_R		  (CLOCK_R * 0.75)
#define CLOCK_SEC_R		  (CLOCK_R * 0.9)

#define PI 			      	3.14159265358979323846
#define TWO_PI	      	(PI * 2)
#define HALF_PI	     		(PI / 2)

#define MIN(a, b) 												                      (((a) < (b)) ? (a) : (b))
#define MAP(val, val_min, val_max, boundary_min, boundary_max)	((val - val_min) * (boundary_max - boundary_min) / (val_max - val_min) + boundary_min)
#define NORM(val, val_min, val_max)				              				(MAP(val, val_min, val_max, 0, 1))


/****************************************************************/

/* SDL management. */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
unsigned int getTicks;

unsigned int red, green, blue;

/* Clock management. */
float offsetFrameColor = 0;

/* Time management. */
time_t timestamp;
struct tm* now;
int offsetUTC = 0;        /* beetween UTC and "real" country time. */
float hA;             /* Hours angle. */
float hAOrig;         /* Saved hours angle. */
float mA;             /* Minutes angle. */
float sA;             /* Seconds angle. */


/****************************************************************/

void HSV2RGB(float h, float s, float v, unsigned int* r, unsigned int* g, unsigned int* b)
/*
	https://www.rapidtables.com/convert/color/hsv-to-rgb.html (source)
	https://en.wikipedia.org/w/index.php?title=HSL_and_HSV
*/
{
	float Chroma;
	float X;
	float hprime;
	float match_lightness;
	float fr, fg, fb;

	if (h < 0) h = 0;
	if (s < 0) s = 0;
	if (v < 0) v = 0;

	if (h > 360) h = 359;
	if (s > 100) s = 100;
	if (v > 100) v = 100;

	s = s / 100.0;	
	v = v / 100.0;

	Chroma = v * s;
	hprime = h / 60.0;

	X = Chroma * (1.0 - fabs(fmod(hprime,2.0) - 1.0));

	fr = fg = fb = 0;

	/* hprime = floor(hprime); */

	switch ((int)floor(hprime))
	{
		case 0:
			fr = Chroma;
			fg = X;
			break;
		case 1:
			fr = X;
			fg = Chroma;
			break;
		case 2:
			fg = Chroma;
			fb = X;
			break;
		case 3:
			fg = X;
			fb = Chroma;
			break;
		case 4:
			fr = X;
			fb = Chroma;
			break;
		default:
			fr = Chroma;
			fb = X;
			break;
	}

	match_lightness = v - Chroma;
	*r = abs((int)round((fr + match_lightness) * 255.0));
	*g = abs((int)round((fg + match_lightness) * 255.0));
	*b = abs((int)round((fb + match_lightness) * 255.0));
}

void circle_Andres_algo(SDL_Renderer* renderer, int xc, int yc, int r, int red, int green, int blue)
/*
	 Circle generation using Andres' algorithm.
	 https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_cercle_d%27Andres
*/
{
	int x = 0;
	int y = r;
	int d = r - 1;

	while (y >= x)
	{
		/* Drawing all 8 pixels present in circle by symmetric position. */
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc + x, yc + y);
		SDL_RenderDrawPoint(renderer , xc - x, yc + y);
		SDL_RenderDrawPoint(renderer , xc + x, yc - y);
		SDL_RenderDrawPoint(renderer , xc - x, yc - y);
		SDL_RenderDrawPoint(renderer , xc + y, yc + x);
		SDL_RenderDrawPoint(renderer , xc - y, yc + x);
		SDL_RenderDrawPoint(renderer , xc + y, yc - x);
		SDL_RenderDrawPoint(renderer , xc - y, yc - x);

		/* Checking next (x, y) position to draw circle. */
		if (d >= (x << 1))
		{
			d = d - (x << 1) - 1;
			x++;
		}
		else if (d < ((r-y) << 1))
		{
			d = d + (y << 1) - 1;
			y--;
		}
			else
			{
				d = d + ((y - x - 1) << 1);
				y--;
				x++;
			}
	}
}

void circle_Andres_algo_rotation_color(SDL_Renderer* renderer, int xc, int yc, int r)
/*
	 Circle generation using Andres' algorithm.
	 https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_cercle_d%27Andres
*/
{
	int x = 0;
	int y = r;
	int d = r - 1;

	double a;

	while (y >= x)
	{
		/* Drawing all 8 pixels present in circle by symmetric position. */
		a = (atan2(y, x) * 180 / PI);
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc + x, yc + y);

		a = (atan2(y, -x) * 180 / PI);
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc - x, yc + y);

		a = (atan2(-y, x) * 180 / PI) + 360;
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc + x, yc - y);

		a = (atan2(-y, -x) * 180 / PI) + 360;
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc - x, yc - y);

		a = (atan2(x, y) * 180 / PI);
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc + y, yc + x);

		a = (atan2(x, -y) * 180 / PI);
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc - y, yc + x);
		
		a = (atan2(-x, y) * 180 / PI) + 360;
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc + y, yc - x);

		a = fmod((atan2(-x, -y) * 180 / PI) + 360, 360);   // Remove a color glitch at 180°. Value of atan == 540° instead 180°. 
		HSV2RGB(fmod(360 - (a - offsetFrameColor), 360), 100, 100, &red, &green, &blue);
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer , xc - y, yc - x);

		/* Checking next (x, y) position to draw circle. */
		if (d >= (x << 1))
		{
			d = d - (x << 1) - 1;
			x++;
		}
		else if (d < ((r-y) << 1))
		{
			d = d + (y << 1) - 1;
			y--;
		}
			else
			{
				d = d + ((y - x - 1) << 1);
				y--;
				x++;
			}
	}
}

void big_point(SDL_Renderer* renderer, int x, int y, int w)
{
	int r;

	for (r = 0; r < w; r += 1)
	{
		circle_Andres_algo(renderer, SCREEN_MID_X + x, SCREEN_MID_X + y, r, 0, 0, 0);					// Drawing circle with Bresenham's algorithm	
	}
}

void clock_update()
{
	int r;
	float a;
	
	timestamp = time(NULL);
	now = localtime(&timestamp);

	sA = MAP(now->tm_sec, 0.0, 60.0, 0.0, TWO_PI) - HALF_PI;
	mA = MAP(now->tm_min + NORM(now->tm_sec, 0.0, 60.0), 0.0, 60.0, 0.0, TWO_PI) - HALF_PI;
	hA = MAP(now->tm_hour % 12 + NORM(now->tm_min, 0.0, 60.0), 0.0, 24.0, 0.0, TWO_PI * 2) - HALF_PI;

	for (r = CLOCK_R_IN; r < CLOCK_R_OUT; r += 1)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		circle_Andres_algo_rotation_color(renderer, SCREEN_MID_X, SCREEN_MID_Y, r);					// Drawing circle with Bresenham's algorithm.
	}

    for (a = 0; a < 2 * PI; a += PI/6)
    {
      big_point(renderer, CLOCK_R * cos(a), CLOCK_R * sin(a), 4);
    }
	
	
	SDL_SetRenderDrawColor(renderer, 0, 255, 21, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer,SCREEN_MID_X, SCREEN_MID_Y, (SCREEN_MID_X) + (int)(CLOCK_SEC_R * cos(sA)), (SCREEN_MID_Y) + (int)(CLOCK_SEC_R * sin(sA)));

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer,SCREEN_MID_X, SCREEN_MID_Y, (SCREEN_MID_X) + (int)(CLOCK_MIN_R * cos(mA)), (int)(SCREEN_MID_Y) + (CLOCK_MIN_R * sin(mA)));
	
	SDL_SetRenderDrawColor(renderer, 64, 0, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer,SCREEN_MID_X, SCREEN_MID_Y, (SCREEN_MID_X) + (int)(CLOCK_HOURS_R * cos(hA)), (int)(SCREEN_MID_Y) + (CLOCK_HOURS_R * sin(hA)));

	if(SDL_GetTicks() > (getTicks + 1000) / 60)
	{
		offsetFrameColor = fmod(offsetFrameColor + (360 / 60), 360);
	    getTicks = SDL_GetTicks();
	}

}

void main_SDL()
{
	bool end_main_loop = false;
	SDL_Event event;
	
	/* Main loop */
	while (!end_main_loop)
	{
		/*set background color to black*/
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		clock_update();
		
		/* Checking SDL events. */
		while(SDL_PollEvent(&event) != 0)
		{
			/* User requests quit */
			if(event.type == SDL_QUIT)
			{
				end_main_loop = true;
			}

			if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))
			{
				end_main_loop = true;
			}
		
		}

		/* Display renderer into the window. */
		SDL_RenderPresent(renderer);

/*		
		if(SDL_GetTicks() < (getTicks + 1000 / fps))
		{
		    getTicks = SDL_GetTicks();
		}
*/		
	}
}

bool init_SDL()
{
	/* RealSDL init. */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
		return  false;
	}
	
	else
	{
		/* Create a window. */
		window = SDL_CreateWindow("Cercle et ligne" , SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED , SCREEN_WIDTH , SCREEN_HEIGHT , 0);
		if(window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			return  false;
		}

		/* Create a renderer. */
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(renderer == NULL)
		{
			printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
			return  false;
		}
		
	}

	getTicks = SDL_GetTicks();
	
	return true;
}

void close_SDL()
{
	/* Destroy the renderer and window. */
	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	/* Clean up SDL. */
	SDL_Quit();
}

int main(int argc, char* argv[])
{	
	int rc = 0;

	if (init_SDL())
	{
		main_SDL();
	}
	
	else
	{
		printf( "Failed to initialize!\n" );
		rc = 1;
	}

	close_SDL();

	return rc;
}
