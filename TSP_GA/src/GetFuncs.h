#pragma once

// Variables :

int dig = 48;
bool dpoint = false;

int a = 0;
int frac_a = 0;

int netFrac = 0;


void Checkdig()
{
	dig = _getch();

	while (47 < dig && dig < 58)
	{
		a = a * 10 + dig - 48;

		printf("%d", (dig - 48));

		dig = _getch();
	}


	if (dig == '.' && !dpoint)
	{
		printf(".");

		dig = _getch();

		while (47 < dig && dig < 58)
		{
			frac_a = frac_a * 10 + dig - 48;

			printf("%d", (dig - 48));

			dig = _getch();
		}

		dpoint = true;
	}
}


int DigCounter(int number)
{
	int k = 0;

	while (number % 10 != 0)
	{
		k++;
		number /= 10;
	}

	return k;
}
