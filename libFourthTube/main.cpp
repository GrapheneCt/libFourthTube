#include "ftube.h"
#include "main.h"

#define API_ENDPOINT				"https://m.youtube.com/youtubei/v1"
#define API_KEY						"AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8"

#define API_SEARCH					API_ENDPOINT "/search?key=" API_KEY "&prettyPrint=false"
#define API_NEXT					API_ENDPOINT "/next?key=" API_KEY "&prettyPrint=false"
#define API_VIDEO					API_ENDPOINT "/player?key=" API_KEY "&prettyPrint=false"
#define API_LIVE_COMMENTS_BASE		"https://www.youtube.com/live_chat?is_popout=1&v=%s"
#define API_LIVE_COMMENTS			API_ENDPOINT "live_chat/get_live_chat?key=" API_KEY "&prettyPrint=false"
//#define API_VISITOR_DATA			"https://www.youtube.com/sw.js_data"

// Search and Navigation (MWEB)
#define POST_CONTEXT_SEARCH			"{\"context\": {\"client\": {\"hl\": \"%s\", \"gl\": \"%s\", \"clientName\": \"MWEB\", \"clientVersion\": \"2.20260415.01.00\"}}, \"params\": \"%s\", \"query\": \"%s\"}"
#define POST_CONTEXT_SEARCH_CONT	"{\"context\": {\"client\": {\"hl\": \"%s\", \"gl\": \"%s\", \"clientName\": \"MWEB\", \"clientVersion\": \"2.20260415.01.00\"}}, \"continuation\": \"%s\"}"
#define POST_CONTEXT_COMMENTS_NEXT	"{\"context\": {\"client\": {\"hl\": \"%s\", \"gl\": \"%s\", \"clientName\": \"MWEB\", \"clientVersion\": \"2.20240315.00.00\", \"utcOffsetMinutes\": 0}}, \"continuation\": \"%s\"}"

// Playback and Comments (ANDROID_VR)
#define POST_CONTEXT_COMMENTS		"{\"videoId\": \"%s\", \"context\": {\"client\": {\"hl\": \"%s\",\"gl\": \"%s\",\"clientName\": \"ANDROID_VR\",\"clientVersion\": \"1.50.45\",\"userAgent\": \"com.google.android.apps.youtube.vr.oculus/1.50.45 (Linux; U; Android 12)\",\"visitorData\": \"%s\"}}}"
#define POST_CONTEXT_VIDEO			"{\"videoId\": \"%s\", \"context\": {\"client\": {\"hl\": \"%s\",\"gl\": \"%s\",\"clientName\": \"ANDROID_VR\",\"clientVersion\": \"1.50.45\",\"userAgent\": \"com.google.android.apps.youtube.vr.oculus/1.50.45 (Linux; U; Android 12)\",\"visitorData\": \"%s\"}}}"

