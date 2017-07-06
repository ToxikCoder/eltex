typedef unsigned int UINT;
typedef unsigned long ULONG;

#define DBSIZE 5
#define LASTNAMELENGTH 30

struct WORKER
{
	char* lastName;
	short departmentNumber;
	UINT birthYear;
	ULONG wage;
};

int init();
void release();
void bubbleSort(struct WORKER** db, int size);
void PrintDBContent();

