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
const double PACMAN_RADIUS = 30.0; 
const double PACMAN_START_MOVING_SPEED = 1.0;

const double PELLET_RADIUS = 5.0;
const double MASS = 10.0;
const double VERT_DENSITY = 2 * M_PI / 10;
const int NUM_PELLETS = 14;

const int NUM_VERTICES = 15;
const double INITIAL_VELOCITY = 0.0;

body_t *pacman;

vector_t random_loc(){
    vector_t vec_center = (vector_t) {rand() % (int)(WINDOW_MAX.x), rand() % (int)(WINDOW_MAX.y)};
    return vec_center;
}

const double FLATLINE_180 = (M_PI / 180);

// creates the list_t for shape of pacman (pacman vertices)
list_t *pacman_init(){
    list_t *pacman_list = list_init(NUM_VERTICES, free);
    double start_angle = (2 * M_PI); //not sure if this is the right angle, have to test
    double change_angle = (360 / NUM_VERTICES) * FLATLINE_180;

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


// Places pacman on screen.
void add_pacman(scene_t *scene, body_t *pacman){
    scene_add_body(scene, pacman);
}


// Creates the pellet shape and centers it at a random location.
list_t *pellet_init(){
    list_t *pellet_list = list_init(NUM_VERTICES, free);
    double start_angle = (360 / NUM_VERTICES) * FLATLINE_180;
    double change_angle = (360 / NUM_VERTICES) * FLATLINE_180;

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


void add_pellets(scene_t *scene, int num_pellets){
    for (int i = 0; i < num_pellets; i++){
        scene_add_body(scene, body_init(pellet_init(), MASS, rgb_color_init()));
    }
}


bool pacman_close_to_pellet(vector_t pacman_centroid, vector_t pellet_centroid){
    return (pacman_centroid.x + PACMAN_RADIUS > pellet_centroid.x && pacman_centroid.x - PACMAN_RADIUS < pellet_centroid.x 
         && pacman_centroid.y + PACMAN_RADIUS > pellet_centroid.y && pacman_centroid.y - PACMAN_RADIUS < pellet_centroid.y);
}


void pacman_eat_pellet(scene_t *scene){
    list_t *pacman_shape = body_get_shape(pacman);
    vector_t pacman_centroid = polygon_centroid(pacman_shape);

    for (size_t i = 1; i < scene_bodies(scene); i++) {
        body_t *curr_pellet = (body_t *) scene_get_body(scene, i);
        vector_t pellet_centroid = polygon_centroid(body_get_shape(curr_pellet));

        if(pacman_close_to_pellet(pacman_centroid, pellet_centroid)){
            scene_remove_body(scene, i);
            break;
        }
    }
}

void pacman_wrap_around(body_t *pacman){
    for(int i = 0; i < list_size(body_get_shape(pacman)); i++){
        vector_t *point = list_get(body_get_shape(pacman), i);
        vector_t curr_centroid = body_get_centroid(pacman);

        // y low
        if (point->y <= WINDOW_MIN.y){ 
            vector_t wrapped_centroid = {.x = curr_centroid.x, .y = curr_centroid.y + WINDOW_MAX.y};
            body_set_centroid(pacman, wrapped_centroid);
        }
        // y high
        else if (point->y >= WINDOW_MAX.y){
            vector_t wrapped_centroid = {.x = curr_centroid.x, .y = curr_centroid.y - WINDOW_MAX.y};
            body_set_centroid(pacman, wrapped_centroid);
        }
        // x low
        else if (point->x <= WINDOW_MIN.x){ 
            vector_t wrapped_centroid = {.x = curr_centroid.x + WINDOW_MAX.x, .y = curr_centroid.y};
            body_set_centroid(pacman, wrapped_centroid);
        }
        // x high
        else if (point->x >= WINDOW_MAX.x){
            vector_t wrapped_centroid = {.x = curr_centroid.x - WINDOW_MAX.x, .y = curr_centroid.y};
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
        held_time += 1.5;
        if (key == DOWN_ARROW){
            //body_set_rotation(pacman, (3 * M_PI) / 2); //3 * M_PI / 2
            vector_t down_v = {.x = 0.0, .y = -1.0 * PACMAN_START_MOVING_SPEED};
            body_set_velocity(pacman, vec_multiply(held_time, down_v));
        }
        else if (key == UP_ARROW){
            //body_set_rotation(pacman, M_PI / 2); //M_PI / 2
            vector_t up_v = {.x = 0.0, .y = PACMAN_START_MOVING_SPEED};
            body_set_velocity(pacman, vec_multiply(held_time, up_v));
        }
        else if (key == LEFT_ARROW){
            body_set_rotation(pacman, M_PI);
            vector_t left_v = {.x = -1.0 * PACMAN_START_MOVING_SPEED, .y = 0};
            body_set_velocity(pacman, vec_multiply(held_time, left_v));
        }
        else if (key == RIGHT_ARROW){
            body_set_rotation(pacman, 0);
            vector_t right_v = {.x = PACMAN_START_MOVING_SPEED, .y = 0};
            body_set_velocity(pacman, vec_multiply(held_time, right_v));
        }
    }
    else{
        /* this should stop the pacman from moving since the key is release. */
        held_time = 0;
        vector_t velocity = {0, 0};
        body_set_velocity(pacman, velocity);
    } 
}

int main(){
    sdl_init(WINDOW_MIN, WINDOW_MAX);
        
    scene_t *scene = scene_init();
    double clock = 0.0;

    pacman = body_init(pacman_init(), MASS, rgb_color_init());
    add_pacman(scene, pacman);

    add_pellets(scene, NUM_PELLETS);

    sdl_on_key(handler);

    while(!sdl_is_done()){
        double dt = time_since_last_tick();
        clock += dt;

        if (clock >= 3.0){
            add_pellets(scene, 1);
            clock = 0.0;
        }
        
        pacman_wrap_around(pacman);
        pacman_eat_pellet(scene);

        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}