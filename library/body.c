#include <stdlib.h>
#include <stdio.h>
#include "body.h"

typedef struct body{
    double mass;
    list_t *shape;
    rgb_color_t color;
    vector_t centroid;
    vector_t velocity;
}body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color)
{
    body_t *my_body = malloc(sizeof(body_t));

    my_body->mass = mass;
    my_body->shape = shape;
    my_body->color = color;

    vector_t initial_velocity = {.x = 0, .y = 0};

    my_body->velocity = initial_velocity;
    vector_t centroid = polygon_centroid(shape);
    my_body->centroid = centroid;

    return my_body;
}

void body_free(body_t *body)
{
    list_free(body->shape);
    free(body);
}

list_t *body_get_shape(body_t *body)
{
    return body->shape;
}

vector_t body_get_centroid(body_t *body)
{
    return body->centroid;
}

vector_t body_get_velocity(body_t *body)
{
    return body->velocity;
}

rgb_color_t body_get_color(body_t *body)
{
    return body->color;
}

void body_set_centroid(body_t *body, vector_t x)
{
    body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v)
{
    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle)
{
    polygon_rotate(body->shape, angle, body->centroid);
}

void body_tick(body_t *body, double dt)
{
    vector_t translation = (body->velocity);
    vec_multiply(dt, translation);
    polygon_translate(body->shape, translation);
}