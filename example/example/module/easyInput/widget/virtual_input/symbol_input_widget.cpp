#include "symbol_input_widget.h"
#include "../basewidget/slipwidget.h"
#include "../basewidget/mtablewidget.h"

Symbol_Input_Widget::Symbol_Input_Widget(QWidget *parent) : QWidget(parent)
{
    fuhao_rowcnt = 1;

    QPalette pl;
    pl.setBrush(QPalette::Base,QBrush(QColor(255,255,255,100)));
    QFont font("Microsoft Yahei", 8, QFont::Normal);

    list.append(QString::fromUtf8("中文"));
    list.append(QString::fromUtf8("英文"));
    list.append(QString::fromUtf8("网络"));
    list.append(QString::fromUtf8("数学"));
    list.append(QString::fromUtf8("特殊"));
    list.append(QString::fromUtf8("序号"));
    list.append(QString::fromUtf8("部首"));
    list.append(QString::fromUtf8("希腊"));
    list.append(QString::fromUtf8("拉丁"));

    QStringList strlist;
    strlist<<"，"<<"。"<<"？"<<"！"<<"~"<<"、"<<"："<<"＇"<<"；"<<"‘"<<"’"<<"——"<<"（"<<"）"<<"！"<<"【"<<"】"<<"《"<<"》";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"."<<"@"<<"~"<<"-"<<","<<":"<<"*"<<"?"<<"!"<<"_"<<"#"<<"/"<<"="<<"+"<<"^"<<"&"<<";"<<"%"<<"$"<<"￥"<<"("<<")"<<"\\"<<"<"<<">"<<"|"<<"["<<"]"<<"\""<<"{"<<"}"<<"¥";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"@"<<"-"<<"_"<<"/"<<"wap."<<"bbs."<<".net"<<".org"<<".com"<<"www."<<"http://"<<".cn"<<"news."<<"blog."<<"ftp://"<<".中国";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"+"<<"-"<<"×"<<"÷"<<"﹤"<<"﹥"<<"≦"<<"≧"<<"≈"<<"≠"<<"±"<<"∴"<<"∵"<<"㏒"<<"㏑"<<"∑"<<"∏"<<"∅"<<"₁"<<"₂"<<"₃"<<"½"<<"⅓"<<"¼"<<"¾"<<"°"<<"℃"<<"℉"<<"〒"<<"∮"<<"∯"<<"∰"<<"∫"<<"∬"<<"∭"<<"∩"<<"∪"<<"º"<<"¹"<<"²"<<"³"<<"ⁿ"<<"∂"<<"∆";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"♤"<<"♧"<<"♡"<<"♢"<<"♪"<<"♬"<<"♭"<<"✔"<<"✘"<<"♞"<<"♟"<<"☇"<<"✌"<<"☞"<<"☟"<<"☝"<<"☜"<<"♀"<<"♂"<<"☺"<<"☹"<<"◐"<<"◑"<<"…"<<"‥"<<"↖"<<"↑"<<"↗"<<"→"<<"↘"<<"↓"<<"↙"<<"←"<<"㊣"<<"↕"<<"↔"<<"⊱"<<"⋛"<<"⋌"<<"⋚"<<"㊤"<<"㊦"<<"㊧"<<"㊨"<<"㊥"<<"◤"<<"◥"<<"◢"<<"◣"<<"＊"<<"❤";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"①"<<"②"<<"③"<<"④"<<"⑤"<<"⑥"<<"⑦"<<"⑧"<<"⑨"<<"⑩"<<"⑪"<<"⑫"<<"⑬"<<"⑭"<<"⑮"<<"⑯"<<"⑰"<<"⑱"<<"⑲"<<"⑳"
               <<"⑴"<<"⑵"<<"⑶"<<"⑷"<<"⑸"<<"⑹"<<"⑺"<<"⑻"<<"⑼"<<"⑽"<<"⑾"<<"⑿"<<"⒀"<<"⒁"<<"⒂"<<"⒃"<<"⒄"<<"⒅"<<"⒆"<<"⒇"
               <<"⒈"<<"⒉"<<"⒊"<<"⒋"<<"⒌"<<"⒍"<<"⒎"<<"⒏"<<"⒐"<<"⒑"<<"⒒"<<"⒓"<<"⒔"<<"⒕"<<"⒖"<<"⒗"<<"⒘"<<"⒙"<<"⒚"<<"⒛"
               <<"Ⅰ"<<"Ⅱ"<<"Ⅲ"<<"Ⅳ"<<"Ⅴ"<<"Ⅵ"<<"Ⅶ"<<"Ⅷ"<<"Ⅸ"<<"Ⅹ"<<"Ⅺ"<<"Ⅻ"<<"ⅰ"<<"ⅱ"<<"ⅲ"<<"ⅳ"<<"ⅴ"<<"ⅵ"<<"ⅶ"<<"ⅷ"<<"ⅸ"<<"ⅹ"
               <<"❶"<<"❷"<<"❸"<<"❹"<<"❺"<<"❻"<<"❼"<<"❽"<<"❾"<<"❿"
               <<"㈠"<<"㈡"<<"㈢"<<"㈣"<<"㈤"<<"㈥"<<"㈦"<<"㈧"<<"㈨"<<"㈩";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"丶"<<"冫"<<"氵"<<"灬"<<"阝"<<"卩"<<"刂"<<"忄"<<"讠"<<"扌"<<"亻"<<"彳"
               <<"厃"<<"⺁"<<"𠘨"<<"⺆"<<"⺄"<<"广"<<"疒"<<"饣"<<"钅"<<"礻"<<"衤"<<"辶"
               <<"牜"<<"釒"<<"飠"<<"⺪"<<"⻊"<<"糹"<<"⺝"<<"丩"<<"丬"<<"犭"<<"纟"<<"廴"
               <<"攵"<<"夊"<<"皿"<<"臼"<<"虍"<<"勹"<<"尢"<<"廾"<<"歺"<<"夬"<<"氺"<<"丂"
               <<"爫"<<"癶"<<"耂"<<"⺻"<<"⺮"<<"⺳"<<"⺶"<<"⺷"<<"⺈"<<"龵"<<"彐"<<"⺋"
               <<"罒"<<"覀"<<"⻗"<<"亠"<<"宀"<<"冖"<<"艹"<<"⺌"<<"丷"<<"⺧"<<"亇"<<"彡";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"α"<<"β"<<"γ"<<"δ"<<"ε"<<"ζ"<<"η"<<"θ"<<"ι"<<"κ"<<"λ"<<"μ"<<"ν"<<"ξ"<<"ο"<<"π"<<"ρ"<<"σ"<<"τ"<<"υ"<<"φ"<<"χ"<<"ψ"<<"ω";
    listlist.append(strlist);
    strlist.clear();
    strlist<<"À"<<"Á"<<"Â"<<"Ã"<<"Ä"<<"Å"<<"Æ"<<"Ç"<<"È"<<"É"<<"Ê"<<"Ë"<<"Ì"<<"Í"<<"Î"<<"Ï"<<"Ð"<<"Ñ"<<"Ò"<<"Ó"<<"Ô"<<"Õ"<<"Ö"<<"Ø"<<"Ù"<<"Ú"<<"Û"<<"Ü"<<"Ý"<<"Þ"<<"Š"<<"Ÿ"<<"Œ";
    listlist.append(strlist);

    mlistwidget = new SlipWidget(this);
    connect(mlistwidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(itemClicked(QListWidgetItem *)));
    mlistwidget->setPalette(pl);
    mlistwidget->setFont(font);
    mlistwidget->set_listtext(list);

    if(mlistwidget->count() > 0){
        mlistwidget->setCurrentRow(0);
    }

    mtablewidget = NULL;

}

