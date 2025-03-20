#ifndef _MARRAY_H
#define _MARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _arrayobj Array;
typedef struct _arraynode Array_Node;

struct _arraynode{
    void* data;
};

struct _arrayobj{
    struct _arrayobjro
    {
        int count;
        int node_max; // index的最大指针数（链表最大节点数）
        Array_Node *index; // 数组存储空间
    }ro;

    struct _arrayobjfunction
    {
        Array_Node *(*prepend)(Array *obj,void *data); // 添加到头部
        Array_Node *(*append)(Array *obj,void *data); // 添加到尾部
        Array_Node *(*insert)(Array *obj,int index,void *data); // 插入节点到index

        int (*removeFirst)(Array *obj); // 删除第一项
        int (*removeFirst_d)(Array *obj); // 删除第一项(包含data)
        int (*removeLast)(Array *obj); // 删除最后一项
        int (*removeLast_d)(Array *obj); // 删除最后一项(包含data)

        int (*remove)(Array *obj,int index); // 删除index处的节点
        int (*remove_d)(Array *obj,int index); // 删除index处的节点(包含data)
        int (*remove_node)(Array *obj,Array_Node *node); // 删除node节点
        int (*remove_node_d)(Array *obj,Array_Node *node); // 删除node节点(包含data)

        void (*clear)(Array *obj); // 删除链表全部节点
        void (*clear_d)(Array *obj); // 删除链表全部节点(包含data)

        void (*destroy)(Array *obj); // 删除链表
        void (*destroy_d)(Array *obj); // 删除链表,包含data

        int (*get_node_index)(Array *obj,Array_Node *node); // 获取节点的index
        Array_Node *(*at)(Array *obj,int index); // 获取第index个节点
        Array_Node *(*first)(Array *obj); // 获取第1个节点
        Array_Node *(*last)(Array *obj); // 获取最后个节点

        int (*find_node_index)(Array *obj,void *data); // 根据data指针查找node,要求表中数据指针唯一，否则返回第一个找到的,没找到返回负数
        
        int (*swap)(Array *obj,int i,int j); // 交换i和j
        int (*move)(Array *obj,int i,int j); // 移动i到j
    }func;
    
};

Array *init_array(); // 初始化链表

Array_Node *array_insert_node_head(Array *obj,void *data); // 添加到头部
Array_Node *array_insert_node_tail(Array *obj,void *data); // 添加节点到尾部
Array_Node *array_insert_at(Array *obj,int index,void *data); // 插入节点到index

int array_removeFirst(Array *obj); // 删除第1个
int array_removeFirst_d(Array *obj);
int array_removeLast(Array *obj); // 删除最后一个
int array_removeLast_d(Array *obj);

int array_remove_node_at(Array *obj,int index); // 删除index处的节点
int array_remove_node_at_d(Array *obj,int index); // 删除index处的节点(包含data)

int array_remove_node(Array *obj,Array_Node *node); // 删除node节点
int array_remove_node_d(Array *obj,Array_Node *node); // 删除node节点(包含data)

void array_delete_all_node(Array *obj); // 删除链表全部节点
void array_delete_all_node_d(Array *obj); // 删除链表全部节点(包含data)

void array_destroy(Array *obj); // 删除链表
void array_destroy_d(Array *obj); // 删除链表,包含data

int array_get_node_index(Array *obj,Array_Node *node); // 获取节点的index
Array_Node *array_get_node_at(Array *obj,int index); // 获取第index个节点
Array_Node *array_get_nodeFirst(Array *obj); // 获取第1个节点
Array_Node *array_get_nodeLast(Array *obj); // 获取最后1个节点

int array_find_node_index(Array *obj,void *data); // 根据data指针查找node,要求表中数据指针唯一，否则返回第一个找到的,没找到返回负数

int array_swap_node(Array *obj,int i,int j); // 交换i和j
int array_move_node(Array *obj,int i,int j); // 移动i到j

#ifdef __cplusplus
}
#endif

#endif