char SEARCH_JSON_FILTER[] =			"{\"estimatedResults\":true,\"continuationContents\":{\"sectionListContinuation\":{\"contents\":true,\"continuations\":true}},\"contents\":{\"sectionListRenderer\":{\"contents\":true,\"continuations\":true}}}";
char VIDEO_JSON_FILTER[] =			"{\"playabilityStatus\":{\"status\":true},\"streamingData\":{\"dashManifestUrl\":true,\"hlsManifestUrl\":true,\"expiresInSeconds\":true,\"formats\":[{\"itag\":true,\"url\":true,\"mimeType\":true,\"bitrate\":true,\"width\":true,\"height\":true,\"lastModified\":true,\"contentLength\":true,\"quality\":true,\"fps\":true,\"qualityLabel\":true,\"projectionType\":true,\"averageBitrate\":true,\"audioQuality\":true,\"approxDurationMs\":true,\"audioSampleRate\":true,\"audioChannels\":true,\"qualityOrdinal\":true,\"audioTrack\":true}],\"adaptiveFormats\":[{\"itag\":true,\"url\":true,\"mimeType\":true,\"bitrate\":true,\"width\":true,\"height\":true,\"initRange\":{\"start\":true,\"end\":true},\"indexRange\":{\"start\":true,\"end\":true},\"lastModified\":true,\"contentLength\":true,\"quality\":true,\"fps\":true,\"qualityLabel\":true,\"projectionType\":true,\"averageBitrate\":true,\"approxDurationMs\":true,\"qualityOrdinal\":true,\"audioTrack\":true,\"audioQuality\":true,\"audioSampleRate\":true,\"audioChannels\":true,\"isDrc\":true}]},\"videoDetails\":{\"videoId\":true,\"title\":true,\"lengthSeconds\":true,\"channelId\":true,\"shortDescription\":true,\"thumbnail\":{\"thumbnails\":[{\"url\":true,\"width\":true,\"height\":true}]},\"viewCount\":true,\"author\":true,\"isUnpluggedCorpus\":true,\"isLiveContent\":true},\"playerConfig\":{\"exoPlayerConfig\":{\"useExoPlayer\":true,\"useAdaptiveBitrate\":true,\"maxInitialByteRate\":true,\"minDurationForQualityIncreaseMs\":true,\"maxDurationForQualityDecreaseMs\":true,\"minDurationToRetainAfterDiscardMs\":true,\"lowWatermarkMs\":true,\"highWatermarkMs\":true,\"lowPoolLoad\":true,\"highPoolLoad\":true,\"sufficientBandwidthOverhead\":true,\"bufferChunkSizeKb\":true,\"httpConnectTimeoutMs\":true,\"httpReadTimeoutMs\":true,\"numAudioSegmentsPerFetch\":true,\"numVideoSegmentsPerFetch\":true,\"minDurationForPlaybackStartMs\":true,\"enableExoplayerReuse\":true,\"useRadioTypeForInitialQualitySelection\":true,\"blacklistFormatOnError\":true,\"enableBandaidHttpDataSource\":true,\"httpLoadTimeoutMs\":true,\"canPlayHdDrm\":true,\"videoBufferSegmentCount\":true,\"audioBufferSegmentCount\":true,\"useAbruptSplicing\":true,\"minRetryCount\":true,\"minChunksNeededToPreferOffline\":true,\"secondsToMaxAggressiveness\":true,\"enableSurfaceviewResizeWorkaround\":true,\"enableVp9IfThresholdsPass\":true,\"matchQualityToViewportOnUnfullscreen\":true,\"lowAudioQualityConnTypes\":[true],\"useDashForLiveStreams\":true,\"enableLibvpxVideoTrackRenderer\":true,\"lowAudioQualityBandwidthThresholdBps\":true,\"enableVariableSpeedPlayback\":true,\"preferOnesieBufferedFormat\":true,\"minimumBandwidthSampleBytes\":true,\"useDashForOtfAndCompletedLiveStreams\":true,\"disableCacheAwareVideoFormatEvaluation\":true,\"useLiveDvrForDashLiveStreams\":true,\"cronetResetTimeoutOnRedirects\":true,\"emitVideoDecoderChangeEvents\":true,\"onesieVideoBufferLoadTimeoutMs\":true,\"onesieVideoBufferReadTimeoutMs\":true,\"libvpxEnableGl\":true,\"enableVp9EncryptedIfThresholdsPass\":true,\"enableOpus\":true,\"usePredictedBuffer\":true,\"maxReadAheadMediaTimeMs\":true,\"useMediaTimeCappedLoadControl\":true,\"allowCacheOverrideToLowerQualitiesWithinRange\":true,\"allowDroppingUndecodedFrames\":true,\"minDurationForPlaybackRestartMs\":true,\"serverProvidedBandwidthHeader\":true,\"liveOnlyPegStrategy\":true,\"enableRedirectorHostFallback\":true,\"enableHighlyAvailableFormatFallbackOnPcr\":true,\"recordTrackRendererTimingEvents\":true,\"minErrorsForRedirectorHostFallback\":true,\"enableVp9IfInHardware\":true,\"enableVp9EncryptedIfInHardware\":true,\"useOpusMedAsLowQualityAudio\":true,\"minErrorsForPcrFallback\":true,\"useStickyRedirectHttpDataSource\":true,\"onlyVideoBandwidth\":true,\"useRedirectorOnNetworkChange\":true,\"enableMaxReadaheadAbrThreshold\":true,\"cacheCheckDirectoryWritabilityOnce\":true,\"predictorType\":true,\"slidingPercentile\":true,\"slidingWindowSize\":true,\"maxFrameDropIntervalMs\":true,\"ignoreLoadTimeoutForFallback\":true,\"serverBweMultiplier\":true,\"drmMaxKeyfetchDelayMs\":true,\"maxResolutionForWhiteNoise\":true,\"whiteNoiseRenderEffectMode\":true,\"enableLibvpxHdr\":true,\"enableCacheAwareStreamSelection\":true,\"useExoCronetDataSource\":true,\"whiteNoiseScale\":true,\"whiteNoiseOffset\":true,\"preventVideoFrameLaggingWithLibvpx\":true,\"enableMediaCodecHdr\":true,\"enableMediaCodecSwHdr\":true,\"liveOnlyWindowChunks\":true,\"bearerMinDurationToRetainAfterDiscardMs\":[true],\"forceWidevineL3\":true,\"useAverageBitrate\":true,\"useMedialibAudioTrackRendererForLive\":true,\"useExoPlayerV2\":true,\"logMediaRequestEventsToCsi\":true,\"onesieFixNonZeroStartTimeFormatSelection\":true,\"liveOnlyReadaheadStepSizeChunks\":true,\"liveOnlyBufferHealthHalfLifeSeconds\":true,\"liveOnlyMinBufferHealthRatio\":true,\"liveOnlyMinLatencyToSeekRatio\":true,\"manifestlessPartialChunkStrategy\":true,\"ignoreViewportSizeWhenSticky\":true,\"enableLibvpxFallback\":true,\"disableLibvpxLoopFilter\":true,\"enableVpxMediaView\":true,\"hdrMinScreenBrightness\":true,\"hdrMaxScreenBrightnessThreshold\":true,\"onesieDataSourceAboveCacheDataSource\":true,\"httpNonplayerLoadTimeoutMs\":true,\"numVideoSegmentsPerFetchStrategy\":true,\"maxVideoDurationPerFetchMs\":true,\"maxVideoEstimatedLoadDurationMs\":true,\"estimatedServerClockHalfLife\":true,\"estimatedServerClockStrictOffset\":true,\"minReadAheadMediaTimeMs\":true,\"readAheadGrowthRate\":true,\"useDynamicReadAhead\":true,\"useYtVodMediaSourceForV2\":true,\"enableV2Gapless\":true,\"useLiveHeadTimeMillis\":true,\"allowTrackSelectionWithUpdatedVideoItagsForExoV2\":true,\"maxAllowableTimeBeforeMediaTimeUpdateSec\":true,\"enableDynamicHdr\":true,\"v2PerformEarlyStreamSelection\":true,\"v2UsePlaybackStreamSelectionResult\":true,\"v2MinTimeBetweenAbrReevaluationMs\":true,\"avoidReusePlaybackAcrossLoadvideos\":true,\"enableInfiniteNetworkLoadingRetries\":true,\"reportExoPlayerStateOnTransition\":true,\"manifestlessSequenceMethod\":true,\"useLiveHeadWindow\":true,\"enableDynamicHdrInHardware\":true,\"ultralowAudioQualityBandwidthThresholdBps\":true,\"retryLiveNetNocontentWithDelay\":true,\"ignoreUnneededSeeksToLiveHead\":true,\"adaptiveLiveHeadWindow\":true,\"drmMetricsQoeLoggingFraction\":true,\"liveNetNocontentMaximumErrors\":true,\"waitForDrmLicenseBeforeProcessingAndroidStuckBufferfull\":true,\"slidingPercentileScalar\":true,\"minAdaptiveVideoQuality\":true,\"retryLiveEmptyChunkWithDelay\":true,\"platypusBackBufferDurationMs\":true}}}";
char LIVE_COMMENTS_JSON_FILTER[] =	"{\"contents\":{\"liveChatRenderer\":{\"actions\":[{\"addChatItemAction\":{\"item\":{\"liveChatTextMessageRenderer\":{\"message\":{\"runs\":[{\"text\":true,\"emoji\":{\"image\":{\"thumbnails\":[{\"url\":true,\"width\":true,\"height\":true}],\"accessibility\":{\"accessibilityData\":{\"label\":true}}},\"isCustomEmoji\":true}}]},\"authorName\":{\"simpleText\":true},\"authorPhoto\":{\"thumbnails\":[{\"url\":true,\"width\":true,\"height\":true}]},\"timestampUsec\":true,\"authorBadges\":[{\"liveChatAuthorBadgeRenderer\":{\"customThumbnail\":{\"thumbnails\":[{\"url\":true,\"width\":true,\"height\":true}]},\"tooltip\":true}}],\"authorExternalChannelId\":true}}}}],\"liveChatCurrentFilter\":true}}}";

#define VISITOR_DATA_RESPONSE_SIZE	2048
#define SEARCH_JSON_FILTER_SIZE		200
#define VIDEO_JSON_FILTER_SIZE		3500
#define LIVE_COMMENTS_JSON_FILTER_SIZE	700

FTAllocator FTMemoryHandler::s_allocate = NULL;
FTDeallocator FTMemoryHandler::s_free = NULL;
FTReallocator FTMemoryHandler::s_reallocate = NULL;
static FTGetter s_get = NULL;
static FTPoster s_post = NULL;

static FTRegion s_region = FT_REGION_EN_US;
static char s_visitorData[100];

struct Filters : public FTMemoryHandler
{
	ArduinoJson::StaticJsonDocument<SEARCH_JSON_FILTER_SIZE> searchFilter;
	ArduinoJson::StaticJsonDocument<VIDEO_JSON_FILTER_SIZE> videoFilter;
	ArduinoJson::StaticJsonDocument<LIVE_COMMENTS_JSON_FILTER_SIZE> liveCommentsFilter;
};
static Filters *s_filters;

