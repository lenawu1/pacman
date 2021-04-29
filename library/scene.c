#ifndef __SCENE_H__
#define __SCENE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "body.h"
#include "list.h"

const size_t INIT_CAPACITY = 100;

typedef struct scene{
    list_t *bodies;
    size_t size;
}scene_t;

scene_t *scene_init(void)
{
    scene_t *scene = malloc(sizeof(scene_t));
    assert(scene != NULL);
    scene->bodies = list_init(INIT_CAPACITY, (free_func_t) body_free);
    return scene;
}

void scene_free(scene_t *scene){
    list_free(scene->bodies);
    free(scene);   
}

size_t scene_bodies(scene_t *scene){
    return list_size(scene->bodies);
}

body_t *scene_get_body(scene_t *scene, size_t index){
    assert(index < scene_bodies(scene));
    return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body){
    list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index){
    assert(index <= list_size(scene->bodies));
    body_t *removed = list_remove(scene->bodies, index);
    body_free(removed);
}

void scene_tick(scene_t *scene, double dt){
    for (size_t i = 0; i < list_size(scene->bodies); i++){
        body_tick(list_get(scene->bodies, i), dt);
    }
}

#endif // #ifndef __SCENE_H__
