/*
* Message queueing
*
* Current implementation follows the POSIX standard with some limitations,
* and with the exception of mode_t and O_NONBLOCK, which is currently unsupported
*/

#ifndef _MQUEUE_H
#define _MQUEUE_H

/* Message queue descriptor */
typedef int mqd_t;

/* Message queue attributes */
typedef struct {
    long mq_flags; /* Message queue flags */
    long mq_maxmsg; /* Maximum number of messages */
    long mq_msgsize; /* Maximum message size */
    long mq_curmsg; /* Number of messages currently queued; */
} mq_attr;

/* Available oflags */
#define O_RDONLY 1 << 0
#define O_WRONLY 1 << 1
#define O_RDWR   1 << 2
#define O_CREAT  1 << 3
#define O_EXCL   1 << 4

/*
* Establish a connection to the message queue with
* _name_ and access type specified by _oflag_.
*
* On success, returns the message queue descriptor.
* On failure, returns -1 and sets errno to indicate
* the error.
*
* Available oflags:
*
* Mandatory and exclusive:
* O_RDONLY   Open queue as read-only
* O_WRONLY   Open queue as write-only
* O_RDWR     Open queue as read and write
*
* Optional:
* O_CREAT    Create new message queue
* O_EXCL     Fail if the queue name exists
* O_NONBLOCK Currently unsupported
*
* errno values:
* EACCES     The message queue exists but access is
*            denied, or the message queue doesn't
*            exist and creation is denied
* EEXIST     O_CREAT and O_EXCL are set and the
*            message queue exists
* EINVAL     O_CREAT is set, attr is not null and
*            mq_maxmsg or mq_msgsize are <= 0
* ENOENT     Message queue doesn't exist and
*            O_CREAT is not set
* ENOSPC     Insufficient space for the creation
*            of a new message queue
*/
mqd_t mq_open(const char *name, int oflag, ...);

#endif /* _MQUEUE_H */
