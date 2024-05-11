#include "test_suite.h"

#include <fcall.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

static void _test_Tversion(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Tversion,
        .tag = NOTAG,
        .msize = 128,
        .version = "9p2000",
    };

    res = fcall_msg_size(&fin);
    if (res != 19)
        puts("FCALL::ERROR::fcall_msg_size Tversion failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 19)
        puts("FCALL::ERROR::fcall_msg_to_buf Tversion failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 19 || fin.type != fout.type || fin.tag != fout.tag ||
        fin.msize != fout.msize || strcmp(fin.version, fout.version) != 0)
        puts("FCALL::ERROR::fcall Tversion failure");
}

static void _test_Rversion(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Rversion,
        .tag = NOTAG,
        .msize = 128,
        .version = "9p2000",
    };

    res = fcall_msg_size(&fin);
    if (res != 19)
        puts("FCALL::ERROR::fcall_msg_size Rversion failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 19)
        puts("FCALL::ERROR::fcall_msg_to_buf Rversion failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 19 || fin.type != fout.type || fin.tag != fout.tag ||
        fin.msize != fout.msize || strcmp(fin.version, fout.version) != 0)
        puts("FCALL::ERROR::fcall Tversion failure");
}

static void _test_Rerror(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Rerror,
        .tag = 0,
        .ename = "Error",
    };

    res = fcall_msg_size(&fin);
    if (res != 14)
        puts("FCALL::ERROR::fcall_msg_size Rerror failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 14)
        puts("FCALL::ERROR::fcall_msg_to_buf Rerror failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 14 || fin.type != fout.type || fin.tag != fout.tag ||
        strcmp(fin.ename, fout.ename) != 0)
        puts("FCALL::ERROR::fcall Rerror failure");
}

static void _test_Tattach(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Tattach,
        .fid = NOFID,
        .tag = NOTAG,
        .afid = NOFID,
        .uname = "user",
        .aname = "/",
    };

    res = fcall_msg_size(&fin);
    if (res != 24)
        puts("FCALL::ERROR::fcall_msg_size Tattach failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 24)
        puts("FCALL::ERROR::fcall_msg_to_buf Tattach failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 24 || fin.type != fout.type || fin.fid != fout.fid ||
        fin.tag != fout.tag || fin.msize != fout.msize ||
        strcmp(fin.version, fout.version) != 0)
        puts("FCALL::ERROR::fcall Tattach failure");
}

static void _test_Rattach(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Rattach,
        .tag = NOTAG,
        .qid = {
            .type = 0,
            .version = 1,
            .id = 2,
        },
    };

    res = fcall_msg_size(&fin);
    if (res != 23)
        puts("FCALL::ERROR::fcall_msg_size Rattach failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 23)
        puts("FCALL::ERROR::fcall_msg_to_buf Rattach failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 23 || fin.type != fout.type || fin.tag != fout.tag ||
        fin.qid.type != fout.qid.type || fin.qid.version != fout.qid.version ||
        fin.qid.id != fout.qid.id)
        puts("FCALL::ERROR::fcall Rattach failure");
}

static void _test_Topen(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Topen,
        .tag = NOTAG,
        .fid = NOFID,
        .mode = (char) ~0,
    };

    res = fcall_msg_size(&fin);
    if (res != 12)
        puts("FCALL::ERROR::fcall_msg_size Topen failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 12)
        puts("FCALL::ERROR::fcall_msg_to_buf Topen failure");

    res = fcall_buf_to_msg(buf, &fout);
    if (res != 12)
        puts("FCALL::ERROR::fcall_buf_to_msg Topen failure");

    if (fin.type != fout.type || fin.tag != fout.tag || fin.fid != fout.fid ||
        fin.mode != fout.mode)
        puts("FCALL::ERROR::fcall Topen failure");
}

static void _test_Ropen(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Ropen,
        .tag = NOTAG,
        .qid = {
            .type = (unsigned) ~0,
            .version = (unsigned) ~0,
            .id = (unsigned long) ~0,
        },
        .iounit = 0,
    };

    res = fcall_msg_size(&fin);
    if (res != 27)
        puts("FCALL::ERROR::fcall_msg_size Ropen failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 27)
        puts("FCALL::ERROR::fcall_msg_to_buf Ropen failure");

    res = fcall_buf_to_msg(buf, &fout);
    if (res != 27)
        puts("FCALL::ERROR::fcall_buf_to_msg Ropen failure");

    if (fin.type != fout.type || fin.tag != fout.tag || fin.qid.type != fout.qid.type ||
        fin.qid.version != fout.qid.version || fin.qid.id != fout.qid.id ||
        fin.iounit != fout.iounit)
        puts("FCALL::ERROR::fcall Ropen failure");
}

static void _test_Twalk(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Twalk,
        .tag = NOTAG,
        .fid = NOFID,
        .newfid = NOFID,
        .nwname = 3,
        .wname = (pstring *) "\1\0a\2\0bc\3\0def",
    };

    res = fcall_msg_size(&fin);
    if (res != 29)
        puts("FCALL::ERROR::fcall_msg_size Twalk failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 29)
        puts("FCALL::ERROR::fcall_msg_to_buf Twalk failure");

    res = fcall_buf_to_msg(buf, &fout);
    if (res != 29)
        puts("FCALL::ERROR::fcall_buf_to_msg Twalk failure");

    if (fin.type != fout.type || fin.tag != fout.tag || fin.fid != fout.fid ||
        fin.newfid != fout.newfid || fin.nwname != fout.nwname ||
        memcmp(fin.wname, fout.wname, 12) != 0)
        puts("FCALL::ERROR::fcall Twalk failure");
}

