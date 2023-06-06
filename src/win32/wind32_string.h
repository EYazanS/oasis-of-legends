#ifndef string_h
#define string_h

void con_cat_strings(
	size_t sourceACount, char *sourceA,
	size_t sourceBCount, char *sourceB,
	size_t destCount, char *dest)
{
	for (size_t index = 0; index < sourceACount; ++index)
	{
		*dest++ = *sourceA++;
	}

	for (size_t index = 0; index < sourceBCount; ++index)
	{
		*dest++ = *sourceB++;
	}

	*dest++ = 0;
}

#endif