#include "mArray.h"
#include "printFunction.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>


// 数组最大节点数初始值
#define INIT_LIST_NODE_NUM 1024*2
// 数组个数阈值
#define ARRAY_THRESHOLD_VALUE 10
// 每次扩展的数组数
#define ARRAY_EXTENSION_VALUE 1024

/*
    功能：
        初始化数组
    参数：
    返回值：
        成功：数组指针
        失败：NULL
*/
Array *init_array()
{
    Array *array = (Array *)malloc(sizeof(Array));
    if(array == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        return NULL;
    }

    array->ro.node_max = INIT_LIST_NODE_NUM;
    array->ro.index = (Array_Node *)malloc(sizeof(Array_Node)*array->ro.node_max);
    if(array->ro.index == NULL){
        FATAL_PRINT_LOG("malloc:%s\n",strerror(errno));
        free(array);
        return NULL;
    }

    array->ro.count = 0;

    // 数组方法
    array->func.prepend = array_insert_node_head;
    array->func.append = array_insert_node_tail;
    array->func.insert = array_insert_at;

    array->func.removeFirst = array_removeFirst;
    array->func.removeFirst_d = array_removeFirst_d;
    array->func.removeLast = array_removeLast;
    array->func.removeLast_d = array_removeLast_d;

    array->func.remove = array_remove_node_at;
    array->func.remove_d = array_remove_node_at_d;
    array->func.remove_node = array_remove_node;
    array->func.remove_node_d = array_remove_node_d;

    array->func.clear = array_delete_all_node;
    array->func.clear_d = array_delete_all_node_d;

    array->func.destroy = array_destroy;
    array->func.destroy_d = array_destroy_d;

    array->func.get_node_index = array_get_node_index;
    array->func.at = array_get_node_at;
    array->func.first = array_get_nodeFirst;
    array->func.last = array_get_nodeLast;

    array->func.find_node_index = array_find_node_index;

    array->func.swap = array_swap_node;
    array->func.move = array_move_node;

    return array;
}
/*
功能：扩展数组存储空间内存
参数：
    obj:数组对象
返回值：
    成功:0
    失败:-1
*/
static int memory_extension(Array *obj)
{
    if(obj == NULL){
        return -1;
    }
    if(obj->ro.count >= obj->ro.node_max-ARRAY_THRESHOLD_VALUE){
        // 扩展内存
        Array_Node *p = (Array_Node *)realloc(obj->ro.index,(obj->ro.node_max+ARRAY_EXTENSION_VALUE)*sizeof(Array_Node));
        if(p == NULL){
            FATAL_PRINT_LOG("realloc:%s\n",strerror(errno));
            return -1;
        }
        obj->ro.index = p;
        obj->ro.node_max += ARRAY_EXTENSION_VALUE;
    } else if(obj->ro.node_max >= obj->ro.count+ARRAY_THRESHOLD_VALUE+INIT_LIST_NODE_NUM+ARRAY_EXTENSION_VALUE){
        Array_Node *p = (Array_Node *)realloc(obj->ro.index,(obj->ro.node_max-ARRAY_EXTENSION_VALUE)*sizeof(Array_Node));
        if(p == NULL){
            FATAL_PRINT_LOG("realloc:%s\n",strerror(errno));
            return 0;
        }
        obj->ro.index = p;
        obj->ro.node_max -= ARRAY_EXTENSION_VALUE;
    }
    return 0;
}
/*
功能：添加到头部
参数：
    obj:数组对象
    data:插入的数据的指针
返回值：
    成功:对应的节点
    失败:NULL
*/
Array_Node *array_insert_node_head(Array *obj,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    if(memory_extension(obj) != 0){
        return NULL;
    }

    // 全部向后移动一位
    int i;
    for(i=obj->ro.count-1;i>=0;i--){
        obj->ro.index[i+1] = obj->ro.index[i];
    }

    Array_Node *node = &obj->ro.index[0];
    node->data = data;

    obj->ro.count++;

    return node;
}
/*
功能：添加节点到尾部
参数：
    obj:数组对象
    data:插入的数据的指针
返回值：
    成功:对应的节点
    失败:NULL
*/
Array_Node *array_insert_node_tail(Array *obj,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    if(memory_extension(obj) != 0){
        return NULL;
    }
    
    Array_Node *node = &obj->ro.index[obj->ro.count];
    node->data = data;

    obj->ro.count++;

    return node;
}
/*
功能：插入节点到index
参数：
    obj:数组对象
    index:插入的位置(0到最后范围，超过则插入最近位置)
    data:插入的数据的指针
返回值：
    成功:对应的节点
    失败:NULL
*/
Array_Node *array_insert_at(Array *obj,int index,void *data)
{
    if(obj == NULL){
        return NULL;
    }
    if(memory_extension(obj) != 0){
        return NULL;
    }
    if(index < 0){
        index = 0;
    } else if(index > obj->ro.count){
        index = obj->ro.count;
    }

    // index及后全部向后移动一位
    int i;
    for(i=obj->ro.count-1;i>=index;i--){
        obj->ro.index[i+1] = obj->ro.index[i];
    }

    Array_Node *node = &obj->ro.index[index];
    node->data = data;

    obj->ro.count++;

    return node;
}

