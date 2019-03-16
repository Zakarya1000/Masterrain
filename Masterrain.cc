#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <chipmunk.h>

extern "C" {
#include <SDL.h>
#include <cairo.h>

#include <stdio.h>
}

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

char WINDOW_NAME[] = "Masterrain";
SDL_Window * gWindow = NULL;

void die(string message) {
    printf("%s\n", message);
    exit(1);
}

void init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) die("SDL");
    if (! SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) die("texture");

    gWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if (gWindow == NULL) die("window");
}

void close()
{
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

default_random_engine gen((long unsigned int) time(0));
uniform_real_distribution<double> dist(0.0, 1.0);

double random(double min, double max) {
     return dist(gen) * (max - min) + min;
 }

cpSpace * space;

int BLIT_READY;

double mouse_angle = 0.0;
bool mouse_down = false;

set<SDL_Keycode> keys_down;

void drawtree(cairo_t * cr, float x, float y, float size, float rot) {
  cairo_save(cr);
  cairo_translate(cr, x, y);
  cairo_rotate(cr, rot);
  cairo_scale(cr, size, size);
  cairo_new_sub_path(cr);
  cairo_arc(cr, -0.1, -0.1, 0.05, 0, 2*M_PI);
  cairo_set_source_rgb(cr, 0, 1.0, 0);
  cairo_fill_preserve(cr);
  cairo_set_source_rgb(cr, 0, 0.5, 0);
  cairo_set_line_width(cr, 0.02);
  cairo_stroke(cr);
  cairo_new_sub_path(cr);
  cairo_arc(cr, 0, 0, 0.1, 0, 2*M_PI);
  cairo_set_source_rgb(cr, 0, 1.0, 0);
  cairo_fill_preserve(cr);
  cairo_set_source_rgb(cr, 0, 0.5, 0);
  cairo_set_line_width(cr, 0.02);
  cairo_stroke(cr);
  cairo_restore(cr);
}

float treex1 = random(-1, 1);
float treey1 = random(-1, 1);
float treerot1 = random(0, 2*M_PI);

float treex2 = random(-1, 1);
float treey2 = random(-1, 1);
float treerot2 = random(0, 2*M_PI);

float guy_x = 0;
float guy_y = 0;

void drawlayer1(cairo_t * cr);
void drawfocuslayer2(cairo_t * cr);
void drawlayer3(cairo_t * cr);

void gameloop(cairo_t * cr) {
    // 0,0 at center of window and 1,1 at top right
    cairo_scale(cr, SCREEN_WIDTH/2.0, -SCREEN_HEIGHT/2.0);
    cairo_translate(cr, 1, -1);

    while (true) {
      //player movement
      if (keys_down.count(SDLK_w)) guy_y += 0.01;
      if (keys_down.count(SDLK_s)) guy_y -= 0.01;
      if (keys_down.count(SDLK_a)) guy_x -= 0.01;
      if (keys_down.count(SDLK_d)) guy_x += 0.01;

      // if the player is too close to the tree, he gets pushed away
      float distance = sqrt(pow(guy_x - treex1, 2) + pow(guy_y - treey1, 2));
//      cout << distance << endl;
      if (distance < 0.15){
        float slope = (guy_y - treey1) / (guy_x - treex1);
        guy_x += 0.01 / slope;
        guy_y += 0.01 * slope;
      }

        drawlayer1(cr);

        drawfocuslayer2(cr);

        drawlayer3(cr);

        SDL_Event e;
        e.type = BLIT_READY;
        SDL_PushEvent(& e);

        this_thread::sleep_for(chrono::milliseconds(20));

      }
    }
    void drawlayer3(cairo_t * cr) {
      cairo_save(cr);
      cairo_translate(cr, -guy_x, -guy_y);
      drawtree(cr, treex2, treey2, 1, treerot2);
      drawtree(cr, treex1, treey1, 1, treerot1);
      cairo_restore(cr);
    }

    void drawlayer1(cairo_t * cr) {
      // clear screen
      // grass
      cairo_rectangle(cr, -1, -1, 2, 2);
      cairo_set_source_rgb(cr, 0.5, 1, 0.5);
      cairo_fill(cr);
      cairo_save(cr);
      cairo_translate(cr, -guy_x, -guy_y);
      cairo_restore(cr);
    }

    void drawfocuslayer2(cairo_t * cr) {

        // text genoration
        /*
        cairo_save(cr);
        cairo_select_font_face(cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, .5);
        cairo_scale(cr, 1, -1);
        cairo_move_to(cr, -1, -.5);
        char temp[100];
        for (auto sym : keys_down) {
          cairo_text_path(cr, itoa(sym, temp, 99));
          cairo_rel_move_to(cr, 0.1, 0);
        }
        cairo_restore(cr);
        */

	cairo_save(cr);

	cairo_rotate(cr, mouse_angle);

	// guy
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0, 0, 0.1, 0, 2*M_PI); //body
	cairo_set_source_rgb(cr, 246.0/255.0, 187.0/255.0, 66.0/255.0);
	cairo_fill_preserve(cr); //fill body
	cairo_set_line_width(cr, 0.01);
	cairo_set_source_rgb(cr, 150.0/255.0, 83.0/255.0, 83.0/255.0);
	cairo_stroke(cr); //outline body
	cairo_new_sub_path(cr);
/*
	cairo_arc(cr, -0.05, 0.05, 0.025, 0, 2*M_PI); //left eye
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0.05, 0.05, 0.025, 0, 2*M_PI); // right eye
	cairo_set_source_rgb(cr, 0,0,0);
	cairo_fill_preserve(cr); // fill eyes
	cairo_set_line_width(cr, 0.01);
	cairo_set_source_rgb(cr, 0.5,0.5,1);
	cairo_stroke(cr); // outline eyes
*/
	// axe
	cairo_save(cr);
	if (mouse_down) cairo_rotate(cr, M_PI/2);
  else cairo_rotate(cr, M_PI/4);

	cairo_move_to(cr, 0.1, 0);
	cairo_line_to(cr, 0.34, 0);
	cairo_set_line_width(cr, 0.02);
	cairo_set_source_rgb(cr, 150.0/255.0, 83.0/255.0, 83.0/255.0); // handle color
	cairo_stroke(cr);
	cairo_new_sub_path(cr);
	cairo_arc(cr, 0.25, 0.01, 0.08, 0, M_PI);
	cairo_set_source_rgb(cr, 246.0/255.0, 187.0/255.0, 66.0/255.0); // blade color
	cairo_fill_preserve(cr);
	cairo_set_line_width(cr, 0.02);
	cairo_set_source_rgb(cr, 248.0/255.0, 200.0/255.0, 104.0/255.0); // edge color
	cairo_stroke(cr);

	cairo_restore(cr);

	cairo_restore(cr);

}

