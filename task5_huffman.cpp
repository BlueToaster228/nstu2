#include "lab1/structures.h"
#include "helpers.h"

const int SymbolTextSize = 257;
const int CodeTextSize = 1024;
const int NodeCapacity = 512;

struct HuffmanNode
{
	int weight;
	char symbols[SymbolTextSize];
	HuffmanNode* left;
	HuffmanNode* right;
};

HuffmanNode nodes[NodeCapacity];
bool active[NodeCapacity];
int nodeCount = 0;

int frequency[256];
char codes[256][CodeTextSize];

HuffmanNode* TakeNode()
{
	nodes[nodeCount].weight = 0;
	nodes[nodeCount].symbols[0] = '\0';
	nodes[nodeCount].left = nullptr;
	nodes[nodeCount].right = nullptr;
	active[nodeCount] = true;
	nodeCount++;
	return &nodes[nodeCount - 1];
}

int FindMinimum()
{
	int best = -1;
	for (int index = 0; index < nodeCount; index++)
	{
		if (!active[index])
		{
			continue;
		}
		if (best < 0 || nodes[index].weight < nodes[best].weight)
		{
			best = index;
		}
	}
	return best;
}

void AssignCodes(HuffmanNode* node, char* path, int depth)
{
	if (node->left == nullptr && node->right == nullptr)
	{
		path[depth] = '\0';
		unsigned char symbol = (unsigned char)node->symbols[0];
		if (depth == 0)
		{
			strcpy(codes[symbol], "0");
		}
		else
		{
			strcpy(codes[symbol], path);
		}
		return;
	}
	path[depth] = '0';
	AssignCodes(node->left, path, depth + 1);
	path[depth] = '1';
	AssignCodes(node->right, path, depth + 1);
}

void BuildTree()
{
	for (int symbol = 0; symbol < 256; symbol++)
	{
		if (frequency[symbol] > 0)
		{
			HuffmanNode* leaf = TakeNode();
			leaf->weight = frequency[symbol];
			leaf->symbols[0] = (char)symbol;
			leaf->symbols[1] = '\0';
		}
	}
	int distinct = nodeCount;
	if (distinct == 0)
	{
		return;
	}
	while (true)
	{
		int first = FindMinimum();
		if (first < 0)
		{
			break;
		}
		active[first] = false;
		int second = FindMinimum();
		if (second < 0)
		{
			active[first] = true;
			break;
		}
		active[second] = false;
		HuffmanNode* parent = TakeNode();
		parent->weight = nodes[first].weight + nodes[second].weight;
		strcpy(parent->symbols, nodes[first].symbols);
		strcat(parent->symbols, nodes[second].symbols);
		parent->left = &nodes[first];
		parent->right = &nodes[second];
		active[nodeCount - 1] = true;
	}
	char path[CodeTextSize];
	int root = -1;
	for (int index = 0; index < nodeCount; index++)
	{
		if (active[index])
		{
			root = index;
		}
	}
	AssignCodes(&nodes[root], path, 0);
}

void BuildTable(char* output)
{
	output[0] = '\0';
	for (int symbol = 0; symbol < 256; symbol++)
	{
		if (frequency[symbol] > 0)
		{
			if (output[0] != '\0')
			{
				strcat(output, ",");
			}
			char piece[CodeTextSize];
			sprintf(piece, "%c:%d:%s", (char)symbol, frequency[symbol], codes[symbol]);
			strcat(output, piece);
		}
	}
}

void EncodeText(const char* text, char* output)
{
	output[0] = '\0';
	for (unsigned int position = 0; position < strlen(text); position++)
	{
		strcat(output, codes[(unsigned char)text[position]]);
	}
}