Symbol_Input_Widget::~Symbol_Input_Widget()
{
    if(mtablewidget != NULL){
        for(int i=0;i<listlist.count();i++){
            mtablewidget[i]->deleteLater();
        }
        delete[] mtablewidget;
    }
}

void Symbol_Input_Widget::reset()
{

}

QString Symbol_Input_Widget::current_text()
{
    QString str;
    int row = mlistwidget->currentRow();
    if(row < 0 || row >= mlistwidget->count()){
        return str;
    }

    int mrow = mtablewidget[row]->currentRow();
    int mcolumn = mtablewidget[row]->currentColumn();

    QTableWidgetItem *item = mtablewidget[row]->item(mrow,mcolumn);
    if(item != NULL){
        str = item->text();
    }
    return str;
}

bool Symbol_Input_Widget::set_pretab()
{
    int row = mlistwidget->currentRow();
    row--;
    if(row < 0 || row >= mlistwidget->count()){
        row = mlistwidget->count()-1;
    }

    mlistwidget->setCurrentRow(row);

    if(row >= 0 && row < mlistwidget->count()){
        // 点击第i个
        if(row < listlist.count()){
            for(int j=0;j<listlist.count();j++){
                if(j == row){
                    mtablewidget[j]->show();
                } else {
                    mtablewidget[j]->hide();
                }
            }
        }
    }
    return true;
}

bool Symbol_Input_Widget::set_nexttab()
{
    int row = mlistwidget->currentRow();
    row++;
    if(row < 0 || row >= mlistwidget->count()){
        row = 0;
    }

    mlistwidget->setCurrentRow(row);

    if(row >= 0 && row < mlistwidget->count()){
        // 点击第i个
        if(row < listlist.count()){
            for(int j=0;j<listlist.count();j++){
                if(j == row){
                    mtablewidget[j]->show();
                } else {
                    mtablewidget[j]->hide();
                }
            }
        }
    }
    return true;
}

