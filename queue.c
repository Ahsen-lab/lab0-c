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

    struct list_head *cur, *tmp;
    element_t *elem = NULL;

    list_for_each_safe (cur, tmp, l) {
        elem = list_entry(cur, element_t, list);
        q_release_element(elem);
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

    element_t *elem = list_entry(head->next, element_t, list);
    list_del_init(head->next);

    if (sp) {
        strncpy(sp, elem->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *elem = list_entry(head->prev, element_t, list);
    list_del_init(head->prev);

    if (sp) {
        strncpy(sp, elem->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return elem;
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

    element_t *elem = list_entry(mid, element_t, list);
    list_del_init(mid);
    q_release_element(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    element_t *cur, *tmp;
    bool found_dup = false;
    list_for_each_entry_safe (cur, tmp, head, list) {
        if (&tmp->list != head && strcmp(cur->value, tmp->value) == 0) {
            list_del_init(&cur->list);
            q_release_element(cur);
            found_dup = true;
        } else if (found_dup) {
            list_del_init(&cur->list);
            q_release_element(cur);
            found_dup = false;
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

    struct list_head *cur, *tmp;

    list_for_each_safe (cur, tmp, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k < 2 ||
        k > q_size(head))
        return;

    struct list_head *cur, *tmp, *tmp_head = head;
    int count = 0;

    list_for_each_safe (cur, tmp, head) {
        list_move(cur, tmp_head);

        count++;
        if (count == k) {
            tmp_head = tmp->prev;
            count = 0;
        }
    }
}

/**
 * list_append() - Add all nodes from a list to the end of another list
 * @list: pointer to the head of the list with the node entries to add
 * @head: pointer to the head of the list to add the nodes to
 *
 * All nodes from @list, including the @list head, are added to the end of the
 * list of @head.
 */
void list_append(struct list_head *list, struct list_head *head)
{
    struct list_head *head_last = head->prev;
    struct list_head *list_last = list->prev;

    head_last->next = list;
    list_last->next = head;
    list->prev = head_last;
    head->prev = list_last;
}

/* Merge Two Sorted Lists */
struct list_head *merge_two_lists(struct list_head *l1, struct list_head *l2)
{
    struct list_head *merged_head = NULL, **merged_tail = &merged_head,
                     **cur = NULL;
    element_t *e1 = NULL, *e2 = NULL;

    while (l1->next != merged_head && l2->next != merged_head) {
        e1 = list_entry(l1, element_t, list);
        e2 = list_entry(l2, element_t, list);
        cur = ((strcmp(e1->value, e2->value) <= 0) ? &l1 : &l2);
        *merged_tail = *cur;
        *cur = (*cur)->next;
        list_del_init(*merged_tail);
        list_add_tail(*merged_tail, merged_head);
        merged_tail = &(*merged_tail)->next;
    }
    merged_head == l1->next ? list_append(l2, merged_head)
                            : list_append(l1, merged_head);
    return merged_head;
}

/* Merge K Sorted Lists */
struct list_head *merge_k_lists(struct list_head *h)
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

    slow = merge_k_lists(h);
    fast = merge_k_lists(mid);
    return merge_two_lists(slow, fast);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *new_queue = head->next;
    list_del_init(head);
    new_queue = merge_k_lists(new_queue);
    list_add(head, new_queue->prev);
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

    element_t *cur, *tmp;
    element_t *first = list_entry(head->next, element_t, list);

    int size = 0, deleted_nodes = 0;

    list_for_each_entry_safe (cur, tmp, head, list) {
        size++;
        if (strcmp(first->value, cur->value) > 0) {
            list_del_init(&cur->list);
            q_release_element(cur);
            deleted_nodes++;
        } else {
            first = cur;
        }
    }
    q_reverse(head);
    return size - deleted_nodes;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *head_chain = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return head_chain->size;

    struct list_head *queue = head_chain->q->next, *chain = NULL;
    list_del_init(head_chain->q);

    queue_contex_t *cur, *tmp;
    int total = head_chain->size;

    list_for_each_entry_safe (cur, tmp, head, chain) {
        if (cur == head_chain)
            continue;
        chain = cur->q->next;
        list_del_init(cur->q);
        queue = merge_two_lists(queue, chain);
        total += cur->size;
    }
    list_add(head_chain->q, queue->prev);
    return total;
}
