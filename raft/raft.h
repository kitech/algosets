#ifndef RAFT_HPP
#define RAFT_HPP

#include <thread>

#include <QtCore>
#include <QtNetwork>

#include <zmq.h>


class RaftNode : public QThread
{
    Q_OBJECT;
public:
    enum {RNT_FLOWER = 0, RNT_RC, RNT_LEADER};

    int _term = 0;
    int _ntype = 0;
    int _nid = 0;
    unsigned short _port = -1;
    static constexpr int _port_base = 5000;
    static constexpr int _timeout = 5;

    void *_sctx = NULL;
    void *_ssock = NULL;

    // store peers's conn info
    void *_peer_ctxs[10];
    void *_peer_socks[10];

public:
    RaftNode(int id)
        : QThread(0)
    {
        _nid = id;
        _port = _port_base + id;

        _sctx = zmq_ctx_new();
        zmq_ctx_set(_sctx, ZMQ_IO_THREADS, 1);

        _ssock = zmq_socket(_sctx, ZMQ_REP);
        zmq_bind(_ssock, QString("tcp://*:%1").arg(_port).toLatin1().data());
    }

    ~RaftNode()
    {

    }

    void run()
    {
        int cnter = 0;
    //    exec();
        qDebug()<<"running "<<_nid;
        while (true) {
            zmq_pollitem_t items[1];
            items[0].socket = _ssock;
            items[0].events = ZMQ_POLLIN;

            int rc = zmq_poll(items, 1, -1);
            if (rc == 1) {
                char buf[100];
                rc = zmq_recv(_ssock, buf, 100, 0);
                qDebug()<<_nid<<"recv msg:"<<rc<<errno;
                this->on_proc_recv(_ssock, buf);

            } else {
                qDebug()<<_nid<<"poll err:"<<rc<<errno;
            }

            cnter ++;
//            if (cnter++ == 3) break;
        }
    }


public slots: // seft emi
    void on_select_me()
    {
        char buf[32];
        for (int i = 0; i < 5; i++) {
            if (i == _nid) continue;
            int rc = 0;
            void *ctx = zmq_ctx_new();
            zmq_ctx_set(ctx, ZMQ_IO_THREADS, 1);

            void *sock = zmq_socket(ctx, ZMQ_REQ);
            rc = zmq_connect(sock, QString("tcp://localhost:%1").arg(_port_base+i).toLatin1().data());
            qDebug()<<_nid<<" connect to "<<i<<rc<<errno<<zmq_errno();

            sprintf(buf, "Hello, %d -> %d", _nid, i);
            rc = zmq_send(sock, buf, strlen(buf), 0);
//            rc = zmq_send(sock, "Hello", 5, 0);

//            zmq_msg_t request;
//            zmq_msg_init_size(&request, 6);
//            memcpy(zmq_msg_data(&request), "Hello", 5);
//            rc = zmq_msg_send(&request, _ssock, 0);
            qDebug()<<_nid<<"send to "<<i<<rc<<errno<<zmq_errno();

            // cleanup
            zmq_close(sock);
            zmq_ctx_destroy(ctx);

        }
    }

public slots: // recv
    void on_proc_recv(void *zsock, char *req)
    {
        qDebug()<<_nid<<zsock<<req;
    }

public: // util
    void log()
    {

    }
};

#endif // RAFT_HPP

