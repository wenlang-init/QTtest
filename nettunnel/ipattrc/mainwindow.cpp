#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <pcapplusplus/PcapPlusPlusVersion.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/PcapFileDevice.h>
#include <iostream>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

using namespace std;

#define TESTPCAPFILE "D:/work/QT/network/nettunnel/bin/1_packet.pcap"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qdebug<<pcpp::getPcapPlusPlusVersion()<<QString(TESTPCAPFILE);
    // open a pcap file for reading
    pcpp::PcapFileReaderDevice reader(TESTPCAPFILE);
    if (!reader.open())
    {
        std::cerr << "Error opening the pcap file" << std::endl;
        return ;
    }

    // read the first (and only) packet from the file
    pcpp::RawPacket rawPacket;
    if (!reader.getNextPacket(rawPacket))
    {
        std::cerr << "Couldn't read the first packet in the file" << std::endl;
        return;
    }

    // parse the raw packet into a parsed packet
    pcpp::Packet parsedPacket(&rawPacket);

    // verify the packet is IPv4
    if (parsedPacket.isPacketOfType(pcpp::IPv4))
    {
        // extract source and dest IPs
        pcpp::IPv4Address srcIP = parsedPacket.getLayerOfType<pcpp::IPv4Layer>()->getSrcIPv4Address();
        pcpp::IPv4Address destIP = parsedPacket.getLayerOfType<pcpp::IPv4Layer>()->getDstIPv4Address();

        // print source and dest IPs
        std::cout << "Source IP is '" << srcIP << "'; "
                  << "Dest IP is '" << destIP << "'" << std::endl;
    }

    // close the file
    reader.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
