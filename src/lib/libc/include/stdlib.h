#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/types.h>

/* --- Memory allocation functions ---*/

/*
* Allocate unused space for an object of _size_ in bytes
*
* On success, returns a pointer to the allocated space.
* On failure, returns a null pointer and sets errno to
* indicate the error
*
* errno:
* - EINVAL Size is 0
* - ENOMEM Not enough available space
*/
void* malloc(size_t size);

/*
* Allocate unused space for an array of _nelem_, each of
* size _elsize_
*
* On success, returns a pointer to the allocated space.
* On failure, returns a null pointer and sets errno to
* indicate the error.
*
* errno:
* - ENOMEM Not enough available memory
* - EINVAL Either _nelem_ or _elsize_ is zero
*/
void *calloc(size_t nelem, size_t elsize);

/*
* Replace allocation size for object pointed to by _ptr_ and put
* the contents of _ptr_ up to the lesser of the new and old sizes.
*
* If _ptr_ is a null pointer, the behaviour is equivalent to
* malloc.
*
* On success, returns a pointer to the allocated space and _ptr_
* is deallocated.
* On failure, returns a null pointer and sets errno to
* indicate the error. The old object _ptr_ is left as is.
*
* errno:
* - EINVAL Size is 0 or _ptr_ wasn't returned by previous memory
*   allocation
* - ENOMEM Not enough available space
*/
void* realloc(void *ptr, size_t size);

/*
* Deallocate space pointed to by _ptr_
*/
void free(void *ptr);

/* --- Process functions --- */

/*
* Terminate process
*
* Status is currently unsupported
*/
void exit(int status);

/* String conversion functions */

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type long representation in radix
* _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
long strtol(const char *restrict str, char **restrict endptr, int base);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type long long representation in
* radix _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
long long strtoll(const char *restrict nptr, char **restrict endptr, int base);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type unsigned long representation
* in radix _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
unsigned long strtoul(const char *restrict str, char **restrict endptr, int base);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type unsigned long long representation
* in radix _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
unsigned long long strtoull(const char *restrict str, char **restrict endptr, int base);

#endif /* _STDLIB_H */