bool Symbol_Input_Widget::set_pretext()
{
    int row = mlistwidget->currentRow();
    if(row < 0 || row >= mlistwidget->count()){
        return false;
    }

    int mrow = mtablewidget[row]->currentRow();
    int mcolumn = mtablewidget[row]->currentColumn();
    mcolumn--;
    if(mcolumn < 0){
        mrow--;
        if(mrow < 0){
            mrow=mtablewidget[row]->rowCount()-1;
        }
        // 最后一个的位置
        mcolumn = mtablewidget[row]->columnCount()-1;
    }

    if(mtablewidget[row]->item(mrow,mcolumn) == NULL || mtablewidget[row]->item(mrow,mcolumn)->text().isEmpty() == true){
        // 上一个在最后一行，且无效。找到第一个有效的
        for(int i=0;i<mtablewidget[row]->columnCount();i++){
            if(mtablewidget[row]->item(mrow,i) == NULL || mtablewidget[row]->item(mrow,i)->text().isEmpty() == true){
                if(i > 0){
                    mcolumn = i-1;
                } else {
                    mcolumn = 0;
                }
                break;
            } else {
            }
        }
    }

    mtablewidget[row]->setCurrentCell(mrow,mcolumn);

    return true;
}

bool Symbol_Input_Widget::set_nexttext()
{
    int row = mlistwidget->currentRow();
    if(row < 0 || row >= mlistwidget->count()){
        return false;
    }

    int mrow = mtablewidget[row]->currentRow();
    int mcolumn = mtablewidget[row]->currentColumn();
    mcolumn++;
    if(mcolumn >=mtablewidget[row]->columnCount()){
        mcolumn = 0;
        mrow++;
        if(mrow >= mtablewidget[row]->rowCount()){
            mrow=0;
        }
    }
    // 下一个在最后一行，判断是否有效
    if(mrow == mtablewidget[row]->rowCount()-1){
        if(mcolumn < mtablewidget[row]->columnCount()){
            if(mtablewidget[row]->item(mrow,mcolumn) == NULL || mtablewidget[row]->item(mrow,mcolumn)->text().isEmpty() == true){
                mcolumn = 0;
                mrow = 0;
            } else {
            }
        }
    }

    mtablewidget[row]->setCurrentCell(mrow,mcolumn);
    return true;
}

void Symbol_Input_Widget::resizeEvent(QResizeEvent *event)
{

    if(this->width()/60 != fuhao_rowcnt){
        fuhao_rowcnt = this->width()/60;
        // 重新构建符号table
        set_fuhao_table(); // 符号表数据
    }

    mlistwidget->setGeometry(0,0,this->width()/5,this->height());
    for(int j=0;j<listlist.count();j++){
        mtablewidget[j]->setGeometry(mlistwidget->geometry().right(),0,this->width()-mlistwidget->width(),this->height());
    }

    QWidget::resizeEvent(event);
}

void Symbol_Input_Widget::set_fuhao_table()
{
    if(mtablewidget != NULL){
        for(int i=0;i<listlist.count();i++){
            mtablewidget[i]->deleteLater();
        }
        delete[] mtablewidget;
    }

    QPalette pl;
    pl.setBrush(QPalette::Base,QBrush(QColor(255,255,255,100)));
    QFont font("Microsoft Yahei", 8, QFont::Normal);

    mtablewidget = new MTableWidget*[listlist.count()];
    int fuhao_w = (this->width()/5*4)/fuhao_rowcnt;
    for(int i=0;i<listlist.count();i++){
        mtablewidget[i] = new MTableWidget(this,fuhao_rowcnt,fuhao_w);
        //mtablewidge[i]t->setRowCount(1);
        mtablewidget[i]->set_move_state(2);
        connect(mtablewidget[i],SIGNAL(cellClicked(int,int)),this,SLOT(cellClicked(int,int)));
        mtablewidget[i]->setPalette(pl);
        mtablewidget[i]->setFont(font);
        mtablewidget[i]->setStyleSheet("QTableWidget::item{border:0px solid rgb(0,0,0);border-bottom:1px solid rgb(0,0,0);}"); // item下绘制横线
        mtablewidget[i]->set_listtext(listlist[i]);
        if(mtablewidget[i]->columnCount() > 0 && mtablewidget[i]->rowCount() > 0){
            mtablewidget[i]->setCurrentCell(0,0);
        }
    }

    for(int i=0;i<listlist.count();i++){
        if(i == 0){
            mtablewidget[i]->show();
        } else {
            mtablewidget[i]->hide();
        }
    }
}

void Symbol_Input_Widget::itemClicked(QListWidgetItem *item)
{
    int i=0;
    for(i=0;i<mlistwidget->count();i++){
        if(item == mlistwidget->item(i)){
            break;
        }
    }
    if(i >= 0 && i < mlistwidget->count()){
        // 点击第i个
        if(i < listlist.count()){
            for(int j=0;j<listlist.count();j++){
                if(j == i){
                    mtablewidget[j]->show();
                } else {
                    mtablewidget[j]->hide();
                }
            }
        }
    }
}

void Symbol_Input_Widget::cellClicked(int row, int column)
{
    MTableWidget *tablewidget = qobject_cast<MTableWidget*>(sender());
    if(tablewidget == NULL)return;
    QTableWidgetItem *item = tablewidget->item(row,column);
    if(item != NULL){
        emit clicked_chinese(item->text());
    }
}
