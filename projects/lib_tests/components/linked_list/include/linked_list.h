#include <stddef.h>
#include <inttypes.h>

//this file contains all error definitions specific to linked list implementation
#include "linked_list_errors.h"
/*
 this should describe general interface of linked list
 it shouldn't care about implementation of linked list
 so that later different kinds of linked list can be implemented without changing interface
*/


/**
 * @brief Default name of list used when during list initialization passed NULL as name.
 * 
 * @note Redefine this to change it.
*/
#ifndef LINKEDL_DEFAULT_NAME
#define LINKEDL_DEFAULT_NAME   "linked_list"
#endif


/**
 * Linked list handle.
*/
typedef struct linked_list_obj* linkedl_t;

/**
 * @brief Creates linked list.
 * 
 * @param size_t size of single item in list
 * @param char* name of list, used for debugging only, can be NULL
 * 
 * @warning This function will call exit() when allocation for the list fails.
 * 
 * @note When name is NULL, name will be set to LINKEDL_DEFAULT_NAME
 * 
 * @return linkedl_t handle to linked list.
*/
linkedl_t linkedl_create(size_t item_size, char* name);

/**
 * @brief Destroys linked list.
 * @param linkedl_t handle to list to destroy
 * @return
 *          - 0 on sucess,
 *          - err code on failure.
*/
int linkedl_destroy(linkedl_t list);

/**
 * @brief Insert new item at end of the list.
 * @note To add item at beginning of the list use linkedl_add_first()
 * 
 * @param linkedl_t handle to list to insert
 * @param void* data to insert
 * @param char* name of item
 *
 * @warning This function will call exit() when allocation for item fails.
 * 
 * @return
 *          - 0 on sucess,
 *          - err code on failure.
*/
uint64_t linkedl_add_last(linkedl_t list, void* data, char* name);

/**
 * @brief Insert new item at beginning of the list.
 * @note To add item at the end of the list use linkedl_add_last()
 *
 * @param linkedl_t handle to list to insert
 * @param void* data to insert. 
 * @param char* name of item
 * 
 * @warning This function will call exit() when allocation for item fails.
 * 
 * @return
 *          - 0 on sucess,
 *          - err code on failure.
*/
uint64_t linkedl_add_first(linkedl_t list, void* data, char* name);

/**
 * @brief Delete item from list.
 * 
 * @param linkedl_t list from which delete item
 * @param char*  identificator of item to delete
 * 
 * @return  
 *          - 0 on success,
            - err on failure. 
*/
int linkedl_delete(linkedl_t list, char* name);

/**
 * @brief Find item in list using it's name.
 * 
 * @param linkedl_t list in which perform search.
 * @param char*     name of item to find.
 * 
 * @note On duplicate names/ID, it returns first found item.
 * 
 * @return
 *          - on success returns pointer to searched item,
 *          - NULL on failure.
 * 
*/
void* linkedl_find(linkedl_t list, char* name);

/**
 * @brief Find item in list using it's ID.
 * 
 * @param linkedl_t list in which perform search.
 * @param uint64_T  ID of item to find.
 * 
 * @note On duplicate names/ID, it returns first found item.
 * 
 * @return
 *          - on success returns pointer to searched item,
 *          - NULL on failure.
 * 
*/
void* linkedl_find_with_id(linkedl_t list, uint64_t id);

/**
 * @brief Deletes all items from the list, and returns it to initial state.
 * 
 * @param linkedl_t list to clear
 * 
 * @return
 *          - 0 on success
 *          - err on failure
*/
int linkedl_clear(linkedl_t list);