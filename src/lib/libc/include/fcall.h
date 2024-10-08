/*
* 9p2000 protocol implementation
*
* Data is represented in little-endian order
* Variable size parameters are prefixed with an unsigned short containing
* the size of the parameter. Strings are not NULL terminated
*
* size[4] Tversion tag[2] msize[4] version[s]
* size[4] Rversion tag[2] msize[4] version[s]
*
* Negotiates protocol version and maximum message size.
*
* Note: Implemented but practically unused
*
* - _version_ is always the string 9p2000.
*
* size[4] Rerror tag[2] ename[s]
*
* Error message _ename_
*
* size[4] Tattach tag[2] fid[4] afid[4] uname[s] aname[s]
* size[4] Rattach tag[2] qid[16]
*
* Attach file tree _aname_ to _fid_.
* Response contains qid of _aname_.
*
* Note: Because the 9p2000 protocol is meant to be used on top of other protocols
* not currently implemented, contrary to the 9p protocol, _uname_ currently contains
* the mqueue channel to be associated with the _fid_.
*
* - _afid_ is currently unused and its value should be NOFID
*
* size[4] Twalk tag[2] fid[4] newfid[4] nwname[2] nwname * wname[s]
* size[4] Rwalk tag[2] nwqid[2] nwqid * wqid[16]
*
* Traverse the directory hierarchy _nwname_ path elements from the file represented
* by _fid_. The response contains _nwqid_ qids for each of the successfuly traversed
* folders. If the whole path is traversed, meaning _nwname_ == _nwqid_, the resulting
* file is represented by _newfid_.
*
* size[4] Topen tag[2] fid[4] mode[1]
* size[4] Ropen tag[2] qid[16] iounit[4]
*
* Open file represented by _fid_ with _mode_. _iounit_ is the maximum number
* of bytes guaranteed to be transferred by one 9p message
*
* size[4] Tcreate tag[2] fid[4] name[s] perm[4] mode[1]
* size[4] Rcreate tag[2] qid[16] iounit[4]
*
* Create file with _name_ in the directory represented by _fid_
* with permissions _perm_. Then, open file with _mode_. _iounit_ is the maximum
* number of bytes guaranteed to be transferred by one 9p message.
*
* size[4] Tread tag[2] fid[4] offset[8] count[4]
* size[4] Rread tag[2] count[4] data[count]
*
* Request _count_ bytes of data from file represented by _fid_,
* starting from _offset_.
*
* size[4] Twrite tag[2] fid[4] offset[8] count[4] data[count]
* size[4] Rwrite tag[2] count[4]
*
* Write _count_ bytes of data from file represented by _fid_,
* starting from _offset_.
*
* size[4] Tclunk tag[2] fid[4]
* size[4] Rclunk tag[2]
*
* Close file represented by _fid_
*
* size[4] Tstat tag[2] fid[4]
* size[4] Rstat tag[2] nstat[2] stat[nstat]
*
* Inquire about the file represented by _fid_.
* _stat_ contains:
* - qid[16]   file qid
* - length[8] file size in bytes
* - name[s]   file name
*
* size[4] Tmount tag[2] fid[4] mfid[4]
* size[4] Rmount tag[2] qid[16]
*
* Send Tattach message to file associated with _fid_ and establish file tree on
* directory associated with _mfid_
*/

#ifndef _FCALL_H
#define _FCALL_H

#include <pstring.h>

#define NOFID (unsigned int)~0
#define NOTAG (unsigned short)~0

#define FCALL_ERROR(e,s) pstring (e) = {sizeof(s) - 1, (s)}

#define CHAR_SIZE 1
#define SHRT_SIZE 2
#define INT_SIZE  4
#define LONG_SIZE 8

#define FCALL_SIZE_OFF 0
#define FCALL_TAG_OFF  5
#define FCALL_FID_OFF  7

/* File modes */
#define OREAD   1 << 0 /* Read only */
#define OWRITE  1 << 1 /* Write only */
#define ORDWR   1 << 2 /* Read and write */
#define OEXEC   1 << 3 /* Execute */
#define OTRUNC  1 << 4 /* Truncate */
#define ORCLOSE 1 << 5 /* Remove when fid is cluncked */

