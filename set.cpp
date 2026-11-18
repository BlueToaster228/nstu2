#include "set.h"

namespace SetOperations
{
	void Init(BitSet* set)
	{
		for (register int index = 0; index < 4; index++)
		{
			set->regs[index] = 0;
		}
	}

	void Add(BitSet* set, unsigned char value)
	{
		register unsigned int reg = value / 64;
		register unsigned long long bit = value % 64;
		__asm__ volatile
		(
			"btsq %1, %0"
			: "+m" (set->regs[reg])
			: "r" (bit)
			: "cc"
		);
	}

	void Remove(BitSet* set, unsigned char value)
	{
		register unsigned int reg = value / 64;
		register unsigned long long bit = value % 64;
		__asm__ volatile
		(
			"btrq %1, %0"
			: "+m" (set->regs[reg])
			: "r" (bit)
			: "cc"
		);
	}

	bool Contains(BitSet* set, unsigned char value)
	{
		register unsigned int reg = value / 64;
		register unsigned long long bit = value % 64;
		register unsigned char carry = 0;
		__asm__ volatile
		(
			"btq %2, %1\n\t"
			"setc %b0"
			: "=q" (carry)
			: "m" (set->regs[reg]), "r" (bit)
			: "cc"
		);
		return carry != 0;
	}

	int Count(BitSet* set)
	{
		register unsigned int total = 0;
		for (register int index = 0; index < 4; index++)
		{
			register unsigned long long reg = set->regs[index];
			register unsigned long long pieces = 0;
			__asm__ volatile
			(
				"popcntq %1, %0"
				: "=r" (pieces)
				: "r" (reg)
				: "cc"
			);
			total += (unsigned int)pieces;
		}
		return (int)total;
	}

	void Read(BitSet* set, char* output)
	{
		output[0] = '\0';
		register bool first = true;
		for (register int value = 0; value < 256; value++)
		{
			if (Contains(set, (unsigned char)value))
			{
				if (!first)
				{
					strcat(output, ",");
				}
				char piece[TextSize];
				sprintf(piece, "%d", value);
				strcat(output, piece);
				first = false;
			}
		}
	}

	bool NextElement(BitSet* set, unsigned char from, unsigned char& found)
	{
		for (register int value = from; value < 256; value++)
		{
			if (Contains(set, (unsigned char)value))
			{
				found = (unsigned char)value;
				return true;
			}
		}
		return false;
	}
}
