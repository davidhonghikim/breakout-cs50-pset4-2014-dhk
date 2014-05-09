// David Honghi Kim
// david.honghi.kim@gmail.com
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

// size of paddle
#define PADDLE_W 50
#define PADDLE_H 10
#define PADDLE_Y HEIGHT-40
// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 20

// velocity
#define VELOCITY 3.0
// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

string fillColor[] = {"RED", "ORANGE", "YELLOW", "GREEN", "CYAN", "BLACK"};
// initial velocity
double xVelocity = VELOCITY;
double yVelocity = VELOCITY;

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

    //start game
    int gameStarted = 0;
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {   
        while(gameStarted != 1){
            GEvent event = getNextEvent(MOUSE_EVENT);
            // if we heard one
            if (event != NULL){
                if (getEventType(event) == MOUSE_CLICKED){
                    move(ball, xVelocity, yVelocity);
                    gameStarted++;
                }
            }
        }
        move(ball, xVelocity, yVelocity); // keep moving ball
        GEvent event = getNextEvent(MOUSE_EVENT); // check for mouse event
        if (event != NULL) // if we heard one
        {
            if (getEventType(event) == MOUSE_MOVED) // if the event was movement
            {
                // ensure paddle follows mouse horizontally
                double x = getX(event) - getWidth(paddle) / 2;
                setLocation(paddle, x, PADDLE_Y);
                // restrict paddle - right edge of window
                if (getX(paddle) + getWidth(paddle) >= getWidth(window))
                {
                    setLocation(paddle, (getWidth(window)-getWidth(paddle)), PADDLE_Y);
                }
                // restrict paddle - left edge of window
                else if (getX(paddle) <= 0)
                {
                    setLocation(paddle, 0, PADDLE_Y);
                }
            }
        }
        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            xVelocity = -xVelocity;
        }
        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            xVelocity = -xVelocity;
        }
         // bounce off top edge of window
        else if (getY(ball) <= 0)
        {
            yVelocity = -yVelocity;
        }
        // linger before moving again
        pause(10);
        
        //detect collisions
        GObject object = detectCollision(window, ball);
        if(object != NULL){
            if(object == paddle && yVelocity > 0){
                yVelocity = -yVelocity;
            }
            else if(strcmp(getType(object),"GRect") == 0 && object != paddle){
                removeGWindow(window, object);
                yVelocity = -yVelocity;
                bricks--;
                updateScoreboard(window, label, ++points);
            }
        }
        
        if (getY(ball) > PADDLE_Y) //detect losing life
        {
            lives--;
            move(ball, 0, 0);
            addAt(window, ball, WIDTH/2 - RADIUS/2, HEIGHT/2);
            gameStarted = 0;
        } 
    }
    
    // winner
    if(points == 50){
        GLabel winner = newGLabel("WINNER!!!");
        setFont(winner, "SansSerif-48");
        setColor(winner, "GRAY");
        setLocation(winner, (getWidth(window) - getWidth(winner))/2, (getHeight(window) + 100 - getFontAscent(winner)) / 2);
        add(window, winner);
        removeGWindow(window, ball);
    }
    
    // loser
    if(lives == 0 ){
        GLabel loser = newGLabel("GAME OVER LOSER!!!");
        setFont(loser, "SansSerif-36");
        setColor(loser, "GRAY");
        setLocation(loser, (getWidth(window) - getWidth(loser))/2, (getHeight(window) + 100 - getFontAscent(loser)) / 2);
        add(window, loser);
        removeGWindow(window, ball);
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
    // TODO - loop bricks for each row with space between bricks, create new row and change color
    int bXStart=2; // brick x starting point
    int bYStart=50; // brick y starting point
    int xPad=4; // space between columns
    int yPad=4; // space between rows
    int bWidth = 35; // brick width
    int bHeight = 10; // brick height
    int color = 0; // rotate brick colors
    int rowCounter = 0;
    int colCounter = 0;
    
    while(rowCounter < ROWS){
        while(colCounter < COLS){
            GRect brick = newGRect(bXStart+xPad, bYStart, bWidth, bHeight); // create brick
 	        setColor(brick, fillColor[color]);
	        setFilled(brick,true);
            add(window, brick);
            bXStart+=bWidth+xPad;
            colCounter++;
        }
        color++; // changes color
        bYStart+=bHeight+yPad; // moves down to next row
        bXStart = 2; //starts new row
        colCounter=0; //resets cols
        rowCounter++;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH/2 - RADIUS/2, HEIGHT/2, RADIUS, RADIUS);
    setColor(ball, fillColor[5]);
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(WIDTH/2 - PADDLE_W/2, PADDLE_Y, PADDLE_W, PADDLE_H);
 	setColor(paddle, fillColor[5]);
	setFilled(paddle,true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-48");
    setColor(label, "GRAY");
    setLocation(label, (getWidth(window) - getWidth(label)) / 2, (getHeight(window) - getHeight(label)) / 2);
    add(window, label);
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
    object = getGObjectAt(window, x + RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + RADIUS, y + RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