/* File permissions */
#define DMDIR 1 << 31 /* Directory */
#define DMTMP 1 << 16 /* Temporary */

struct qid {
    unsigned int  type;
    unsigned int  version;
    unsigned long id;
};

struct fcall_stat {
    struct qid    qid;      /* File qid */
    unsigned long length;   /* File length */
    unsigned char buffer[]; /* Variable length buffer */
};

/* qid types */
#define QTFILE 0
#define QTDIR  1 << 31

/*
* A data structure for storing and parsing 9p messages.
* Strings are expected to be of type pstring.
*/
typedef struct {
    unsigned int   fid;  /* Associated file descriptor */
    unsigned char  type; /* Message type */
    unsigned short tag;  /* Identifying tag */
    union {
        struct { /* Tversion, Rversion */
            unsigned msize;   /* Maximum message size */
            pstring* version; /* Protocol version */
        };
        struct { /* Rerror */
            pstring* ename; /* Error string */
        };
        struct { /* Tattach */
            unsigned afid;  /* Authentication fid. Currently unused */
            pstring* uname; /* Associated user. Currently unused */
            pstring* aname; /* File tree to access */
        };
        struct { /* Rattach, Rcreate, Ropen, Rmount */
            struct qid qid;    /* File information */
            unsigned   iounit; /* Maximum number of bytes guaranteed */
                               /* to be read or written. Currently unused */
        };
        struct { /* Twalk */
            unsigned       newfid; /* Fid to be associated with the last element */
            unsigned short nwname; /* Number of path elements */
            pstring*       wname;  /* Pointer to path elements */
        };
        struct { /* Rwalk */
            unsigned short nwqid; /* Number of qids */
            struct qid*    wqid;  /* Pointer to qids */
        };
        struct { /* Tcreate, Topen */
            pstring* name; /* File name to be created */
            unsigned perm; /* File permissions */
            char     mode; /* File open mode */
        };
        struct { /* Tread, Rread, Twrite, Rwrite */
            unsigned long  offset; /* Where to start reading /writing (Tread, Twrite) */
            unsigned       count;  /* Number of bytes (Tread, Rread, Twrite, Rwrite) */
            unsigned char* data;   /* Data read/to write (Rread, Twrite) */
        };
        struct { /* Rstat */
            unsigned short     nstat; /* Stat length */
            struct fcall_stat* stat;  /* File stat */
        };
        struct { /* Tmount */
            unsigned mfid; /* Mount directory */
        };
    };
} fcall;

enum fcall_types {
    Tversion = 100,
    Rversion,
    Terror, /* Illegal */
    Rerror,
    Tattach,
    Rattach,
    Twalk,
    Rwalk,
    Topen,
    Ropen,
    Tcreate,
    Rcreate,
    Tread,
    Rread,
    Twrite,
    Rwrite,
    Tclunk,
    Rclunk,
    Tstat,
    Rstat,
    Tmount,
    Rmount,
};

/*
* Calculate the number of bytes _fcall_ message size
*
* On failure, returns 0
*/
unsigned fcall_msg_size(const fcall *fcall);

/*
* Writes _fcall_ struct and to buffer pointed to by _buf_,
* with at most _length_ bytes
*
* On success, returns the length of the resulting buffer.
* On failure, returns 0
*/
unsigned fcall_msg_to_buf(const fcall *fcall, unsigned char *buf, unsigned length);

/*
* Creates _fcall_ struct from buffer pointed to by _buf_
*
* On success, returns the length the number of bytes the buffer occupied
* On failure, returns 0
*/
unsigned fcall_buf_to_msg(unsigned char *buf, fcall *fcall);

/*
* Calculates the size needed for a slash delimited path assembled from the nwname and
* wname fields of _fcall_. Fails on malformed input.
*
* On success, returns the size.
* On failure, returns 0.
*/
unsigned short fcall_path_size(fcall *fcall);

/*
* Splits _path_ delimited by slashes to _wname_, an array of malloc'd pstring elements.
*
* On success, returns number of elements.
* On failure, returns -1 and no memory is allocated.
*/
int fcall_path_split(pstring **wname, const char *path);

#endif /* _FCALL_H */
