#ifndef PTR_HIDE_DATA_H_
#define PTR_HIDE_DATA_H_

// here, assume user ensure real pointer at least 4-bytes alignment.
#define PTR_HIDE_DATA_MASK    (4 - 1)

#define PTR_HIDE_DATA_GET_PTR(ptr)    (ptr & ~PTR_HIDE_DATA_MASK)
#define PTR_HIDE_DATA_GET_DATA(ptr)   (ptr & PTR_HIDE_DATA_MASK)
#define PTR_HIDE_DATA_SET_DATA(ptr, data)	(ptr |= (data & PTR_HIDE_DATA_MASK))

#endif

/*

reference: The defination of rb_node in linux kernel is as follow

struct rb_node {
    unsigned long  __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)

*/