#include <stdint.h>

#include "ftube.h"
#include "main.h"

extern "C" 
{
	int32_t ftSetRegion(FTRegion region)
	{
		return _ftSetRegion(region);
	}

	int32_t ftInit(FTAllocator allocator, FTDeallocator deallocator, FTReallocator reallocator, FTGetter getter, FTPoster poster)
	{
		return _ftInit(allocator, deallocator, reallocator, getter, poster);
	}

	int32_t ftTerm()
	{
		return _ftTerm();
	}

	int32_t ftParseSearch(void **ppContext, const char *request, const char *continuation, FTItemType type, FTSort sort, FTDate date, FTFeature feature, FTDuration dur, FTItem **firstItem, const char **continuationToken)
	{
		return _ftParseSearch(ppContext, request, continuation, type, sort, date, feature, dur, firstItem, continuationToken);
	}

	int32_t ftCleanup(void *pContext)
	{
		return _ftCleanup(pContext);
	}

	int32_t ftParseVideo(void **ppContext, const char *videoId, uint32_t videoQuality, uint32_t audioQuality, FTItem **item)
	{
		return _ftParseVideo(ppContext, videoId, videoQuality, audioQuality, item);
	}

	int32_t ftParseComments(void **ppContext, const char *videoId, const char *continuation, FTCommentSort sort, FTItem **item, const char **continuationToken)
	{
		return _ftParseComments(ppContext, videoId, continuation, sort, item, continuationToken);
	}

	int32_t ftParseLiveComments(void **ppContext, const char *videoId, uint32_t maxNum, FTItem **firstItem)
	{
		return _ftParseLiveComments(ppContext, videoId, maxNum, firstItem);
	}
}