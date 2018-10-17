#ifndef MISC_POOLALLOCATOR_INCLUDED
#define MISC_POOLALLOCATOR_INCLUDED

#include <stddef.h>



template <class ContentParam,size_t pageSizeParam =8192>
class PoolAllocator
	{
	/* Embedded classes: */
	public:
	typedef ContentParam Content; //!< Type of allocated object
	static const size_t pageSize=pageSizeParam; //!< Size of memory page in bytes

	private:
	struct AllocationSlot //!< Structure representing memory allocation slots
		{
		/* Elements: */
		public:
		AllocationSlot* succ;
		};

	struct Chunk;

	struct ChunkHeader //!< Structure for memory chunk headers
		{
		/* Elements: */
		public:
		Chunk* succ; //!< Pointer to next chunk in list

		/* Constructors and destructors: */
		ChunkHeader(void)
			:succ(0)
			{
			}
		};

	struct Chunk //!< Structure for chunks of memory
		{
		/* Elements: */
		public:
		ChunkHeader header; //!< Chunk header
		char mem[pageSize-sizeof(ChunkHeader)]; //!< Uninitialized memory in chunk
		};

	/* Elements: */
	Chunk* firstChunk; //!< Pointer to head of chunk list
	size_t slotSize; //!< Size of an allocation slot
	size_t numSlotsPerChunk; //!< Number of allocation slots per memory chunk
	AllocationSlot* firstSlot; //!< Pointer to head of free allocation slot list

	/* Private methods: */
	void growPool(void) //!< Increases memory pool size
		{
		/* Allocate a new chunk: */
		Chunk* newChunk=new Chunk;
		newChunk->header.succ=firstChunk;
		firstChunk=newChunk;

		/* Create a linked list of free allocation slots in the new chunk: */
		char* slotPtr=newChunk->mem;
		for(size_t i=0;i<numSlotsPerChunk-1;++i,slotPtr+=slotSize)
			reinterpret_cast<AllocationSlot*>(slotPtr)->succ=reinterpret_cast<AllocationSlot*>(slotPtr+slotSize);

		/* Connect the new free list to the old free list: */
		reinterpret_cast<AllocationSlot*>(slotPtr)->succ=firstSlot;
		firstSlot=reinterpret_cast<AllocationSlot*>(newChunk->mem);
		}

	/* Constructors and destructors: */
	public:
	PoolAllocator(void) //!< Creates an empty memory pool
		:firstChunk(0),firstSlot(0)
		{
		/* Determine size of an allocation slot (allow for very small or oddly-sized content types): */
		slotSize=sizeof(Content);
		if(slotSize<sizeof(AllocationSlot)) //!< Pad slot size for contents smaller than link
			slotSize=sizeof(AllocationSlot);
		else if(slotSize%sizeof(AllocationSlot)!=0) //!< Pad slot size to properly align links
			slotSize+=sizeof(AllocationSlot)-slotSize%sizeof(AllocationSlot);

		/* Calculate number of allocation slots per memory chunk: */
		numSlotsPerChunk=(pageSize-sizeof(ChunkHeader))/slotSize;
		}
	~PoolAllocator(void) //!< Destroys pool and releases all allocated memory chunks
		{
		while(firstChunk!=0)
			{
			Chunk* succ=firstChunk->header.succ;
			delete firstChunk;
			firstChunk=succ;
			}
		}

	/* Methods: */
	void* allocate(void)
		{
		if(firstSlot==0) //!< We ran out of memory
			growPool(); //!< Grow the pool

		/* Return the first free item: */
		void* result=firstSlot;
		firstSlot=firstSlot->succ;
		return result;
		}

	void free(void* item)
		{
		/* Put the new free item at the head of the list: */
		AllocationSlot* newSlot=reinterpret_cast<AllocationSlot*>(item);
		newSlot->succ=firstSlot;
		firstSlot=newSlot;
		}
	};

#endif