// 12 02 10 xx
const char* s_itemTypes[] = {
	"\x12\x02\x10\x01",
	"\x12\x02\x10\x03",
	"\x12\x02\x10\x02",
	"\x00\x00\x00\x00"
};

// 08 xx
const char* s_sorts[] = {
	"\x08\x00",
	"\x08\x01",
	"\x08\x02",
	"\x08\x03"
};

// 12 02 08 xx
const char* s_dates[] = {
	"\x12\x02\x08\x01",
	"\x12\x02\x08\x02",
	"\x12\x02\x08\x03",
	"\x12\x02\x08\x04",
	"\x12\x02\x08\x05",
	"\x00\x00\x00\x00"
};

// 12 02 40 xx
const char* s_features[] = {
	"\x12\x02\x40\x01",
	"\x00\x00\x00\x00"
};

// 12 02 18 xx
const char* s_durations[] = {
	"\x12\x02\x18\x01",
	"\x12\x02\x18\x03",
	"\x12\x02\x18\x02",
	"\x00\x00\x00\x00"
};

const char *s_regionString1[] = { "en" };
const char *s_regionString2[] = { "US" };

const char *s_commentSortStrings[] = { "top", "new" };

static int32_t __ftUpdateVisitorData(const char *input)
{
	const char *start = ft_strstr(input, "\"visitorData\":\"");
	if (!start)
	{
		return FT_EINVAL;
	}

	start += 15;
	const char *end = ft_strchr(start, '"');
	if (!end)
	{
		return FT_EINVAL;
	}

	size_t len = end - start;
	if (len >= sizeof(s_visitorData) - 1)
	{
		return FT_ENOMEM;
	}

	ft_memcpy(s_visitorData, start, len);
	s_visitorData[len] = '\0';

	ft_printf("\n\n");

	ft_printf("[FT] new visitor data %s\n", s_visitorData);

	return FT_OK;
}

static uint32_t __ftTextToSeconds(const char* timeStr)
{
	uint32_t hours = 0;
	uint32_t minutes = 0;
	uint32_t seconds = 0;
	const char* p = timeStr;

	if (ft_strnlen(timeStr, 0xFFFFFFFF) > 5)
	{
		while (*p >= '0' && *p <= '9')
		{
			hours = hours * 10 + (*p - '0');
			p++;
		}

		if (*p != ':')
		{
			return hours * 3600;
		}

		p++;
	}

	while (*p >= '0' && *p <= '9')
	{
		minutes = minutes * 10 + (*p - '0');
		p++;
	}

	if (*p != ':')
	{
		return minutes * 60;
	}

	p++;

	while (*p >= '0' && *p <= '9')
	{
		seconds = seconds * 10 + (*p - '0');
		p++;
	}

	return hours * 3600 + minutes * 60 + seconds;
}

static const char *__ftGetTextString(ArduinoJson::JsonVariantConst const& item)
{
	return item["runs"][0]["text"];
}

static void __ftGetTextStringComposite(ArduinoJson::JsonVariantConst const& item, FTString& res)
{
	uint32_t k = 0;

	ArduinoJson::JsonVariantConst runs = item["runs"];

	while (!runs[k].isNull())
	{
		if (!runs[k]["emoji"].isNull())
		{
			res += runs[k]["emoji"]["image"]["accessibility"]["accessibilityData"]["label"];
		}
		else
		{
			res += runs[k]["text"];
		}
		res += " ";
		k++;
	}
}

static void __ftParseVideoItemForSearch(FTItemVideo *item, ArduinoJson::JsonVariantConst const& doc)
{
	ArduinoJson::JsonVariantConst docItem = doc.containsKey("compactVideoRenderer") ? doc["compactVideoRenderer"] : doc["videoWithContextRenderer"];

	item->title = __ftGetTextString(docItem["headline"]);
	item->id = docItem["videoId"];
	item->author = __ftGetTextString(docItem["shortBylineText"]);
	item->authorId = docItem["channelThumbnail"]["channelThumbnailWithLinkRenderer"]["navigationEndpoint"]["browseEndpoint"]["browseId"];
	if (docItem["lengthText"].isNull())
	{
		item->isLive = true;
		item->published = __ftGetTextString(docItem["shortViewCountText"]);
	}
	else
	{
		item->lengthSec = __ftTextToSeconds(__ftGetTextString(docItem["lengthText"]));
		item->published = __ftGetTextString(docItem["publishedTimeText"]);
	}
	item->thmbUrl = docItem["thumbnail"]["thumbnails"][0]["url"];
	item->thmbUrlHq = docItem["thumbnail"]["thumbnails"][1]["url"];
	if (!item->thmbUrlHq)
	{
		item->thmbUrlHq = item->thmbUrl;
	}
	item->isShort = docItem["shouldDisplayAsBigShortsSingleton"].as<bool>();

	ft_printf("\n\n");

	ft_printf("[FT] parse video title %s\n", item->title);
	ft_printf("[FT] parse video id %s\n", item->id);
	ft_printf("[FT] parse video author %s\n", item->author);
	ft_printf("[FT] parse video authorId %s\n", item->authorId);
	ft_printf("[FT] parse video lengthSec %u\n", item->lengthSec);
	ft_printf("[FT] parse video isLive %d\n", item->isLive);
	ft_printf("[FT] parse video isShort %d\n", item->isShort);
	ft_printf("[FT] parse video published %s\n", item->published);
	ft_printf("[FT] parse video thmbUrl %s\n", item->thmbUrl);
	ft_printf("[FT] parse video thmbUrlHq %s\n", item->thmbUrlHq);
}

static void __ftParsePlaylistItemForSearch(FTItemPlaylist *item, ArduinoJson::JsonVariantConst const& doc)
{
	ArduinoJson::JsonVariantConst docItem = doc.containsKey("compactRadioRenderer") ? doc["compactRadioRenderer"] : doc["compactPlaylistRenderer"];

	item->title = __ftGetTextString(docItem["title"]);
	item->id = docItem["playlistId"];
	item->author = __ftGetTextString(docItem["shortBylineText"]);
	item->authorId = docItem["shortBylineText"]["runs"][0]["navigationEndpoint"]["browseEndpoint"]["browseId"];
	item->published = __ftGetTextString(docItem["publishedTimeText"]);
	item->thmbUrl = docItem["thumbnail"]["thumbnails"][0]["url"];
	item->thmbUrlHq = docItem["thumbnail"]["thumbnails"][1]["url"];
	item->videoCount = docItem["videoCountText"]["runs"][0]["text"].as<uint32_t>();

	ft_printf("\n\n");

	ft_printf("[FT] parse video title %s\n", item->title);
	ft_printf("[FT] parse video id %s\n", item->id);
	ft_printf("[FT] parse video author %s\n", item->author);
	ft_printf("[FT] parse video authorId %s\n", item->authorId);
	ft_printf("[FT] parse video published %s\n", item->published);
	ft_printf("[FT] parse video thmbUrl %s\n", item->thmbUrl);
	ft_printf("[FT] parse video thmbUrl %s\n", item->thmbUrlHq);
	ft_printf("[FT] parse video videoCount %u\n", item->videoCount);
}

