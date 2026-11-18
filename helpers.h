#pragma once
#include <cstdio>
#include <cstring>
#include <cstdlib>

static void SplitCsv(const char* text, char parts[][TextSize], int& count)
{
	count = 0;
	int length = 0;
	parts[0][0] = '\0';
	int size = (int)strlen(text);
	for (int position = 0; position <= size; position++)
	{
		if (text[position] == ',' || text[position] == '\0')
		{
			if (length > 0 && count < ValueCapacity)
			{
				parts[count][length] = '\0';
				count++;
			}
			if (count < ValueCapacity)
			{
				parts[count][0] = '\0';
			}
			length = 0;
		}
		else if (length < TextSize - 1)
		{
			parts[count][length] = text[position];
			length++;
		}
	}
}

static void SplitWords(const char* text, char words[][TextSize], int limit)
{
	int count = 0;
	int length = 0;
	words[0][0] = '\0';
	int size = (int)strlen(text);
	for (int position = 0; position <= size; position++)
	{
		if (text[position] == ' ' || text[position] == '\t' || text[position] == '\0')
		{
			if (length > 0 && count < limit)
			{
				words[count][length] = '\0';
				count++;
			}
			if (count < limit)
			{
				words[count][0] = '\0';
			}
			length = 0;
		}
		else if (length < TextSize - 1)
		{
			words[count][length] = text[position];
			length++;
		}
	}
}
