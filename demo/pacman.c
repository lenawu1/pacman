#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "body.h"
#include "color.h"
#include <math.h>

const vector_t MIN = {.x = 0, .y = 0};
const vector_t MAX = {.x = 1000, .y = 500};
const double MOUTH_ANGLE = M_PI / 3;
const double PACMAN_RADIUS = 10.0; 
const double PELLET_RADIUS = 2.0;
const double PACMAN_START_MOVING_SPEED = 10.0;
const vector_t CENTER = {500, 250}; // where pacman will spawn
const int NUM_VERTICES = 15;
const double MASS = 10.0; // not sure why we need this, need it to initialize body_t

// we need these
body_t *pacman;
scene_t *scene;

void add_pacman_vertices(double x, double y, list_t *vertices){
    vector_t pacman_vector = {.x = x, .y = y};
    vector_t *v = malloc(sizeof(vector_t));
    *v = pacman_vector;
    int num_vertices = sizeof(vertices);
    list_add(vertices, v);
}

body_t *create_pacman(){
    list_t *pacman = pacman_init();
    return body_init(pacman, MASS, rgb_color_init());
}

const double NUM_PELLETS = 10;
const double PELLET_RADIUS = 2;

#define WINDOW_WIDTH = 1000
#define WINDOW_HEIGHT = 500

vector_t random_loc(){
    vector_t vec_loc = vec_init(rand() % (int) (WINDOW_WIDTH - PELLET_RADIUS/2) + PELLET_RADIUS/2,
        );
    return vec_loc;
}

void *create_pellet(scene_t *scene){
    list_t *pellets = list_init(NUM_PELLETS, free);
    // making each pellet
    polygon_translate(pellets, random_loc()); //randomize the center for each pellet

}

//method to check if any
list_t *pacman_init(){
    list_t *pacman_list = malloc(NUM_VERTICES * sizeof(vector_t));
    //how do we do the math for a semicircle with a mouth?
    //how do we take into account size of radius 
    //and position of center of circle?
    //take each vector initialized and add it to the list using list_add
    //return that list
    return pacman_list;
}

list_t *pellet_init(){
    list_t *pellet_list = malloc(NUM_VERTICES * sizeof(vector_t));
    //we want 15 vertices to connect using draw_polygon
    //360 / 15 = 24 degrees 
    //loop 15 times to initialize vectors that change position 
    //by 24 degrees each time
    //how do we take into account size of radius 
    //and position of center of circle?
    //take each vector initialized and add it to the list using list_add
    //return that list
    double start_angle = (360 / NUM_VERTICES) * (M_PI / 180); 
    double change_angle = (360 / NUM_VERTICES) * (M_PI / 180); // ik i have to pull out the magic number

    for (int i = 0; i < NUM_VERTICES; i++){
        vector_t *vertex = malloc(sizeof(vector_t));
        vertex->x = cos(start_angle + (i + 1) * change_angle) * PELLET_RADIUS + center.x; 
        vertex->y = sin(start_angle + (i + 1) * change_angle) * PELLET_RADIUS + center.y;
        }
    return pellet_list;
}

/* when it eats a pellet we want to free the list of vectors of that pellet
this function might not be void, but i will just put it down as void. */
void pacman_eat_pellet();

/* we need to make sure that pacman wraps around. method for checking if
it is at the boundary. */
void pacman_wrap_around();

/* potential helper function for wrap around. */
bool pacman_at_boundary();

/** 
 * See documentation in sdl_wrapper.h line 40.
 * Creates a key_hander_t called key_handler which can be passed into sdl_on_key to control
 * keyboard input.
 * 
 * @param key the key pressed
 * @param type can be KEY_PRESSED or KEY_RELEASED
 * @param held_time how long the key was held down
**/
void key_handler(char key, key_event_type_t type, double held_time){
    if (type == KEY_PRESSED){
        held_time += 1.0; // idk, however often the program updates or something, this might slow it down
        switch(key){
            case DOWN_ARROW:
                body_set_rotation(pacman, 3 * M_PI / 2); // 270 degrees
                vector_t down_v = {.x = 0.0, .y = -1.0 * PACMAN_START_MOVING_SPEED};
                body_set_velocity(pacman, vec_multiply(held_time, down_v));
                break; // one at a time
            case UP_ARROW:
                body_set_rotation(pacman, M_PI / 2); // 90 degrees
                vector_t up_v = {.x = 0.0, .y = PACMAN_START_MOVING_SPEED};
                body_set_velocity(pacman, vec_multiply(held_time, up_v));
                break;
            case LEFT_ARROW:
                body_set_rotation(pacman, M_PI); // 180 degrees
                vector_t left_v = {.x = -1.0 * PACMAN_START_MOVING_SPEED, .y = 0};
                body_set_velocity(pacman, vec_multiply(held_time, left_v));
                break;
            case RIGHT_ARROW:
                body_set_rotation(pacman, 0.0);
                vector_t right_v = {.x = PACMAN_START_MOVING_SPEED, .y = 0};
                body_set_velocity(pacman, vec_multiply(held_time, right_v));
                break;
        }
    }
    else{
        /* this should stop the pacman from moving since the key is released
        but i am too lazy to write it rn so i just said return but
        i will fix it tmw. */
        return;
    }
}

int main(){
    scene_t *scene = scene_init();
    sdl_init(MIN, MAX);
    double clock = 0.0;

    // add pacman
    // add pellets

    sdl_on_key(key_handler); //handles keyboard input - inside while loop or not? - asked devin

    while(!sdl_is_done())
    {
        double dt = time_since_last_tick();
        clock += dt;

        // spawn pellets at time intervals, kinda like gravity stars
        
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
}