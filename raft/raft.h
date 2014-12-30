#ifndef RAFT_HPP
#define RAFT_HPP

#include <cassert>
#include <thread>

#include <QtCore>
#include <QtNetwork>

#include <zmq.h>


class RaftNode : public QThread
{
    Q_OBJECT;
public:
    enum {RNT_NONE = 0, RNT_FLOWER, RNT_CANDIDATE, RNT_LEADER};
    enum {
        RCMD_VOTE_ME = 1,
        RCMD_AGREE_VOTE,
        RCMD_LEADER_CONFIRM,
        RCMD_PING_LEADER,
        RCMD_LEADER_PONG,
        RCMD_DATA,
        RCMD_MAX = 128
    };

protected:
    struct ShareState
    {
        QDateTime _begin_time;
    };

    struct CommonState : public ShareState
    {
        int _term = 0;
        int _ntype = RNT_FLOWER;
        int _nid = 0;
        unsigned short _port = -1;

        void *_sctx = NULL;
        void *_ssock = NULL;

        // store peers's conn info
        void *_peer_ctxs[10] = {0};
        void *_peer_socks[10] = {0};

        QTimer _ticks;
        int _leader_id = -1;
        int _candidate_id = -1;
        int _pkt_seq = 0;
    };

    struct LeaderState : public ShareState
    {
        bool _leave_reign = false;
    };

    struct CandidateState : public ShareState
    {
        int _agree_timeout = 3;
        QMap<int, bool> _peer_resps; // nid => agree/not
    };

    struct FlowerState : public ShareState
    {
        // node state
        int _ping_leader_times = 0;
        int _ping_leader_error_times = 0;
    };

private:
    CommonState _cms;
    LeaderState _lds;
    CandidateState _cds;
    FlowerState _fls;

    static constexpr int _port_base = 5000;
    static constexpr int _timeout = 5;
    static constexpr int _max_leader_reign_time = 60 * 10;

public:
    RaftNode(int id) : QThread(0)
    {
        _cms._nid = id;
        _cms._port = _port_base + id;

       _cms._sctx = zmq_ctx_new();
        zmq_ctx_set(_cms._sctx, ZMQ_IO_THREADS, 1);

        _cms._ssock = zmq_socket(_cms._sctx, ZMQ_REP);
        zmq_bind(_cms._ssock, QString("tcp://*:%1").arg(_cms._port).toLatin1().data());

        _cms._begin_time = QDateTime::currentDateTime();
    }

    ~RaftNode()
    {

    }

    void run()
    {
        int cnter = 0;
    //    exec();
        qDebug()<<"running "<<_cms._nid;
        while (true) {
            zmq_pollitem_t items[1];
            items[0].socket = _cms._ssock;
            items[0].events = ZMQ_POLLIN | ZMQ_POLLERR;

            qDebug()<<_cms._nid<<" polling...";
            int rc = zmq_poll(items, 1, -1);
            qDebug()<<_cms._nid<<" polled:"<<rc
                   <<(items[0].revents & ZMQ_POLLIN)<<(items[0].revents & ZMQ_POLLERR);
            if (rc == 1) {
                while (true) {
                    char buf[128];
                    rc = zmq_recv(_cms._ssock, buf, 128, ZMQ_DONTWAIT);
                    // qDebug()<<_cms._nid<<"recv msg:"<<rc<<errno;
                    if (rc == -1 && errno == EAGAIN) break;
                    if (rc == -1 && errno != EAGAIN) {
                        qDebug()<<_cms._nid<<"recv err:"<<rc<<errno;
                        // (errno == EFSM)???
                        break;
                    }
                    assert(rc > 0);
                    buf[rc] = 0;
                    this->on_proc_recv(_cms._ssock, buf);
                }
            } else {
                qDebug()<<_cms._nid<<"poll err:"<<rc<<errno;
            }
            qDebug()<<_cms._nid<<" next poll:"<<cnter;
            cnter ++;
//            if (cnter++ == 3) break;
        }
        qDebug()<<_cms._nid<<" thread exited, ooops."<<cnter;
    }


public slots: // seft emi
    void dumpState()
    {
        qDebug()<<"nid:"<<_cms._nid
               <<"ntype:"<<_cms._ntype;
        qDebug()<<"term:"<<_cms._term
               <<"leader:"<<_cms._leader_id
               <<"candidate:"<<_cms._candidate_id;
        qDebug()<<"btime:"<<_cms._begin_time;
        qDebug()<<"";
    }

    void on_ticks_timeout()
    {

    }

    void on_state_machine()
    {
        switch (_cms._ntype) {
        case RNT_FLOWER:
            if (_cms._leader_id < 0) {
                on_vote_me();
            } else {
                // keep state
                qDebug()<<_cms._nid<<"already has leader:"<<_cms._leader_id;
            }
            break;
        default:
            qDebug()<<_cms._nid<<"unknown ntype:"<<_cms._ntype;
            break;
        }
    }