// 删除第1个
int array_removeFirst(Array *obj){
    if(obj == NULL){
        return -1;
    }

    return array_remove_node_at(obj,0);
}

int array_removeFirst_d(Array *obj){
    if(obj == NULL){
        return -1;
    }

    return array_remove_node_at_d(obj,0);
}

// 删除最后一个
int array_removeLast(Array *obj){
    if(obj == NULL){
        return -1;
    }

    return array_remove_node_at(obj,obj->ro.count-1);
    return 0;
}

int array_removeLast_d(Array *obj){
    if(obj == NULL){
        return -1;
    }

    return array_remove_node_at_d(obj,obj->ro.count-1);
    return 0;
}

/*
功能：删除index处的节点
参数：
    obj:数组对象
    index:删除的位置(0到最后范围，超过不处理)
返回值：
    成功:0
    失败:-1成功，-2删除成功扩展内存失败
*/
int array_remove_node_at(Array *obj,int index){
    if(obj == NULL){
        return -1;
    }

    if(index < 0 || index >= obj->ro.count){
        return -1;
    }

    // index后全部向前移动一位
    int i;
    for(i=index;i<obj->ro.count-1;i++){
        obj->ro.index[i] = obj->ro.index[i+1];
    }

    obj->ro.count--;

    if(memory_extension(obj) != 0){
        return -2;
    }
    return 0;
}
/*
功能：删除index处的节点(包含data)
参数：
    obj:数组对象
    index:删除的位置(0到最后范围，超过不处理)
返回值：
    成功:0
    失败:-1
*/
int array_remove_node_at_d(Array *obj,int index){
    if(obj == NULL){
        return -1;
    }

    if(index < 0 || index >= obj->ro.count){
        return -1;
    }

    if(obj->ro.index[index].data != NULL){
        free(obj->ro.index[index].data);
    }
    // index后全部向前移动一位
    int i;
    for(i=index;i<obj->ro.count-1;i++){
        obj->ro.index[i] = obj->ro.index[i+1];
    }
    
    obj->ro.count--;

    if(memory_extension(obj) != 0){
        return -2;
    }
    return 0;
}
/*
功能：删除node节点
参数：
    obj:数组对象
    node:删除的节点
返回值：
    成功:0
    失败:-1
*/
int array_remove_node(Array *obj,Array_Node *node)
{
    if(obj == NULL || node == NULL){
        return -1;
    }
    int index = array_get_node_index(obj,node);
    if(index < 0){
        return -1;
    }
    return array_remove_node_at(obj,index);
}
/*
功能：删除node节点(包含data)
参数：
    obj:数组对象
    node:删除的节点
返回值：
    成功:0
    失败:-1
*/
int array_remove_node_d(Array *obj,Array_Node *node)
{
    if(obj == NULL || node == NULL){
        return -1;
    }
    int index = array_get_node_index(obj,node);
    if(index < 0){
        return -1;
    }
    return array_remove_node_at_d(obj,index);
}
/*
功能：删除链表全部节点
参数：
    obj:数组对象
返回值：无
*/
void array_delete_all_node(Array *obj)
{
    if(obj == NULL){
        return;
    }

    obj->ro.count = 0;

    if(memory_extension(obj) != 0){
        return;
    }
}
/*
功能：删除链表全部节点(包含data)
参数：
    obj:数组对象
返回值：无
*/
void array_delete_all_node_d(Array *obj)
{
    if(obj == NULL){
        return;
    }

    int i;
    for(i=0;i<obj->ro.count;i++){
        if(obj->ro.index[i].data != NULL){
            free(obj->ro.index[i].data);
        }
    }

    obj->ro.count = 0;

    if(memory_extension(obj) != 0){
        return;
    }
}
/*
功能：删除链表
参数：
    obj:数组对象
返回值：无
*/
void array_destroy(Array *obj)
{
    if(obj == NULL){
        return;
    }
    free(obj->ro.index);
    free(obj);
}
/*
功能：删除链表,包含data
参数：
    obj:数组对象
返回值：无
*/
void array_destroy_d(Array *obj)
{
    if(obj == NULL){
        return;
    }
    int i;
    for(i=0;i<obj->ro.count;i++){
        if(obj->ro.index[i].data != NULL){
            free(obj->ro.index[i].data);
        }
    }
    free(obj->ro.index);
    free(obj);
}
/*
功能：获取节点的index
参数：
    obj:数组对象
    index:获取的位置(0到最后范围，超过不处理)
返回值：
    成功:对应的节点index
    失败:-1
*/
int array_get_node_index(Array *obj,Array_Node *node)
{
    if(obj == NULL || node == NULL){
        return -1;
    }
    int i;
    for(i=0;i<obj->ro.count;i++){
        if(node == &obj->ro.index[i]){
            return i;
        }
    }
    return -1;
} 
/*
功能：获取第index个节点
参数：
    obj:数组对象
    index:获取的位置(0到最后范围，超过不处理)
返回值：
    成功:对应的节点
    失败:NULL
*/
Array_Node *array_get_node_at(Array *obj,int index)
{
    if(obj == NULL){
        return NULL;
    }
    if(index < 0 || index >= obj->ro.count){
        return NULL;
    }
    return &obj->ro.index[index];
}
// 获取第1个节点
Array_Node *array_get_nodeFirst(Array *obj){
    if(obj == NULL){
        return NULL;
    }
    if(obj->ro.count <= 0){
        return NULL;
    }
    return &obj->ro.index[0];
}
// 获取最后1个节点
Array_Node *array_get_nodeLast(Array *obj){
    if(obj == NULL){
        return NULL;
    }
    if(obj->ro.count <= 0){
        return NULL;
    }
    return &obj->ro.index[obj->ro.count-1];
}

