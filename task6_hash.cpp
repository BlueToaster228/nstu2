#include "lab1/structures.h"
#include "helpers.h"
#include <ctime>

const int TableSize = 103;

struct HashTable
{
	int count;
	int method;
	int keys[TableSize];
	bool used[TableSize];
	bool removed[TableSize];
};

HashTable table;

int Fold(int key)
{
	long long rest = key;
	if (rest < 0)
	{
		rest = -rest;
	}
	int sum = 0;
	while (rest > 0)
	{
		sum += (int)(rest % 1000);
		rest /= 1000;
	}
	return sum;
}

int FoldIndex(int key)
{
	return Fold(key) % TableSize;
}

int DoubleStep(int key)
{
	return 1 + (key % (TableSize - 1));
}

void InitTable(HashTable* table)
{
	table->count = 0;
	table->method = 1;
	for (int index = 0; index < TableSize; index++)
	{
		table->keys[index] = 0;
		table->used[index] = false;
		table->removed[index] = false;
	}
}

int StepFor(HashTable* table, int key)
{
	if (table->method == 2)
	{
		return DoubleStep(key);
	}
	return 1;
}

bool AddKey(HashTable* table, int key, int& probes)
{
	probes = 0;
	if (table->count >= TableSize)
	{
		return false;
	}
	int step = StepFor(table, key);
	int index = FoldIndex(key);
	while (table->used[index] && !table->removed[index] && table->keys[index] != key)
	{
		probes++;
		index = (index + step) % TableSize;
	}
	if (table->used[index] && !table->removed[index] && table->keys[index] == key)
	{
		return false;
	}
	table->keys[index] = key;
	table->used[index] = true;
	table->removed[index] = false;
	table->count++;
	probes++;
	return true;
}

bool FindKey(HashTable* table, int key, int& probes)
{
	probes = 0;
	int step = StepFor(table, key);
	int index = FoldIndex(key);
	for (int attempt = 0; attempt < TableSize; attempt++)
	{
		if (!table->used[index])
		{
			return false;
		}
		if (!table->removed[index] && table->keys[index] == key)
		{
			probes++;
			return true;
		}
		probes++;
		index = (index + step) % TableSize;
	}
	return false;
}

bool DeleteKey(HashTable* table, int key)
{
	int step = StepFor(table, key);
	int index = FoldIndex(key);
	for (int attempt = 0; attempt < TableSize; attempt++)
	{
		if (!table->used[index])
		{
			return false;
		}
		if (!table->removed[index] && table->keys[index] == key)
		{
			table->removed[index] = true;
			table->count--;
			return true;
		}
		index = (index + step) % TableSize;
	}
	return false;
}

void ReadTable(HashTable* table, char* output)
{
	output[0] = '\0';
	bool first = true;
	for (int index = 0; index < TableSize; index++)
	{
		if (table->used[index] && !table->removed[index])
		{
			if (!first)
			{
				strcat(output, ",");
			}
			char piece[TextSize];
			sprintf(piece, "%d", table->keys[index]);
			strcat(output, piece);
			first = false;
		}
	}
}

