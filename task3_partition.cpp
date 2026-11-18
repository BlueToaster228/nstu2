#include "set.h"
#include "helpers.h"

int elements[256];
int elementCount = 0;
bool used[256];
int buckets[256][256];
int bucketSizes[256];
int bucketCount = 0;
bool solutionFound = false;

void FillBucket(int bucketIndex, int currentSum, int startIndex, int target)
{
	if (solutionFound)
	{
		return;
	}
	if (currentSum == target)
	{
		if (bucketIndex + 1 == bucketCount)
		{
			solutionFound = true;
			return;
		}
		FillBucket(bucketIndex + 1, 0, 0, target);
		return;
	}
	for (int index = startIndex; index < elementCount && !solutionFound; index++)
	{
		if (used[index])
		{
			continue;
		}
		if (currentSum + elements[index] > target)
		{
			continue;
		}
		used[index] = true;
		buckets[bucketIndex][bucketSizes[bucketIndex]] = elements[index];
		bucketSizes[bucketIndex]++;
		FillBucket(bucketIndex, currentSum + elements[index], index + 1, target);
		if (!solutionFound)
		{
			bucketSizes[bucketIndex]--;
			used[index] = false;
		}
	}
}

int main(int argc, char* argv[])
{
	char file[TextSize];
	char query[4096];
	file[0] = '\0';
	query[0] = '\0';
	for (int index = 1; index < argc; index++)
	{
		if (strcmp(argv[index], "--file") == 0 && index + 1 < argc)
		{
			strcpy(file, argv[index + 1]);
			index++;
		}
		else if (strcmp(argv[index], "--query") == 0 && index + 1 < argc)
		{
			strcpy(query, argv[index + 1]);
			index++;
		}
	}
	if (file[0] == '\0' || query[0] == '\0')
	{
		printf("usage: split --file file.data --query 'SPLIT 15'\n");
		return 1;
	}

	FileRow rows[InstanceCapacity];
	int rowCount = 0;
	InputOutput::ReadFile(file, rows, rowCount);
	static char parts[ValueCapacity][TextSize];
	BitSet set;
	SetOperations::Init(&set);
	for (int index = 0; index < rowCount; index++)
	{
		if (rows[index].type != 'E')
		{
			continue;
		}
		int partCount = 0;
		SplitCsv(rows[index].value, parts, partCount);
		for (int position = 0; position < partCount; position++)
		{
			SetOperations::Add(&set, (unsigned char)atoi(parts[position]));
		}
	}

	char tokens[WordCapacity][TextSize];
	SplitWords(query, tokens, WordCapacity);
	if (strcmp(tokens[0], "SPLIT") != 0)
	{
		printf("usage: split --file file.data --query 'SPLIT 15'\n");
		return 1;
	}
	int target = atoi(tokens[1]);

	for (int value = 0; value < 256; value++)
	{
		if (SetOperations::Contains(&set, (unsigned char)value))
		{
			elements[elementCount] = value;
			elementCount++;
		}
	}

	long long total = 0;
	for (int index = 0; index < elementCount; index++)
	{
		total += elements[index];
	}
	if (target <= 0 || total % target != 0)
	{
		printf("-> ERROR\n");
		return 0;
	}
	bucketCount = (int)(total / target);
	if (bucketCount == 0 || bucketCount > elementCount)
	{
		printf("-> ERROR\n");
		return 0;
	}
	for (int index = 0; index < bucketCount; index++)
	{
		bucketSizes[index] = 0;
	}
	for (int index = 0; index < elementCount; index++)
	{
		used[index] = false;
	}
	solutionFound = false;
	FillBucket(0, 0, 0, target);
	if (!solutionFound)
	{
		printf("-> ERROR\n");
		return 0;
	}

	printf("-> ");
	for (int bucket = 0; bucket < bucketCount; bucket++)
	{
		printf("{");
		for (int position = 0; position < bucketSizes[bucket]; position++)
		{
			if (position > 0)
			{
				printf(",");
			}
			printf("%d", buckets[bucket][position]);
		}
		printf("} ");
	}
	printf("\n");
	return 0;
}
