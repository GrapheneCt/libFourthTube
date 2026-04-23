#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "curl/curl.h"
#include "curl/easy.h"

#include "ftube.h"

// Debug dump control
//#define DUMP_REQUESTS
//#define DUMP_RESPONSES

// Select which test to run
//#define TEST_SEARCH
//#define TEST_SEARCH_CONTINUATION
//#define TEST_VIDEO
//#define TEST_COMMENTS
//#define TEST_COMMENTS_CONTINUATION
//#define TEST_COMMENTS_LIVE
//#define DIRECT_TEST

// Proxy option
//#define CURL_PROXY		"socks5://192.168.3.120:12334"

bool FTGetterCb(const char *curl, void **ppRespBuf, size_t *pRespBufSize, const char **pHeaders, uint32_t headerNum, int32_t *pRespCode, FTAllocator ftalloc, FTDeallocator ftdealloc, FTReallocator ftrealloc)
{
	std::string url = curl;
#ifdef DUMP_REQUESTS
	printf("[FT] POST request: %s\n", url.c_str());
#endif

	std::size_t scheme_end = url.find("://");
	std::size_t host_start = (scheme_end != std::string::npos) ? scheme_end + 3 : 0;

	std::size_t path_start = url.find('/', host_start);

	std::string base_url;
	std::string path;

	if (path_start != std::string::npos) {
		base_url = url.substr(0, path_start);
		path = url.substr(path_start);
	}
	else {
		base_url = url;
		path = "/";
	}

	httplib::Client cli(base_url);

	httplib::Headers head;

	/*
	for (int i = 0; i < headerNum; i++)
	{
		printf("header: %s : %s\n", pHeaders[i]);
		head.emplace(pHeaders[i]);
	}
	*/

	auto resp = cli.Get(path, head);

	if (pRespCode)
	{
		*pRespCode = resp->status;
	}

	if (resp->status != 200)
	{
		printf("[FT] fail: %d\n", resp->status);
		return false;
	}
	else
	{
#ifdef DUMP_RESPONSES
		printf("[FT] response: %s\n", resp->body.c_str());
#endif

		*ppRespBuf = ftalloc(resp->body.length());
		memcpy(*ppRespBuf, resp->body.c_str(), resp->body.length());
		*pRespBufSize = resp->body.length();

		return true;
	}

	return false;
}

bool FTPosterCb(const char *curl, void *pBuf, size_t bufSize, const char **pHeaders, uint32_t headerNum, void **ppRespBuf,
	size_t *pRespBufSize, int32_t *pRespCode, FTAllocator ftalloc, FTDeallocator ftdealloc, FTReallocator ftrealloc)
{
	std::string url = curl;
#ifdef DUMP_REQUESTS
	printf("[FT] POST request: %s\n", url.c_str());
	printf("[FT] POST body: %s\n", pBuf);
#endif

	std::size_t scheme_end = url.find("://");
	std::size_t host_start = (scheme_end != std::string::npos) ? scheme_end + 3 : 0;

	std::size_t path_start = url.find('/', host_start);

	std::string base_url;
	std::string path;

	if (path_start != std::string::npos) {
		base_url = url.substr(0, path_start);
		path = url.substr(path_start);
	}
	else {
		base_url = url;
		path = "/";
	}

	httplib::Client cli(base_url);

	httplib::Headers head;

	/*
	for (int i = 0; i < headerNum; i++)
	{
		printf("header: %s : %s\n", pHeaders[i]);
		head.emplace(pHeaders[i]);
	}
	*/

	auto resp = cli.Post(path, head, (const char *)pBuf, bufSize, "application/json");

	if (pRespCode)
	{
		*pRespCode = resp->status;
	}

	if (resp->status != 200)
	{
		printf("[FT] fail: %d\n", resp->status);
		return false;
	}
	else
	{
#ifdef DUMP_RESPONSES
			printf("[FT] response: %s\n", resp->body.c_str());
#endif

		*ppRespBuf = ftalloc(resp->body.length());
		memcpy(*ppRespBuf, resp->body.c_str(), resp->body.length());
		*pRespBufSize = resp->body.length();

		return true;
	}
}

namespace utils {

	class CurlDownloadContext
	{
	public:

		CurlDownloadContext()
		{
			m_buf = NULL;
			m_pos = 0;
		}

		~CurlDownloadContext() {};

		void *m_buf;
		uint32_t m_pos;
	};

	size_t CurlDownloadCore(char *buffer, size_t size, size_t nitems, void *userdata)
	{
		utils::CurlDownloadContext *obj = static_cast<utils::CurlDownloadContext *>(userdata);
		size_t toCopy = size * nitems;
	
		if (toCopy != 0)
		{
			obj->m_buf = realloc(obj->m_buf, obj->m_pos + toCopy);
			memcpy(static_cast<char *>(obj->m_buf) + obj->m_pos, buffer, toCopy);
			obj->m_pos += toCopy;
		}
	
		return toCopy;
	}
}

bool FTPosterCbCurl(const char *url, void *pBuf, size_t bufSize, const char **pHeaders, uint32_t headerNum, void **ppRespBuf,
	size_t *pRespBufSize, int32_t *pRespCode, FTAllocator ftalloc, FTDeallocator ftdealloc, FTReallocator ftrealloc)
{
#ifdef DUMP_REQUESTS
	printf("[FT] POST request: %s\n", url);
	printf("[FT] POST body: %s\n", pBuf);
#endif

	CURL *curl = curl_easy_init();
	//curl_easy_setopt(curl, CURLOPT_SHARE, s_share);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 64 * 1024);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::CurlDownloadCore);
	//curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (PlayStation Vita 3.74) AppleWebKit/537.73 (KHTML, like Gecko) Silk/3.2");
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pBuf);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)bufSize);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	//curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, uTimeOutMSecs);
#ifdef CURL_PROXY
	curl_easy_setopt(curl, CURLOPT_PROXY, CURL_PROXY);
