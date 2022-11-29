#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define SHMSIZE 4096
#define SHMNAME "/my_shm"
#define SEMNAME "/my_sem"

void p(sem_t *semd);
void v(sem_t *semd);

void main(int argc, char *argv[])
{
    FILE *fp1, *fp2; // fp1을 fp2에 복사
    char buf[SHMSIZE];
    int status, shmd, count;
    void *shmaddr;
    pid_t pid;
    sem_t *semd;
    int val;
    struct stat file1;

    if (lstat(argv[1], &file1) < 0)
    {
        perror("stat");
        exit(1);
    }

    if ((semd = sem_open(SEMNAME, O_CREAT, 0600, 1)) == SEM_FAILED)
    {
        perror("sem_open failed");
        exit(1);
    }

    sem_getvalue(semd, &val);
    // memset(buf, 0, SHMSIZE);

    if ((pid = fork()) == 0) //자식프로세스
    {
        if ((shmd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666)) == -1) //공유메모리
        {
            perror("shm_open failed");
            exit(1);
        }
        if (ftruncate(shmd, SHMSIZE) != 0)
        {
            perror("ftruncate failed");
            exit(1);
        }
        if ((shmaddr = mmap(0, SHMSIZE, PROT_WRITE, MAP_SHARED, shmd, 0)) == MAP_FAILED)
        {
            perror("mmap failed");
            exit(1);
        }
        if ((fp1 = fopen(argv[1], "r")) == NULL)
        {
            perror("fopen");
            exit(1);
        }
        if (mlock(shmaddr, SHMSIZE) != 0)
        {
            perror("mlock failed");
            exit(1);
        }

        p(semd);
        printf("Child Process %d : semaphore in use\n", getpid());

        while (!feof(fp1))
        {
            count = fread(buf, sizeof(char), SHMSIZE, fp1);
            strcpy((char *)shmaddr, buf);
        }
        v(semd);
        printf("Child Process %d : putting semaphore\n", getpid());

        if (munmap(shmaddr, SHMSIZE) == -1)
        {
            perror("munmap failed");
            exit(1);
        }

        fclose(fp1);
        close(shmd);

        exit(0);
    }
    else if (pid > 0) //부모프로세스
    {
        pid = wait(&status); //자식프로세스 종료까지 대기

        if ((shmd = shm_open(SHMNAME, O_RDWR, 0666)) == -1)
        {
            perror("shm_open failed");
            exit(1);
        }
        if ((shmaddr = mmap(0, SHMSIZE, PROT_READ, MAP_SHARED, shmd, 0)) == MAP_FAILED)
        {
            perror("mmap failed");
            exit(1);
        }
        if (mlock(shmaddr, SHMSIZE) != 0)
        {
            perror("mlock failed");
            exit(1);
        }

        if ((fp2 = fopen(argv[2], "w")) == NULL) // w : 파일이 존재하는 경우 기존 내용을 지우고 쓰기
        {
            perror("fopen");
            exit(1);
        }

        p(semd);
        printf("Parent Process %d : semaphore in use\n", getpid());

        fwrite(shmaddr, sizeof(char), file1.st_size, fp2); //
        fclose(fp2);

        if (munmap(shmaddr, SHMSIZE) == -1)
        {
            perror("munmap failed");
            exit(1);
        }

        printf("Parents Process %d : putting semaphore\n", getpid());
        v(semd);

        close(shmd);

        if (shm_unlink(SHMNAME) == -1)
        {
            perror("shm_unlink failed");
            exit(1);
        }
        if (sem_close(semd) == -1)
        {
            perror("sem_close failed");
            exit(1);
        }
    }
    else
    {
        perror("fork Error");
        exit(1);
    }
}

void p(sem_t *semd)
{
    int ret;
    if ((ret = sem_trywait(semd)) != 0 && errno == EAGAIN)
        sem_wait(semd);
    else if (ret != 0)
    {
        perror("sem_trywait failed");
        exit(1);
    }
}
void v(sem_t *semd)
{
    if (sem_post(semd) != 0)
    {
        perror("sem_post failed");
        exit(1);
    }
}
