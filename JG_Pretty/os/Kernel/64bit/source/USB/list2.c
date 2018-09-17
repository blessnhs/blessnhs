/*
*  license and disclaimer for the use of this source code as per statement below
*  Lizenz und Haftungsausschluss für die Verwendung dieses Sourcecodes siehe unten
*/

#include "list2.h"
#include "../DynamicMemory.h"

void list_construct(list_t* list)
{
    list->head = 0;
    list->tail = 0;
}

element* list_alloc_elem(size_t size, const char* description)
{
    element* newElement = AllocateMemory(sizeof(element) + size);
    if (newElement)
        newElement->data = newElement + 1;
    return newElement;
}

element* list_append(list_t* list, void* data)
{
    element* newElement = AllocateMemory(sizeof(element));
    if (newElement)
    {
        newElement->data = data;
        list_append_elem(list, newElement);
    }
    return newElement;
}

void list_append_elem(list_t* list, element* elem)
{
    elem->next = 0;
    elem->prev = list->tail;

    if (list->head == 0)
    {
        list->head = elem;
    }
    else
    {
        list->tail->next = elem;
    }
    list->tail = elem;
}

element* list_insert(list_t* list, element* next, void* data)
{
    if (next == 0)
    {
        return (list_append(list, data));
    }

    element* newElement = AllocateMemory(sizeof(element));
    if (newElement)
    {
        newElement->data = data;

        if (next == list->head)
        {
            newElement->next = list->head;
            newElement->prev = 0;
            list->head->prev = newElement;
            list->head       = newElement;
        }
        else
        {
            newElement->prev = next->prev;
            newElement->next = next;
            next->prev->next = newElement;
            next->prev       = newElement;
        }

        return newElement;
    }

    return (0);
}

element* list_delete(list_t* list, element* elem)
{
    if (list->head == 0)
    {
        return (0);
    }

    if (list->head == list->tail)
    {
    	FreeMemory(elem);
        list->head = list->tail = 0;
        return (0);
    }

    element* temp = elem->next;

    if (elem == list->head)
    {
        list->head       = elem->next;
        list->head->prev = 0;
    }
    else if (elem == list->tail)
    {
        list->tail       = elem->prev;
        list->tail->next = 0;
    }
    else
    {
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;
    }

    FreeMemory(elem);

    return temp;
}

void list_destruct(list_t* list)
{
    element* cur = list->head;

    while (cur)
    {
        element* nex = cur->next;
        FreeMemory(cur);
        cur = nex;
    }
    list->head = list->tail = 0;
}

void list_FreeMemory(list_t* list)
{
    list_destruct(list);
    FreeMemory(list);
}

void list_free(list_t* list)
{
    list_destruct(list);
    FreeMemory(list);
}

element* list_getElement(list_t* list, uint32_t number)
{
    element* cur = list->head;
    while (true)
    {
        if (number == 0 || cur == 0)
        {
            return (cur);
        }

        --number;
        cur = cur->next;
    }
}

element* list_find(const list_t* list, void* data)
{
    element* cur = list->head;
    while (cur && cur->data != data)
    {
        cur = cur->next;
    }

    return (cur);
}

size_t list_getCount(const list_t* list)
{
    size_t count = 0;
    element* e;
    for (e = list->head; e; e = e->next)
    {
        count++;
    }
    return (count);
}

bool list_isEmpty(const list_t* list)
{
    return (list->head == 0);
}

void list_show(const list_t* list)
{
    Printf("\nlist element->data: ");
    element* e;
    for (e = list->head; e; e = e->next)
    {
        Printf("\ndata: %X", e->data);
    }
    return;
}


/*
* Copyright (c) 2009-2017 The PrettyOS Project. All rights reserved.
*
* http://www.prettyos.de
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
