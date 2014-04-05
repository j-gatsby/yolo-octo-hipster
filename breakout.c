//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// space above bricks and under paddle
#define GAP (HEIGHT/8)

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3 

// paddle dimensions
#define PHEIGHT (HEIGHT/100)
#define PWIDTH	(WIDTH/8)

// brick dimensions
#define BHEIGHT (HEIGHT/60)
#define BWIDTH 	((WIDTH - (2 * (COLS))) / COLS)

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);
void removeGWindow(GWindow window, GObject grid);
void freeGObject(GObject gobj);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window); 

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // initial velocity
    double velocityX = drand48() + 1.25;
    double velocityY = drand48() + 1.25;

    // wait for click before starting
    waitForClick();
	
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
         // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows top cursor
                double x = getX(event) - PWIDTH / 2;
                //setLocation(paddle, x, y);
                setLocation(paddle, x, (HEIGHT - GAP));
            }
        }
        
        // add diagonal movement to ball
		move(ball, velocityX, velocityY);
		
        // bounce off edge of window
        if ((getX(ball) + RADIUS * 2 >= WIDTH) || (getX(ball) <= 0))
        {
            velocityX = -velocityX;
        }
        
        if (getY(ball) <= 0)
        {
        	velocityY = -velocityY;
        }
        // linger before moving again
        pause(10);
        
        if (getY(ball) + RADIUS * 2 >= HEIGHT)
        {
        	lives--;
        	setLocation(paddle,((WIDTH/2) - (PWIDTH/2)), (HEIGHT - GAP));
        	setLocation(ball, WIDTH/2 - RADIUS, HEIGHT/2 - RADIUS);
        	waitForClick();
        }
        
        GObject object = detectCollision(window, ball);
        
        if (object != NULL)
        {
		    if (object == paddle)
		    {
		    	velocityY = -velocityY;
		    }
		    
		   else if (strcmp(getType(object), "GRect") == 0)
		   {
		    	removeGWindow(window, object);
		    	velocityY = -velocityY;
		    	bricks--;
		    	points++;
		   }
        }
        
    }
    
    if (lives == 0)
    {
    	double x, y;
    	
    	GLabel gameover = newGLabel("YOU LOSE!!!");
    	setFont(gameover, "SansSerif-48");
    	setColor(gameover, "RED"),
    	x = (WIDTH - getWidth(gameover)) / 2;
    	y = (HEIGHT - getFontAscent(gameover)) / 2;
    	setLocation(gameover, x, y);
    	add(window, gameover);
    }

    if (bricks == 0)
    {
    	double x, y;
    	
    	GLabel gamewin = newGLabel("YOU WIN!!!");
    	setFont(gamewin, "SansSerif-48");
    	setColor(gamewin, "GREEN");
    	x = (WIDTH - getWidth(gamewin)) / 2;
    	y = (HEIGHT - getFontAscent(gamewin)) / 2;
    	setLocation(gamewin, x, y);
    	add(window, gamewin);
    }
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    for (int i = 0; i < ROWS; i++)
    {
    	for (int j = 0; j < COLS; j++)
    	{
    		GRect grid = newGRect((j * (BWIDTH + 2)) +1, (GAP + ((BHEIGHT + 2) * i)) + 2, BWIDTH, BHEIGHT);
    		if (((i+j)%2) == 1)
    		{
    			setColor(grid, "BLUE");
    			setFilled(grid, true);
    		}
    		else 
    		{
    			setColor(grid, "BLACK");
    			setFilled(grid, true);
    		}
    		
    		add(window, grid);
    	}
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
    GOval ball = newGOval(WIDTH/2 - RADIUS, HEIGHT/2 - RADIUS, RADIUS * 2, RADIUS *2);
    setColor(ball, "DARK_GRAY");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // DONE
    GRect paddle = newGRect(((WIDTH/2) - (PWIDTH/2)), (HEIGHT - GAP), PWIDTH, PHEIGHT);
    setColor(paddle, "RED");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    double x, y;
    	
	GLabel label = newGLabel("0");
	setFont(label, "SansSerif-60");
	setColor(label, "LIGHT_GRAY");
	x = ((WIDTH) - getWidth(label)) / 2;
	y = ((HEIGHT) - getHeight(label)) / 2;
	add(window, label);
	setLocation(label, x, y);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