    void on_vote_me()
    {
        assert(_cms._ntype != RNT_CANDIDATE);
        assert(_cms._leader_id < 0);

        _cms._ntype = RNT_CANDIDATE;
        _cms._term += 1;
        _cds._begin_time = QDateTime::currentDateTime();

        qDebug()<<_cms._nid<<" pub vote...";
        int cnter = 0;
        char buf[128];
        for (int i = 0; i < 5; i++) {
            if (i == _cms._nid) continue;
            int rc = 0;
            void *sock = NULL;

            connect_to_peer(i);
            sock = _cms._peer_socks[i];

            QMap<QString, QString> cmd;
            cmd["cmd"] = QString::number(RCMD_VOTE_ME);
            cmd["nid"] = QString::number(_cms._nid);

            QJsonObject jsobj;
            jsobj.insert("cmd", QJsonValue(QString::number(RCMD_VOTE_ME)));
            jsobj.insert("nid", QJsonValue(QString::number(_cms._nid)));
            jsobj.insert("to_nid", QJsonValue(QString::number(i)));
            jsobj.insert("seq", QJsonValue(QString::number(++_cms._pkt_seq)));
            jsobj.insert("term", QString::number(_cms._term));
            QByteArray jstr = QJsonDocument(jsobj).toJson(QJsonDocument::Compact);
//            qDebug()<<_cms._nid<<"cmd:"<<jstr.length()<<jstr;

            sprintf(buf, "Hello, %d -> %d", _cms._nid, i);
            strncpy(buf, jstr.data(), jstr.length());
            buf[jstr.length()] = 0;

//            rc = zmq_send(sock, jstr.data(), jstr.length(), 0);
            rc = zmq_send(sock, buf, strlen(buf), 0);
//            rc = zmq_send(sock, "Hello", 5, 0);
            assert(rc > 0);
            cnter ++;

//            zmq_msg_t request;
//            zmq_msg_init_size(&request, 6);
//            memcpy(zmq_msg_data(&request), "Hello", 5);
//            rc = zmq_msg_send(&request, _ssock, 0);
//            qDebug()<<_cms._nid<<"send to "<<i<<rc<<errno<<zmq_errno();

            // cleanup
//            zmq_close(sock);
//            zmq_ctx_destroy(ctx);

        }

        qDebug()<<_cms._nid<<" pub vote:"<<cnter;
    }

    bool has_vote_dispute()
    {
        return true;
    }

    void on_resolve_vote_dispute()
    {

    }

    void connect_to_peer(int nid)
    {
        int rc = 0;
        void *ctx = NULL;
        void *sock = NULL;

        if (_cms._peer_ctxs[nid] == NULL) {
            ctx = zmq_ctx_new();
            zmq_ctx_set(ctx, ZMQ_IO_THREADS, 1);

            sock = zmq_socket(ctx, ZMQ_REQ);
            rc = zmq_connect(sock, QString("tcp://localhost:%1").arg(_port_base+nid).toLatin1().data());
//            qDebug()<<_cms._nid<<" connect to "<<nid<<rc<<errno<<zmq_errno();
            assert(rc == 0);
            _cms._peer_ctxs[nid] = ctx;
            _cms._peer_socks[nid] = sock;
        } else {
            ctx = _cms._peer_ctxs[nid];
            sock = _cms._peer_socks[nid];
        }
    }

    void on_agree_vote(QJsonObject &jsobj)
    {
        int nid = jsobj.value("nid").toString().toInt();
        _cms._leader_id = nid;
        _cms._term = jsobj.value("term").toString().toInt();

        QJsonObject njsobj;
        njsobj.insert("cmd", QString::number(RCMD_AGREE_VOTE));
        njsobj.insert("nid", QString::number(_cms._nid));
        njsobj.insert("to_nid", QString::number(nid));

        char buf[128];
        QByteArray jstr = QJsonDocument(njsobj).toJson(QJsonDocument::Compact);
        strncpy(buf, jstr.data(), jstr.length());
        buf[jstr.length()] = 0;
        qDebug()<<_cms._nid<<"send cmd:"<<jstr.length()<<jstr;

        connect_to_peer(nid);
        assert(_cms._peer_socks[nid] != NULL);
        void *sock = _cms._peer_socks[nid];
        int rc = zmq_send(sock, buf, strlen(buf), 0);
        assert(rc > 0);
        qDebug()<<_cms._nid<<"send cmd done:"<<rc<<jstr.length()<<jstr;
    }

    //
    void on_leader_confirm()
    {

    }

    //
    void on_ping_leader()
    {

    }

    //
    void on_leader_pong()
    {

    }

    //
    void on_leader_leave_reign()
    {

    }

public slots: // recv
    void on_proc_recv(void *zsock, char *req)
    {
        QJsonDocument jdoc = QJsonDocument::fromJson(QByteArray(req, strlen(req)));
        qDebug()<<_cms._nid<<zsock<<req<<jdoc;
        QJsonObject jsobj = jdoc.object();
        int cmd = jsobj.value("cmd").toString().toInt();
        switch (cmd) {
        case RCMD_VOTE_ME:
            if (_cms._ntype == RNT_CANDIDATE) {
                qDebug()<<_cms._nid<<"i'am candidate. reject";
            } else if (_cms._ntype == RNT_LEADER) {
                qDebug()<<_cms._nid<<"i'am leader. reject";
            } else {
                if (_cms._leader_id >= 0) {
                    qDebug()<<_cms._nid<<"i'am candidate. reject";
                } else {
                    // i agree
                    on_agree_vote(jsobj);
                }
            }
            break;
        default:
            qDebug()<<_cms._nid<<"unknown cmd:"<<jsobj.value("cmd")<<jsobj.value("cmd").toInt()
                   <<strlen(req)<<jdoc;
            break;
        }

    }

public: // util
    void log()
    {

    }
};

/*
 * refer:
 * http://www.jdon.com/artichect/raft.html
 */

#endif // RAFT_HPP

