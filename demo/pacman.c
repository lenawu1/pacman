#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "list.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "body.h"
#include "color.h"
#include <math.h>
                                                                                                                                                                                                                                      
const vector_t CENTER = {.x = 500, .y = 250}; // where pacman will spawn

const vector_t WINDOW_MIN = {.x = 0, .y = 0};
const vector_t WINDOW_MAX = {.x = 1000, .y = 500};

const double MOUTH_ANGLE = M_PI / 3.0;
const double PACMAN_RADIUS = 50.0; 
const double PACMAN_START_MOVING_SPEED = 1.0;

const double PELLET_RADIUS = 2.0;
const double MASS = 10.0; // not sure why we need this, need it to initialize body_t
const double VERT_DENSITY = 2 * M_PI / 10;
const int NUM_PELLETS = 14;

const int NUM_VERTICES = 15;
const double MAX_VELOCITY = 0.0;

//these are up here so that functions run independently
body_t *pacman;
scene_t *scene;

vector_t random_loc(){    
    vector_t vec_center = {
        .x = rand() % (int)((WINDOW_MAX.x- PELLET_RADIUS/2) + PELLET_RADIUS/2),
        .y = rand() % (int)((WINDOW_MAX.y - PELLET_RADIUS/2) + PELLET_RADIUS/2)
    };
    return vec_center;
}

// creates the list_t for shape of pacman (pacman vertices)
list_t *pacman_init(){
    list_t *pacman_list = list_init(NUM_VERTICES, free);
    double start_angle = (2 * M_PI); //not sure if this is the right angle, have to test
    double change_angle = (360 / NUM_VERTICES) * (M_PI / 180); // ik i have to pull out the magic number

    vector_t *c = malloc(sizeof(vector_t));
    c->x = CENTER.x;
    c->y = CENTER.y;

    list_add(pacman_list, c);
    
    for (int i = 0; i < NUM_VERTICES - 1; i++){ // -1 for already added center
        vector_t *vertex = malloc(sizeof(vector_t));
        vertex->x = cos(start_angle + (i + 1) * change_angle) * PACMAN_RADIUS + c->x;
        vertex->y = sin(start_angle + (i + 1) * change_angle) * PACMAN_RADIUS + c->y;
        list_add(pacman_list, vertex);
    }

    return pacman_list;
}

// creates the list_t for shape of pellet (pellet vertices)
list_t *pellet_init(){
    list_t *pellet_list = list_init(NUM_VERTICES, free);
    double start_angle = (360 / NUM_VERTICES) * (M_PI / 180);
    double change_angle = (360 / NUM_VERTICES) * (M_PI / 180); // ik i have to pull out the magic number

    vector_t *c = malloc(sizeof(vector_t));
    vector_t pellet_center = random_loc();
    c->x = pellet_center.x;
    c->y = pellet_center.y;

    for (int i = 0; i < NUM_VERTICES; i++){
        vector_t *vertex = malloc(sizeof(vector_t));
        vertex->x = cos(start_angle + (i + 1) * change_angle) * PELLET_RADIUS + c->x; 
        vertex->y = sin(start_angle + (i + 1) * change_angle) * PELLET_RADIUS + c->y;
        list_add(pellet_list, vertex);
    }
    return pellet_list;
}


list_t *multiple_pellets(){
    list_t *multiple_pellet_list = list_init(NUM_VERTICES, free); 
    for(int i = 0; i < NUM_PELLETS; i++){
        list_add(multiple_pellet_list, pellet_init());
    }
    return multiple_pellet_list;
}


/* when it eats a pellet we want to free the list of vectors of that pellet
this function might not be void, but i will just put it down as void. */
//void pacman_eat_pellet();


void pacman_wrap_around()
{
    int num_vertices = list_size(body_get_shape(pacman));
    for(int i = 0; i < num_vertices; i++)
    {
        double y = ((vector_t*)list_get(body_get_shape(pacman), i))->y;
        double x = ((vector_t*)list_get(body_get_shape(pacman), i))->x;
        vector_t curr_centroid = body_get_centroid(pacman);
        if (y <= WINDOW_MIN.y){ 
            vector_t wrapped_centroid = 
            {
                .x = curr_centroid.x, 
                .y = WINDOW_MAX.y - PACMAN_RADIUS
            };
            body_set_centroid(pacman, wrapped_centroid);
        }
        else if (y >= WINDOW_MAX.y)
        {
            vector_t wrapped_centroid = 
            {
                .x = curr_centroid.x, 
                .y = WINDOW_MIN.y + PACMAN_RADIUS
            };
            body_set_centroid(pacman, wrapped_centroid);
        }
        else if (x <= WINDOW_MIN.x)
        {
            vector_t wrapped_centroid = 
            {
                .x = WINDOW_MAX.x - PACMAN_RADIUS, 
                .y = curr_centroid.y
            };
            body_set_centroid(pacman, wrapped_centroid);
        }
        else if (x >= WINDOW_MAX.x)
        {
            vector_t wrapped_centroid = 
            {
                .x = WINDOW_MIN.x + PACMAN_RADIUS, 
                .y = curr_centroid.y
            };
            body_set_centroid(pacman, wrapped_centroid);
        }
    }
}


/** 
 * See documentation in sdl_wrapper.h line 40.
 * Creates a key_hander_t called handler which can be passed into sdl_on_key to control
 * keyboard input.
 * 
 * @param key the key pressed
 * @param type can be KEY_PRESSED or KEY_RELEASED
 * @param held_time how long the key was held down
**/
void handler(char key, key_event_type_t type, double held_time){
    if (type == KEY_PRESSED){
        held_time += 1.0; // idk, however often the program updates or something, this might slow it down
        switch(key){
            case DOWN_ARROW:
                body_set_rotation(pacman, 0.0); //3 * M_PI / 2
                vector_t down_v = {.x = 0.0, .y = -1.0 * PACMAN_START_MOVING_SPEED};
                body_set_velocity(pacman, vec_multiply(held_time, down_v));
                break; // one at a time
            case UP_ARROW:
                body_set_rotation(pacman, 0.0); //M_PI / 2
                vector_t up_v = {.x = 0.0, .y = PACMAN_START_MOVING_SPEED};
                body_set_velocity(pacman, vec_multiply(held_time, up_v));
                break;
            case LEFT_ARROW:
                body_set_rotation(pacman, M_PI);
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
        held_time = 0;
        body_set_velocity(pacman, VEC_ZERO);
    }
}

int main(){
    sdl_init(WINDOW_MIN, WINDOW_MAX);
        
    scene = scene_init();
    double clock = 0.0;
    
    //pellets = body
    //sdl_draw_polygon(pacman_init(), rgb_color_init());
    pacman = body_init(pacman_init(), MASS, rgb_color_init()); //cause of error
    scene_add_body(scene, pacman); 

    // sdl_draw_polygon(body_get_shape(pacman), rgb_color_init());

    // add pellets

    sdl_on_key(handler); //handles keyboard input - inside while loop or not? - asked devin

    while(!sdl_is_done()){
        double dt = time_since_last_tick();
        clock += dt;

        // spawn pellets at time intervals, kinda like gravity stars
        
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
}