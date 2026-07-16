#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#include <raylib.h>

#include "GetFuncs.h"

#define N 50 // Maximum number for cities
#define M 50 // Maximum number for chromosome
#define T 10 // The number of random chromosomes that are selected (Tournament Size)
#define PM 0.05 // Mutation Rate (Probability of Mutation)
#define Q 2000 // number of generations

// --- GRPHICS ---
#define SCREEN_W 1000
#define SCREEN_H 700
#define DRAW_W 800
#define DRAW_H 500
#define OFFSET_X ((SCREEN_W - DRAW_W) / 2)
#define OFFSET_Y ((SCREEN_H - DRAW_H) / 2)
#define NORM_W 225.0
#define NORM_H 125.0

/*			Description
* Chromosome -> An array representing a complete tour (a permutation of cities)
* Gene -> Each element representing a city in the tour
* Tournament Selection -> A selection method where a small random subset of individuals is chosen and the best among them is selected as a parent
* Crossover -> The combination of the genes of the parent chromosomes
* Mutation -> Introduces small random modifications in a tour while preserving permutation validity
*/

// prototypes :
void citiesNumber();
void getPos();
void distanceCalculation();
int findIndex(int arr[], int n, int target);
void InitChrm(int crnchrm[]);
double EvalFunc(int pths[][N], int j);
void crossover(const int parent1[], const int parent2[], int child[]);
void Mutation(int chrm[]);

// --- GRPHICS ---
void DrawCities(double normCits[][2]);
void DrawPath(int route[], double normCits[][2]);
void NormalizeCities(double normCits[][2]);


int n = 2; // number of cites
int c1, c2; // crossover points ( c2 > c1 )

int totalGenCount = 0;

double cits[N][2]; // coordinates of cities (x , y)

double dist[N][N]; // distance between cities i and j





