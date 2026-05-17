#include "mList.h"
#include <stdlib.h>
#if defined(__unix__)
#include <errno.h>
#endif
#include <string.h>
#include "printFunction.h"


#ifdef LIST_ARRAY_INDEX
// 链表最大节点数初始值
#define INIT_LIST_NODE_NUM 1024*2
// 数组个数阈值
#define ARRAY_THRESHOLD_VALUE 10
// 每次扩展的数组数
#define ARRAY_EXTENSION_VALUE 1024
#endif
/*
    功能：
        初始化链表(添加链表头)
    参数：
    返回值：
        成功：链表指针
        失败：NULL
*/
List *init_list()
{
    List *listobj = (List*)malloc(sizeof(List));
    if(listobj == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return NULL;
    }

    listobj->ro.head = (List_Node*)malloc(sizeof(List_Node));
    List_Node *head = listobj->ro.head;
    if(head == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        free(listobj);
        return NULL;
    }
    head->prev = NULL;
    head->next = NULL;
    struct _head_info *headinfo = (struct _head_info*)malloc(sizeof(struct _head_info));
    if(headinfo == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        free(listobj->ro.head);
        free(listobj);
        return NULL;
    }
    headinfo->count = 0;
    head->data = headinfo;
    #ifdef LIST_ARRAY_INDEX
    headinfo->node_max = INIT_LIST_NODE_NUM;
    // 初始化数组指针
    headinfo->index = (List_Node **)malloc(headinfo->node_max*sizeof(List_Node *));
    if(headinfo->index == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        free(headinfo);
        free(listobj->ro.head);
        free(listobj);
        return NULL;
    }
    memset(headinfo->index,0,headinfo->node_max*sizeof(List_Node *));
    
    listobj->index = headinfo->index;
    #endif
    listobj->ro.count = headinfo->count;

    listobj->func.prepend = insert_node_head;
    listobj->func.append = insert_node_tail;
    listobj->func.insert_next = insert_node_next;
    listobj->func.insert_prev = insert_node_prev;
    #ifdef LIST_ARRAY_INDEX
    listobj->func.insert_at = insert_at;
    listobj->func.swap = swap_node;
    listobj->func.move = move_node;
    #endif
    listobj->func.first = get_node_first;
    listobj->func.last = get_node_last;
    listobj->func.at = get_node_at;
    listobj->func.find_node = get_node;
    #ifdef LIST_ARRAY_INDEX
    listobj->func.find_node_index = find_node_index;
    listobj->func.get_index = get_index;
    #endif
    listobj->func.removeFirst = remove_node_First;
    listobj->func.removeFirst_d = remove_node_First_d;
    listobj->func.removeLast = remove_node_Last;
    listobj->func.removeLast_d = remove_node_Last_d;

    listobj->func.remove_node = remove_node;
    listobj->func.remove_node_d = remove_node_all;
    #ifdef LIST_ARRAY_INDEX
    listobj->func.remove = remove_node_at;
    listobj->func.remove_d = remove_node_at_d;
    #endif
    listobj->func.clear = destroy_list_node;
    listobj->func.clear_d = destroy_list_node_all;
    listobj->func.destroy = destroy_list;
    listobj->func.destroy_d = destroy_list_all;

    return listobj;
}

