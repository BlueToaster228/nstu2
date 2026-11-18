#pragma once
#include "lab1/structures.h"

struct BitSet
{
	unsigned long long regs[4];
};

namespace SetOperations
{
	void Init(BitSet* set);
	void Add(BitSet* set, unsigned char value);
	void Remove(BitSet* set, unsigned char value);
	bool Contains(BitSet* set, unsigned char value);
	int Count(BitSet* set);
	void Read(BitSet* set, char* output);
	bool NextElement(BitSet* set, unsigned char from, unsigned char& found);
}