void Analyze()
{
	unsigned int seed = 12345;
	int generated[TableSize];
	int generatedCount = 0;
	for (int probe = 0; probe < TableSize; probe++)
	{
		bool duplicate = false;
		do
		{
			seed = seed * 1103515245u + 12345u;
			generated[generatedCount] = (int)(seed % 1000000000u);
			duplicate = false;
			for (int index = 0; index < generatedCount; index++)
			{
				if (generated[index] == generated[generatedCount])
				{
					duplicate = true;
				}
			}
		}
		while (duplicate);
		generatedCount++;
	}

	printf("load | linear add probes | linear find avg | double add probes | double find avg\n");
	double loads[4] = {0.3, 0.5, 0.7, 0.9};
	for (int loadIndex = 0; loadIndex < 4; loadIndex++)
	{
		int amount = (int)(TableSize * loads[loadIndex]);
		long long linearProbes = 0;
		long long linearFindProbes = 0;
		long long doubleProbes = 0;
		long long doubleFindProbes = 0;

		InitTable(&table);
		table.method = 1;
		for (int index = 0; index < amount; index++)
		{
			int probes = 0;
			AddKey(&table, generated[index], probes);
			linearProbes += probes;
		}
		for (int index = 0; index < amount; index++)
		{
			int probes = 0;
			FindKey(&table, generated[index], probes);
			linearFindProbes += probes;
		}

		InitTable(&table);
		table.method = 2;
		for (int index = 0; index < amount; index++)
		{
			int probes = 0;
			AddKey(&table, generated[index], probes);
			doubleProbes += probes;
		}
		for (int index = 0; index < amount; index++)
		{
			int probes = 0;
			FindKey(&table, generated[index], probes);
			doubleFindProbes += probes;
		}

		printf("%.1f | %lld | %.2f avg | %lld | %.2f avg\n",
			loads[loadIndex], linearProbes,
			amount > 0 ? (double)linearFindProbes / amount : 0.0,
			doubleProbes,
			amount > 0 ? (double)doubleFindProbes / amount : 0.0);
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
		printf("usage: hashtab --file file.data --query 'LADD 523456795' | 'DADD 523456795' | 'ANALYZE'\n");
		return 1;
	}

	FileRow rows[InstanceCapacity];
	int rowCount = 0;
	InputOutput::ReadFile(file, rows, rowCount);
	static char parts[ValueCapacity][TextSize];
	InitTable(&table);
	for (int index = 0; index < rowCount; index++)
	{
		if (rows[index].type == 'N')
		{
			table.method = atoi(rows[index].value);
		}
	}
	for (int index = 0; index < rowCount; index++)
	{
		int partCount = 0;
		SplitCsv(rows[index].value, parts, partCount);
		if (rows[index].type == 'T')
		{
			for (int position = 0; position < partCount; position++)
			{
				int probes = 0;
				AddKey(&table, atoi(parts[position]), probes);
			}
		}
	}

	char tokens[WordCapacity + 2][TextSize];
	SplitWords(query, tokens, WordCapacity + 2);
	char* command = tokens[0];

	static char result[OutputSize];
	result[0] = '\0';
	int probes = 0;

	if (strcmp(command, "ANALYZE") == 0)
	{
		Analyze();
		return 0;
	}
	if (strcmp(command, "FOLD") == 0)
	{
		sprintf(result, "%d", Fold(atoi(tokens[1])));
		printf("-> %s\n", result);
		return 0;
	}
	char methodLetter = command[0];
	const char* keyword = command + 1;
	if (methodLetter == 'L')
	{
		table.method = 1;
	}
	else if (methodLetter == 'D')
	{
		table.method = 2;
	}
	else
	{
		printf("usage: hashtab --file file.data --query 'LADD key' | 'DADD key' | 'LAT key' | 'DAT key' | 'LDEL key' | 'DDEL key' | 'LREAD' | 'DREAD' | 'FOLD key' | 'ANALYZE'\n");
		return 1;
	}
	int key = atoi(tokens[1]);

	if (strcmp(keyword, "ADD") == 0)
	{
		bool added = AddKey(&table, key, probes);
		strcpy(result, added ? "TRUE" : "FALSE");
	}
	else if (strcmp(keyword, "AT") == 0)
	{
		bool found = FindKey(&table, key, probes);
		strcpy(result, found ? "TRUE" : "FALSE");
	}
	else if (strcmp(keyword, "DEL") == 0)
	{
		bool deleted = DeleteKey(&table, key);
		strcpy(result, deleted ? "TRUE" : "FALSE");
	}
	else if (strcmp(keyword, "READ") == 0)
	{
		ReadTable(&table, result);
	}
	else
	{
		printf("usage: hashtab --file file.data --query 'LADD key' | 'DADD key' | 'LAT key' | 'DAT key' | 'LDEL key' | 'DDEL key' | 'LREAD' | 'DREAD' | 'FOLD key' | 'ANALYZE'\n");
		return 1;
	}

	rowCount = 0;
	strcpy(rows[rowCount].name, "table");
	rows[rowCount].type = 'T';
	ReadTable(&table, rows[rowCount].value);
	rowCount++;
	char methodText[TextSize];
	sprintf(methodText, "%d", table.method);
	strcpy(rows[rowCount].name, "method");
	rows[rowCount].type = 'N';
	strcpy(rows[rowCount].value, methodText);
	rowCount++;
	InputOutput::WriteFile(file, rows, rowCount);

	if (result[0] != '\0')
	{
		printf("-> %s\n", result);
	}
	return 0;
}
