#include "lab1/structures.h"
#include "helpers.h"

int main(int argc, char* argv[])
{
	char query[4096];
	query[0] = '\0';
	for (int index = 1; index < argc; index++)
	{
		if (strcmp(argv[index], "--query") == 0 && index + 1 < argc)
		{
			strcpy(query, argv[index + 1]);
			index++;
		}
	}
	if (query[0] == '\0')
	{
		printf("usage: baseball --query '5 2 C D +'\n");
		return 1;
	}

	char tokens[WordCapacity + 8][TextSize];
	SplitWords(query, tokens, WordCapacity + 8);
	int tokenCount = 0;
	while (tokens[tokenCount][0] != '\0')
	{
		tokenCount++;
	}

	Stack stack;
	StackOperations::Init(&stack);
	long long total = 0;
	bool error = false;

	for (int index = 0; index < tokenCount && !error; index++)
	{
		char* token = tokens[index];
		if (strcmp(token, "+") == 0)
		{
			int first = 0;
			int second = 0;
			if (!StackOperations::Pop(&stack, second) || !StackOperations::Pop(&stack, first))
			{
				error = true;
				break;
			}
			StackOperations::Push(&stack, first);
			StackOperations::Push(&stack, second);
			StackOperations::Push(&stack, first + second);
		}
		else if (strcmp(token, "D") == 0)
		{
			int top = 0;
			if (!StackOperations::Pop(&stack, top))
			{
				error = true;
				break;
			}
			StackOperations::Push(&stack, top);
			StackOperations::Push(&stack, top * 2);
		}
		else if (strcmp(token, "C") == 0)
		{
			int removed = 0;
			if (!StackOperations::Pop(&stack, removed))
			{
				error = true;
				break;
			}
		}
		else
		{
			StackOperations::Push(&stack, atoi(token));
		}
	}

	if (error)
	{
		printf("-> ERROR\n");
		return 0;
	}
	int value = 0;
	while (StackOperations::Pop(&stack, value))
	{
		total += value;
	}
	printf("-> %lld\n", total);
	return 0;
}