#endif

	struct curl_slist *headers = NULL;
	if (headerNum)
	{
		for (int i = 0; i < headerNum; i++)
		{
			headers = curl_slist_append(headers, pHeaders[i]);
		}

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}

	utils::CurlDownloadContext ctx;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ctx);

	long responseCode = 0;
	CURLcode ret = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	curl_slist_free_all(headers);
	printf("[FT] error: %d, resp: %d\n", ret, responseCode);
	//ctx.m_buf = realloc(ctx.m_buf, ctx.m_pos + 1);
	//((char *)ctx.m_buf)[ctx.m_pos - 1] = 0;
	//printf("[FT] buflen: %d\n", strlen((char *)ctx.m_buf));
	//printf("[FT] mylen: %d\n", ctx.m_pos);
	if (ctx.m_buf)
	{
		//((char *)ctx.m_buf)[ctx.m_pos] = 0;
#ifdef DUMP_RESPONSES
		printf("[FT] response: %s\n", ctx.m_buf);
#endif
	}
	if (pRespCode)
	{
		*pRespCode = responseCode;
	}
	if (ret != CURLE_OK || responseCode != 200)
	{
		if (ctx.m_buf)
		{
			free(ctx.m_buf);
		}
		curl_easy_cleanup(curl);
		return false;
	}

	curl_easy_cleanup(curl);

	*ppRespBuf = ctx.m_buf;
	*pRespBufSize = ctx.m_pos;

	return true;
}

// Direct test
void directTest()
{
	char *resp = NULL;
	uint32_t respsz = 0;

	const char *ctx = "{\"context\": {\"client\": {\"hl\": \"en\", \"gl\": \"US\", \"clientName\": \"WEB\", \"clientVersion\": \"2.20250821.07.00\", \"utcOffsetMinutes\": 0}, \"request\": {}, \"user\": {}}, \"continuation\": \"0ofMyAOIAhpeQ2lrcUp3b1lWVU55UjFSVFdGZE5hVUZYYjFCc2IzZFRkVTFXV2pSUkVnc3lPVmxpV1hSclRWcGpSUm9UNnFqZHVRRU5DZ3N5T1ZsaVdYUnJUVnBqUlNBQ01BQSUzRCilnZW--6mPAzAAQAJKZwgAGAAgAEoIEAAYACAAMABQidiPv_upjwNYA3gAogEAqgEMEAAaACIAKgQIABAAsAEAwAEAyAGJ2I-_-6mPA-IBCwi54LnFBhDZluxD6AEA-AEAiAIAkAIAmgILCLngucUGENmW7ENQzqzFvvupjwNY0oP7w8-pjwOCAQQIBBgBiAEAoAGorJS_-6mPA7oBAggK0AG34LnFBg%3D%3D\"}";

	FTPosterCb(
		"https://www.youtube.com/youtubei/v1/live_chat/get_live_chat?key=AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8&prettyPrint=false",
		(void *)ctx,
		strlen(ctx),
		NULL,
		0,
		(void **)&resp,
		&respsz,
		NULL,
		malloc,
		free,
		realloc);
}

int main()
{
	void *ctx = NULL;
	FTItem *pres = NULL;
	const char *cont = NULL;
	std::string contStor;

	curl_global_init(CURL_GLOBAL_ALL);

	int32_t ret = ftInit(malloc, free, realloc, FTGetterCb, FTPosterCbCurl);
	ftSetRegion(FT_REGION_EN_US);
	printf("ftInit: 0x%08X\n", ret);

#ifdef TEST_SEARCH
	ret = ftParseSearch(&ctx, "Test", NULL, FT_ITEM_TYPE_VIDEO, FT_SORT_RELEVANCE, FT_DATE_ALL, FT_FEATURE_NONE, FT_DURATION_ALL, &pres, &cont);
	printf("ftParseSearch: 0x%08X\n", ret);

	if (cont)
		contStor = cont;

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);

#ifdef TEST_SEARCH_CONTINUATION
	ret = ftParseSearch(&ctx, NULL, contStor.c_str(), FT_ITEM_TYPE_VIDEO, FT_SORT_RELEVANCE, FT_DATE_ALL, FT_FEATURE_NONE, FT_DURATION_ALL, &pres, NULL);
	printf("ftParseSearch: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
#endif
#endif

#ifdef TEST_VIDEO
	ret = ftParseVideo(&ctx, "02hnaZL_OCw", FT_VIDEO_VOD_QUALITY_720P, FT_AUDIO_VOD_QUALITY_MEDIUM, &pres);
	printf("ftParseVideo: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
#endif
	
#ifdef TEST_COMMENTS
	ret = ftParseComments(&ctx, "jeFZSnA7lLc", NULL, FT_COMMENT_SORT_TOP, &pres, &cont);
	printf("ftParseComments: 0x%08X\n", ret);

	contStor = pres[0].commentItem->replyContinuation;

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);

#ifdef TEST_COMMENTS_CONTINUATION
	ret = ftParseComments(&ctx, "jeFZSnA7lLc", contStor.c_str(), FT_COMMENT_SORT_TOP, &pres, &cont);
	printf("ftParseComments: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
#endif
#endif

#ifdef TEST_COMMENTS_LIVE
	ret = ftParseLiveComments(&ctx, "6RIxtZsvyIY", 6, &pres);
	printf("ftParseLiveComments: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
#endif

#ifdef DIRECT_TEST
	directTest();
#endif

	ftTerm();

	printf("[FT] Test completed\n");

	return 0;
}