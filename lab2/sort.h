enum SORT_ORDER {
	ASCENDING,
	DESCENDING
};

#define MAXLEN 60

/*
 * reads string from console
 * @string char* - scanned string
 * @maxlen int - maximum length of string
 * @return int - length of scanned string
 */
int inp_str(char* string, int maxlen);

/*
 * print string to console
 * @string char* - string to print
 * @length int - length of string to print
 * @number int - number of string to print
 * @return void
 */
void out_str(char* string, int length, int number);

/*
 * sort string in defined order
 * @string char* - string to sort
 * @size int - size of array to sort
 * @order enum SORT_ORDER - sorting order(ascending and descending)
 * @return int - number of shuffles
 */
int sort(char* string, int size, enum SORT_ORDER order);