bool DecodeCode(const char* code, char* output)
{
	output[0] = '\0';
	int root = -1;
	for (int index = 0; index < nodeCount; index++)
	{
		if (active[index])
		{
			root = index;
		}
	}
	if (root < 0)
	{
		return false;
	}
	HuffmanNode* current = &nodes[root];
	for (unsigned int position = 0; position < strlen(code); position++)
	{
		if (current->left == nullptr && current->right == nullptr)
		{
			strcat(output, current->symbols);
			current = &nodes[root];
		}
		if (code[position] == '0')
		{
			current = current->left;
		}
		else
		{
			current = current->right;
		}
		if (current == nullptr)
		{
			return false;
		}
	}
	if (current->left == nullptr && current->right == nullptr)
	{
		strcat(output, current->symbols);
		return true;
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
	if (file[0] == '\0' || query[0] == '\0')
	{
		printf("usage: huffman --file file.data --query 'HENC abracadabra'\n");
		return 1;
	}

	FileRow rows[InstanceCapacity];
	int rowCount = 0;
	InputOutput::ReadFile(file, rows, rowCount);
	char storedText[4096];
	char storedCode[CodeTextSize];
	char storedTable[CodeTextSize];
	storedText[0] = '\0';
	storedCode[0] = '\0';
	storedTable[0] = '\0';
	for (int index = 0; index < rowCount; index++)
	{
		if (rows[index].type == 'X')
		{
			strcpy(storedText, rows[index].value);
		}
		else if (rows[index].type == 'C')
		{
			strcpy(storedCode, rows[index].value);
		}
		else if (rows[index].type == 'B')
		{
			strcpy(storedTable, rows[index].value);
		}
	}

	char tokens[WordCapacity + 8][TextSize];
	SplitWords(query, tokens, WordCapacity + 8);
	int tokenCount = 0;
	while (tokens[tokenCount][0] != '\0')
	{
		tokenCount++;
	}

	static char result[OutputSize];
	result[0] = '\0';

	if (strcmp(tokens[0], "HENC") == 0)
	{
		char text[4096];
		text[0] = '\0';
		for (int index = 1; index < tokenCount; index++)
		{
			if (index > 1)
			{
				strcat(text, " ");
			}
			strcat(text, tokens[index]);
		}
		if (text[0] == '\0')
		{
			strcpy(text, storedText);
		}
		for (int index = 0; index < 256; index++)
		{
			frequency[index] = 0;
		}
		nodeCount = 0;
		for (unsigned int position = 0; position < strlen(text); position++)
		{
			frequency[(unsigned char)text[position]]++;
		}
		BuildTree();
		char table[CodeTextSize];
		BuildTable(table);
		char code[CodeTextSize];
		EncodeText(text, code);
		printf("-> %s\n", code);
		printf("table: %s\n", table);
		strcpy(storedText, text);
		strcpy(storedCode, code);
		strcpy(storedTable, table);
	}
	else if (strcmp(tokens[0], "HDEC") == 0)
	{
		for (int index = 0; index < 256; index++)
		{
			frequency[index] = 0;
		}
		nodeCount = 0;
		const char* piece = storedTable;
		while (*piece != '\0')
		{
			char pair[CodeTextSize];
			int length = 0;
			while (*piece != ',' && *piece != '\0')
			{
				pair[length] = *piece;
				length++;
				piece++;
			}
			pair[length] = '\0';
			if (*piece == ',')
			{
				piece++;
			}
			char* separator = strchr(pair, ':');
			if (separator == nullptr)
			{
				continue;
			}
			*separator = '\0';
			char* second = strchr(separator + 1, ':');
			if (second != nullptr)
			{
				*second = '\0';
			}
			frequency[(unsigned char)pair[0]] = atoi(separator + 1);
		}
		BuildTree();
		char decoded[4096];
		if (!DecodeCode(storedCode, decoded))
		{
			printf("-> ERROR\n");
			return 0;
		}
		strcpy(result, decoded);
		printf("-> %s\n", result);
		return 0;
	}
	else
	{
		printf("usage: huffman --file file.data --query 'HENC text' | 'HDEC'\n");
		return 1;
	}

	rowCount = 0;
	strcpy(rows[rowCount].name, "text");
	rows[rowCount].type = 'X';
	strcpy(rows[rowCount].value, storedText);
	rowCount++;
	strcpy(rows[rowCount].name, "code");
	rows[rowCount].type = 'C';
	strcpy(rows[rowCount].value, storedCode);
	rowCount++;
	strcpy(rows[rowCount].name, "table");
	rows[rowCount].type = 'B';
	strcpy(rows[rowCount].value, storedTable);
	rowCount++;
	InputOutput::WriteFile(file, rows, rowCount);
	return 0;
}
