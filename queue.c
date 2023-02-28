#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    struct list_head *node, *safe;
    element_t *entry = NULL;

    list_for_each_safe (node, safe, l) {
        entry = list_entry(node, element_t, list);
        free(entry->value);
        free(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));

    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));

    if (!node)
        return false;

    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_entry(head->next, element_t, list);
    list_del_init(head->next);

    if (sp) {
        strncpy(sp, entry->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *entry = list_entry(head->prev, element_t, list);
    list_del_init(head->prev);

    if (sp) {
        strncpy(sp, entry->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *mid = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        mid = mid->next;
        fast = fast->next->next;
    }

    element_t *entry = list_entry(mid, element_t, list);
    list_del_init(mid);
    free(entry->value);
    free(entry);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    element_t *entry, *safe;
    bool needDel = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head && strcmp(entry->value, safe->value) == 0) {
            list_del_init(&entry->list);
            free(entry->value);
            free(entry);
            needDel = true;
        } else if (needDel) {
            list_del_init(&entry->list);
            free(entry->value);
            free(entry);
            needDel = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;

    struct list_head *h;
    for (h = head; (h->next != head) && (h->next->next != head);
         h = h->next->next)
        list_move(h->next->next, h);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node, *safe;

    list_for_each_safe (node, safe, head) {
        list_del(node);
        list_add(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k < 2 ||
        k > q_size(head))
        return;

    struct list_head *node, *safe, *tmpHead = head;
    int count = 0;

    list_for_each_safe (node, safe, head) {
        list_del(node);
        list_add(node, tmpHead);

        count++;
        if (count == k) {
            tmpHead = safe->prev;
            count = 0;
        }
    }
}

/* Merge Two Sorted Lists */
struct list_head *mergeTwoLists(struct list_head *l1, struct list_head *l2)
{
    /* cppcheck-suppress nullPointerRedundantCheck */
    struct list_head *l1_tail = l1->prev, *l2_tail = l2->prev;
    l1_tail->next = NULL;
    l2_tail->next = NULL;

    struct list_head *head = NULL, **ptr = NULL, **node = NULL, *prev = NULL;
    for (ptr = &head; l1 && l2; ptr = &(*ptr)->next) {
        element_t *l1_entry = list_entry(l1, element_t, list);
        element_t *l2_entry = list_entry(l2, element_t, list);
        node = ((strcmp(l1_entry->value, l2_entry->value) <= 0) ? &l1 : &l2);
        *ptr = *node;
        (*ptr)->prev = prev;
        *node = (*node)->next;
        prev = *ptr;
    }

    if (l1) {
        *ptr = l1;
        head->prev = l1_tail;
        l1_tail->next = head;
    } else {
        *ptr = l2;
        head->prev = l2_tail;
        l2_tail->next = head;
    }
    (*ptr)->prev = prev;
    return head;
}

/* Merge K Sorted Lists */
struct list_head *mergeKLists(struct list_head *h)
{
    if (list_empty(h))
        return h;

    struct list_head *slow = h, *fast = h->next, *mid;

    while (fast != h && fast->next != h) {
        slow = slow->next;
        fast = fast->next->next;
    }
    mid = slow->next;
    mid->prev = h->prev;
    h->prev->next = mid;
    slow->next = h;
    h->prev = slow;

    slow = mergeKLists(h);
    fast = mergeKLists(mid);
    return mergeTwoLists(slow, fast);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *newHead = head->next;
    list_del_init(head);
    newHead = mergeKLists(newHead);
    list_add(head, newHead->prev);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return 1;

    q_reverse(head);

    element_t *entry, *safe;
    element_t *entryHead = list_entry(head->next, element_t, list);

    int size = 0, del = 0;

    list_for_each_entry_safe (entry, safe, head, list) {
        size++;
        if (strcmp(entryHead->value, entry->value) > 0) {
            list_del_init(&entry->list);
            free(entry->value);
            free(entry);
            del++;
        } else {
            entryHead = entry;
        }
    }
    q_reverse(head);
    return size - del;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *chainHead = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return chainHead->size;

    struct list_head *queue = chainHead->q->next, *chain = NULL;
    list_del_init(chainHead->q);

    queue_contex_t *entry, *safe;
    int total = chainHead->size;

    list_for_each_entry_safe (entry, safe, head, chain) {
        if (entry == chainHead)
            continue;
        chain = entry->q->next;
        list_del_init(entry->q);
        queue = mergeTwoLists(queue, chain);
        total += entry->size;
    }
    list_add(chainHead->q, queue->prev);
    return total;
}