static void __ftParseChannelItemForSearch(FTItemChannel *item, ArduinoJson::JsonVariantConst const& doc)
{
	ArduinoJson::JsonVariantConst docItem = doc["compactChannelRenderer"];

	item->author = __ftGetTextString(docItem["displayName"]);
	item->authorId = docItem["channelId"];
	item->subCount = __ftGetTextString(docItem["videoCountText"]); // YouTube displays sub count instead of video count now
	item->thmbUrl = docItem["thumbnail"]["thumbnails"][0]["url"];

	ft_printf("\n\n");

	ft_printf("[FT] parse video author %s\n", item->author);
	ft_printf("[FT] parse video authorId %s\n", item->authorId);
	ft_printf("[FT] parse video thmbUrl %s\n", item->thmbUrl);
	ft_printf("[FT] parse video subCount %s\n", item->subCount);
}

static void __ftParseComment(FTItemComment *item, ArduinoJson::JsonVariantConst const& doc)
{
	ArduinoJson::JsonVariantConst commentDoc = doc["commentThreadRenderer"].isNull() ? doc["commentRenderer"] : doc["commentThreadRenderer"]["comment"]["commentRenderer"];

	item->author = __ftGetTextString(commentDoc["authorText"]);
	item->authorId = commentDoc["authorEndpoint"]["browseEndpoint"]["browseId"];
	item->published = __ftGetTextString(commentDoc["publishedTimeText"]);
	item->thmbUrl = commentDoc["authorThumbnail"]["thumbnails"][0]["url"];
	item->content = __ftGetTextString(commentDoc["contentText"]);
	item->voteCount = __ftGetTextString(commentDoc["voteCount"]);
	item->replyCount = commentDoc["replyCount"].as<uint32_t>();
	item->isOwner = commentDoc["authorIsChannelOwner"].as<bool>();
	item->likedByOwner = commentDoc["isLiked"].as<bool>();
	item->replyContinuation = doc["commentThreadRenderer"]["replies"]["commentRepliesRenderer"]["contents"][0]["continuationItemRenderer"]["button"]["buttonRenderer"]["command"]["continuationCommand"]["token"];

	ft_printf("\n\n");

	ft_printf("[FT] parse comment author %s\n", item->author);
	ft_printf("[FT] parse comment authorId %s\n", item->authorId);
	ft_printf("[FT] parse comment published %s\n", item->published);
	ft_printf("[FT] parse comment thmbUrl %s\n", item->thmbUrl);
	ft_printf("[FT] parse comment content %s\n", item->content);
	ft_printf("[FT] parse comment voteCount %s\n", item->voteCount);
	ft_printf("[FT] parse comment isOwner %d\n", item->isOwner);
	ft_printf("[FT] parse comment likedByOwner %d\n", item->likedByOwner);
	ft_printf("[FT] parse comment replyContinuation %s\n", item->replyContinuation);
}

static void __ftParseLiveComment(FTItemComment *item, ArduinoJson::JsonVariantConst const& doc)
{
	item->author = doc["authorName"]["simpleText"];
	item->authorId = doc["authorExternalChannelId"];
	item->thmbUrl = doc["authorPhoto"]["thumbnails"][0]["url"];
	__ftGetTextStringComposite(doc["message"], *((FTString *)item->reserved));
	item->content = ((FTString *)item->reserved)->GetRawString();

	ft_printf("\n\n");

	ft_printf("[FT] parse live comment author %s\n", item->author);
	ft_printf("[FT] parse live comment authorId %s\n", item->authorId);
	ft_printf("[FT] parse live comment thmbUrl %s\n", item->thmbUrl);
	ft_printf("[FT] parse live comment content %s\n", item->content);
}

int32_t _ftSetRegion(FTRegion region)
{
	if (region >= FT_REGION_MAX)
	{
		return FT_EINVAL;
	}

	s_region = region;

	return FT_OK;
}

int32_t _ftInit(FTAllocator allocator, FTDeallocator deallocator, FTReallocator reallocator, FTGetter getter, FTPoster poster)
{
	if (!allocator || !deallocator || !getter || !poster)
	{
		return FT_EINVAL;
	}

	if (FTMemoryHandler::s_allocate)
	{
		return FT_EALREADY;
	}

	ft_memset(s_visitorData, 0, sizeof(s_visitorData));
	ft_memcpy(s_visitorData, "CgtXa1hoT0xQa3JyRSjWwcLEBjIKCgJSVRIEGgAgYg==", 44);

	FTMemoryHandler::s_allocate = allocator;
	FTMemoryHandler::s_free = deallocator;
	FTMemoryHandler::s_reallocate = reallocator;
	s_get = getter;
	s_post = poster;

	s_filters = new Filters();
	ArduinoJson::deserializeJson(s_filters->searchFilter, SEARCH_JSON_FILTER, sizeof(SEARCH_JSON_FILTER));
	ArduinoJson::deserializeJson(s_filters->videoFilter, VIDEO_JSON_FILTER, sizeof(VIDEO_JSON_FILTER));
	ArduinoJson::deserializeJson(s_filters->liveCommentsFilter, LIVE_COMMENTS_JSON_FILTER, sizeof(LIVE_COMMENTS_JSON_FILTER));

	ft_printf("[FT] filter usage: %u, overflow: %d\n", s_filters->videoFilter.memoryUsage(), s_filters->videoFilter.overflowed());

	return FT_OK;
}

int32_t _ftTerm()
{
	if (!FTMemoryHandler::s_allocate)
	{
		return FT_EALREADY;
	}

	FTMemoryHandler::s_allocate = NULL;
	FTMemoryHandler::s_free = NULL;
	FTMemoryHandler::s_reallocate = NULL;
	s_get = NULL;
	s_post = NULL;

	delete s_filters;

	return FT_OK;
}

