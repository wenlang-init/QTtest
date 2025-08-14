#include "widget.h"
#include "ui_widget.h"
#include "gst/gst.h"
#include "lognone.h"

void startFeed(GstElement *source,
               guint       size)
{
    // INFO_LOG("------------ GStFilePlayer ------------------ start
    // feeding\n");
    guint8 *data;
    GstMapInfo map;

    auto buffer = gst_buffer_new_allocate(NULL, size, NULL);

    gst_buffer_map(buffer, &map, GST_MAP_WRITE);
    data = (guint8 *)map.data;

    QList<QByteArray> m_voiceDataList;
    QString m_sFileName;

    if (m_voiceDataList.isEmpty() && !m_sFileName.isEmpty())
    {
        // QFile file(sFileName);
        // int iReadSize = 320;
        // while (!file.atEnd())
        // {
        //     QByteArray tempData = file.read(iReadSize);
        //     if (tempData.size() == iReadSize)
        //     {
        //         voiceDataList.append(tempData);
        //     }
        // }
        // file.close();
    }

    if (m_voiceDataList.isEmpty())
    {
        auto silenceVoice = QByteArray(320, char(0));
        memcpy(data, silenceVoice, silenceVoice.size());
    }
    else
    {
        memcpy(data, m_voiceDataList.first(), m_voiceDataList.first().size());
        m_voiceDataList.pop_front();

        if (m_sFileName.isEmpty() ||
            (m_voiceDataList.isEmpty()))
        {
            INFO_LOG("stop file Player, fileName:%s\n",
                     m_sFileName.toStdString().c_str());
            m_sFileName.clear();
            m_voiceDataList.clear();
        }
    }

    int ret = -1;
    gst_buffer_unmap(buffer, &map);
    g_signal_emit_by_name(source, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);
}

void stopFeed(GstElement *source) {
    INFO_LOG("Stop feeding\n");
}

void errorCallback(GstBus *bus, GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        FATAL_LOG("End of stream\n");

        // g_main_loop_quit(loop);
        break;

    case GST_MESSAGE_ERROR:
    {
        GError *err = nullptr;
        gchar  *debug = nullptr;
        gst_message_parse_error(msg, &err, &debug);
        FATAL_LOG("Error: %s\n", err->message);
        g_error_free(err);
        g_free(debug);
        g_main_loop_quit(loop);
        break;
    }

    default:
        break;
    }
}

void _test_() {
    gst_init(NULL, NULL);

    // Create a GLib Main Loop and set it to run
    GMainLoop *m_gMainLoop = g_main_loop_new(NULL, FALSE);

    // Create the empty m_audioPipeline
    GstElement *m_audioPipeline = gst_pipeline_new("playerPipeline");

    // Create the elements
    GstElement *m_appSrc = gst_element_factory_make("appsrc", "playerAppSrc");
    GstElement *m_audioSink = gst_element_factory_make("pulsesink",
                                                       "playerAutoaudiosink");
    QString m_sWebrtcdspName = "filePlayerWebrtcdsp";
    GstElement *m_webrtcDsb = gst_element_factory_make("webrtcdsp",
                                                       m_sWebrtcdspName.toStdString().c_str());

    if (!m_audioPipeline || !m_appSrc || !m_webrtcDsb || !m_audioSink)
    {
        FATAL_LOG("Not all elements could be created, %s_%s_%s_%s\n",
                  m_audioPipeline == nullptr ? "null" : "notNull",
                  m_appSrc == nullptr ? "null" : "notNull",
                  m_webrtcDsb == nullptr ? "null" : "notNull",
                  m_audioSink == nullptr ? "null" : "notNull");

        if (m_audioPipeline) {
            gst_element_set_state(m_audioPipeline, GST_STATE_NULL);
            gst_object_unref(m_audioPipeline);
        }
        gst_deinit();
        return;
    }

    // Configure appsrc
    g_object_set(G_OBJECT(m_appSrc), "is-live", TRUE, "blocksize", 320,
                 NULL);
    GstCaps *audioCaps = gst_caps_new_simple("audio/x-raw",
                                             "format",
                                             G_TYPE_STRING,
                                             "S16LE",
                                             "rate",
                                             G_TYPE_INT,
                                             8000,
                                             "channels",
                                             G_TYPE_INT,
                                             1,

// 示例：立体声，2通道
                                             "layout",
                                             G_TYPE_STRING,
                                             "interleaved",
                                             NULL);
    g_object_set(G_OBJECT(m_appSrc), "caps", audioCaps, NULL);
    gst_caps_unref(audioCaps);

    g_signal_connect(m_appSrc, "need-data",   GCallback(startFeed), NULL);
    g_signal_connect(m_appSrc, "enough-data", GCallback(stopFeed),  NULL);

    // Configure webrtc
    // update webrtcdsp args
    // gst_element_set_state(m_audioPipeline, GST_STATE_READY);
    g_object_set(G_OBJECT(m_webrtcDsb),
                 "gain-control", 1,         // 是否需要使用 webrtcdsp处理
                 "gain-control-mode", 1,    // agc的模式, 1:自动, 2:固定
                 "target-level-dbfs", 1,    // echo-cancel,level:
                                            // 0:low,1:moderate, 2:high
                 "compression-gain-db", 30, // agc 增益值 0~90
                 NULL);

    // update webrtcdsp args
    // gst_element_set_state(m_audioPipeline, GST_STATE_PLAYING);
    g_object_set(G_OBJECT(m_webrtcDsb),
                 "noise-suppression",
                 false,
                 "echo-cancel",
                 false,
                 NULL);

    // Configure m_audioSink
    g_object_set(G_OBJECT(m_audioSink), "buffer-time", 60000, NULL);

    // Link all elements that can be automatically linked because they have
    // "Always" pads
    gst_bin_add_many(GST_BIN(m_audioPipeline),
                     m_appSrc,
                     m_webrtcDsb,
                     m_audioSink,
                     NULL);

    if (gst_element_link_many(m_appSrc, m_webrtcDsb, m_audioSink, NULL) != TRUE)
    {
        FATAL_LOG("Elements could not be linked.\n");

        if (m_audioPipeline) {
            gst_element_set_state(m_audioPipeline, GST_STATE_NULL);
            gst_object_unref(m_audioPipeline);
        }
        gst_deinit();
        return;
    }

    // Instruct the m_bus to emit signals for each received message, and connect
    // to the interesting signals
    auto bus = gst_element_get_bus(m_audioPipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(G_OBJECT(bus),
                     "message::error",
                     (GCallback)errorCallback,
                     m_gMainLoop);
    gst_object_unref(bus);

    // Start playing the m_audioPipeline
    gst_element_set_state(m_audioPipeline, GST_STATE_PLAYING);

    INFO_LOG("init suc, begin m_gMainLoop\n");
    g_main_loop_run(m_gMainLoop);
    INFO_LOG("exit g_main_loop\n"); // g_main_loop_quit(m_gMainLoop);

    gst_element_set_state(m_audioPipeline, GST_STATE_NULL);
    gst_object_unref(m_audioPipeline);

    gst_deinit();
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    DEBUG_LOG("gst version:%s\n", gst_version_string());
    _test_();
}

Widget::~Widget()
{
    // g_main_loop_quit(m_gMainLoop);
    delete ui;
}
