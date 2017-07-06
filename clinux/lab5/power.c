//
// Created by user on 13.11.16.
//

long power(short num, unsigned int power)
{
	if(power == 0) return 1;
	long result = num;
	for(unsigned int i = 1; i < power; ++i)
	{
		result *= num;
	}
	return result;
}
