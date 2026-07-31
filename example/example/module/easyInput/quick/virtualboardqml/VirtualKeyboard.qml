import QtQuick
// import QtQuick.Controls
// import QtQuick.Layouts

Rectangle {
    id:virtualkb
    width: 400
    height: 300
    z: 99
    anchors.margins:0

    // 输出结果
    signal get_code_value(string value)
    // 按键结果
    signal input_key_signal(string value)

    Image {
        id:backgroundimage
        anchors.fill: parent
        source: "qrc:/picture/Background.png"
        // Image.Stretch // 拉伸填充，可能会失真
        // Image.PreserveAspectFit // 保持比例适应，可能会有空白
        // Image.PreserveAspectCrop // 保持比例并裁剪填充
        // Image.Tile // 平铺填充
        // Image.TileVertically // 垂直平铺填充
        // Image.TileHorizontally // 水平平铺填充
        // Image.Pad // 保持原始大小，超出部分裁剪
        fillMode: Image.PreserveAspectCrop
        smooth: true // 平滑缩放
    }
    color:Qt.rgba(0,0,0,255)

    VirtualKeyInput{
        id:virtualkeyinput
        anchors.fill:parent
        onGet_code_value: function(value){
            virtualkb.get_code_value(value)
        }

        onInput_key_signal:function(value){
            virtualkb.input_key_signal(value)
        }
    }
}
