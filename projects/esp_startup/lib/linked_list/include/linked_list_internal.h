#include <inttypes.h>

/**
 * @brief Node object struct.
*/
struct node_obj {
    uint64_t id;                        ///< ID of node.
    void* item;                         ///< Data hold in node.
    struct node_obj* next;              ///< Pointer to next node in list.
};
typedef struct node_obj* node_t;        ///< List node type.

/**
 * @struct Linked list object type.
*/
struct linked_list_obj {
    struct node_obj* head;          ///< Linked list first node.
    uint16_t items_num;             ///< Number of items stored.
    uint16_t size;                  ///< Size of single item.
    char* name;                     ///< Name of list.
};

/**
 * @brief  Find last node in list.
 * 
 * @param linkedl_t list handle
 * 
 * @return 
 *          - last node
 *          - NULL if no last node was found
 * 
*/
node_t linkedl_get_last_node(linkedl_t list);

/**
 * @brief Find node using it's ID.
 * 
 * @param linkedl_t list handle
 * @param uint64_t id of node to found - has to be different from 0
 * 
 * @return
 *          - found node
 *          - NULL on error or if no node was found
 * 
*/
node_t linkedl_find_node_with_id(linkedl_t list, uint64_t id);

/**
 * @brief Find previous relative to passed node.
 * 
 * @param linkedl_t list handle
 * @param node_t current node
 * 
 * @return 
 *          - previous node
 *          - NULL if no previous node was found
 * 
 * @note NULL is returned also when there was no error, but node is equal to head in list.
 * 
*/
node_t linkedl_find_previous_node(linkedl_t list, node_t node);

/**
 * @brief Creates and initializes new node.
 * 
 * @note This function will exit when the allocation for memory fails.
*/
node_t linkedl_create_node(linkedl_t list, char* name);

/**
 * @brief Hash string.
 * 
 * @param char* string to hash
 * 
 * @return uin64_t hashed value.
*/
uint64_t linkedl_hash_string(char* string);