int32_t _ftParseSearch(void **ppContext, const char *request, const char *continuation, FTItemType type, FTSort sort, FTDate date, FTFeature feature, FTDuration dur, FTItem **firstItem, const char **continuationToken)
{
	char body[1024];
	int32_t ret = FT_OK;
	uint32_t k = 0;
	void *respBuf = NULL;
	size_t respBufSz = 0;

	const char *header[1];
	header[0] = "Content-Type: application/json";

	if (!ppContext || !firstItem)
	{
		return FT_EINVAL;
	}

	if (!request && !continuation)
	{
		return FT_EINVAL;
	}

	FTContext *ctx = new FTContext();
	if (!ctx)
	{
		return FT_ENOMEM;
	}

	if (!continuation)
	{
		char base64Params[FT_BASE64_ENC_BUF_SIZE(20)];
		uint8_t params[20];
		uint8_t *pParams = params;

		ft_memcpy(pParams, s_sorts[sort], sizeof(uint16_t));
		pParams += sizeof(uint16_t);

		if (type != FT_ITEM_TYPE_ALL)
		{
			ft_memcpy(pParams, s_itemTypes[type], sizeof(uint32_t));
			pParams += sizeof(uint32_t);
		}

		if (date != FT_DATE_ALL)
		{
			ft_memcpy(pParams, s_dates[date], sizeof(uint32_t));
			pParams += sizeof(uint32_t);
		}

		if (feature != FT_FEATURE_NONE)
		{
			ft_memcpy(pParams, s_features[feature], sizeof(uint32_t));
			pParams += sizeof(uint32_t);
		}

		if (dur != FT_DURATION_ALL)
		{
			ft_memcpy(pParams, s_durations[dur], sizeof(uint32_t));
			pParams += sizeof(uint32_t);
		}

		ret = __ftBase64Encoder(base64Params, params, pParams - params);
		if (ret <= 0)
		{
			delete ctx;
			return FT_EINVAL;
		}

		ft_snprintf(
			body,
			sizeof(body),
			POST_CONTEXT_SEARCH,
			s_regionString1[s_region],
			s_regionString2[s_region],
			base64Params,
			request
		);

		ret = s_post(API_SEARCH, body, ft_strnlen(body, sizeof(body)), header, 1, &respBuf, &respBufSz, NULL, FTMemoryHandler::s_allocate, FTMemoryHandler::s_free, FTMemoryHandler::s_reallocate);
		if (!ret || !respBuf || !respBufSz)
		{
			delete ctx;
			return FT_EBADF;
		}
	}
	else
	{
		ft_snprintf(
			body,
			sizeof(body),
			POST_CONTEXT_SEARCH_CONT,
			s_regionString1[s_region],
			s_regionString2[s_region],
			continuation
		);

		ret = s_post(API_SEARCH, body, ft_strnlen(body, sizeof(body)), header, 1, &respBuf, &respBufSz, NULL, FTMemoryHandler::s_allocate, FTMemoryHandler::s_free, FTMemoryHandler::s_reallocate);
		if (!ret || !respBuf || !respBufSz)
		{
			delete ctx;
			return FT_EBADF;
		}
	}

	FTContext::Buffer *buf = ctx->CreateBufferEx((uint32_t)((float)respBufSz * 0.5f), respBuf, respBufSz);
	if (!buf)
	{
		delete ctx;
		return FT_ENOMEM;
	}

	__ftUpdateVisitorData(buf->m_respBuffer);

	ArduinoJson::DeserializationError err = ArduinoJson::deserializeJson(buf->m_respDoc, buf->m_respBuffer, buf->m_respBufferSz, ArduinoJson::DeserializationOption::Filter(s_filters->searchFilter));
	if (err.code() != ArduinoJson::DeserializationError::Ok)
	{
		delete ctx;
		return FT_EBADF;
	}

#ifdef FT_DUMP_BUFFER_USAGE
	ft_printf("[FT] alloc sz: %u, usage %u\n", (uint32_t)((float)respBufSz * 0.5f), buf->m_respDoc.memoryUsage());
#endif

#ifdef FT_DUMP_PRETTY_RESPONSE
	std::string res;
	ArduinoJson::serializeJsonPretty(buf->m_respDoc, res);
	ft_printf("[FT] finaldoc: %s\n", res.c_str());
#endif

	const char *treeUpper = "contents";
	const char *treeLower = "sectionListRenderer";
	if (continuation)
	{
		treeUpper = "continuationContents";
		treeLower = "sectionListContinuation";
	}

	if (continuationToken)
	{
		if (continuation)
		{
			*continuationToken = buf->m_respDoc[treeUpper][treeLower]["continuations"][0]["nextContinuationData"]["continuation"];
		}
		else
		{
			*continuationToken = buf->m_respDoc[treeUpper][treeLower]["continuations"][0]["reloadContinuationData"]["continuation"];
		}

		ft_printf("[FT] my cont: %s\n", *continuationToken);
	}

	FTItem *oitem = NULL;
	ret = 0;

	ArduinoJson::JsonVariantConst contents = buf->m_respDoc[treeUpper][treeLower]["contents"][0]["itemSectionRenderer"]["contents"];

	while (!contents[k].isNull())
	{
		ArduinoJson::JsonVariantConst item = contents[k];
		k++;

		FTItemType type = FT_ITEM_TYPE_VIDEO;

		if (item.size() == 0)
		{
			continue;
		}

		if (item.containsKey("compactVideoRenderer") || item.containsKey("videoWithContextRenderer"))
		{
			type = FT_ITEM_TYPE_VIDEO;
		}
		else if (item.containsKey("compactRadioRenderer") || item.containsKey("compactPlaylistRenderer"))
		{
			type = FT_ITEM_TYPE_PLAYLIST;
		}
		else if (item.containsKey("compactChannelRenderer"))
		{
			type = FT_ITEM_TYPE_CHANNEL;
		}
		else
		{
			continue;
		}

		if (!oitem)
		{
			oitem = ctx->CreateItem(type, true);
			*firstItem = oitem;
		}
		else
		{
			oitem->next = ctx->CreateItem(type);
			oitem = oitem->next;
		}

		if (!oitem || !oitem->videoItem)
		{
			delete ctx;
			return FT_ENOMEM;
		}

		switch (type)
		{
		case FT_ITEM_TYPE_VIDEO:
			__ftParseVideoItemForSearch(oitem->videoItem, item);
			break;
		case FT_ITEM_TYPE_PLAYLIST:
			__ftParsePlaylistItemForSearch(oitem->playlistItem, item);
			break;
		case FT_ITEM_TYPE_CHANNEL:
			__ftParseChannelItemForSearch(oitem->channelItem, item);
			break;
		}

		ret++;
	}

	*ppContext = ctx;

	return ret;
}

int32_t _ftCleanup(void *pContext)
{
	if (!pContext)
	{
		return FT_EINVAL;
	}

	FTContext *ctx = static_cast<FTContext *>(pContext);

	delete ctx;

	return FT_OK;
}

