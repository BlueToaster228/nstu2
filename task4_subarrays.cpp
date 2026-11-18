#include "lab1/structures.h"
#include "helpers.h"

const int SubArrayMax = 5000;
const int SubArrayText = 256;

char printed[SubArrayMax][SubArrayText];
int printedCount = 0;

bool AlreadyPrinted(const char* text)
{
	for (int index = 0; index < printedCount; index++)
	{
		if (strcmp(printed[index], text) == 0)
		{
			return true;
		}
	}
	return false;
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
	if (file[0] == '\0')
	{
		printf("usage: subarrays --file file.data --query 'SUBS'\n");
		return 1;
	}

	FileRow rows[InstanceCapacity];
	int rowCount = 0;
	InputOutput::ReadFile(file, rows, rowCount);
	static char parts[ValueCapacity][TextSize];
	Array array;
	ArrayOperations::Init(&array);
	for (int index = 0; index < rowCount; index++)
	{
		if (rows[index].type != 'M')
		{
			continue;
		}
		int partCount = 0;
		SplitCsv(rows[index].value, parts, partCount);
		for (int position = 0; position < partCount; position++)
		{
			ArrayOperations::InsertEnd(&array, atoi(parts[position]));
		}
	}

	printf("-> [");
	bool first = true;
	if (!AlreadyPrinted("{}"))
	{
		strcpy(printed[printedCount], "{}");
		printedCount++;
		printf("{}");
		first = false;
	}
	for (int start = 0; start < array.count; start++)
	{
		for (int end = start; end < array.count; end++)
		{
			char piece[SubArrayText];
			strcpy(piece, "{");
			for (int position = start; position <= end; position++)
			{
				char number[TextSize];
				sprintf(number, "%d", array.elements[position]);
				if (position > start)
				{
					strcat(piece, ",");
				}
				strcat(piece, number);
			}
			strcat(piece, "}");
			if (AlreadyPrinted(piece))
			{
				continue;
			}
			strcpy(printed[printedCount], piece);
			printedCount++;
			if (!first)
			{
				printf(", ");
			}
			printf("%s", piece);
			first = false;
		}
	}
	printf("]\n");
	return 0;
}
