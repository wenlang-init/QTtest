#ifndef _MLIST_H
#define _MLIST_H

#ifdef __cplusplus
extern "C" {
#endif

//#define LIST_ARRAY_INDEX

typedef struct _listobj List;
typedef struct _listnode List_Node;

struct _listnode{
    List_Node *prev;
    List_Node *next;
    void* data;
};

struct _listobj{
    //int count;

    #ifdef LIST_ARRAY_INDEX
    List_Node **index;
    #endif
    struct _ro
    {
        List_Node *head;
        int count;
    }ro;

    struct _function
    {
        List_Node *(*prepend)(List *obj,void *data); // 添加到头部
        List_Node *(*append)(List *obj,void *data); // 添加到尾部
        List_Node *(*insert_next)(List *obj,List_Node *node,void *data); // 插入到node之前
        List_Node *(*insert_prev)(List *obj,List_Node *node,void *data); // 插入到node之后

        #ifdef LIST_ARRAY_INDEX
        List_Node *(*insert_at)(List *obj,int index,void *data); // 插入到index位置

        int (*swap)(List *obj,int i,int j); // 交换i和j
        int (*move)(List *obj,int i,int j); // 移动i到j
        #endif

        List_Node *(*find_node)(List *obj,void *data); // 根据data指针查找node,要求表中数据指针唯一，否则返回第一个找到的,没找到返回NULL
        List_Node *(*at)(List *obj,int index); // 获取第index个node
        #ifdef LIST_ARRAY_INDEX
        int (*find_node_index)(List *obj,void *data); // 根据data指针查找node,要求表中数据指针唯一，否则返回第一个找到的,没找到返回负数
        int (*get_index)(List *obj,List_Node *node); // 根据节点获取下标,失败返回负数
        #endif

        List_Node *(*first)(List *obj); // 获取第1个节点
        List_Node *(*last)(List *obj); // 获取最后1个节点

        int (*removeFirst)(List *obj); // 删除第一项
        int (*removeFirst_d)(List *obj); // 删除第一项(包含data)
        int (*removeLast)(List *obj); // 删除最后一项
        int (*removeLast_d)(List *obj); // 删除最后一项(包含data)

        int (*remove_node)(List *obj,List_Node *node); // 删除node
        int (*remove_node_d)(List *obj,List_Node *node); // 删除node（包含data,使用free(data)方式释放内存，要求匹配的内存申请机制才可调此方法）
        #ifdef LIST_ARRAY_INDEX
        int (*remove)(List *obj,int index); // 删除index处的节点
        int (*remove_d)(List *obj,int index); // 删除index处的节点（包含data,使用free(data)方式释放内存，要求匹配的内存申请机制才可调此方法）
        #endif
        void (*clear)(List *obj); // 删除表所有节点
        void (*clear_d)(List *obj); // 删除表所有节点（包含data,使用free(data)方式释放内存，要求匹配的内存申请机制才可调此方法）

        void (*destroy)(List *obj); // 删除表对象
        void (*destroy_d)(List *obj); // 删除表对象（包含data,使用free(data)方式释放内存，要求匹配的内存申请机制才可调此方法）

    }func;
    
};

struct _head_info
{
    int count; // 子节点个数
    #ifdef LIST_ARRAY_INDEX
    int node_max; // index的最大指针数（链表最大节点数）
    List_Node **index; // 根据count申请一片内存来保存各节点指针，实现数组下标方式来访问。当count接近申请的内存时扩张内存
    #endif
};

List *init_list(); // 初始化链表(添加链表头)
List_Node *insert_node_head(List *obj,void *data); // 添加到头部
List_Node *insert_node_tail(List *obj,void *data); // 添加节点到尾部
List_Node *insert_node_next(List *obj,List_Node *node,void *data); // 在node后插入节点
List_Node *insert_node_prev(List *obj,List_Node *node,void *data); // 在node前插入节点
#ifdef LIST_ARRAY_INDEX
List_Node *insert_at(List *obj,int index,void *data); // 插入节点到index
#endif
List_Node *get_node_first(List *obj); // 获取第一个
List_Node *get_node_last(List *obj); // 获取最后一个
List_Node *get_node_at(List *obj,int index); // 获取第index个node
List_Node *get_node(List *obj,void *data); // 查找node
List_Node *get_node_next(List *obj,List_Node *node); // 获取node的下一个节点
List_Node *get_node_prev(List *obj,List_Node *node); // 获取node的上一个节点
#ifdef LIST_ARRAY_INDEX
int find_node_index(List *obj,void *data); // 根据data指针查找node,要求表中数据指针唯一，否则返回第一个找到的,没找到返回负数
int get_index(List *obj,List_Node *node); // 根据节点获取下标,要求表中数据指针唯一，否则返回第一个找到的,失败返回负数
#endif
List_Node *get_node_index(List *obj,int index); // 获取第index个节点
void destroy_list_node(List *obj); // 删除链表全部节点(不包含头)
void destroy_list_node_all(List *obj); // 删除链表全部节点(不包含头,包含data)

int remove_node_First(List *obj); // 删除第一个
int remove_node_First_d(List *obj); // 删除第一个,包含data
int remove_node_Last(List *obj); // 删除第一个
int remove_node_Last_d(List *obj); // 删除第一个,包含data

int remove_node(List *obj,List_Node *node); // 删除node
void destroy_list(List *obj); // 删除链表
int remove_node_all(List *obj,List_Node *node); // 删除node,包含data
void destroy_list_all(List *obj); // 删除链表,包含data

#ifdef LIST_ARRAY_INDEX
int remove_node_at(List *obj,int index); // 删除index处的节点
int remove_node_at_d(List *obj,int index); // 删除index处的节点(包含data)

int swap_node(List *obj,int i,int j); // 交换i和j
int move_node(List *obj,int i,int j); // 移动i到j
#endif
#ifdef __cplusplus
}
#endif

#endif
