#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

int main(void)
{
	int fd;
	int nread, cnt1 = 0, cnt2 = 0, errcnt = 0;
	double typing_sec;
	int typing_per_min;
	char ch, text1[] = {"Heaven helps those who help themselves."}, text2[] = {"The die is cast."};
	struct termios init_attr, new_attr;
	struct timespec begin, end;

	fd = open(ttyname(fileno(stdin)), O_RDWR);
	tcgetattr(fd, &init_attr);

	new_attr = init_attr;
	new_attr.c_lflag &= ~ICANON;
	new_attr.c_lflag &= ~ECHO; /* ~(ECHO | ECHOE | ECHOK | ECHONL); */
	new_attr.c_cc[VMIN] = 1;
	new_attr.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSANOW, &new_attr) != 0)
	{
		fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
	}

	clock_gettime(CLOCK_MONOTONIC, &begin);
	printf("다음 문장을 그대로 입력하시오\n%s\n", text1);
	while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n')
	{
		if (ch == text1[cnt1++])
			write(fd, &ch, 1);
		else
		{
			write(fd, "*", 1);
			errcnt++;
		}
	}

	printf("\n다음 문장을 그대로 입력하시오\n%s\n", text2);
	while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n')
	{
		if (ch == text2[cnt2++])
			write(fd, &ch, 1);
		else
		{
			write(fd, "*", 1);
			errcnt++;
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	int typing_total = cnt1 + cnt2 + errcnt;
	typing_sec = ((end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / (double)100000000);
	typing_per_min = ((typing_total / typing_sec) * 60);

	printf("\n\n총 타자수 : %d\n", typing_total);
	printf("걸린 시간: %.2lf초\n", typing_sec);
	printf("평균 분당 타자수 : %d\n", typing_per_min);
	printf("오타수 %d\n", errcnt);
	tcsetattr(fd, TCSANOW, &init_attr);
	close(fd);
}
