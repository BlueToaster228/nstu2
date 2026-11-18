#include "set.h"

int main()
{
	BitSet set;
	SetOperations::Init(&set);
	SetOperations::Add(&set, 0);
	SetOperations::Add(&set, 5);
	SetOperations::Add(&set, 100);
	SetOperations::Add(&set, 255);
	SetOperations::Add(&set, 5);

	char text[OutputSize];
	SetOperations::Read(&set, text);
	printf("read:    -> %s\n", text);
	printf("count:   -> %d\n", SetOperations::Count(&set));
	printf("has 5:   -> %s\n", SetOperations::Contains(&set, 5) ? "TRUE" : "FALSE");
	printf("has 7:   -> %s\n", SetOperations::Contains(&set, 7) ? "TRUE" : "FALSE");
	printf("has 0:   -> %s\n", SetOperations::Contains(&set, 0) ? "TRUE" : "FALSE");
	printf("has 255: -> %s\n", SetOperations::Contains(&set, 255) ? "TRUE" : "FALSE");

	SetOperations::Remove(&set, 100);
	SetOperations::Remove(&set, 0);
	printf("has 100 after remove: -> %s\n", SetOperations::Contains(&set, 100) ? "TRUE" : "FALSE");
	printf("has 0 after remove:   -> %s\n", SetOperations::Contains(&set, 0) ? "TRUE" : "FALSE");
	SetOperations::Read(&set, text);
	printf("read:    -> %s\n", text);
	printf("count:   -> %d\n", SetOperations::Count(&set));

	unsigned char found = 0;
	if (SetOperations::NextElement(&set, 6, found))
	{
		printf("next after 6: -> %d\n", found);
	}
	else
	{
		printf("next after 6: -> none\n");
	}
	if (SetOperations::NextElement(&set, 250, found))
	{
		printf("next after 250: -> %d\n", found);
	}
	else
	{
		printf("next after 250: -> none\n");
	}
	return 0;
}
