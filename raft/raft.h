#ifndef RAFT_HPP
#define RAFT_HPP

#include <cassert>
#include <thread>

#include <QtCore>
#include <QtNetwork>


class RaftNode : public QThread
{
    Q_OBJECT;
public:
    enum {RNT_NONE = 0, RNT_FLOWER, RNT_CANDIDATE, RNT_LEADER};
    enum {
        RCMD_VOTE_ME = 1,
        RCMD_AGREE_VOTE,
        RCMD_LEADER_CONFIRM,
        RCMD_LEADER_PING,
        RCMD_FLOWER_PONG,
        RCMD_DATA,
        RCMD_MAX = 128
    };

protected:
public:
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

        QTcpServer *_ssock2 = NULL;
        QHash<QTcpSocket*, int> _cli_socks; // 连接到本机的客户端列表

        // 连接到其他节点服务的socket
        QHash<int, QTcpSocket*>_peer_socks3;

        int _timer_scale = 1; // for daemon use
        QTimer _ticks;
        int _leader_id = -1;
        int _candidate_id = -1;
        int _pkt_seq = 0;
    };

    struct LeaderState : public ShareState
    {
        bool _leave_reign = false;
        QTimer _tmer;
        int _ping_timeout = 500;
    };

    struct CandidateState : public ShareState
    {
        int _wait_agree_timeout = 300; //msec
        QMap<int, QByteArray> _peer_resps; // nid => agree/not
        QTimer _tmer;
    };

    struct FlowerState : public ShareState
    {
        // node state
        int _ping_leader_times = 0;
        int _ping_leader_error_times = 0;
        QDateTime _leader_last_update_time;
        QTimer _tmer;
        int _wait_leader_ping_timeout = 500;
    };

private:
public:
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

        _cms._begin_time = QDateTime::currentDateTime();
        this->init();
    }

    ~RaftNode()
    {

    }

    void on_server_new_connection()
    {
        QTcpServer *s = _cms._ssock2;
        QTcpSocket *c = s->nextPendingConnection();
        this->_cms._cli_socks.insert(c, 1);

        connect(c, &QTcpSocket::readyRead, this, &RaftNode::on_server_ready_read);
    }

    // TODO 现在必须保证一次收取一条完整的消息
    void on_server_ready_read()
    {
        QTcpSocket *c = (QTcpSocket*)sender();
        QByteArray ba = c->readAll();

        this->on_proc_recv(c, ba);
    }

    // 在线程中初始化QTcpServer端，其相关操作在线程中执行。
    void init()
    {
        _cms._ssock2 = new QTcpServer();
        QTcpServer *s = _cms._ssock2;
        connect(s, &QTcpServer::newConnection, this, &RaftNode::on_server_new_connection);

        s->listen(QHostAddress::Any, _cms._port);

        // leader ping timer
        _lds._tmer.setSingleShot(false);
        connect(&_lds._tmer, &QTimer::timeout, this, &RaftNode::on_leader_ping);
        // candiate wait timer
        _cds._tmer.setSingleShot(true);
        connect(&_cds._tmer, &QTimer::timeout, this, &RaftNode::on_vote_resp_timeout);
        // follower wait ping timer
        _fls._tmer.setSingleShot(true);
        connect(&_fls._tmer, &QTimer::timeout, this, &RaftNode::on_wait_leader_ping_timeout);
    }

    void run()
    {
//        this->init();
        this->exec();
    }