#ifdef LIST_ARRAY_INDEX
/*
    功能：
        扩展index内存
    参数：
        obj:链表对象
        headinfo：节点头保存的数据
    返回值：
        成功：0
*/
static int memory_extension(List *obj,struct _head_info *headinfo)
{
    if(headinfo == NULL){
        return 0;
    }
    if(headinfo->count >= headinfo->node_max - ARRAY_THRESHOLD_VALUE){
        // 扩展内存
        List_Node **p = (List_Node **)realloc(headinfo->index,(headinfo->node_max+ARRAY_EXTENSION_VALUE)*sizeof(List_Node *)); // 失败原内存不变
        if(p == NULL){
            return -1;
        }
        headinfo->index = p;

        memset(headinfo->index + headinfo->node_max,0,ARRAY_EXTENSION_VALUE*sizeof(List_Node *));

        headinfo->node_max += ARRAY_EXTENSION_VALUE;

        obj->index = headinfo->index;
        //FATAL_PRINT_LOG("abc:%d\n",headinfo->node_max);
    } else if(headinfo->node_max >= headinfo->count + ARRAY_THRESHOLD_VALUE + INIT_LIST_NODE_NUM + ARRAY_EXTENSION_VALUE){
        // 释放内存
        List_Node **p = (List_Node **)realloc(headinfo->index,(headinfo->node_max-ARRAY_EXTENSION_VALUE)*sizeof(List_Node *)); // 失败原内存不变
        if(p == NULL){
            FATAL_PRINT_LOG("reduce menory failed\n");
            return 0;
        }
        headinfo->index = p;

        headinfo->node_max -= ARRAY_EXTENSION_VALUE;

        obj->index = headinfo->index;
        //FATAL_PRINT_LOG("xzz:%d\n",headinfo->node_max);
    }
    
    return 0;
}
// 插入到数组头,array:数组指针,count:原数组数,node:数组数据
static void insert_array_head(List_Node **array,int count,List_Node *node)
{
    if(count == 0){
        array[0] = node;
        return;
    }

    int i;
    for(i=count+1;i>0;i--){
        array[i] = array[i-1];
    }
    array[0] = node;
}
// 插入到尾部
static void insert_array_tail(List_Node **array,int count,List_Node *node)
{
    array[count] = node;
}
#endif
// 判断node是否在列表中,0:表示在
static int check_node(List *obj,List_Node *node)
{
    if(obj == NULL || node == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    List_Node *p = head->next;
    while (p != NULL)
    {
        if(p == node){
            return 0;
        }
        p = p->next;
    }
    return -2;
}
/*
    功能：
        添加节点到头部
    参数：
        obj:链表对象
        data：链表要处理/保存的数据
    返回值：
        成功：链表新插入的节点指针
        失败：NULL
*/
List_Node *insert_node_head(List *obj,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return NULL;
    } else {
        List_Node *n = (List_Node *)malloc(sizeof(List_Node));
        if(n == NULL){
            FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
            return NULL;
        }
        struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
        if(0 != memory_extension(obj,headinfo)){
            FATAL_PRINT_LOG("insert_node_head failed\n");
            return NULL;
        }
        // 插入指针到index头
        insert_array_head(headinfo->index,headinfo->count,n);
    #endif
        if(head->next != NULL){
            n->next = head->next;
            head->next->prev = n;
            head->next = n;
            n->prev = head;
        } else {
            n->next = NULL;
            head->next = n;
            n->prev = head;
        }
        n->data = data;
        headinfo->count++;

        obj->ro.count = headinfo->count;
        return n;
    }
}

