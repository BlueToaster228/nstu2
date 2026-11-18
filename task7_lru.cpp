#include "lab1/structures.h"
#include "helpers.h"

struct LruNode
{
	int key;
	int value;
	LruNode* prev;
	LruNode* next;
};

struct LruCache
{
	int capacity;
	int count;
	LruNode* head;
	LruNode* tail;
};

LruNode nodes[256];
int nodeCount = 0;

LruNode* TakeNode()
{
	nodes[nodeCount].key = 0;
	nodes[nodeCount].value = 0;
	nodes[nodeCount].prev = nullptr;
	nodes[nodeCount].next = nullptr;
	nodeCount++;
	return &nodes[nodeCount - 1];
}

void InitCache(LruCache* cache, int capacity)
{
	cache->capacity = capacity;
	cache->count = 0;
	cache->head = nullptr;
	cache->tail = nullptr;
}

void MoveToHead(LruCache* cache, LruNode* node)
{
	if (cache->head == node)
	{
		return;
	}
	if (node->prev != nullptr)
	{
		node->prev->next = node->next;
	}
	if (node->next != nullptr)
	{
		node->next->prev = node->prev;
	}
	if (cache->tail == node)
	{
		cache->tail = node->prev;
	}
	node->prev = nullptr;
	node->next = cache->head;
	if (cache->head != nullptr)
	{
		cache->head->prev = node;
	}
	cache->head = node;
	if (cache->tail == nullptr)
	{
		cache->tail = node;
	}
}

LruNode* FindNode(LruCache* cache, int key)
{
	LruNode* current = cache->head;
	while (current != nullptr)
	{
		if (current->key == key)
		{
			return current;
		}
		current = current->next;
	}
	return nullptr;
}

void PutValue(LruCache* cache, int key, int value)
{
	LruNode* existing = FindNode(cache, key);
	if (existing != nullptr)
	{
		existing->value = value;
		MoveToHead(cache, existing);
		return;
	}
	if (cache->capacity <= 0)
	{
		return;
	}
	if (cache->count == cache->capacity)
	{
		LruNode* victim = cache->tail;
		if (victim->prev != nullptr)
		{
			victim->prev->next = nullptr;
		}
		cache->tail = victim->prev;
		if (cache->head == victim)
		{
			cache->head = nullptr;
		}
		cache->count--;
	}
	LruNode* node = TakeNode();
	node->key = key;
	node->value = value;
	node->next = cache->head;
	if (cache->head != nullptr)
	{
		cache->head->prev = node;
	}
	cache->head = node;
	if (cache->tail == nullptr)
	{
		cache->tail = node;
	}
	cache->count++;
}

int GetValue(LruCache* cache, int key)
{
	LruNode* node = FindNode(cache, key);
	if (node == nullptr)
	{
		return -1;
	}
	MoveToHead(cache, node);
	return node->value;
}

void ReadCache(LruCache* cache, char* output)
{
	output[0] = '\0';
	LruNode* current = cache->head;
	while (current != nullptr)
	{
		if (current != cache->head)
		{
			strcat(output, ",");
		}
		char piece[TextSize];
		sprintf(piece, "%d:%d", current->key, current->value);
		strcat(output, piece);
		current = current->next;
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
		printf("usage: lru --file file.data --query 'CAP 2 SET 1 2 GET 1'\n");
		return 1;
	}

	FileRow rows[InstanceCapacity];
	int rowCount = 0;
	InputOutput::ReadFile(file, rows, rowCount);
	static char parts[ValueCapacity][TextSize];
	LruCache cache;
	InitCache(&cache, 2);
	for (int index = 0; index < rowCount; index++)
	{
		if (rows[index].type != 'R')
		{
			continue;
		}
		int partCount = 0;
		SplitCsv(rows[index].value, parts, partCount);
		for (int position = partCount - 1; position >= 0; position--)
		{
			char* separator = strchr(parts[position], ':');
			if (separator == nullptr)
			{
				continue;
			}
			*separator = '\0';
			PutValue(&cache, atoi(parts[position]), atoi(separator + 1));
		}
	}

	char tokens[WordCapacity * 8][TextSize];
	SplitWords(query, tokens, WordCapacity * 8);
	int tokenCount = 0;
	while (tokens[tokenCount][0] != '\0')
	{
		tokenCount++;
	}

	char result[4096];
	result[0] = '\0';
	int position = 0;
	while (position < tokenCount)
	{
		char* command = tokens[position];
		if (strcmp(command, "CAP") == 0 && position + 1 < tokenCount)
		{
			cache.capacity = atoi(tokens[position + 1]);
			if (cache.count > cache.capacity && cache.capacity > 0)
			{
				while (cache.count > cache.capacity && cache.tail != nullptr)
				{
					LruNode* victim = cache.tail;
					cache.tail = victim->prev;
					if (cache.tail != nullptr)
					{
						cache.tail->next = nullptr;
					}
					if (cache.head == victim)
					{
						cache.head = nullptr;
					}
					cache.count--;
				}
			}
			position += 2;
		}
		else if (strcmp(command, "SET") == 0 && position + 2 < tokenCount)
		{
			PutValue(&cache, atoi(tokens[position + 1]), atoi(tokens[position + 2]));
			position += 3;
		}
		else if (strcmp(command, "GET") == 0 && position + 1 < tokenCount)
		{
			char piece[TextSize];
			sprintf(piece, "%d ", GetValue(&cache, atoi(tokens[position + 1])));
			strcat(result, piece);
			position += 2;
		}
		else
		{
			position++;
		}
	}

	rowCount = 0;
	strcpy(rows[rowCount].name, "cache");
	rows[rowCount].type = 'R';
	ReadCache(&cache, rows[rowCount].value);
	rowCount++;
	InputOutput::WriteFile(file, rows, rowCount);

	if (result[0] != '\0')
	{
		printf("-> %s\n", result);
	}
	return 0;
}
