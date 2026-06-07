#ifndef _PY_T9_CORE_H
#define _PY_T9_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL (void*)0
#endif

typedef unsigned long long u64;
// 最大个数为MAX_WORD_LINE行
#define MAX_WORD_LINE 100
// 查找时最大显示行数(部分匹配时,全匹配时还是实际行数)
#define MAX_SECTION_LINE 6
// 最大匹配的数字按键数
#define MAX_KEY_NUM_VALUE 16

struct _word_data{
    char *PY; // 拼音
    char *MB; // t9输出中文
};

// 当前查找的数据的存储结构
typedef struct{
    int count; // 数据条数
    char T9[MAX_KEY_NUM_VALUE+1]; // 输入数字字符
    struct _word_data data[MAX_WORD_LINE]; // 实际字的数据(最大个数为12行)
}table_entry_t;

// 特殊位置标志
struct _array_word{
    int count; // 该标志下的数量
    int start; // 中文数组位置标志
    int end; // 结束位置下标
};

// 每条数据存储的单元指针
struct _lib_word_data{
    char *T9key; // 按键
    char *chinese; // 汉字
    char *pinyin; // 拼音
    int key_count; // 数字键个数
    u64 Key_code; // 按键编码,由低到高每4位表示一个按键数(2~9，最多处理16个按键)。如：23456789则为"(u64)2<<60|(u64)3<<56|(u64)4<<52|(u64)5<<48|(u64)6<<44|(u64)7<<40|(u64)8<<36|(u64)9<<32"
};

typedef struct _py_obj PY_OBJ;

struct _py_obj{
    // 固定数据
    struct _pyro{
        char *lib_data; // 字库数据
        struct _lib_word_data *lib_array; // 字库数组
        int lib_array_count; // 数的个数
        /**
         * 表示中文数组下标开始位置，start_index[0]保留，start_index[1~9]对应字符'1'~'9'的第一个位置
        **/
        struct _array_word start_index_zhcn[10];
    }ro;
    // 可变数据
    struct _pyrw{
        int index; // 当前查找的位置
        table_entry_t find_data; // 当前查找到的数据
    }rw;
    // 对象函数方法
    struct _pyfunction{
        // 删除拼音对象
        void (*delete_pyobj)(PY_OBJ *obj);
        /* 根据按键值获取拼音和汉字数据,失败返回NULL,key为按键字符串
        数据样式：
            32226
            fa bao,发包 法宝
            da ban,大板 大班 大阪 大半 打扮
            da bao,大包 大宝 打包
            da can,大餐
        */
        const table_entry_t *(*get_py_cn_from_key)(PY_OBJ *obj,char *key);
        // 重置搜索
        void (*reset_search)(PY_OBJ *obj);
    }func;
};
// 初始化拼音对象,lib_words_name字库文件,为NULL时使用默认值
PY_OBJ *init_pyobj(char *lib_words_name);
// 删除拼音对象
void delete_pyobj(PY_OBJ *obj);
/* 根据按键值获取拼音和汉字数据,失败返回NULL,key为按键字符串
        数据样式：
            32226
            fa bao,发包 法宝
            da ban,大板 大班 大阪 大半 打扮
            da bao,大包 大宝 打包
            da can,大餐
*/
const table_entry_t *get_py_cn_from_key(PY_OBJ *obj,char *key);
// 重置搜索
void reset_search(PY_OBJ *obj);


void test_t9py();

#ifdef __cplusplus
}
#endif

#endif // !PY_T9_CORE_H