public slots: // seft emi
    QString ntname(int t)
    {
        switch (t) {
        case RNT_CANDIDATE: return "candidate";
        case RNT_FLOWER:  return "flower";
        case RNT_LEADER:  return "leader";
        default:  return "unknown";
        }
    }

    void dumpState()
    {
        qDebug()<<"nid:"<<_cms._nid
               <<"ntype:"<<_cms._ntype<<ntname(_cms._ntype);
        qDebug()<<"term:"<<_cms._term
               <<"leader:"<<_cms._leader_id
               <<"candidate:"<<_cms._candidate_id;
        qDebug()<<"btime:"<<_cms._begin_time;
        qDebug()<<"clisocks:"<<_cms._cli_socks.count();
        qDebug()<<"peersocks:"<<_cms._peer_socks3.count();
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
                on_vote_me2();
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

    void on_vote_resp_timeout()
    {
        qDebug()<<sender();
        qDebug()<<_cms._nid<<_cds._peer_resps.count();

        QHash<int, QJsonObject> resps;
        for (auto it = _cds._peer_resps.begin(); it != _cds._peer_resps.end(); it++) {
            QJsonObject jsobj = QJsonDocument::fromJson(it.value()).object();
            resps.insert(it.key(), jsobj);
        }

        QHash<int, int> agrees;
        for (auto it = resps.begin(); it != resps.end(); it++) {
            QJsonObject jsobj = it.value();
            int agreed = jsobj.value("agree").toInt();
            agrees.insert(it.key(), agreed);
        }

        qDebug()<<_cms._nid<<agrees<<agrees.values().count(1);

        if (agrees.values().count(1) >= (5 / 2 + 1)) {
            // be leader
            qDebug()<<_cms._nid<<"i am become leader.";
            this->on_become_leader();
        } else {
            // split vote
            _cms._ntype = RNT_FLOWER;
            QTimer::singleShot(qrand()%400, this, SLOT(on_state_machine()));
        }
    }

    void on_vote_me2()
    {
        assert(_cms._ntype != RNT_CANDIDATE);
        assert(_cms._leader_id < 0);

        _cms._ntype = RNT_CANDIDATE;
        _cms._term += 1;
        _cds._peer_resps.clear();
        _cds._begin_time = QDateTime::currentDateTime();
        _cds._tmer.start(_cds._wait_agree_timeout);

        qDebug()<<_cms._nid<<" pub vote...";
        int cnter = 0;
        for (int i = 0; i < 5; i++) {
            if (i == _cms._nid) continue;
            int rc = 0;
            QTcpSocket *sock = NULL;

            connect_to_peer2(i);
            sock = _cms._peer_socks3[i];

            QJsonObject jsobj;
            jsobj.insert("cmd", QString::number(RCMD_VOTE_ME));
            jsobj.insert("nid", QString::number(_cms._nid));
            jsobj.insert("to_nid", QString::number(i));
            jsobj.insert("seq", QString::number(++_cms._pkt_seq));
            jsobj.insert("term", QString::number(_cms._term));
            QByteArray jstr = QJsonDocument(jsobj).toJson(QJsonDocument::Compact);
//            qDebug()<<_cms._nid<<"cmd:"<<jstr.length()<<jstr;

            rc = sock->write(jstr);
            assert(rc > 0);
            cnter ++;

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


    void on_peer_ready_read()
    {
        QTcpSocket *sock = (QTcpSocket*)sender();
        QByteArray ba = sock->readAll();

        QJsonDocument jdoc = QJsonDocument::fromJson(ba);
        QJsonObject jsobj = jdoc.object();
        int peer_nid = jsobj.value("nid").toString().toInt();
        _cds._peer_resps.insert(peer_nid, ba);

        qDebug()<<_cms._nid<<"resp:"<<ba.length()<<ba<<_cds._peer_resps.count();
    }
    void on_peer_close()
    {
        qDebug()<<"closed"<<sender();
    }

    void connect_to_peer2(int nid)
    {
        QTcpSocket *sock = NULL;
        if (_cms._peer_socks3.value(nid) == NULL) {
            sock = new QTcpSocket(this);
            connect(sock, &QTcpSocket::readyRead, this, &RaftNode::on_peer_ready_read);
            connect(sock, &QTcpSocket::disconnected, this, &RaftNode::on_peer_close);

            sock->connectToHost("localhost", _port_base+nid);
            sock->waitForConnected();
            _cms._peer_socks3.insert(nid, sock);
        } else {
            sock = _cms._peer_socks3.value(nid);
        }
    }

    void on_agree_vote(QTcpSocket *sock, QJsonObject &jsobj)
    {
        int nid = jsobj.value("nid").toString().toInt();
        _cms._leader_id = nid;
        _cms._term = jsobj.value("term").toString().toInt();

        QJsonObject njsobj;
        njsobj.insert("cmd", QString::number(RCMD_AGREE_VOTE));
        njsobj.insert("nid", QString::number(_cms._nid));
        njsobj.insert("to_nid", QString::number(nid));
        njsobj.insert("agree", 1);

        QByteArray jstr = QJsonDocument(njsobj).toJson(QJsonDocument::Compact);
        qDebug()<<_cms._nid<<"send cmd:"<<jstr.length()<<jstr;

        int rc = sock->write(jstr);
        assert(rc > 0);
        qDebug()<<_cms._nid<<"send cmd done:"<<rc<<jstr.length()<<jstr;
    }

    void on_reject_vote(QTcpSocket *sock, QJsonObject &jsobj)
    {
        int nid = jsobj.value("nid").toString().toInt();

        QJsonObject njsobj;
        njsobj.insert("cmd", QString::number(RCMD_AGREE_VOTE));
        njsobj.insert("nid", QString::number(_cms._nid));
        njsobj.insert("to_nid", QString::number(nid));
        njsobj.insert("agree", 0);

        QByteArray jstr = QJsonDocument(njsobj).toJson(QJsonDocument::Compact);
        qDebug()<<_cms._nid<<"send cmd:"<<jstr.length()<<jstr;

        int rc = sock->write(jstr);
        assert(rc > 0);
        qDebug()<<_cms._nid<<"send cmd done:"<<rc<<jstr.length()<<jstr;
    }

    void on_become_leader()
    {
        _cms._ntype = RNT_LEADER;
        _cms._leader_id = _cms._nid;
        _lds._begin_time = QDateTime::currentDateTime();

        for (int i = 0; i < 5; i++) {
            if (i == _cms._nid) continue;

            QJsonObject njsobj;
            njsobj.insert("cmd", QString::number(RCMD_LEADER_CONFIRM));
            njsobj.insert("nid", QString::number(_cms._nid));
            njsobj.insert("to_nid", QString::number(i));
            njsobj.insert("term", _cms._term);

            QByteArray jstr = QJsonDocument(njsobj).toJson(QJsonDocument::Compact);
//            qDebug()<<_cms._nid<<"send cmd:"<<jstr.length()<<jstr;

            QTcpSocket *sock = _cms._peer_socks3.value(i);
            int rc = sock->write(jstr);
            assert(rc > 0);
            qDebug()<<_cms._nid<<"send cmd done:"<<rc<<jstr.length()<<jstr;
        }

        _lds._tmer.start(_lds._ping_timeout);
    }

    //
    void on_leader_confirm(QTcpSocket *sock, QJsonObject &jsobj)
    {
        Q_UNUSED(sock);
        qDebug()<<_cms._nid<<"confirmed leader:"<<jsobj<<",curr leader:"
               <<_cms._leader_id<<_cms._term;

        _cms._leader_id = jsobj.value("nid").toString().toInt();
        _cms._term = jsobj.value("term").toInt();

    }

    //
    void on_leader_ping()
    {
        for (int i = 0; i < 5; i++) {
            if (i == _cms._nid) continue;

            QJsonObject njsobj;
            njsobj.insert("cmd", QString::number(RCMD_LEADER_PING));
            njsobj.insert("nid", QString::number(_cms._nid));
            njsobj.insert("to_nid", QString::number(i));
            njsobj.insert("term", _cms._term);

            QByteArray jstr = QJsonDocument(njsobj).toJson(QJsonDocument::Compact);
//            qDebug()<<_cms._nid<<"send cmd:"<<jstr.length()<<jstr;

            QTcpSocket *sock = _cms._peer_socks3.value(i);
            int rc = sock->write(jstr);
            assert(rc > 0);
            qDebug()<<_cms._nid<<"send cmd done:"<<rc<<jstr.length()<<jstr;
        }

    }

    //
    void on_wait_leader_ping_timeout()
    {
        qDebug()<<"fffffffff";
    }

    //
    void on_flower_pong(QTcpSocket *sock, QJsonObject &jsobj)
    {
        Q_UNUSED(sock);
        Q_UNUSED(jsobj);
        _fls._leader_last_update_time = QDateTime::currentDateTime();

    }

    //
    void on_leader_leave_reign()
    {

    }

public slots: // recv

    void on_proc_recv(QTcpSocket *c, QByteArray &msg)
    {

        QJsonDocument jdoc = QJsonDocument::fromJson(msg);
        qDebug()<<_cms._nid<<c<<jdoc;
        QJsonObject jsobj = jdoc.object();
        int cmd = jsobj.value("cmd").toString().toInt();

        switch (cmd) {
        case RCMD_VOTE_ME:
            if (_cms._ntype == RNT_CANDIDATE) {
                qDebug()<<_cms._nid<<"i'am candidate. reject,"<<jsobj.value("nid");
                this->on_reject_vote(c, jsobj);
            } else if (_cms._ntype == RNT_LEADER) {
                qDebug()<<_cms._nid<<"i'am leader. reject,"<<jsobj.value("nid");
                this->on_reject_vote(c, jsobj);
            } else {
                if (_cms._leader_id >= 0) {
                    qDebug()<<_cms._nid<<"already has leader,reject,"<<jsobj.value("nid")
                           <<",leader is:"<<_cms._leader_id;
                    this->on_reject_vote(c, jsobj);
                } else {
                    // i agree
                    on_agree_vote(c, jsobj);
                }
            }
            break;
        case RCMD_LEADER_CONFIRM:
            this->on_leader_confirm(c, jsobj);
            break;
        case RCMD_LEADER_PING:
            this->on_flower_pong(c, jsobj);
            break;
        default:
            qDebug()<<_cms._nid<<"unknown cmd:"<<jsobj.value("cmd")<<jsobj.value("cmd").toInt()
                   <<msg.length()<<jdoc;
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
 * http://raftconsensus.github.io/
 * http://blog.csdn.net/cszhouwei/article/details/38374603
 * http://www.jdon.com/artichect/raft.html
 */

#endif // RAFT_HPP

