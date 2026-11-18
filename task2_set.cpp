#include "set.h"
#include "helpers.h"

char setNames[InstanceCapacity][TextSize];
BitSet sets[InstanceCapacity];
int setCount = 0;

int FindName(const char* wanted)
{
	for (int index = 0; index < setCount; index++)
	{
		if (strcmp(setNames[index], wanted) == 0)
		{
			return index;
		}
	}
	return -1;
}

int TakeName(const char* wanted, bool& created)
{
	created = false;
	int index = FindName(wanted);
	if (index < 0)
	{
		strcpy(setNames[setCount], wanted);
		index = setCount;
		setCount++;
		created = true;
	}
	return index;
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
		printf("usage: set --file file.data --query 'SETADD name value'\n");
		return 1;
	}

	FileRow rows[InstanceCapacity];
	int rowCount = 0;
	InputOutput::ReadFile(file, rows, rowCount);
	static char parts[ValueCapacity][TextSize];
	for (int index = 0; index < rowCount; index++)
	{
		if (rows[index].type != 'E')
		{
			continue;
		}
		int partCount = 0;
		SplitCsv(rows[index].value, parts, partCount);
		bool created = false;
		int slot = TakeName(rows[index].name, created);
		if (created)
		{
			SetOperations::Init(&sets[slot]);
		}
		for (int position = 0; position < partCount; position++)
		{
			SetOperations::Add(&sets[slot], (unsigned char)atoi(parts[position]));
		}
	}

	char tokens[WordCapacity + 2][TextSize];
	SplitWords(query, tokens, WordCapacity + 2);
	char* command = tokens[0];
	char* target = tokens[1];

	static char result[OutputSize];
	result[0] = '\0';
	bool created = false;
	int slot = TakeName(target, created);
	if (created)
	{
		SetOperations::Init(&sets[slot]);
	}
	BitSet* set = &sets[slot];

	if (strcmp(command, "SETADD") == 0)
	{
		SetOperations::Add(set, (unsigned char)atoi(tokens[2]));
	}
	else if (strcmp(command, "SETDEL") == 0)
	{
		SetOperations::Remove(set, (unsigned char)atoi(tokens[2]));
	}
	else if (strcmp(command, "SET_AT") == 0)
	{
		strcpy(result, SetOperations::Contains(set, (unsigned char)atoi(tokens[2])) ? "TRUE" : "FALSE");
	}
	else if (strcmp(command, "SETREAD") == 0)
	{
		SetOperations::Read(set, result);
	}

	rowCount = 0;
	for (int index = 0; index < setCount; index++)
	{
		strcpy(rows[rowCount].name, setNames[index]);
		rows[rowCount].type = 'E';
		SetOperations::Read(&sets[index], rows[rowCount].value);
		rowCount++;
	}
	InputOutput::WriteFile(file, rows, rowCount);

	if (result[0] != '\0')
	{
		printf("-> %s\n", result);
	}
	return 0;
}