int main(int nargs, char * args[])
{
    init();

    SDL_Surface * sdlsurf = SDL_CreateRGBSurface(
        0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
        0x00FF0000, // red
        0x0000FF00, // green
        0x000000FF, // blue
        0); // alpha

    //TODO make sure sdlsurf is locked or doesn't need locking

    cairo_surface_t * csurf = cairo_image_surface_create_for_data(
        (unsigned char *) sdlsurf->pixels,
        CAIRO_FORMAT_RGB24,
        sdlsurf->w,
        sdlsurf->h,
        sdlsurf->pitch);

    cairo_t * cr1 = cairo_create(csurf);
    cairo_t * cr2 = cairo_create(csurf);

    cairo_scale(cr2, SCREEN_WIDTH/2.0, -SCREEN_HEIGHT/2.0);
    cairo_translate(cr2, 1, -1);

    BLIT_READY = SDL_RegisterEvents(1);
    thread loopthread(gameloop, cr1);

    SDL_Surface * wsurf = SDL_GetWindowSurface(gWindow);

    bool done = false;
    while (! done)
    {
        SDL_Event e;
        SDL_WaitEvent(& e); //TODO check for error

        if (e.type == SDL_QUIT) done = true;
        else if (e.type == BLIT_READY) {
            SDL_BlitSurface(sdlsurf, NULL, wsurf, NULL);
            SDL_UpdateWindowSurface(gWindow);
        }
	else if (e.type == SDL_MOUSEMOTION) {
            double x = e.motion.x;
            double y = e.motion.y;
//            cout << x << "," << y << " ";

            cairo_device_to_user(cr2, & x, & y);

            mouse_angle = cpvtoangle(cpv(x,y)) - M_PI/2;
//            cout << x << "," << y << " " << mouse_angle << endl;
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN) {
	    mouse_down = true;
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
	    mouse_down = false;
	}
  else if (e.type == SDL_KEYDOWN) {
    keys_down.insert(e.key.keysym.sym);
	}
	else if (e.type == SDL_KEYUP) {
    keys_down.erase(e.key.keysym.sym);
	}
    }

    loopthread.detach();

    close();

    return 0;
}