int32_t _ftParseVideo(void **ppContext, const char *videoId, uint32_t videoQuality, uint32_t audioQuality, FTItem **item)
{
	char body[512];
	int32_t ret = FT_OK;
	uint32_t k = 0;

	if (!ppContext || !videoId || !item)
	{
		return FT_EINVAL;
	}

	FTContext *ctx = new FTContext();
	if (!ctx)
	{
		return FT_ENOMEM;
	}

	ft_snprintf(
		body,
		sizeof(body),
		POST_CONTEXT_VIDEO,
		videoId,
		s_regionString1[s_region],
		s_regionString2[s_region],
		s_visitorData
	);

	const char *header[1];
	header[0] = "Content-Type: application/json";

	void *respBuf = NULL;
	size_t respBufSz = 0;
	ret = s_post(API_VIDEO, body, ft_strnlen(body, sizeof(body)), header, 1, &respBuf, &respBufSz, NULL, FTMemoryHandler::s_allocate, FTMemoryHandler::s_free, FTMemoryHandler::s_reallocate);
	if (!ret || !respBuf || !respBufSz)
	{
		delete ctx;
		return FT_EBADF;
	}

	FTContext::Buffer *buf = ctx->CreateBufferEx((uint32_t)((float)respBufSz * FT_VIDEO_BUFFER_SIZE_COEFF), respBuf, respBufSz);
	if (!buf)
	{
		delete ctx;
		return FT_ENOMEM;
	}

	//__ftUpdateVisitorData(buf->m_respBuffer);

	ArduinoJson::DeserializationError err = ArduinoJson::deserializeJson(buf->m_respDoc, buf->m_respBuffer, buf->m_respBufferSz, ArduinoJson::DeserializationOption::Filter(s_filters->videoFilter));
	if (err.code() != ArduinoJson::DeserializationError::Ok)
	{
		delete ctx;
		return FT_EBADF;
	}

#ifdef FT_DUMP_BUFFER_USAGE
	ft_printf("[FT] alloc sz: %u, usage %u\n", (uint32_t)((float)respBufSz * FT_VIDEO_BUFFER_SIZE_COEFF), buf->m_respDoc.memoryUsage());
#endif

#ifdef FT_DUMP_PRETTY_RESPONSE
	std::string res;
	ArduinoJson::serializeJsonPretty(buf->m_respDoc, res);
	ft_printf("[FT] finaldoc: %s\n", res.c_str());
#endif

	const char *status = buf->m_respDoc["playabilityStatus"]["status"];
	if (status[0] != 'O' || status[1] != 'K')
	{
		delete ctx;
		return FT_EBADF;
	}

	FTItem *oitem = ctx->CreateItem(FT_ITEM_TYPE_VIDEO, true);
	if (!oitem)
	{
		delete ctx;
		return FT_ENOMEM;
	}
	FTItemVideo *videoItem = oitem->videoItem;

	ArduinoJson::JsonVariantConst videoDetails = buf->m_respDoc["videoDetails"];
	ArduinoJson::JsonVariantConst adaptiveFormats = buf->m_respDoc["streamingData"]["adaptiveFormats"];
	ArduinoJson::JsonVariantConst oldFormats = buf->m_respDoc["streamingData"]["formats"];

	// Video uses signatureCipher - not supported
	if (adaptiveFormats[0]["url"].isNull())
	{
		delete ctx;
		return FT_EFORMAT;
	}

	videoItem->title = videoDetails["title"];
	videoItem->id = videoDetails["videoId"];
	videoItem->author = videoDetails["author"];
	videoItem->authorId = videoDetails["channelId"];
	videoItem->thmbUrl = videoDetails["thumbnail"]["thumbnails"][0]["url"];
	videoItem->thmbUrlHq = videoDetails["thumbnail"]["thumbnails"][1]["url"];
	videoItem->lengthSec = videoDetails["lengthSeconds"].as<uint32_t>();
	videoItem->dashManifestUrl = buf->m_respDoc["streamingData"]["dashManifestUrl"];
	videoItem->hlsManifestUrl = buf->m_respDoc["streamingData"]["hlsManifestUrl"];
	if (videoItem->dashManifestUrl)
	{
		videoItem->isLive = true;
	}

	videoItem->description = videoDetails["shortDescription"];
	videoItem->viewCount = videoDetails["viewCount"].as<uint32_t>();

	if (videoQuality != FT_VIDEO_VOD_QUALITY_NONE || audioQuality != FT_AUDIO_VOD_QUALITY_NONE)
	{
		FTVODDashManifest *man = ctx->CreateVODDashManifest();
		if (!man)
		{
			delete ctx;
			return FT_ENOMEM;
		}

		ft_printf("[FT] try parse player config\n");

		float minBufferTimeSec = 0.0f;
		ArduinoJson::JsonVariantConst playerStats = buf->m_respDoc["playerConfig"]["exoPlayerConfig"];
		if (!playerStats.isNull())
		{
			minBufferTimeSec = (float)playerStats["minDurationForPlaybackStartMs"].as<uint32_t>() / 1000.0f;
		}

		ft_printf("\n\n");

		ft_printf("[FT] player config minBufferTimeSec %f\n", minBufferTimeSec);

		// Load defaults if config failed to load from YouTube
		if (minBufferTimeSec == 0.0f)
		{
			minBufferTimeSec = 1.5f;
		}

		man->Init(minBufferTimeSec, ((double)adaptiveFormats[0]["approxDurationMs"].as<uint64_t>() / 1000.0f));
		uint32_t vcount = 0;
		uint32_t acount = 0;

		bool idtag298Found = false;

		if (videoQuality != FT_VIDEO_VOD_QUALITY_NONE)
		{
			man->BeginAdaptationSet("video/mp4", true);
			while (!adaptiveFormats[k].isNull())
			{
				ArduinoJson::JsonVariantConst vformat = adaptiveFormats[k];
				bool valid = false;
				char id[2];
				id[1] = 0;
				char codec[16];

				switch (vformat["itag"].as<uint32_t>())
				{
				case 298: // 720p@60
					if (videoQuality & FT_VIDEO_VOD_QUALITY_720P)
					{
						id[0] = '4';
						ft_memcpy(codec, "avc1.4d4020", 12);
						valid = true;
						idtag298Found = true;
					}
					break;
				case 136: // 720p@not 60
					if ((videoQuality & FT_VIDEO_VOD_QUALITY_720P) && !idtag298Found)
					{
						id[0] = '4';
						ft_memcpy(codec, "avc1.4d401f", 12);
						valid = true;
					}
					break;
				case 135:
					if (videoQuality & FT_VIDEO_VOD_QUALITY_480P)
					{
						id[0] = '3';
						ft_memcpy(codec, "avc1.4d401e", 12);
						valid = true;
					}
					break;
				case 134:
					if (videoQuality & FT_VIDEO_VOD_QUALITY_360P)
					{
						id[0] = '2';
						ft_memcpy(codec, "avc1.4d401e", 12);
						valid = true;
					}
					break;
				case 133:
					if (videoQuality & FT_VIDEO_VOD_QUALITY_240P)
					{
						id[0] = '1';
						ft_memcpy(codec, "avc1.4d4015", 12);
						valid = true;
					}
					break;
				case 160:
					if (videoQuality & FT_VIDEO_VOD_QUALITY_144P)
					{
						id[0] = '0';
						ft_memcpy(codec, "avc1.4d400c", 12);
						valid = true;
					}
					break;
				}

				if (valid)
				{
					man->AddRepresentation(
						id,
						codec,
						vformat["bitrate"].as<uint32_t>(),
						vformat["url"],
						vformat["indexRange"]["start"],
						vformat["indexRange"]["end"],
						vformat["initRange"]["start"],
						vformat["initRange"]["end"],
						vformat["width"].as<uint32_t>(),
						vformat["height"].as<uint32_t>(),
						vformat["fps"].as<uint32_t>());
					vcount++;
				}

				k++;
			}
			man->EndAdaptationSet();
			k = 0;
		}

		if (audioQuality != FT_AUDIO_VOD_QUALITY_NONE)
		{
			man->BeginAdaptationSet("audio/mp4", false);
			while (!adaptiveFormats[k].isNull())
			{
				ArduinoJson::JsonVariantConst aformat = adaptiveFormats[k];
				bool valid = false;
				char id[2];
				id[1] = 0;
				char codec[16];

				switch (aformat["itag"].as<uint32_t>())
				{
				case 140:
					if ((audioQuality & FT_AUDIO_VOD_QUALITY_MEDIUM) && aformat["isDrc"].isNull())
					{
						if (!aformat["audioTrack"]["displayName"].isNull() && !ft_strstr(aformat["audioTrack"]["displayName"].as<const char *>(), "original"))
						{
							// Auto dubbed AI bullshit
							break;
						}
						id[0] = '0';
						ft_memcpy(codec, "mp4a.40.2", 10);
						valid = true;
					}
					break;
					// This audio track breaks SceAvPlayer completely for some reason
				case 139:
					if ((audioQuality & FT_AUDIO_VOD_QUALITY_LOW) && aformat["isDrc"].isNull())
					{
						if (!aformat["audioTrack"]["displayName"].isNull() && !ft_strstr(aformat["audioTrack"]["displayName"].as<const char *>(), "original"))
						{
							// Auto dubbed AI bullshit
							break;
						}
						id[0] = '1';
						ft_memcpy(codec, "mp4a.40.5", 10);
						valid = true;
					}
					break;
				case 251:
					videoItem->audioOnlyUrl = aformat["url"];
					break;
				}

				if (valid)
				{
					man->AddRepresentation(
						id,
						codec,
						aformat["bitrate"],
						aformat["url"],
						aformat["indexRange"]["start"],
						aformat["indexRange"]["end"],
						aformat["initRange"]["start"],
						aformat["initRange"]["end"]);
					acount++;
				}

				k++;
			}
			man->EndAdaptationSet();
			k = 0;
		}

		man->Finish();

		videoItem->videoRepCount = vcount;
		videoItem->audioRepCount = acount;

		videoItem->dashManifest = man->GetBuffer();
	}

	videoItem->compositeVideoUrl = oldFormats[0]["url"];

	ft_printf("\n\n");

	ft_printf("[FT] parse video title %s\n", videoItem->title);
	ft_printf("[FT] parse video id %s\n", videoItem->id);
	ft_printf("[FT] parse video author %s\n", videoItem->author);
	ft_printf("[FT] parse video authorId %s\n", videoItem->authorId);
	ft_printf("[FT] parse video published %s\n", videoItem->published);
	ft_printf("[FT] parse video thmbUrl %s\n", videoItem->thmbUrl);
	ft_printf("[FT] parse video thmbUrlHq %s\n", videoItem->thmbUrlHq);
	ft_printf("[FT] parse video isLive %d\n", videoItem->isLive);
	ft_printf("[FT] parse video lengthSec %u\n", videoItem->lengthSec);

	ft_printf("[FT] parse video videoRepCount %u\n", videoItem->videoRepCount);
	ft_printf("[FT] parse video audioRepCount %u\n", videoItem->audioRepCount);
	ft_printf("[FT] parse video description %s\n", videoItem->description);
	ft_printf("[FT] parse video viewCount %u\n", videoItem->viewCount);
	ft_printf("[FT] parse video compositeVideoUrl %s\n", videoItem->compositeVideoUrl);
	ft_printf("[FT] parse video hlsManifestUrl %s\n", videoItem->hlsManifestUrl);
	ft_printf("[FT] parse video dashManifestUrl %s\n", videoItem->dashManifestUrl);

	ft_printf("\n\n");

	ft_printf("[FT] parse video DASH manifest: %s\n", videoItem->dashManifest);

	*item = oitem;
	*ppContext = ctx;

	return FT_OK;
}

