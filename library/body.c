#include <stdlib.h>
#include <stdio.h>
#include "body.h"
#include "list.h"
#include <math.h>

typedef struct body{
    list_t *shape;
    rgb_color_t color;
    vector_t centroid;

    vector_t velocity;

    double mass;
    double direction;
    double angle;
    double abs_theta;
}body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color){
    body_t *my_body = malloc(sizeof(body_t));

    my_body->shape = shape;
    my_body->color = color;
    my_body->abs_theta = 0;
    my_body->centroid = polygon_centroid(shape);

    vector_t initial_velocity = {.x = 0, .y = 0};
    my_body->velocity = initial_velocity;

    my_body->mass = mass;
    my_body->angle = 0;

    return my_body;
}

void body_free(body_t *body){
    list_free(body->shape);
    free(body);
}

list_t *body_get_shape(body_t *body){
    list_t *shape_copy = list_init(list_size(body->shape), list_get_freer(body->shape));

    for (size_t i = 0; i < list_size(body->shape); i++){
        vector_t *prev = list_get(body->shape, i);
        vector_t *new = malloc(sizeof(vector_t));
        new->x = prev->x;
        new->y = prev->y;
        list_add(shape_copy, new);
    }

    return shape_copy;
}

vector_t body_get_centroid(body_t *body){
    return body->centroid;
}

vector_t body_get_velocity(body_t *body){
    return body->velocity;
}

rgb_color_t body_get_color(body_t *body){
    return body->color;
}

void body_set_centroid(body_t *body, vector_t x){
    vector_t translate = vec_subtract(x, body->centroid);
    body->centroid = x;
    polygon_translate(body->shape, translate);
}

void body_set_velocity(body_t *body, vector_t v){
    body->velocity = v; 
}

void body_set_rotation(body_t *body, double angle){
    if (angle != body->abs_theta){
        polygon_rotate(body->shape, angle - body->abs_theta, body->centroid);
    }
    body->abs_theta = angle;
}

void body_tick(body_t *body, double dt){
    vector_t translation = (body->velocity);
    vec_multiply(dt, translation);
    polygon_translate(body->shape, translation);
    body->centroid = polygon_centroid(body->shape);
}