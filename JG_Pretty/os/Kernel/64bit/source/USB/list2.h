#ifndef LIST2_H
#define LIST2_H

#include "../types.h"

// double linked list element
typedef struct element_t element;
struct element_t
{
    void*        data;
    element* prev;
    element* next;
};

typedef struct
{
    element* head;
    element* tail;
} list_t;



#define      list_init() {0, 0}
void         list_construct(list_t* list);                             // Initializes given list_t instance
void         list_destruct(list_t* list);                              // Destructs given list_t instance
void         list_free(list_t* list);                                  // Deletes everything that has been allocated for this list.
element* list_alloc_elem(size_t size, const char* description);    // Allocates memory for a list element with additional space for data
element* list_insert(list_t* list, element* next, void* data); // Inserts a new element before an element of the list (0 = append). Returns a pointer to the new element.
element* list_append(list_t* list, void* data);                    // Inserts a new element at the end of the list and returns a pointer to it.
void         list_append_elem(list_t* list, element* elem);        // Inserts given element at the end of the list.
element* list_delete(list_t* list, element* elem);             // Deletes the element elem and returns a pointer to the element that was behind it.
element* list_getElement(list_t* list, uint32_t number);           // Returns the data at the position "number".
element* list_find(const list_t* list, void* data);                // Finds an element with data in the list and returns a pointer to it.
size_t       list_getCount(const list_t* list);                        // Returns the number of elements of the list
bool         list_isEmpty(const list_t* list);
void         list_show(const list_t* list);


#endif
