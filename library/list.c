#include <stdlib.h>
#include <stdio.h>
#include "body.h"

#include "list.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

/**
 * A growable array of pointers.
 * Can store values of any pointer type (e.g. vector_t*, body_t*).
 * The list automatically grows its internal array when more capacity is needed.
 */
typedef struct list{
    void **data;
    size_t size;
    size_t capacity;
    free_func_t freer;
}list_t;

/**
 * A function that can be called on list elements to release their resources.
 * Examples: free, body_free
 */
typedef void (*free_func_t)(void *);

/**
 * Allocates memory for a new list with space for the given number of elements.
 * The list is initially empty.
 * Asserts that the required memory was allocated.
 *
 * @param initial_size the number of elements to allocate space for
 * @param freer if non-NULL, a function to call on elements in the list
 *   in list_free() when they are no longer in use
 * @return a pointer to the newly allocated list
 */
list_t *list_init(size_t initial_size, free_func_t freer){
    list_t *list = malloc(sizeof(list_t));
    void **array_data = malloc(initial_size * sizeof(void*));

    assert(list != NULL);
    assert(array_data != NULL);

    list->data = array_data;
    list->size = 0;
    list->capacity = initial_size;
    list->freer = freer;

    return list;
}

/**
 * Releases the memory allocated for a list.
 *
 * @param list a pointer to a list returned from list_init()
 */
void list_free(list_t *list){
    int size = list->size;

    free_func_t fft = list->data;

    for (int i = 0; i < size; i++){
        fft(list->data[i]);
    }
    
    fft(list->data);
    fft(list);
}

/**
 * Gets the size of a list (the number of occupied elements).
 * Note that this is NOT the list's capacity.
 *
 * @param list a pointer to a list returned from list_init()
 * @return the number of elements in the list
 */
size_t list_size(list_t *list){
    return list->size;
}

/**
 * Gets the element at a given index in a list.
 * Asserts that the index is valid, given the list's current size.
 *
 * @param list a pointer to a list returned from list_init()
 * @param index an index in the list (the first element is at 0)
 * @return the element at the given index, as a void*
 */
void *list_get(list_t *list, size_t index){
    assert(index < list->size);
    return list->data[index];
}

/**
 * Removes the element at a given index in a list and returns it,
 * moving all subsequent elements towards the start of the list.
 * Asserts that the index is valid, given the list's current size.
 *
 * @param list a pointer to a list returned from list_init()
 * @return the element at the given index in the list
 */
void *list_remove(list_t *list, size_t index){
    assert(list->size > 0);
    void *temp = list->data[index];
    for (int i = index; i < list->size - 1; i++){
        list->data[i] = list->data[i + 1];
    }
    list->size--;
    return temp;
}

/**
 * Appends an element to the end of a list.
 * If the list is filled to capacity, resizes the list to fit more elements
 * and asserts that the resize succeeded.
 * Also asserts that the value being added is non-NULL.
 *
 * @param list a pointer to a list returned from list_init()
 * @param value the element to add to the end of the list
 */
void list_add(list_t *list, void *value){
    assert(value != NULL);
    if (list->size >= list->capacity){
        int prev_size = list->size;
        list_resize(list, prev_size * 2);
    }
    list->data[list->size] = value;
    list->size++;
}

/**
 * Resizes given list to capactiy size.
 * 
 * @param list a pointer to the list being resized
 * @param size the new size of the list
*/
void list_resize(list_t *list, size_t size){
    list->capacity = size;
    void **temp = realloc(list->data, sizeof(void*) * size);
    list->data = temp;
}