/*
功能：根据data指针查找node,要求表中数据指针唯一，否则返回第一个找到的,没找到返回负数
参数：
    obj:数组对象
    data:要查找的数据的指针
返回值：
    成功:找到的位置
    失败:-1
*/
int array_find_node_index(Array *obj,void *data)
{
    if(obj == NULL){
        return -1;
    }
    int i;
    for(i=0;i<obj->ro.count;i++){
        if(data == obj->ro.index[i].data){
            return i;
        }
    }
    return -1;
}
/*
功能：交换i和j
参数：
    obj:数组对象
    i,j:要交换的位置
返回值：
    成功:0
    失败:-1
*/
int array_swap_node(Array *obj,int i,int j)
{
    if(obj == NULL){
        return -1;
    }
    if(i < 0 || i >= obj->ro.count || j < 0 || j >= obj->ro.count){
        return -1;
    }

    Array_Node node = obj->ro.index[i];
    obj->ro.index[i] = obj->ro.index[j];
    obj->ro.index[j] = node;

    return 0;
}
/*
功能：移动i到j
参数：
    obj:数组对象
    i,j:要移动的位置
返回值：
    成功:0
    失败:-1
*/
int array_move_node(Array *obj,int i,int j)
{
    if(obj == NULL){
        return -1;
    }
    if(i < 0 || i >= obj->ro.count || j < 0 || j >= obj->ro.count){
        return -1;
    }
    if(i < j){
        // 右移,i到j间的数据和j前移一位
        Array_Node node = obj->ro.index[i];
        int k;
        for(k=i;k<j;k++){
            obj->ro.index[k] = obj->ro.index[k+1];
        }
        obj->ro.index[j] = node;
    } else if(i > j){
        // 左移,i到j间的数据和j后移一位
        Array_Node node = obj->ro.index[i];
        int k;
        for(k=i;k>j;k--){
            obj->ro.index[k] = obj->ro.index[k-1];
        }
        obj->ro.index[j] = node;
    }
    return 0;
}