static void _test_Rwalk(void) {
    char buf[256];
    struct qid qids[2] = {
        {
            .type = (unsigned int) ~0,
            .version = (unsigned int) ~0,
            .id = (unsigned long) ~0,
        },
        {
            .type = (unsigned int) ~0,
            .version = (unsigned int) ~0,
            .id = (unsigned long) ~0,
        },
    };
    unsigned res;
    fcall fout, fin = {
        .type = Rwalk,
        .tag = NOTAG,
        .nwqid = 2,
        .wqid = qids,
    };

    res = fcall_msg_size(&fin);
    if (res != 41)
        puts("FCALL::ERROR::fcall_msg_size Rwalk failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 41)
        puts("FCALL::ERROR::fcall_msg_to_buf Rwalk failure");

    res = fcall_buf_to_msg(buf, &fout);
    if (res != 41)
        puts("FCALL::ERROR::fcall_buf_to_msg Rwalk failure");

    if (fin.type != fout.type || fin.tag != fout.tag || fin.nwqid != fout.nwqid ||
        memcmp(fin.wqid, fout.wqid, fin.nwqid * sizeof(struct qid)) != 0)
        puts("FCALL::ERROR::fcall Rwalk failure");
}

static void _test_Tcreate(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Tcreate,
        .tag = 0,
        .fid = 0,
        .name = "file",
        .perm = 0,
        .mode = 0,
    };

    res = fcall_msg_size(&fin);
    if (res != 22)
        puts("FCALL::ERROR::fcall_msg_size Tcreate failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 22)
        puts("FCALL::ERROR::fcall_msg_to_buf Tcreate failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 22 || fin.type != fout.type || fin.tag != fout.tag ||
        fin.fid != fout.fid || strcmp(fin.name, fout.name) != 0 ||
        fin.perm != fout.perm || fin.mode != fout.mode)
        puts("FCALL::ERROR::fcall Tcreate failure");
}

static void _test_Rcreate(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Rcreate,
        .tag = 0,
        .qid = {
            .type = 0,
            .version = 1,
            .id = 2,
        },
        .iounit = 0,
    };

    res = fcall_msg_size(&fin);
    if (res != 27)
        puts("FCALL::ERROR::fcall_msg_size Rcreate failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 27)
        puts("FCALL::ERROR::fcall_msg_to_buf Rcreate failure");

    res = fcall_buf_to_msg(buf, &fout);

    if (res != 27 || fin.type != fout.type || fin.tag != fout.tag ||
        fin.qid.type != fout.qid.type || fin.qid.version != fout.qid.version ||
        fin.qid.id != fout.qid.id || fin.iounit != fout.iounit)
        puts("FCALL::ERROR::fcall Rcreate failure");
}

static void _test_Tread(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Tread,
        .tag = NOTAG,
        .fid = NOFID,
        .offset = (unsigned long) ~0,
        .count = (unsigned) ~0,
    };

    res = fcall_msg_size(&fin);
    if (res != 23)
        puts("FCALL::ERROR::fcall_msg_size Tread failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 23)
        puts("FCALL::ERROR::fcall_msg_to_buf Tread failure");

    res = fcall_buf_to_msg(buf, &fout);
    if (res != 23)
        puts("FCALL::ERROR::fcall_buf_to_msg Tread failure");

    if (fin.type != fout.type || fin.tag != fout.tag || fin.fid != fout.fid ||
        fin.offset != fout.offset || fin.count != fout.count)
        puts("FCALL::ERROR::fcall Tread failure");
}

static void _test_Rread(void) {
    char buf[256];
    unsigned res;
    fcall fout, fin = {
        .type = Rread,
        .tag = NOTAG,
        .count = 4,
        .data = "data"
    };

    res = fcall_msg_size(&fin);
    if (res != 15)
        puts("FCALL::ERROR::fcall_msg_size Rread failure");

    res = fcall_msg_to_buf(&fin, buf, 256);
    if (res != 15)
        puts("FCALL::ERROR::fcall_msg_to_buf Rread failure");

    res = fcall_buf_to_msg(buf, &fout);
    if (res != 15)
        puts("FCALL::ERROR::fcall_buf_to_msg Rread failure");

    if (fin.type != fout.type || fin.tag != fout.tag || fin.count != fout.count ||
        memcmp(fin.data, fout.data, 4) != 0)
        puts("FCALL::ERROR::fcall Rread failure");
}

void test_fcall(void) {
    _test_Tversion();
    _test_Rversion();
    _test_Rerror();
    _test_Tattach();
    _test_Rattach();
    _test_Twalk();
    _test_Rwalk();
    _test_Topen();
    _test_Ropen();
    _test_Tcreate();
    _test_Rcreate();
    _test_Tread();
    _test_Rread();
}
