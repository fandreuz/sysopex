#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define N 1024

int global_array[N];
int current_idx;

pthread_t t1;
pthread_t t2;

#include <stdio.h>
#include <stdlib.h>
#define swap(t, x, y) \
	{                 \
		t z = x;      \
		x = y;        \
		y = z;        \
	}
typedef int TYPE;

int partition(TYPE[], int, int);
void quick_sort(TYPE[], int, int);
void print_array(TYPE[], int);

int partition(TYPE A[], int p, int r)
{
	TYPE x = A[r]; //pivot
	int i = p - 1, j;
	for (j = p; j < r; j++)
	{
		if (A[j] <= x)
		{
			i = i + 1;
			swap(TYPE, A[i], A[j]);
		}
	}
	i = i + 1;
	swap(TYPE, A[i], A[r]);
	return i;
}
void quick_sort(TYPE A[], int p, int r)
{
	if (p < r)
	{
		int t = (rand() % (r - p + 1) + p);
		swap(TYPE, A[t], A[r]);
		//Used to avoid O(n^2) worst case

		int q = partition(A, p, r);
		quick_sort(A, p, q - 1);
		quick_sort(A, q + 1, r);
	}
}

void sort_array()
{
	printf("ordini?\n");
	quick_sort(global_array, 0, current_idx);
	printf("si!\n");
}

void process_signal_handler(int signum)
{
	printf("%d\n", signum);
	printf("ho ricevuot qualcosa!\n");
	if (pthread_self() == t1)
	{
		if (current_idx == N)
		{
			printf("terminating t1\n");

			if (pthread_kill(t2, SIGTERM) != 0)
			{
				perror("pthread_kill");
				exit(1);
			}

			exit(0);
		}
		else
		{
			printf("ricevuto t1!\n");
			global_array[current_idx] = N - current_idx - 1;
			current_idx++;

			if (pthread_kill(t2, SIGUSR1) != 0)
			{
				perror("pthread_kill");
				exit(1);
			}

			printf("inviato a t2!\n");
			pause();
			printf("icoa\n");
		}
	}
	else if (pthread_self() == t2)
	{
		if (signum == SIGTERM)
		{
			printf("terminating t2\n");
			exit(0);
		}
		else
		{
			printf("ricevuto t2!\n");
			sort_array();
			printf("ordinato!\n");
			if (pthread_kill(t1, SIGUSR1) != 0)
			{
				perror("pthread_kill");
				exit(1);
			}
			printf("inviato a t1!\n");
			pause();
		}
	}
}

void *thread1_entry_point(void *arg)
{
	printf("ciao\n");

	if (signal(SIGUSR1, process_signal_handler) == SIG_ERR)
	{
		perror("signal");
		exit(1);
	}

	if (pthread_kill(t1, SIGUSR1) != 0)
	{
		perror("pthread_kill");
		exit(1);
	}

	printf("inviato!\n");
	pause();
	printf("[t1] bye\n");
	return NULL;
}

void *thread2_entry_point(void *arg)
{
	if (signal(SIGUSR1, process_signal_handler) == SIG_ERR || signal(SIGTERM, process_signal_handler) == SIG_ERR)
	{
		perror("signal");
		exit(1);
	}

	pause();
	printf("goodby\n");
	return NULL;
}

int main()
{
	if (signal(SIGUSR1, process_signal_handler) == SIG_ERR || signal(SIGTERM, process_signal_handler) == SIG_ERR)
	{
		perror("signal");
		exit(1);
	}

	int res = pthread_create(&t1, NULL, thread1_entry_point, NULL);
	if (res != 0)
	{
		perror("pthread_create");
		exit(1);
	}

	res = pthread_create(&t2, NULL, thread2_entry_point, NULL);
	if (res != 0)
	{
		perror("pthread_create");
		exit(1);
	}

	res = pthread_join(t1, NULL);
	if (res != 0)
	{
		perror("pthread_join");
		exit(1);
	}

	printf("t1 è morto.\n");

	res = pthread_join(t2, NULL);
	if (res != 0)
	{
		perror("pthread_join");
		exit(1);
	}

	printf("t2 è morto.\n");
}