int32_t _ftParseComments(void **ppContext, const char *videoId, const char *continuation, FTCommentSort sort, FTItem **firstItem, const char **continuationToken)
{
	char body[1024];
	int32_t ret = FT_OK;
	uint32_t k = 0;
	const char *contStart = NULL;
	void *respBuf = NULL;
	size_t respBufSz = 0;

	const char *header[1];
	header[0] = "Content-Type: application/json";

	if (!ppContext || !videoId || !firstItem)
	{
		return FT_EINVAL;
	}

	if (!continuation)
	{
		ft_snprintf(
			body,
			sizeof(body),
			POST_CONTEXT_COMMENTS,
			videoId,
			s_regionString1[s_region],
			s_regionString2[s_region],
			s_visitorData
		);

		ret = s_post(API_NEXT, body, ft_strnlen(body, sizeof(body)), header, 1, &respBuf, &respBufSz, NULL, FTMemoryHandler::s_allocate, FTMemoryHandler::s_free, FTMemoryHandler::s_reallocate);
		if (!ret || !respBuf || !respBufSz)
		{
			return FT_EBADF;
		}

		//__ftUpdateVisitorData(buf->m_respBuffer);

		contStart = ft_strstr((char *)respBuf, "\"sectionListRenderer\":{\"continuations\":[{\"reloadContinuationData\":{\"continuation\":\"");
		if (!contStart)
		{
			return FT_EBADF;
		}

		contStart += 83;
		char *contEnd = ft_strchr((char *)contStart, '"');
		*contEnd = 0;

		if (sort == FT_COMMENT_SORT_NEW)
		{
			char tmp[128];
			__ftBase64StringDeformat((char *)contStart, contEnd - contStart);
			ret = __ftBase64Decoder(tmp, contStart, ft_strnlen(contStart, 0xFFFFFFFF));
			if (ret < 0)
			{
				return FT_EBADF;
			}

			ft_printf("[FT] dec len: %d\n", ret);

			// byte 0x23: 0 - top, 1 - new
			tmp[0x23] = 1;

			ret = __ftBase64Encoder((char *)contStart, tmp, ret);
			if (ret < 0)
			{
				return FT_EBADF;
			}

			ft_printf("[FT] enc len: %d\n", ret);
		}
	}
	else
	{
		contStart = continuation;
	}

	FTContext *ctx = new FTContext();
	if (!ctx)
	{
		return FT_ENOMEM;
	}

	ft_printf("[FT] my cont: %s\n", contStart);

	ft_snprintf(
		body,
		sizeof(body),
		POST_CONTEXT_COMMENTS_NEXT,
		s_regionString1[s_region],
		s_regionString2[s_region],
		contStart
	);

	if (respBuf)
	{
		FTMemoryHandler::s_free(respBuf);
	}

	respBuf = NULL;
	respBufSz = 0;
	ret = s_post(API_NEXT, body, ft_strnlen(body, sizeof(body)), header, 1, &respBuf, &respBufSz, NULL, FTMemoryHandler::s_allocate, FTMemoryHandler::s_free, FTMemoryHandler::s_reallocate);
	if (!ret || !respBuf || !respBufSz)
	{
		delete ctx;
		return FT_EBADF;
	}

	FTContext::Buffer *buf = ctx->CreateBufferEx((uint32_t)((float)respBufSz * FT_COMMENTS_BUFFER_SIZE_COEFF), respBuf, respBufSz);
	if (!buf)
	{
		delete ctx;
		return FT_ENOMEM;
	}

	__ftUpdateVisitorData(buf->m_respBuffer);

	ArduinoJson::DeserializationError err = ArduinoJson::deserializeJson(buf->m_respDoc, buf->m_respBuffer, buf->m_respBufferSz);
	if (err.code() != ArduinoJson::DeserializationError::Ok)
	{
		delete ctx;
		return FT_EBADF;
	}

#ifdef FT_DUMP_BUFFER_USAGE
	ft_printf("[FT] alloc sz: %u, usage %u\n", (uint32_t)((float)respBufSz * FT_COMMENTS_BUFFER_SIZE_COEFF), buf->m_respDoc.memoryUsage());
#endif

#ifdef FT_DUMP_PRETTY_RESPONSE
	std::string res;
	ArduinoJson::serializeJson(buf->m_respDoc, res);
	ft_printf("[FT] finaldoc: %s\n", res.c_str());
#endif


	int32_t commentSectionId = -1;
	const char *command = "reloadContinuationItemsCommand";
	if (continuation)
	{
		command = "appendContinuationItemsAction";
		commentSectionId = 0;
	}
	else
	{
		for (int i = 0; i < buf->m_respDoc["onResponseReceivedEndpoints"].size(); i++)
		{
			if (!buf->m_respDoc["onResponseReceivedEndpoints"][i]["reloadContinuationItemsCommand"].isNull() && !ft_strcmp(buf->m_respDoc["onResponseReceivedEndpoints"][i]["reloadContinuationItemsCommand"]["slot"], "RELOAD_CONTINUATION_SLOT_BODY"))
			{
				commentSectionId = i;
				break;
			}
		}

		if (commentSectionId < 0)
		{
			delete ctx;
			return FT_EBADF;
		}
	}

	ArduinoJson::JsonVariantConst comments = buf->m_respDoc["onResponseReceivedEndpoints"][commentSectionId][command]["continuationItems"];

	FTItem *oitem = NULL;
	ret = 0;

	while (!comments[k].isNull())
	{
		ArduinoJson::JsonVariantConst comment = comments[k];

		if (!comment["continuationItemRenderer"].isNull())
		{
			if (continuationToken)
			{
				if (comment["continuationItemRenderer"]["continuationEndpoint"].isNull())
				{
					*continuationToken = comment["continuationItemRenderer"]["button"]["buttonRenderer"]["command"]["continuationCommand"]["token"];
				}
				else
				{
					*continuationToken = comment["continuationItemRenderer"]["continuationEndpoint"]["continuationCommand"]["token"];
				}

				ft_printf("[FT] Comment cont token: %s\n", *continuationToken);
			}
			break;
		}

		if (!oitem)
		{
			oitem = ctx->CreateItem(FT_ITEM_TYPE_COMMENT, true);
			*firstItem = oitem;
		}
		else
		{
			oitem->next = ctx->CreateItem(FT_ITEM_TYPE_COMMENT);
			oitem = oitem->next;
		}

		if (!oitem || !oitem->commentItem)
		{
			delete ctx;
			return FT_ENOMEM;
		}

		__ftParseComment(oitem->commentItem, comment);

		k++;
		ret++;
	}
	k = 0;

	*ppContext = ctx;

	return ret;
}