int main()
{
	// initializing cites variables
	citiesNumber(); // get number of cities from user
	getPos(); // Get x and y coordinates from the user

	InitWindow(SCREEN_W, SCREEN_H, "TSP_Project");
	SetTargetFPS(60);

	// the distances are calculated in this function and stored in the "dist" array
	distanceCalculation();

	int pths[M][N]; // the first index for each path and the second for each node is in path

	// to change seed in each run
	srand(time(NULL));

	// initializing all chromosomes
	for (int j = 0; j < M; j++)
	{
		InitChrm(pths[j]);
	}

	// the total distance traveled between cities is stored here
	double lngths[M];

	// calculate the cost of each trip
	for (int j = 0; j < M; j++)
	{
		lngths[j] = EvalFunc(pths, j);
	}



	int globalBest[N];
	double globalBestLen = 1e15;

	globalBestLen = lngths[0];


	for (int k = 0; k < n; k++)
		globalBest[k] = pths[0][k];

	int gensPerFrame = 50;

	while (!WindowShouldClose())
	{
		for (int gen = 0; gen < gensPerFrame && totalGenCount < Q; gen++, totalGenCount++)
		{

#pragma region Tournament Selection Part

			int best1, best2, worst1, worst2, s, r;
			double bestE1, bestE2, worstE1, worstE2;

			best1 = best2 = worst1 = worst2 = -1;
			bestE1 = bestE2 = 1e15;
			worstE1 = worstE2 = -1.0;

			for (r = 0; r < T; r++)
			{
				s = rand() % M;

				// --- BEST ---
				if (lngths[s] < bestE1)
				{
					bestE2 = bestE1; best2 = best1;
					if (worst1 == best1 || worst1 == best2)
						worst1 = (worst1 + 1) % M;
					bestE1 = lngths[s];
					best1 = s;
				}
				else if (lngths[s] < bestE2 && s != best1)
				{
					bestE2 = lngths[s];
					best2 = s;
				}

				// --- WORST ---
				if (worstE1 < lngths[s])
				{
					worstE2 = worstE1; worst2 = worst1;
					if (worst2 == best1 || worst2 == best2 || worst2 == worst1)
						worst2 = (worst2 + 1) % M;
					worstE1 = lngths[s];
					worst1 = s;
				}
				else if (worstE2 < lngths[s] && s != worst1)
				{
					worstE2 = lngths[s];
					worst2 = s;
				}
			}

#pragma endregion

			if (best1 == best2)
				best2 = (best1 + 1) % M;

			if (worst1 == worst2)
				worst2 = (worst1 + 1) % M;

			// initializing cut points ( c2 > c1 )
			c1 = rand() % (n - 1);
			c2 = c1 + 1 + rand() % (n - c1 - 1);

			// children are stored in this array
			int ofspr[2][N];

			crossover(pths[best1], pths[best2], ofspr[0]);
			crossover(pths[best2], pths[best1], ofspr[1]);

			if ((double)rand() / RAND_MAX < PM)
				Mutation(ofspr[0]);

			if ((double)rand() / RAND_MAX < PM)
				Mutation(ofspr[1]);

			int bestPopIndex = -1;
			double bestPopLen = 1e15;

			for (int i = 0; i < M; i++)
			{
				if (lngths[i] < bestPopLen)
				{
					bestPopLen = lngths[i];
					bestPopIndex = i;
				}
			}

			if (worst1 == bestPopIndex) worst1 = best1;
			if (worst2 == bestPopIndex) worst2 = best2;

			for (int i = 0; i < n; i++)
			{
				pths[worst1][i] = ofspr[0][i];
				pths[worst2][i] = ofspr[1][i];
			}

			lngths[worst1] = EvalFunc(pths, worst1);
			lngths[worst2] = EvalFunc(pths, worst2);

			for (int i = 0; i < M; i++)
			{
				if (lngths[i] < globalBestLen)
				{
					globalBestLen = lngths[i];

					for (int k = 0; k < n; k++)
					{
						globalBest[k] = pths[i][k];
					}
				}
			}

		}

		double normCits[N][2];
		NormalizeCities(normCits);

		BeginDrawing();
		ClearBackground(BLACK);

		DrawCities(normCits);
		DrawPath(globalBest, normCits);

		DrawText(TextFormat("Best Length: %.2f", globalBestLen), 20, 20, 20, GREEN);
		DrawText(TextFormat("Generation: %d / %d", totalGenCount, Q), 20, 45, 20, SKYBLUE);

		if (totalGenCount >= Q)
		{
			gensPerFrame = 0;
			DrawText("FINISHED", 20, 70, 20, YELLOW);
		}



		EndDrawing();
	}

	printf("\nBest tour found:\n");
	for (int i = 0; i < n; i++)
	{
		printf("%d -> ", globalBest[i]);
	}

	printf("%d", globalBest[0]);

	printf("\nTotal length : %.3f\n", globalBestLen);



	_getch();
	return 0;
}


// This function receives the number of cities that the user wants to enter and also pays attention to invalid entries
void citiesNumber()
{
	do
	{
		printf("Please enter the number of cities on the way:(Maximum %d) \n", N);
		scanf_s("%d", &n);

		if (n > N || n < 2)
		{
			printf("Invalid input\n");
		}

	} while (n > N || n < 2);
}

// This function receives the coordinates of each city on the page in the (x, y) format
void getPos()
{
	printf("now enter the position of each city in (x, y) format, then press \"enter\" \n");

	for (int i = 0; i < n; i++)
	{
		printf("city%d:", i + 1);

		double x, y;
		printf("(");
		a = 0;
		frac_a = 0;
		dpoint = false;
		Checkdig();
		x = a + (frac_a / (pow(10, DigCounter(frac_a))));
		cits[i][0] = x;
		printf(", ");
		a = 0;
		frac_a = 0;
		dpoint = false;
		Checkdig();
		y = a + (frac_a / (pow(10, DigCounter(frac_a))));
		cits[i][1] = y;
		printf(")\n");
	}
}

// Calculating the distance between cities
void distanceCalculation()
{
	//Calculate the Euclidean distance
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			dist[i][j] = sqrt(pow(cits[i][0] - cits[j][0], 2) + pow(cits[i][1] - cits[j][1], 2));
		}
	}
}

// linear search through the elements of an array(one-dimensional[n]) to find specific value(target)
int findIndex(int arr[], int n, int target)
{
	for (int i = 0; i < n; i++)
		if (arr[i] == target)
			return i;

	return -1;
}