/*
    功能：
        添加节点到尾部
    参数：
        obj:链表对象
        data：链表要处理/保存的数据
    返回值：
        成功：链表新插入的节点指针
        失败：NULL
*/
List_Node *insert_node_tail(List *obj,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return NULL;
    } else {
        struct _head_info *headinfo = (struct _head_info*)head->data;

        List_Node *p = head;
        #ifdef LIST_ARRAY_INDEX
            if(headinfo->count > 0){
                p = headinfo->index[headinfo->count-1];
            }
        #else
            while (p->next != NULL)
            {
                p = p->next;
            }
        #endif
        
        List_Node *n = (List_Node *)malloc(sizeof(List_Node));
        if(n == NULL){
            FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
            return NULL;
        }
        
        #ifdef LIST_ARRAY_INDEX
        if(0 != memory_extension(obj,headinfo)){
            FATAL_PRINT_LOG("insert_node_head failed\n");
            return NULL;
        }
        // 插入指针到index尾部
        insert_array_tail(headinfo->index,headinfo->count,n);
        #endif
        p->next = n;
        n->next = NULL;
        n->prev = p;

        n->data = data;
        headinfo->count++;

        obj->ro.count = headinfo->count;
        return n;
    }
}
/*
    功能：
        在node后插入节点
    参数：
        obj:链表对象
        node：要插入位置前的节点的指针
        data：链表要处理/保存的数据
    返回值：
        成功：链表新插入的节点指针
        失败：NULL
*/
List_Node *insert_node_next(List *obj,List_Node *node,void *data){
    if(node == NULL){
        FATAL_PRINT_LOG("insert failed\n");
        return NULL;
    }
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(0 != check_node(obj,node)){
        FATAL_PRINT_LOG("have not found node in list\n");
        return NULL;
    }

    List_Node *n = (List_Node *)malloc(sizeof(List_Node));
    if(n == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return NULL;
    }
    struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
        if(0 != memory_extension(obj,headinfo)){
            FATAL_PRINT_LOG("insert_node_head failed\n");
            return NULL;
        }
        // 插入指针到index的node之后
        {
            int i,j;
            for(i=0;i<headinfo->count;i++){
                if(headinfo->index[i] == node){
                    i++;
                    break;
                }
            }
            for(j=headinfo->count;j>i;j--){
                headinfo->index[j] = headinfo->index[i-1];
            }
            headinfo->index[j] = n;
        }
    #endif
    if(node->next != NULL){
        node->next->prev = n;
        n->next = node->next;
        node->next = n;
        n->prev = node;
    } else {
        n->next = NULL;
        node->next = n;
        n->prev = node;
    }

    n->data = data;
    headinfo->count++;

    obj->ro.count = headinfo->count;
    return n;
}
/*
    功能：
        在node前插入节点
    参数：
        obj:链表对象
        node：要插入位置后的节点的指针
        data：链表要处理/保存的数据
    返回值：
        成功：链表新插入的节点指针
        失败：NULL
*/
List_Node *insert_node_prev(List *obj,List_Node *node,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(0 != check_node(obj,node)){
        FATAL_PRINT_LOG("have not found node in list\n");
        return NULL;
    }

    if(head == NULL || node == NULL || node == head){
        FATAL_PRINT_LOG("insert failed\n");
        return NULL;
    }
    
    List_Node *n = (List_Node *)malloc(sizeof(List_Node));
    if(n == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return NULL;
    }
    struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
        if(0 != memory_extension(obj,headinfo)){
            FATAL_PRINT_LOG("insert_node_head failed\n");
            return NULL;
        }
        // 插入指针到index的node之前
        {
            int i,j;
            for(i=0;i<headinfo->count;i++){
                if(headinfo->index[i] == node){
                    break;
                }
            }
            for(j=headinfo->count;j>i;j--){
                headinfo->index[j] = headinfo->index[i-1];
            }
            headinfo->index[j] = n;
        }
    #endif
    node->prev->next = n;
    n->prev = node->prev;
    node->prev = n;
    n->next = node;

    n->data = data;
    headinfo->count++;

    obj->ro.count = headinfo->count;
    return n;
}
#ifdef LIST_ARRAY_INDEX
/*
    功能：
        插入节点到index
    参数：
        obj:链表对象
        index：要插入位置
        data：链表要处理/保存的数据
    返回值：
        成功：链表新插入的节点指针
        失败：NULL
*/
List_Node *insert_at(List *obj,int index,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    if(index < 0 || index > headinfo->count){
        //index范围需在已有节点间
        //return NULL;
        if(index < 0)index=0;
        if(index > headinfo->count)index=headinfo->count;
    }
    
    if(0 != memory_extension(obj,headinfo)){
        FATAL_PRINT_LOG("insert_node_head failed\n");
        return NULL;
    }
    List_Node *node;
    if(index == 0){
        // 插到头
        node = insert_node_head(obj,data);
        return node;
    }
    // 插到node之后
    node = headinfo->index[index-1];
    return insert_node_next(obj,node,data);
}
/*
    功能：
        删除index处的节点
    参数：
        obj:链表对象
        index：要删除位置
    返回值：
        成功：0
*/
int remove_node_at(List *obj,int index)
{
    if(obj == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    if(index < 0 || index >= headinfo->count){
        return -2;
    }
    return remove_node(obj,headinfo->index[index]);
}
/*
    功能：
        删除index处的节点(包含data)
    参数：
        obj:链表对象
        index：要删除位置
    返回值：
        成功：0
*/
int remove_node_at_d(List *obj,int index)
{
    if(obj == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    if(index < 0 || index >= headinfo->count){
        return -2;
    }
    return remove_node_all(obj,headinfo->index[index]);
}
/*
    功能：
        交换i和j
    参数：
        obj:链表对象
        i：节点序号
        j：节点序号
    返回值：
        成功：0
*/
int swap_node(List *obj,int i,int j)
{
    if(obj == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    if(i < 0 || i >= headinfo->count || j < 0 || j >= headinfo->count){
        return -2;
    }
    if(i == j){
        return 0;
    }
    List_Node *node1;
    List_Node *node2;
    if(i < j){
        node1 = headinfo->index[i];
        node2 = headinfo->index[j];
    } else {
        node1 = headinfo->index[j];
        node2 = headinfo->index[i];
    }
    // 交换index
    headinfo->index[i] = node2;
    headinfo->index[j] = node1;
    // 交换节点位置
    List_Node *tmp1 = node1->prev;
    List_Node *tmp2 = node1->next;

    if(i - j == 1 || i - j == -1){
        // 相邻
        node1->prev->next = node2;
        node1->prev = node2;
        node1->next = node2->next;

        node2->prev = tmp1;
        if(node2->next != NULL){
            node2->next->prev = node1;
        }
        node2->next = node1;
    } else {
        node1->prev->next = node2;
        node1->prev = node2->prev;
        if(node1->next != NULL){
            node1->next->prev = node2;
        }
        node1->next = node2->next;

        node2->prev->next = node1;
        node2->prev = tmp1;
        if(node2->next != NULL){
            node2->next->prev = node1;
        }
        node2->next = tmp2;
    }
    return 0;
}
/*
    功能：
        移动i到j
    参数：
        obj:链表对象
        i：节点序号
        j：节点序号
    返回值：
        成功：0
*/
int move_node(List *obj,int i,int j)
{
    if(obj == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    if(i < 0 || i >= headinfo->count || j < 0 || j >= headinfo->count){
        return -2;
    }
    if(i == j){
        return 0;
    }
#if 0
    void *data = headinfo->index[i]->data;
    // 删除i
    remove_node(obj,headinfo->index[i]);
    // 插入到j
    if(i > j){
        insert_at(obj,j+1,data);
    } else {
        insert_at(obj,j-1,data);
    }
#else
    List_Node *node1 = headinfo->index[i];
    List_Node *node2 = headinfo->index[j];
    // 移动节点
    if(i - j == 1 || i - j == -1){
        List_Node *tmp1 = node1->prev;
        if(i < j){
            node1 = headinfo->index[i];
            node2 = headinfo->index[j];
        } else {
            node1 = headinfo->index[j];
            node2 = headinfo->index[i];
        }
        // 相邻
        node1->prev->next = node2;
        node1->prev = node2;
        node1->next = node2->next;

        node2->prev = tmp1;
        if(node2->next != NULL){
            node2->next->prev = node1;
        }
        node2->next = node1;
    } else {
        if(i < j){ // 后移:在j的后面
            node1->prev->next = node1->next;
            if(node1->next != NULL){
                node1->next->prev = node1->prev;
            }

            node1->prev = node2;
            node1->next = node2->next;
            if(node2->next != NULL){
                node2->next->prev = node1;
            }
            node2->next = node1;

        } else { // 前移:在j的前面
            node1->prev->next = node1->next;
            if(node1->next != NULL){
                node1->next->prev = node1->prev;
            }

            node1->prev = node2->prev;
            node1->next = node2;
            node2->prev->next = node1;
            node2->prev = node1;
        }
    }
    // 移动index
    if(i<j){
        // 右移,在j的后面
        List_Node *tmp = headinfo->index[i];
        int k;
        for(k=i;k<j;k++){
            headinfo->index[k] = headinfo->index[k+1];
        }
        headinfo->index[j] = tmp;
    } else {
        // 左移,在j的前面
        List_Node *tmp = headinfo->index[i];
        int k;
        for(k=i;k>j;k--){
            headinfo->index[k] = headinfo->index[k-1];
        }
        headinfo->index[j] = tmp;
    }
#endif
    return 0;
}
#endif

List_Node *get_node_first(List *obj){
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return NULL;
    }
    return head->next;
}
List_Node *get_node_last(List *obj){
    if(obj == NULL){
        return NULL;
    }

    List_Node *head = obj->ro.head;
    if(head == NULL){
        return NULL;
    }

    List_Node *p = head->next;
    while (p != NULL)
    {
        if(p->next == NULL){
            return p;
        }
        p = p->next;
    }
    return NULL;
}

List_Node *get_node_at(List *obj,int index){
    if(obj == NULL){
        return NULL;
    }
    if(index < 0 || index >= obj->ro.count){
        return NULL;
    }

    List_Node *head = obj->ro.head;
    if(head == NULL){
        return NULL;
    }

    int count = 0;
    List_Node *p = head->next;
    while (p != NULL)
    {
        if(count == index){
            return p;
        }
        p = p->next;
        count++;
    }
    return NULL;
}
/*
    功能：
        查找data
    参数：
        obj:链表对象
        data：链表查找的数据
    返回值：
        成功：查找到的节点指针
        失败：NULL
*/
List_Node *get_node(List *obj,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL || data == NULL){
        return NULL;
    }
    List_Node *p = head->next;
    while (p != NULL)
    {
        if(p->data == data){
            break;
        }
        p = p->next;
    }
    return p;
}
/*
    功能：
        获取node的下一个节点
    参数：
        obj:链表对象
        node：要查找位置前的节点的指针
    返回值：
        成功：查找到的节点指针
        失败：NULL
*/
List_Node *get_node_next(List *obj,List_Node *node)
{
    if(obj == NULL){
        return NULL;
    }
    if(0 != check_node(obj,node)){
        FATAL_PRINT_LOG("have not found node in list\n");
        return NULL;
    }

    return node->next;
}
/*
    功能：
        获取node的上一个节点
    参数：
        obj:链表对象
        node：要查找位置后的节点的指针
    返回值：
        成功：查找到的节点指针
        失败：NULL
*/
List_Node *get_node_prev(List *obj,List_Node *node)
{
    if(obj == NULL){
        return NULL;
    }
    if(0 != check_node(obj,node)){
        FATAL_PRINT_LOG("have not found node in list\n");
        return NULL;
    }

    return node->prev;
}
#ifdef LIST_ARRAY_INDEX
/*
    功能：
        根据data指针查找node
        要求表中数据指针唯一，否则返回第一个找到的
    参数：
        obj:链表对象
        data:数据指针
    返回值：
        成功：查找到的节点下标
        失败：负数
*/
int find_node_index(List *obj,void *data)
{
    if(obj == NULL || data == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    int i;
    for(i=0;i<headinfo->count;i++){
        if(headinfo->index[i]->data == data){
            return i;
        }
    }
    return -2;
}
/*
    功能：
        根据node指针查找下标
        要求表中数据指针唯一，否则返回第一个找到的
    参数：
        obj:链表对象
        node:节点指针
    返回值：
        成功：查找到的节点下标
        失败：负数
*/
int get_index(List *obj,List_Node *node)
{
    if(obj == NULL || node == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    struct _head_info *headinfo = (struct _head_info*)head->data;
    int i;
    for(i=0;i<headinfo->count;i++){
        if(headinfo->index[i] == node){
            return i;
        }
    }
    return -2;
}
#endif
/*
    功能：
        获取第index个节点
    参数：
        obj:链表对象
        index:下标
    返回值：
        成功：查找到的节点指针
        失败：NULL
*/
List_Node *get_node_index(List *obj,int index)
{
    if(obj == NULL){
        return NULL;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return NULL;
    }
    struct _head_info *headinfo = (struct _head_info*)head->data;
    FATAL_PRINT_LOG("%d,%d\n",headinfo->count,index);
    if(headinfo->count <= index){
        return NULL;
    }
    List_Node *node = head->next;
    int i;
    for(i=0;i<index;i++){
        if(node != NULL){
            node = node->next;
        } else {
            return NULL;
        }
    }
    return node;
}
/*
    功能：
        释放node内存
    参数：
        node：要释放内存的节点的指针
    返回值：
*/
static void delete_node(List_Node *node)
{
    if(node == NULL){
        return;
    }
    free(node);
}
/*
    功能：
        释放node内存,包含data
    参数：
        node：要释放内存的节点的指针
    返回值：
*/
static void delete_node_all(List_Node *node)
{
    if(node == NULL){
        return;
    }

    if(node->data != NULL){ // 在链表外部申请的内存，链表内部释放
        //FATAL_PRINT_LOG("delete node->data,%p\n",node->data);
        free(node->data);
    }

    free(node);
}

int remove_node_First(List *obj){
    if(obj == NULL){
        return -1;
    }
    return remove_node(obj,obj->func.first(obj));
}

int remove_node_First_d(List *obj){
    if(obj == NULL){
        return -1;
    }
    return remove_node_all(obj,obj->func.first(obj));
}

int remove_node_Last(List *obj){
    if(obj == NULL){
        return -1;
    }
    return remove_node(obj,obj->func.last(obj));
}

int remove_node_Last_d(List *obj){
    if(obj == NULL){
        return -1;
    }
    return remove_node_all(obj,obj->func.last(obj));
}

/*
    功能：
        删除node
    参数：
        obj:链表对象
        node：要移除的节点指针
    返回值：
        成功：0
        失败：-1
*/
int remove_node(List *obj,List_Node *node)
{
    if(obj == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    if(0 != check_node(obj,node)){
        FATAL_PRINT_LOG("have not found node in list\n");
        return -1;
    }
    if(head == NULL || node == NULL || node == head){
        FATAL_PRINT_LOG("remove failed\n");
        return -1;
    }
    struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
        if(0 != memory_extension(obj,headinfo)){
            FATAL_PRINT_LOG("remove_node failed\n");
            return -1;
        }
        // 删除index的node位置
        {
            int i,j=0;
            for(i=0;i<headinfo->count;i++){
                if(headinfo->index[i] == node){
                    j = 1;
                }
                if(i == headinfo->count-1){
                    headinfo->index[i] = NULL;
                    break;
                }
                if(j == 1){
                    headinfo->index[i] = headinfo->index[i+1];
                }
            }
        }
    #endif
    if(node->next != NULL){
        node->next->prev = node->prev;
        node->prev->next = node->next;
    } else {
        node->prev->next = NULL;
    }
    delete_node(node);
    if(headinfo->count > 0)
        headinfo->count--;
    obj->ro.count = headinfo->count;
    return 0;
}
/*
    功能：
        删除node,包含data
    参数：
        obj:链表对象
        node：要移除的节点指针
    返回值：
        成功：0
        失败：-1
*/
int remove_node_all(List *obj,List_Node *node)
{
    if(obj == NULL){
        return -1;
    }
    List_Node *head = obj->ro.head;
    if(0 != check_node(obj,node)){
        FATAL_PRINT_LOG("have not found node in list\n");
        return -1;
    }
    if(head == NULL || node == NULL || node == head){
        FATAL_PRINT_LOG("remove failed\n");
        return -1;
    }
    struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
        if(0 != memory_extension(obj,headinfo)){
            FATAL_PRINT_LOG("remove_node_all failed\n");
            return -1;
        }
        // 删除index的node位置
        {
            int i,j=0;
            for(i=0;i<headinfo->count;i++){
                if(headinfo->index[i] == node){
                    j = 1;
                }
                if(i == headinfo->count-1){
                    headinfo->index[i] = NULL;
                    break;
                }
                if(j == 1){
                    headinfo->index[i] = headinfo->index[i+1];
                }
            }
        }
    #endif
    if(node->next != NULL){
        node->next->prev = node->prev;
        node->prev->next = node->next;
    } else {
        node->prev->next = NULL;
    }
    delete_node_all(node);
    if(headinfo->count > 0)
        headinfo->count--;
    obj->ro.count = headinfo->count;
    return 0;
}
/*
    功能：
        删除链表
    参数：
        obj:链表对象
    返回值：
*/
void destroy_list(List *obj){
    if(obj == NULL){
        return;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return;
    }
   
    destroy_list_node(obj);
    
    // 删除head
    struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
    free(headinfo->index);
    obj->index = NULL;
    #endif
    free(headinfo);
    head->data = NULL;
    free(head);
    obj->ro.head = NULL;
    free(obj);
}
/*
    功能：
        删除链表,包含data
    参数：
        obj:链表对象
    返回值：
*/
void destroy_list_all(List *obj){
    if(obj == NULL){
        return;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return;
    }
    
    destroy_list_node_all(obj);

    // 删除head
    struct _head_info *headinfo = (struct _head_info*)head->data;
    #ifdef LIST_ARRAY_INDEX
    free(headinfo->index);
    obj->index = NULL;
    #endif
    free(headinfo);
    head->data = NULL;
    free(head);
    obj->ro.head = NULL;
    free(obj);
}
/*
    功能：
        删除链表全部节点(不包含头,不包含data)
    参数：
        obj:链表对象
    返回值：
*/
void destroy_list_node(List *obj)
{
    if(obj == NULL){
        return;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return;
    }
    List_Node *node = head->next;
    while (node != NULL)
    {
        remove_node(obj,node);
        node = head->next;
    }
}
/*
    功能：
        删除链表全部节点(不包含头,包含data)
    参数：
        obj:链表对象
    返回值：
*/
void destroy_list_node_all(List *obj)
{
    if(obj == NULL){
        return;
    }
    List_Node *head = obj->ro.head;
    if(head == NULL){
        return;
    }
    //struct _head_info *headinfo = (struct _head_info*)head->data;
    //FATAL_PRINT_LOG("delete all count=%d\n",headinfo->count);
    List_Node *node = head->next;
    while (node != NULL)
    {
        //FATAL_PRINT_LOG("%p\n",node);
        remove_node_all(obj,node);
        node = head->next;
    }
    //FATAL_PRINT_LOG("delete all count=%d\n",headinfo->count);
}