int32_t _ftParseLiveComments(void **ppContext, const char *videoId, uint32_t maxNum, FTItem **firstItem)
{
	char body[128];
	int32_t ret = 0;

	if (!ppContext || !videoId || !firstItem)
	{
		return FT_EINVAL;
	}

	FTContext *ctx = new FTContext();
	if (!ctx)
	{
		return FT_ENOMEM;
	}

	ft_snprintf(
		body,
		sizeof(body),
		API_LIVE_COMMENTS_BASE,
		videoId
	);

	const char *header[1];
	header[0] = "User-Agent: Mozilla/5.0 (iPad; CPU OS 16_7_10 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1,gzip(gfe)";

	void *respBuf = NULL;
	size_t respBufSz = 0;
	ret = s_get(body, &respBuf, &respBufSz, header, 1, NULL, FTMemoryHandler::s_allocate, FTMemoryHandler::s_free, FTMemoryHandler::s_reallocate);
	if (!ret || !respBuf || !respBufSz)
	{
		delete ctx;
		return FT_EBADF;
	}

	FTContext::Buffer *buf = ctx->CreateBufferEx((uint32_t)((float)respBufSz * FT_LIVE_COMMENTS_BUFFER_SIZE_COEFF), respBuf, respBufSz);
	if (!buf)
	{
		delete ctx;
		return FT_ENOMEM;
	}

	const char *jsonStart = ft_strstr((char *)respBuf, "{\"responseContext\"");

	__ftUpdateVisitorData(jsonStart);

	ArduinoJson::DeserializationError err = ArduinoJson::deserializeJson(buf->m_respDoc, jsonStart, buf->m_respBufferSz - (jsonStart - buf->m_respBuffer), ArduinoJson::DeserializationOption::Filter(s_filters->liveCommentsFilter));
	if (err.code() != ArduinoJson::DeserializationError::Ok)
	{
		delete ctx;
		return FT_EBADF;
	}


#ifdef FT_DUMP_BUFFER_USAGE
	ft_printf("[FT] alloc sz: %u, usage %u\n", (uint32_t)((float)respBufSz * FT_LIVE_COMMENTS_BUFFER_SIZE_COEFF), buf->m_respDoc.memoryUsage());
#endif

#ifdef FT_DUMP_PRETTY_RESPONSE
	std::string res;
	ArduinoJson::serializeJson(buf->m_respDoc, res);
	ft_printf("[FT] finaldoc: %s\n", res.c_str());
#endif

	
	FTItem *oitem = NULL;
	ret = 0;

	ArduinoJson::JsonVariantConst comments = buf->m_respDoc["contents"]["liveChatRenderer"]["actions"];

	for (int i = comments.size() - 1; i > 0; i--)
	{
		ArduinoJson::JsonVariantConst comment = comments[i];
		
		if (comment["addChatItemAction"]["item"]["liveChatTextMessageRenderer"].isNull())
		{
			continue;
		}

		if (!oitem)
		{
			oitem = ctx->CreateItem(FT_ITEM_TYPE_COMMENT, true);
			*firstItem = oitem;
		}
		else
		{
			oitem->next = ctx->CreateItem(FT_ITEM_TYPE_COMMENT);
			oitem = oitem->next;
		}

		if (!oitem || !oitem->commentItem)
		{
			delete ctx;
			return FT_ENOMEM;
		}

		__ftParseLiveComment(oitem->commentItem, comment["addChatItemAction"]["item"]["liveChatTextMessageRenderer"]);

		ret++;
		maxNum--;

		if (maxNum == 0)
		{
			break;
		}
	}

	*ppContext = ctx;

	return ret;
}

#ifdef __SNC__

extern "C" {

	int __module_stop(SceSize argc, const void *args)
	{
		return SCE_KERNEL_STOP_SUCCESS;
	}

	int __module_exit()
	{
		return SCE_KERNEL_STOP_SUCCESS;
	}

	int __module_start(SceSize argc, void *args)
	{
		return SCE_KERNEL_START_SUCCESS;
	}

	void *memset(void * ptr, int value, size_t num)
	{
		return sceClibMemset(ptr, value, num);
	}
}

#endif