// This function Initialize a chromosome with a random permutation of cities
// InitChrm stands for initialize chromosome
// crnchrm stands for current chromosome
// ind stands for index
void InitChrm(int crnchrm[])
{
	int allcitis[N], k, p, ind;
	for (k = 0; k < n; allcitis[k] = k, k++);

	for (k = n - 1; k >= 0; k--)
	{
		// get random number between 0 and k then floor that for making index from number
		ind = floor(((double)rand()) * k / RAND_MAX + 0.5);
		crnchrm[k] = allcitis[ind];

		for (p = ind; p < k; p++)
			allcitis[p] = allcitis[p + 1];
	}
}

// EvalFunc stands for Evaluation Function : A Function that computes the total travel distance of a tour, including the return from the last city back to the first
double EvalFunc(int pths[][N], int j)
{
	int k;
	double l = 0.0;

	for (k = 1; k < n; k++)
		l += dist[pths[j][k - 1]][pths[j][k]];

	l += dist[pths[j][n - 1]][pths[j][0]];

	return l;
}

// This function takes two variables of type int array, as parent1 and parent2 and crossover them together
void crossover(
	const int parent1[],
	const int parent2[],
	int child[])
{
	for (int i = 0; i < n; i++)
		child[i] = parent2[i];

	int x, idx;

	for (int i = c1; i <= c2; i++)
	{
		x = parent1[i];
		idx = findIndex(child, n, x);

		child[idx] = child[i];
		child[i] = x;
	}
}

void Mutation(int chrm[])
{
	int a1 = rand() % (n - 1);
	int a2 = a1 + 1 + rand() % (n - a1 - 1);

	while (a1 < a2)
	{
		int temp = chrm[a1];
		chrm[a1] = chrm[a2];
		chrm[a2] = temp;
		a1++;
		a2--;
	}
}

// --- GRAPHICS ---
void DrawCities(double normCits[][2])
{
	for (int i = 0; i < n; i++)
	{
		int x = OFFSET_X + (int)(normCits[i][0] / NORM_W * DRAW_W);
		int y = OFFSET_Y + (int)(normCits[i][1] / NORM_H * DRAW_H);

		DrawCircle(x, y, 6, WHITE);
		DrawCircleLines(x, y, 6, DARKGRAY);

		char label[4];
		sprintf_s(label, "%d", i + 1);

		DrawText(label, x + 8, y - 8, 16, PURPLE);
	}
}

void DrawPath(int route[], double normCits[][2])
{
	for (int i = 0; i < n - 1; i++)
	{
		int x1 = OFFSET_X + (int)(normCits[route[i]][0] / NORM_W * DRAW_W);
		int y1 = OFFSET_Y + (int)(normCits[route[i]][1] / NORM_H * DRAW_H);

		int x2 = OFFSET_X + (int)(normCits[route[i + 1]][0] / NORM_W * DRAW_W);
		int y2 = OFFSET_Y + (int)(normCits[route[i + 1]][1] / NORM_H * DRAW_H);

		DrawLine(x1, y1, x2, y2, RED);
	}


	int x1 = OFFSET_X + (int)(normCits[route[n - 1]][0] / NORM_W * DRAW_W);
	int y1 = OFFSET_Y + (int)(normCits[route[n - 1]][1] / NORM_H * DRAW_H);

	int x2 = OFFSET_X + (int)(normCits[route[0]][0] / NORM_W * DRAW_W);
	int y2 = OFFSET_Y + (int)(normCits[route[0]][1] / NORM_H * DRAW_H);

	DrawLine(x1, y1, x2, y2, RED);
}


void NormalizeCities(double normCits[][2])
{
	double minX = cits[0][0], maxX = cits[0][0];
	double minY = cits[0][1], maxY = cits[0][1];

	for (int i = 1; i < n; i++)
	{
		if (cits[i][0] < minX) minX = cits[i][0];
		if (cits[i][0] > maxX) maxX = cits[i][0];
		if (cits[i][1] < minY) minY = cits[i][1];
		if (cits[i][1] > maxY) maxY = cits[i][1];
	}

	double dx = maxX - minX;
	double dy = maxY - minY;

	if (dx == 0) dx = 1;
	if (dy == 0) dy = 1;

	for (int i = 0; i < n; i++)
	{
		normCits[i][0] = ((cits[i][0] - minX) / dx) * NORM_W;
		normCits[i][1] = ((cits[i][1] - minY) / dy) * NORM_H;
	}
}