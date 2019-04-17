#include<iostream>
#include<thread>
using namespace std;

#define MAX_ITERATE	10
#define MAX(x,y)	((x) < (y)) ? (y) : (x);
#define MIN(x,y)	((x) > (y)) ? (y) : (x);

int *a, *b;
int na, nb;
int k;

int search(const int* a, int na, int key);
void split(int seg, int prev_ai, int prev_bj, int* ai, int* bj);
void merge(const int* a, int na, const int* b, int nb, int* c);

int main(void) {
	int *endlistA;	//Indices of where each segment ends in array a
	int *endlistB;	//Indices fo where each segment ends in array b
	int *c;
	thread *t;		//Thread list

	//Initialize array a, b
	cin >> na;
	a = new int[na];
	for (int i = 0; i < na; i++)
		cin >> a[i];
	cin >> nb;
	b = new int[nb];
	for (int i = 0; i < nb; i++)
		cin >> b[i];

	//Initialize k and endlists
	cin >> k;
	endlistA = new int[k];
	endlistB = new int[k];

	//Determine how to split the two arrays so that
	//each thread shares similar workload
	int prevA, prevB;
	prevA = prevB = 0;
	for (int i = 0; i < k - 1; i++) {
		split(i + 1, prevA, prevB, &endlistA[i], &endlistB[i]);
		prevA = endlistA[i];
		prevB = endlistB[i];
	}
	endlistA[k - 1] = na;
	endlistB[k - 1] = nb;
	
	//For debug
	for (int i = 0; i < k; i++) {
		cout << endlistA[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < k; i++) {
		cout << endlistB[i] << " ";
	}
	cout << endl;

	//Merge each pair of segment using k threads
	c = new int[na + nb];
	t = new thread[k];
	int ai, bi, ci;
	ai = bi = ci = 0;
	for (int i = 0; i < k; i++) {
		t[i] = thread(merge, a + ai, endlistA[i] - ai, b + bi, endlistB[i] - bi, c + ci);
		ai = endlistA[i];
		bi = endlistB[i];
		ci = endlistA[i] + endlistB[i];
	}
	for (int i = 0; i < k; i++) {
		t[i].join();
	}

	for (int i = 0; i < na + nb; i++) {
		cout << c[i] << " ";
	}
	cout << endl;
	
	delete[] a; a = NULL;
	delete[] b; b = NULL;
	delete[] c; c = NULL;
	delete[] endlistA; endlistA = NULL;
	delete[] endlistB; endlistB = NULL;	
	delete[] t; t = NULL;
	system("pause");
	return 0;
}

//Given a seg index, return the end indices of array a and b
void split(int seg, int prev_ai, int prev_bj, int* ai, int* bj) {
	int i, j, i_tmp, j_tmp;
	int m, n;
	int cnt = MAX_ITERATE;
	i = MAX(na * seg / k, prev_ai);			//Initial index of a
	n = (na + nb) * seg / k;				//Ideal number of elements in the segment
	while (true) {
		if (i < na) {
			//Find a[i] in b[], and work out the index j so that b[j-1] <= a[i] < b[j]
			j = MAX(search(b, nb, a[i]) + 1, prev_bj);
		}			
		else {
			//There is a great chance that b[0] > a[na-1]
			j = MIN(n - i, nb);
			j = MAX(j, prev_bj);
		}
		//Because a[i-1] <= a[i], we already guarantee that:
		//1. b[j-1] <= a[i]
		//2. a[i-1] < b[j]
		//So each segment can be ripped clearly

		//If time is out, break to avoid infinite loop
		if (--cnt < 0)
			break;

		//Get actual total number of elements in the segment
		m = i + j;
		//Assume we adjust the index of a so that m gets closer to n
		i_tmp = MIN((i * n / m), na);
		i_tmp = MAX(i_tmp, prev_ai);
		j_tmp = j;
		//If i remains stable, then we should adjust j instead
		if (i_tmp == prev_ai) {
			j_tmp = MIN(n - i_tmp, nb);
			j_tmp = MAX(j_tmp, prev_bj);
		}

		//If the effect of the adjust is small enough, we can accept current i
		if (i_tmp == i)
			break;
		//Iterate until m gets close enough to n
		i = i_tmp;
		j = j_tmp;
	}
	*ai = i;
	*bj = j;
	return;
}

//Merge two segments of sorted arrays
void merge(const int* a, int na, const int* b, int nb, int* c) {
	int i, j, k;
	i = j = k = 0;
	while (i < na && j < nb) {
		if (a[i] < b[j])
			c[k++] = a[i++];
		else
			c[k++] = b[j++];
	}
	if (i < na) {
		memcpy(c + k, a + i, sizeof(int)*(na - i));
	}
	if (j < nb) {
		memcpy(c + k, b + j, sizeof(int)*(nb - j));
	}
}

//Given an array and a key, find the index i where a[i] is less or equal than
//but closest to key
int search(const int* a, int na, int key) {
	int left = 0, right = na - 1;
	int middle;
	int index = -1;
	//Binary search
	while (left <= right) {
		middle = (left + right) / 2;
		if (a[middle] < key) {
			left = middle + 1;
		}
		else if (a[middle] > key) {
			right = middle - 1;
		}
		else {
			index = middle;
			break;
		}
	}
	if (index == -1) {
		//a[right] < key < a[left]
		index = right;
	}
	return index;
}