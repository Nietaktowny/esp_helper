/**
 * @file linked_list.c
 * @author Wojciech Mytych
 * @brief Linked list library source file.
 * @version 1.0.6
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "linked_list.h"
#include "linked_list_internal.h"
#include <inttypes.h>
#include "logger.h"
#include "err_controller.h"
#include "memory_utils.h"
#include "err_c_errors.h"
#include <string.h>
#include <stdlib.h>


linkedl_t linkedl_create(size_t item_size, char* name) {
    linkedl_t list = NULL;
    if(!item_size) {
        LOG_ERROR("item size cannot be 0");
        return NULL;
    }
    if(!name) {
        LOG_VERBOSE("no name, using generic name \"linked list\"");
        name = LINKEDL_DEFAULT_NAME;
    }

    //create new list object
    NEW(list, struct linked_list_obj);
    //init list values
    list->head = NULL;
    list->items_num = 0;
    list->name = name;
    list->size = item_size;
    LOG_DEBUG("created linked list: %s, with item size: %u", list->name, list->size);
    return list;
}

int linkedl_destroy(linkedl_t list) {
    ERR_C_CHECK_NULL_PTR(list, LOG_WARN("list is not initialized"));
    int err = linkedl_clear(list);
    if(err != 0) {
        LOG_ERROR("error %d when clearing list %s nodes", err, list->name);
    }
    DELETE(list);
    return err;
}

int linkedl_clear(linkedl_t list) {
    ERR_C_CHECK_NULL_PTR(list, LOG_WARN("list is not initialized"));
    LOG_VERBOSE("about to destroy list: %s", list->name);

    if(!(list->head) && !(list->items_num)) {
        //head is null and items_num is 0, this means that list is empty, just info user and return
        LOG_WARN("list %s is empty", list->name);
        return 0;
    }

    //if list isn't null
    int num = 0;
    node_t ptr = list->head;
    while (ptr != NULL)
    {
        //if it isn't last node
        if(ptr->next != NULL) {
            node_t next_ptr = ptr->next;
            LOG_VERBOSE("deleting node %llu form list %s", ptr->id, list->name);
            memutil_zero_memory(ptr->item, list->size);     //clear user data
            DELETE(ptr->item);
            DELETE(ptr);
            ptr = next_ptr;
            num++;
        } else {
            //if it's last node just free it and break
            memutil_zero_memory(ptr->item, list->size);     //clear user data
            DELETE(ptr->item);
            DELETE(ptr);
            num++;
            break;
        }
    }
    list->items_num = 0;
    list->head = NULL;
    LOG_DEBUG("deleted %d nodes from list: %s", num, list->name);
    return 0;
}

node_t linkedl_create_node(linkedl_t list, char* name) {
    //create node
    node_t node = NULL;
    NEW(node, struct node_obj);
    node->id = linkedl_hash_string(name);
    node->next = NULL;
    NEW_SIZE(node->item, list->size);
    LOG_VERBOSE("created new node with ID %llu updating node data...", node->id);
    return node;
}

uint64_t linkedl_add_last(linkedl_t list, void* data, char* name) {
    node_t new_node = NULL;

    ERR_C_CHECK_NULL_PTR(list, LOG_ERROR("list is not initialized"));
    ERR_C_CHECK_NULL_PTR(data, LOG_ERROR("data cannot be null"));

    // create node to hold item
    new_node = linkedl_create_node(list, name); //there is no need for checking allocation, this function will exit when it fails
    memcpy(new_node->item, data, list->size);

    LOG_VERBOSE("updating list data...");
    //if head is null, this is first node to be created
    if(list->head == NULL) {
        list->head = new_node;
        list->items_num++;
        LOG_DEBUG("added node with ID %llu to list: %s", new_node->id, list->name);
        return new_node->id;        // if everything is ok, then always return id of new node
    }
    //if head isn't null, then search for last node
    node_t last_node = linkedl_get_last_node(list);
    if(!last_node) {
        LOG_WARN("didn't found last node");
        return ERR_LINKEDL_SEARCH_ERR;
    }
    last_node->next = new_node;
    list->items_num++;
    LOG_DEBUG("added node with ID %llu to list: %s", new_node->id, list->name);
    return new_node->id;            // if everything is ok, then always return id of new node
}

uint64_t linkedl_add_first(linkedl_t list, void* data, char* name) {
    node_t new_node = NULL;

    ERR_C_CHECK_NULL_PTR(list, LOG_ERROR("list is not initialized"));

    new_node = linkedl_create_node(list, name);
    ERR_C_CHECK_MEM_ALLOC(new_node, LOG_ERROR("cannot create new node in list: %s", list->name));
    memcpy(new_node->item, data, list->size);

    LOG_VERBOSE("updating list data...");
    //if head is null, and this is first node
    if(!list->head && list->items_num == 0) {
        list->head = new_node;
        list->items_num++;
        LOG_DEBUG("added node with ID %llu to list: %s", new_node->id, list->name);
        return new_node->id;                                               // if everything is ok, then always return id of new node
    }
    //if there is already some node
    new_node->next = list->head;
    list->head = new_node;
    list->items_num++;
    LOG_DEBUG("added node with ID %llu to list: %s", new_node->id, list->name);
    return new_node->id;                                       // if everything is ok, then always return id of new node
}

int linkedl_delete(linkedl_t list, char* name) {
    ERR_C_CHECK_NULL_PTR(list, LOG_WARN("list is not initialized"));
    if(!name) {
        LOG_WARN("name cannot be null");
        return ERR_C_WRONG_ARGS;
    }
    if (!list->head && list->items_num == 0)
    {
        LOG_WARN("list: %s is empty, no items to delete", list->name);
        return 0;
    }
    uint64_t id = linkedl_hash_string(name);
    LOG_VERBOSE("searching for node to delete...");
    node_t node_to_delete = linkedl_find_node_with_id(list, id);
    if(!node_to_delete) {
        LOG_ERROR("not found with current id");
        return ERR_C_WRONG_ARGS;
    }
    LOG_VERBOSE("node found, updating list information...");
    node_t previous_node = linkedl_find_previous_node(list, node_to_delete);
     
    if(previous_node) {
        previous_node->next = node_to_delete->next;
        LOG_VERBOSE("updated information in previous node: %llu, deleting...", previous_node->id);
    } else {
        //no previous node, no need to update information, just dealloc and return
        LOG_VERBOSE("no previous node, no need to update data, deleting...");
    }

    //clear user data
    memutil_zero_memory(node_to_delete->item, list->size);

    //dealloc node
    DELETE(node_to_delete->item);
    DELETE(node_to_delete);

    LOG_DEBUG("deleted node: %llu from list: %s", id, list->name);
    return 0;
}

node_t linkedl_find_previous_node(linkedl_t list, node_t node) {
    if(!list) {
        LOG_WARN("list is not initialized");
        return NULL;
    }
    if(!node) {
        LOG_WARN("node cannot be null");
        return NULL;
    }
    //if node is first, then there is no previous node
    if(list->head == node) {
        LOG_WARN("node is first, no previous node");
        return NULL;
    }
    //start iterating from head
    node_t previous = list->head;
    while (previous != NULL)
    {
        if(previous->next == node) {
            LOG_VERBOSE("found previous: %llu of node: %llu in list: %s", previous->id, node->id, list->name);
            return previous;
        }
        previous = previous->next;
    }

    return NULL;
}

node_t linkedl_get_last_node(linkedl_t list) {
    if(!list) {
        LOG_ERROR("list is not initialized");
        return NULL;
    }

    if(!list->head && !list->items_num) {
        LOG_VERBOSE("list is empty");
        return NULL;
    }
    node_t ptr = list->head;
    while (ptr != NULL)
    {
        if(ptr->next == NULL) {
            LOG_VERBOSE("found last node with ID %llu", ptr->id);
            return ptr;
        }
        ptr = ptr->next;
    }

    LOG_VERBOSE("not found any node");
    return NULL;
}

node_t linkedl_find_node_with_id(linkedl_t list, uint64_t id) {
    if(!list) {
        LOG_ERROR("list is not initialized");
        return NULL;
    }

    if(!list->head && !list->items_num) {
        LOG_ERROR("list is empty");
        return NULL;
    }

    if(id == 0) {
        LOG_ERROR("id of node cannot be 0");
        return NULL;
    }
    node_t ptr = list->head;
    while (ptr != NULL)
    {
        if(ptr->id == id) {
            LOG_VERBOSE("found node with ID %llu", ptr->id);
            return ptr;
        }
        ptr = ptr->next;
    }
    LOG_WARN("didn't found node with id: %llu", id);
    return NULL;
}

void* linkedl_find(linkedl_t list, char* name) {
    if(!list) {
        LOG_ERROR("list is not initialized");
        return NULL;
    }
    if(!name) {
        LOG_ERROR("name of node cannot be null");
        return NULL;
    }
    uint64_t id = linkedl_hash_string(name);
    node_t node = linkedl_find_node_with_id(list, id);
    if(!node) {
        LOG_WARN("cannot find node %llu in list %s", id, list->name);
        return NULL;
    }
    LOG_VERBOSE("found node %llu in list: %s", id, list->name);
    return node->item; 
}

void* linkedl_find_with_id(linkedl_t list, uint64_t id) {
    if(!list) {
        LOG_ERROR("list is not initialized");
        return NULL;
    }
    if(!id) {
        LOG_ERROR("id of node cannot be 0");
        return NULL;
    }
    node_t node = linkedl_find_node_with_id(list, id);
    if(!node) {
        LOG_WARN("cannot find node %llu in list %s", id, list->name);
        return NULL;
    }
    LOG_VERBOSE("found node %llu in list: %s", id, list->name);
    return node->item;     
}

uint64_t linkedl_hash_string(char* string) {
    uint64_t hash = 5381;
    int c = 0;

    if(!string) {
        LOG_ERROR("string pointer cannot be null");
        return 0;
    }

    LOG_VERBOSE("string to hash: %s", string);
    while((c = (*string++))) {
        hash = ((hash << 5) + hash) + c;   /*hash * 33 + c*/
    }
    LOG_VERBOSE("hash is %llu", hash);
    return hash;
}