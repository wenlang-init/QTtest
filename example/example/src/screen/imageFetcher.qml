import QtQuick
import QtQuick.Controls

Item {
    function reLoader(id) {
        //img_preview.source = "";
        img_preview.source = "image://test/";
        img_preview.source += id;
    }
    Image {
        id:img_preview
        anchors.fill:parent
        source: "image://test/"
        cache: false;  //取消缓存
        fillMode: Image.PreserveAspectFit//Image.PreserveAspectCrop
        smooth: true
    }
    // Timer{
    //     //定时器触发时间 单位毫秒
    //     interval: 10;
    //     //触发定时器
    //     running: true;
    //     //不断重复
    //     repeat: true;
    //     //定时器触发时执行
    //     onTriggered: {
    //         img_preview.source = "";
    //         img_preview.source = "image://test/";
    //     }
    // }
}
