#include "py_t9_core.h"
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

// 简拼
#define CHINESE_WORD_LIB_PATH "./chinese_words.txt"
// 繁体
#define CHINESE_FT_WORD_LIB_PATH "./chinese_words2.txt"

// ubuntu
// \033 == \e == 0x1B
#define RESET       "\033[0m"
#define BLACK       "\033[30m"             /* Black 黑色 */
#define RED         "\033[31m"             /* Red 红色 */
#define GREEN       "\033[32m"             /* Green 绿色 */
#define YELLOW      "\033[33m"             /* Yellow 黄的 */
#define BLUE        "\033[34m"             /* Blue 蓝色 */
#define MAGENTA     "\033[35m"             /* Magenta 品红 */
#define CYAN        "\033[36m"             /* Cyan 青色 */
#define WHITE       "\033[37m"             /* White 白色 */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black 粗黑 */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red 粗红 */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

/**
 @brief printf函数的扩展，输出添加文件，行，函数
 @param -参考printf函数
 @return 参考printf函数
*/
#define mprintf(format, ...)\
    do{\
        printf(CYAN"%s:%d"BOLDBLACK"|"RESET BLUE"(%s)"MAGENTA"---"RESET format,__FILE__, __LINE__,  __FUNCTION__, ##__VA_ARGS__);fflush(stdout);\
    } while (0)

// 从文件获取字库数据,成功返回读取的字符数
static int get_lib_data(PY_OBJ *obj,char *lib_words_name){
    FILE *fp = fopen(lib_words_name,"r");
    if(fp == NULL){
        mprintf("fopen \"%s\" failed:%s\n",lib_words_name,strerror(errno));
        return -1;
    }

    fseek(fp,0,SEEK_END);
    long file_len = ftell(fp);
    fseek(fp,0,SEEK_SET);
    if (file_len <= 0){
        mprintf("file lenth:%ld\n",file_len);
        fclose(fp);
        return -2;
    }
    mprintf("file size:%ldbyte\n",file_len);

    obj->ro.lib_data = (char *)malloc(file_len);
    if(obj->ro.lib_data == NULL){
        mprintf("malloc:%s\n",strerror(errno));
        fclose(fp);
        return -3;
    }
    int read_cnt=0;
    int cnt;
    while (1)
    {
        cnt = fread(obj->ro.lib_data+read_cnt,1,file_len-read_cnt,fp);
        
        read_cnt += cnt;
        
        if(cnt == 0){
            break;
        } else if(cnt < 0){
            mprintf("fread:%s\n",strerror(errno));
            fclose(fp);
            free(obj->ro.lib_data);
            return -3;
        }
    }

    fclose(fp);
    return read_cnt;
}

// 解析字库的每条数据到数组,成功返回数据条数
static int get_word_lib_array(PY_OBJ *obj,long datalen){
    if(datalen <= 0){
        mprintf("datalen:%ld\n",datalen);
        return -1;
    }
    // 根据'/n'获取数据条数
    long count=0;
    char *data = obj->ro.lib_data;
    long i;
    for(i=0;i<datalen;i++){
        if(data[i] == '\n'){
            count++;
        }
    }
    //mprintf("conut=%ld\n",count);
    obj->ro.lib_array =  (struct _lib_word_data *)malloc(sizeof(struct _lib_word_data)*count);
    if(obj->ro.lib_array == NULL){
        mprintf("malloc:%s\n",strerror(errno));
        return -2;
    }
    // 数据格式426422454243 钢材价格 T gang cai jia ge
    int index=0; // 下标
    int one_end=-1; // 一条数据结束是置-1
    u64 Key_code;
    int key_i;

    for(i=0;i<datalen;i++){
        if(data[i] == '\n'){
            data[i] = 0; // 置0，便于用字符串访问
            one_end = -1;
            index++;
        } else {
            if(one_end < 0){
                //一条数据开始(一定是字符2~9)
                if(data[i] < '2' || data[i] > '9'){
                    mprintf("lib words error\n");
                    one_end = 100;//丢弃这条数据
                    index--;
                }
                one_end++;
                obj->ro.lib_array[index].T9key = &data[i];
            } else {
                // 中间部分
                if(one_end == 0){
                    if(data[i] == ' '){
                        data[i] = 0;
                        // 计算Key_code
                        obj->ro.lib_array[index].Key_code = 0;
                        int key_len = strlen(obj->ro.lib_array[index].T9key);
                        if(key_len > MAX_KEY_NUM_VALUE)key_len = MAX_KEY_NUM_VALUE;
                        for(key_i=0;key_i<key_len;key_i++){
                            Key_code = (unsigned char)obj->ro.lib_array[index].T9key[key_i]-'0';
                            //obj->ro.lib_array[index].Key_code |= (Key_code << (key_i*4));
                            obj->ro.lib_array[index].Key_code |= (Key_code << ((15-key_i)*4));
                        }
                        obj->ro.lib_array[index].key_count = key_len;

                        one_end++;
                        i++;
                        // 汉字开始地址
                        obj->ro.lib_array[index].chinese = &data[i];
                    }
                } else if(one_end == 1){
                    if(data[i] == ' ' && data[i+1] == 'T' && data[i+2] == ' '){
                        data[i] = 0;
                        data[i+2] = 0;
                        i = i+3;
                        // 拼音开始地址
                        obj->ro.lib_array[index].pinyin = &data[i];
                        one_end++;
                    }
                } else {
                    // 其他
                }
            }
        }
    }
    //mprintf("%p,%s,%d,%llu,%s\n",&obj->ro.lib_array[0],obj->ro.lib_array[0].T9key,obj->ro.lib_array[0].key_count,obj->ro.lib_array[0].Key_code,obj->ro.lib_array[0].pinyin);
    //mprintf("index=%d\n",index);
    return index;
}

#if 0
// 将value的高低位按每4位一起颠倒，如0x0123456789ABCDEF->0xFEDCBA9876543210
static u64 number_perversion(u64 value){
    #if 0
        u64 uk=0x0f;
        u64 nvalue=0;
        int i;
        for(i=0;i<16;i++){
            nvalue += (((value>>((15-i)*4)&uk))<<(i*4));
        }
        return nvalue;
    #endif
    u64 uk=0x0f;
    value = (((value>>60)&uk)<<0)  | (((value>>56)&uk)<<4)  | (((value>>52)&uk)<<8)  | (((value>>48)&uk)<<12) | \
            (((value>>44)&uk)<<16) | (((value>>40)&uk)<<20) | (((value>>36)&uk)<<24) | (((value>>32)&uk)<<28) | \
            (((value>>28)&uk)<<32) | (((value>>24)&uk)<<36) | (((value>>20)&uk)<<40) | (((value>>16)&uk)<<44) | \
            (((value>>12)&uk)<<48) | (((value>>8)&uk)<<52)  | (((value>>4)&uk)<<56)  | (((value>>0)&uk)<<60);
    return value;
}
#endif

// 初始化拼音对象
PY_OBJ *init_pyobj(char *lib_words_name){
    PY_OBJ *obj = (PY_OBJ *)malloc(sizeof(PY_OBJ));
    if(obj == NULL){
        mprintf("malloc:%s\n",strerror(errno));
        return NULL;
    }
    memset(obj,0,sizeof(PY_OBJ));

    if(lib_words_name == NULL){
        lib_words_name = CHINESE_WORD_LIB_PATH;
    }

    long read_len = get_lib_data(obj,lib_words_name);
    if(read_len <= 0){
        mprintf("get word lib failed:%ld\n",read_len);
        free(obj);
        return NULL;
    }

    obj->ro.lib_array_count = get_word_lib_array(obj,read_len);
    if(obj->ro.lib_array_count < 0){
        mprintf("get word lib array failed\n");
        free(obj->ro.lib_data);
        free(obj);
        return NULL;
    }
    mprintf("lib words size=%ld,count=%d\n",read_len,obj->ro.lib_array_count);

    #if 0
    {
    //mprintf("\n");
    // 根据数字按键排序(比较费时)
    int i,j;
    u64 keyvalue_tmp,keyvalue;
    struct _lib_word_data lib_array_tmp;
    for(i=0;i<obj->ro.lib_array_count-1;i++){
        keyvalue = obj->ro.lib_array[i].Key_code;
        //keyvalue = number_perversion(keyvalue);
        for(j=i+1;j<obj->ro.lib_array_count;j++){
            keyvalue_tmp = obj->ro.lib_array[j].Key_code;
            //keyvalue_tmp = number_perversion(keyvalue_tmp);
            if(keyvalue_tmp < keyvalue){
                lib_array_tmp = obj->ro.lib_array[i];
                obj->ro.lib_array[i] = obj->ro.lib_array[j];
                obj->ro.lib_array[j] = lib_array_tmp;
            }
        }
    }
    //mprintf("\n");
    }
    #endif

    #if 0
    {
        int ii=0;
        for(ii=0;ii<obj->ro.lib_array_count;ii++){
            mprintf("%d:%s %s %s\n",ii,obj->ro.lib_array[ii].T9key,obj->ro.lib_array[ii].pinyin,obj->ro.lib_array[ii].chinese);
        }
        //ii = obj->ro.lib_array_count - 1;
        //mprintf("%d:%s %s %s\n",ii,obj->ro.lib_array[ii].T9key,obj->ro.lib_array[ii].pinyin,obj->ro.lib_array[ii].chinese);
    }
    #endif

    // 初始化关键索引
    int i,j=1;
    char last_head_c = 0;
    for(i = 0;i < obj->ro.lib_array_count;i++){
        char c0 = obj->ro.lib_array[i].T9key[0];
        if(c0 < '1' || c0 > '9'){
            mprintf("words error:%s\n",obj->ro.lib_array[i].T9key);
            continue;
        }
        j = c0 - 48;
        if(obj->ro.start_index_zhcn[j].count == 0){
            obj->ro.start_index_zhcn[j].start = i;
        }

        if(last_head_c == 0){
            last_head_c = c0;
        }
        if(last_head_c != c0){
            int k = last_head_c - 48;
            // 结束位置
            last_head_c = c0;
            if(i > 0){
                obj->ro.start_index_zhcn[k].end = i-1;
            }
        }

        obj->ro.start_index_zhcn[j].count++;
    }
    // 最后一个的结尾
    obj->ro.start_index_zhcn[j].end = i-1;

    obj->func.delete_pyobj = delete_pyobj;
    obj->func.get_py_cn_from_key = get_py_cn_from_key;
    obj->func.reset_search = reset_search;

    mprintf("init pinyinobj success.lib words file is \"%s\"\n",lib_words_name);

    return obj;
}

// 删除拼音对象
void delete_pyobj(PY_OBJ *obj){
    if(obj == NULL){
        return;
    }
    free(obj->ro.lib_array);
    free(obj->ro.lib_data);
    free(obj);
}

static int check_key(char *key){
    if(key == NULL)return -1;
    while(*key != 0){
        if(*key > '9' || *key < '1')return -2;
        key++;
    }
    return 0;
}


/* 根据按键值获取拼音和汉字数据,失败返回NULL,key为按键字符串
        数据样式：
            32226
            fa bao,发包 法宝
            da ban,大板 大班 大阪 大半 打扮
            da bao,大包 大宝 打包
            da can,大餐
*/
const table_entry_t *get_py_cn_from_key(PY_OBJ *obj,char *key){
    if(obj == NULL || key == NULL){
        return NULL;
    }

    if(check_key(key) != 0){
        return NULL;
    }

    if(obj->rw.index < 0){
        obj->rw.index = obj->ro.lib_array_count - 1;
    } else if(obj->rw.index >= obj->ro.lib_array_count){
        obj->rw.index = 0;
    }

    size_t key_len = strlen(key);

    struct _lib_word_data *lib_array = obj->ro.lib_array; // 表指针
    //int lib_array_count = obj->ro.lib_array_count; // 表总数
    table_entry_t *find_data = &obj->rw.find_data; // 查找的记录
    int startmin_index = obj->ro.start_index_zhcn[key[0]-48].start; // 开始最小位置
    int endmax_index = obj->ro.start_index_zhcn[key[0]-48].end; // 结束最大位置
    //mprintf("%d,%d\n",startmin_index,endmax_index);

    if(find_data != NULL){
        size_t key_len1 = strlen(find_data->T9);
        if(key_len1 > key_len){
            // T9减少输入数的情况,从前?个位置开始找
            obj->rw.index = startmin_index;
            //obj->rw.index = obj->rw.index - 100;
        }
    } else {
        obj->rw.index = startmin_index;
    }

    if(key_len == 1){
        obj->rw.index = startmin_index;
    }

    if(obj->rw.index > endmax_index || obj->rw.index < startmin_index){
        obj->rw.index = startmin_index;
    }

//    if(key_len+1 > sizeof(find_data->T9)){
//        mprintf("input max number:%u\n",sizeof(find_data->T9)-1);
//        return NULL;
//    }

    size_t key_i;
    if(key_len > MAX_KEY_NUM_VALUE)key_len = MAX_KEY_NUM_VALUE;
    u64 Key_code=0,Key_code_tmp=0;
    for(key_i=0;key_i<key_len;key_i++){
        Key_code_tmp = (unsigned char)key[key_i] - '0';
        //Key_code |= (Key_code_tmp << key_i*4);
        Key_code |= (Key_code_tmp << (15-key_i)*4);
    }
    //mprintf("Key_code=%016llX,%d\n",Key_code,key_len);

    int count = 0;
    int max_len = endmax_index - startmin_index + 1; // 最大查找数
    find_data->count = 0; // 初始化查找数
    int i = obj->rw.index;
    u64 checku64 = ~0;
    //mprintf("%d,%d\n",lib_array_count,i);

    int same_match_index=-1,max_match_index=-1;
    //mprintf("start=%d,maxlen=%d,%d,%d,%d\n",i,max_len,startmin_index,endmax_index,key_len);
    // 查找完全匹配
    while(count++ < max_len){
        if(i > endmax_index)i = startmin_index;
        if(lib_array[i].Key_code == Key_code){
            // 全部相同
            find_data->data[find_data->count].PY = lib_array[i].pinyin;
            find_data->data[find_data->count].MB = lib_array[i].chinese;

            find_data->count++;
            obj->rw.index = i;
            same_match_index = i;
        } else {
            if(find_data->count >= MAX_SECTION_LINE){
                break;
            } else {
                if(find_data->count > 0){
                    max_match_index = same_match_index+1;
                    break;
                } else {
                    if(((checku64<<((MAX_KEY_NUM_VALUE-key_len)*4)) & lib_array[i].Key_code) == Key_code){
                        max_match_index = i;
                        break;
                    }
                }
            }
        }
        i++;
    }
    //mprintf(" %d,%d,%d\n",max_match_index,same_match_index,find_data->count);
    int mmi = max_match_index;
    // 查找部分匹配
    if(find_data->count < MAX_SECTION_LINE && mmi >= 0){
        for(i=find_data->count;i<MAX_SECTION_LINE;i++){
            if(((checku64<<((MAX_KEY_NUM_VALUE-key_len)*4)) & lib_array[mmi].Key_code) == Key_code){
                find_data->data[find_data->count].PY = lib_array[mmi].pinyin;
                find_data->data[find_data->count].MB = lib_array[mmi].chinese;
                mmi++;
                find_data->count++;
            } else {
                break;
            }
        }
    }

    if(find_data->count == 0){
        return NULL;
    }

    if(same_match_index >= 0){
       obj->rw.index = same_match_index+1;
    } else {
        obj->rw.index = max_match_index+1;
    }
    // T9按键
    for(i=0;i<MAX_KEY_NUM_VALUE;i++){
        find_data->T9[i] = key[i];
    }
    find_data->T9[i] = 0;

    return find_data;
}

// 重置搜索
void reset_search(PY_OBJ *obj){
    if(obj == NULL){
        return;
    }
    obj->rw.index = 0;
    memset(&obj->rw.find_data,0,sizeof(table_entry_t));
}

void test_t9py(){
    mprintf("test start\n");
    //"D:/qtwork/pinyin/build-pinyin-Desktop_Qt_5_9_5_MinGW_32bit-Release/release/easyInput/chinese_words1.txt"
    PY_OBJ *obj = init_pyobj(CHINESE_WORD_LIB_PATH);
    if(obj == NULL){
        mprintf("init_pyobj failed\n");
        return;
    }
    int i;

    const table_entry_t *tabledata = obj->func.get_py_cn_from_key(obj,"24546");
    if(tabledata != NULL){
        printf("%s\n",tabledata->T9);
        for(i=0;i<tabledata->count;i++){
            printf("%s,%s\n",tabledata->data[i].PY,tabledata->data[i].MB);
        }
    } else {
        mprintf("not found\n");
    }

    tabledata = obj->func.get_py_cn_from_key(obj,"245464");

    if(tabledata != NULL){
        printf("%s\n",tabledata->T9);
        for(i=0;i<tabledata->count;i++){
            printf("%s,%s\n",tabledata->data[i].PY,tabledata->data[i].MB);
        }
    } else {
        mprintf("not found\n");
    }

    obj->func.reset_search(obj);

    tabledata = obj->func.get_py_cn_from_key(obj,"32226");
    if(tabledata != NULL){
        printf("%s\n",tabledata->T9);
        for(i=0;i<tabledata->count;i++){
            printf("%s,%s\n",tabledata->data[i].PY,tabledata->data[i].MB);
        }
    } else {
        mprintf("not found\n");
    }

    obj->func.delete_pyobj(obj);
    mprintf("test end\n");
}
