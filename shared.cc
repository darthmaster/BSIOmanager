/*
 * shmem.cc
 *
 * Created on: 22.09.2020.
 *     Author: ELNA Team
 */

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>	/* for ftruncate(), close() */
#include <string.h>	/* for strncpy() */

#include "shared.h"

SharedMem::SharedMem(const char* objname) : descriptor(-EPERM), size(sizeof(int))
{
    strncpy(name, objname, sizeof(name));
	descriptor = shm_open((char*)name, O_RDWR | (O_CREAT | O_EXCL), (S_IRUSR | S_IWUSR));
    if(descriptor == -EPERM) {
        /* Область уже существует. */
        if(errno == EEXIST) {
            /* DEBUG */
            printf("Shared memory object already was created (I'm a client).\n");
            descriptor = shm_open((char*)name, O_RDWR, S_IRWXU);
            /* Область уже создана, но прицепиться к ней не удалось. */
            if(descriptor == -EPERM) {
                /*ERROR: Can't connect to shared memory object.*/
                disconnect(descriptor, size);
                throw (int)1;
            }
        }
        else {
            /* Область не создана, и создать ее не удалось. */
            /* ERROR: Can't connect to shared memory object.*/
            disconnect(descriptor, size);
			throw (int)2;
		}
    }
    /* DEBUG */
    else printf("Shared memory object already was created (I'm a server).\n");
    /* Область создалась, но прицепиться к ней не удалось. */
    if(connect(descriptor, size) < 0) {
        /*ERROR: Can't connect to shared memory object.*/
		disconnect(descriptor, size);
		throw (int)3;
	}
}

SharedMem::~SharedMem()
{
    /* DEBUG */
    printf("DESTRUCTOR: '%s' disconnect.\n", name);
	disconnect(descriptor, size);
}


int SharedMem::connect(int descr, off_t size)
{
    if(ftruncate(descr, size) < 0)  //0, -1
        return -1;
    region = (shared_region_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, descr, 0);
    if(region == MAP_FAILED)
        return -1;
    return 0;
}

void SharedMem::disconnect(int descr, off_t size)
{
    munmap(region, size);
    close(descr);
}

void SharedMem::delmemory(void)
{
    shm_unlink(name);
}
