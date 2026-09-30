# include	"RWll.h"
# include	<string.h>


// ---------------------------------------------------------------------------
// allocate_new_datablock											 [private]
// ---------------------------------------------------------------------------
// Allocate and initialize new data block

RWll::RWll_internal_s*
RWll::RWll_internal_s::allocate_new_datablock (void)
{
	RWll_internal_s* ldi = (RWll_internal_s*) malloc (sizeof (RWll_internal_s));

	if (ldi != NULL)
	{
		ldi->avail_in_this_block = RWll_internal_s::SIZEDATA_INDATABLOCK;
		ldi->next_datablock = NULL;
		ldi->filled_in_this_block = 0;
	}

	return ldi;
}



// ---------------------------------------------------------------------------
// RWll										Default Constructor		  [public]
// ---------------------------------------------------------------------------

RWll::RWll (void)
	:	first_block (0),
		last_block (0)
{
}


// ---------------------------------------------------------------------------
// ~RWll									Destructor				  [public]
// ---------------------------------------------------------------------------
// Destroy all allocated data blocks

RWll::~RWll (void)
{
	RWll_internal_s* ldi = first_block;

	while (ldi != NULL)
	{
		RWll_internal_s* ldinext = ldi->next_datablock;
		if (ldi)
			free (ldi);
		ldi = ldinext;
	}
	first_block = last_block = NULL;

	return;
}


// ---------------------------------------------------------------------------
// add_data_in_datablock											  [public]
// ---------------------------------------------------------------------------
// Put data into data block(s); allocate when needed

long
RWll::add_data_in_datablock (const void* buf, long inLen)
{
	RWll_internal_s* ldi;
	const unsigned char* from_copy;

	if (last_block == NULL)
	{
		first_block = last_block = RWll_internal_s::allocate_new_datablock();
		if (first_block == NULL)
			return -1;
	}

	ldi = last_block;
	from_copy = (unsigned char*) buf;

	long	len = inLen;

	while (len > 0)
	{
		if (ldi->avail_in_this_block == 0)
		{
			ldi->next_datablock = RWll_internal_s::allocate_new_datablock();
			if (ldi->next_datablock == NULL)
				return -1;
			ldi = ldi->next_datablock;
			last_block = ldi;
		}

		long	copy_this;
		if (ldi->avail_in_this_block < len)
			copy_this = ldi->avail_in_this_block;
		else
			copy_this = len;

		memcpy (ldi->data + ldi->filled_in_this_block, from_copy, copy_this);

		ldi->filled_in_this_block += copy_this;
		ldi->avail_in_this_block -= copy_this;
		from_copy += copy_this;
		len -= copy_this;
	}

	return inLen;
}


// ---------------------------------------------------------------------------
// concatenate														  [public]
// ---------------------------------------------------------------------------
// Create a contiguous block from all data blocks

void *
RWll::concatenate (size_t &outSize)
{
	unsigned char			*buf = NULL;
	RWll::RWll_internal_s	*ldi;
	long					size = 0;

	for ( ldi = first_block; ldi != NULL; ldi = ldi->next_datablock )
		size += ldi->filled_in_this_block;

	buf = (unsigned char*) malloc (size);
	outSize = size;
	if (buf != NULL)
	{
		size = 0;
		for ( ldi = first_block; ldi != NULL; )
		{
			if (ldi->filled_in_this_block > 0)
			{
				memcpy (buf + size, ldi->data, ldi->filled_in_this_block);
				size += ldi->filled_in_this_block;
			}
			ldi = ldi->next_datablock;
		}
	}

	return buf;
}
