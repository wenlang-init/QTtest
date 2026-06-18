//pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Window
import CustomPlotItem 1.0
import QtCharts

Item{

    function getPlotPtr() {
        return customPlotItem.getOBJQV();
    }

    CustomPlotItem {
        id:customPlotItem
        anchors.fill: parent
        //visible:false
        Component.onCompleted:
        {
            console.log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
            setLabel("x","y")
            setMaxCount(10000000)
            initCustomPlot()
            addGraph("曲线","#ffff0000","#00ffffff")
        }
    }

    ChartView {
        visible:false
        id:chartView
        title: qsTr("①侧温度趋势")
        titleFont.pixelSize: 15
        titleFont.bold: true
        titleColor: "#ff0000ff"
        width: parent.width
        height:parent.height
        antialiasing: true
        //X轴
        ValueAxis {
            id: valueAxisX
            min: 0
            max: 60
            tickCount: 30  //坐标轴上的的刻度个数。所以间隔 = 60 / 30 = 2
            labelFormat: "%.0f"//0 - 控制小数点后有几位小数
        }
        //Y轴
        ValueAxis {
            id: valueAxisY
            min: 20
            max: 300
            tickCount: 27  //坐标轴上的的刻度个数
            labelFormat: "%.1f"//1 - 控制小数点后有几位小数
        }
        //LineSeries是折线，SplineSeries是曲线。子项都是XYPoint
        SplineSeries {
            id:line1
            name: "1"  //线条的名称
            axisX: valueAxisX  //指定X轴
            axisY: valueAxisY  //指定Y轴
            color: Qt.darker("#BB4444",1.2) //线条颜色，默认每条线的颜色不同
            width: 2//线的宽度，但修改线宽后，需要重新设置color，否则修改过width的线条都是黑色
        }
        SplineSeries {
            id:line2
            name: "2"
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#B38A4D",1.2)
            width: 2
        }
        SplineSeries {
            id:line3
            name: "3"
            //style: Qt.SolidLine //样式
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#61B34D",1.2)
            width: 2
        }
        SplineSeries {
            id:line4
            name: "4"
            //style: Qt.SolidLine //样式
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#4D8AB3",1.2)
            width: 2
        }
        SplineSeries {
            id:line5
            name: "5"
            //style: Qt.SolidLine //样式
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#9E4DB3",1.2)
            width: 2
        }
        SplineSeries {
            id:line6
            name: "6"
            //style: Qt.SolidLine //样式
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#AA5566",1.2)
            width: 2
        }
        SplineSeries {
            id:line7
            name: "7"
            //style: Qt.SolidLine //样式
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#808080",1.2)
            width: 2
        }
        SplineSeries {
            id:line8
            name: "8"
            //style: Qt.SolidLine //样式
            axisX: valueAxisX
            axisY: valueAxisY
            color: Qt.darker("#F79709",1.2)
            width: 2
        }
        MouseArea {
            id:ma
            property var currentX: 0
            property var currentY: 0
            anchors.fill: parent
            onWheel:(wheel)=>{
                        chartView.zoom(wheel.angleDelta.y > 0 ? 1.2 : 0.8)
                    }
            onPressed: (mouse)=>{//获取点击时位置
                           ma.cursorShape = Qt.ClosedHandCursor
                           ma.currentX = mouse.x
                           ma.currentY = mouse.y
                       }

            onReleased: (mouse)=>{
                            ma.cursorShape = Qt.ArrowCursorArrowCursor
                        }

            onPositionChanged:(mouse)=>{//拖拽功能实现
                                  var moveX = mouse.x-ma.currentX
                                  var moveY = mouse.y-ma.currentY
                                  currentX = mouse.x
                                  currentY = mouse.y
                                  chartView.scrollLeft(moveX)
                                  chartView.scrollUp(moveY)
                              }

        }
        Timer{
            id:timer
            property int count: 0
            //生成随机数(>=Min,<=Max)
            function getRandomNum(Min,Max)
            {
                var Range = Max - Min;
                var Rand = Math.random();
                return(Min + Math.round(Rand * Range));
            }
            function timeChanged() {
                //随机数模拟数据来源
                line1.append(2 * count, getRandomNum(20,300));
                count++;
                if(count > 10){
                    line1.clear();
                    count = 0;
                }
            }
            interval: 1000;running:true;repeat: true;
            onTriggered: timeChanged();
        }
        Component.onCompleted:
        {
        }
    }
}
