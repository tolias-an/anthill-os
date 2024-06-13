#include "internal/stdio.h"

#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
#include <unistd.h>

FILE open_files[FOPEN_MAX];

static bool initialised = false;
static mqd_t mq_in = -1;
static mqd_t mq_vfs = -1;
static char vfs_name[16];
static unsigned fid_count = 4;
static unsigned short tag_count = 0;

static bool _check_init(void) {
    if (!initialised) {
        if (!file_init())
            return false;
    }

    return true;
}

void file_deinit(void) {
    if (mq_in != -1) { mq_close(mq_in); mq_in = 0; mq_unlink(vfs_name); }

    if (mq_vfs != -1) { mq_close(mq_vfs); mq_vfs = 0; }
}

bool file_init(void) {
    /* This function is called once during the process initialisation. In case it fails
     * because the VFS server has not been initialised or it is called during the VFS
     * server initialisation, it is called with each file_* function */
    pid_t pid = getpid();

    if (snprintf(vfs_name, 16, "vfs/%ld", pid) > 16)
        return false;

    /* Setup vfs queues. */

    if (mq_vfs == -1) {
        mq_vfs = mq_open("vfs/server", O_WRONLY);
        if (mq_vfs == -1) return false;
    }

    if (mq_in == -1) {
        mq_in = mq_open(vfs_name, O_RDONLY | O_CREAT | O_EXCL);
        if (mq_in == -1) return false;
    }

    /* Send Tattach */
    unsigned short tag = tag_count++;
    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, vfs_name, 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);

    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag;
    vfs_msg.fcall.fid = 0;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, mq_vfs, mq_in))
        return false;

    open_files[0].fid = 3;

    initialised = true;

    return true;
}

void *file_alloc(FILE *stream) {
    stream->buf = malloc(BUFSIZ);
    if (!stream->buf)
        { errno = ENOMEM; return 0; }

    stream->buf_end = stream->buf + BUFSIZ;
    stream->buf_pos = stream->buf;

    return stream->buf;
}

FILE *file_open(const char *restrict pathname, int oflag) {
    if (!_check_init())
        { errno = EACCES; return NULL; }

    unsigned int fid = fid_count++;
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twalk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = open_files[0].fid;
    vfs_msg.fcall.newfid = fid;
    vfs_msg.mq_id = mq_in;

    if (fcall_path_split(&vfs_msg.fcall, pathname) == 0)
        { errno = EACCES; return NULL; }

    /* TODO: Relative path handling */
    if (!vfs_msg_send(&vfs_msg, mq_vfs, mq_in))
        { errno = EACCES; free(vfs_msg.fcall.wname); return NULL; }

    free(vfs_msg.fcall.wname);

    if (oflag & O_RDONLY) vfs_msg.fcall.mode = OREAD;
    else if (oflag & O_WRONLY) vfs_msg.fcall.mode = OWRITE;
    else vfs_msg.fcall.mode = O_RDWR;

    if (oflag & O_TRUNC) vfs_msg.fcall.mode |= OTRUNC;

    vfs_msg.fcall.type = Topen;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, mq_vfs, mq_in))
        { errno = EACCES; return NULL; }

    FILE *f = NULL;
    for (unsigned i = 0; i < FOPEN_MAX; i++) {
        if (!(open_files[i].flags & F_OPEN)){
            f = &open_files[i];
            break;
        }
    }

    if (!f)
        { errno = EMFILE; return NULL; }

    f->fid = fid;
    f->flags |= 1 << F_OPEN;
    f->seek_offset = 0;
    f->chunk_index = -1;

    return f;
}

unsigned file_read(FILE *stream) {
    if (!_check_init())
        { errno = EIO; return 0; }

    if (!stream->buf) {
        if (!file_alloc(stream))
            return 0;
    }

    if (stream->buf_end - stream->buf < BUFSIZ) {
        stream->flags |= 1 << F_EOF;
        return 0;
    }

    struct vfs_msg vfs_msg;

    unsigned current_buffer_size = 0;

    while (current_buffer_size < BUFSIZ) {
        unsigned remaining = BUFSIZ - current_buffer_size;
        unsigned count = remaining > VFS_MAX_IOUNIT ? VFS_MAX_IOUNIT : remaining;
        vfs_msg.fcall.type = Tread;
        vfs_msg.fcall.tag = tag_count++;
        vfs_msg.fcall.fid = stream->fid;
        vfs_msg.fcall.count = count;
        vfs_msg.fcall.offset = stream->seek_offset + current_buffer_size;
        vfs_msg.mq_id = mq_in;

        if (!vfs_msg_send(&vfs_msg, mq_vfs, mq_in))
            { errno = EIO; return 0; }

        unsigned n = (vfs_msg.fcall.count < count) ? vfs_msg.fcall.count : count;
        memcpy(stream->buf + current_buffer_size, vfs_msg.fcall.data, n);

        current_buffer_size += n;

        if (vfs_msg.fcall.count < count)
            break;
    }

    stream->buf_end = stream->buf + current_buffer_size;
    stream->chunk_index++;
    return current_buffer_size;
}

unsigned file_write(FILE *stream) {
    if (!_check_init())
        { errno = EIO; return 0; }

    if (!stream->buf)
        return 0;

    struct vfs_msg vfs_msg;
    unsigned remaining = stream->buf_pos - stream->buf;
    unsigned bytes_sent = 0;

    while (remaining > 0) {
        unsigned count = remaining > VFS_MAX_IOUNIT ? VFS_MAX_IOUNIT : remaining;

        vfs_msg.fcall.type = Twrite;
        vfs_msg.fcall.tag = tag_count++;
        vfs_msg.fcall.fid = stream->fid;
        vfs_msg.fcall.offset = stream->seek_offset + bytes_sent;
        vfs_msg.fcall.count = count;
        vfs_msg.fcall.data = (unsigned char *) stream->buf;
        vfs_msg.mq_id = mq_in;

        if (!vfs_msg_send(&vfs_msg, mq_vfs, mq_in))
            { errno = EIO; return bytes_sent; }

        bytes_sent += vfs_msg.fcall.count;

        if (vfs_msg.fcall.count < count)
            { errno = EIO; return bytes_sent; }

        remaining -= vfs_msg.fcall.count;
    }

    stream->buf_pos = stream->buf;
    return bytes_sent;
}
