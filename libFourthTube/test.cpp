#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "curl/curl.h"
#include "curl/easy.h"

#include "ftube.h"

int s_dump_responses = 0;

bool FTGetterCb(const char *curl, void **ppRespBuf, size_t *pRespBufSize, const char **pHeaders, uint32_t headerNum, int32_t *pRespCode, FTAllocator ftalloc, FTDeallocator ftdealloc, FTReallocator ftrealloc)
{
	std::string url = curl;
	printf("request: %s\n", url.c_str());

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
		printf("fail: %d\n", resp->status);
		return false;
	}
	else
	{
		//if (s_dump_responses)
			//printf("response: %s\n", resp->body.c_str());

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
	printf("request: %s\n", url.c_str());
	printf("body: %s\n", pBuf);

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
		printf("fail: %d\n", resp->status);
		return false;
	}
	else
	{
		if (s_dump_responses == 1)
		{
			printf("response: %s\n", resp->body.c_str());
			s_dump_responses = 10;
		}
		else
			s_dump_responses++;

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
	printf("[FT] POST request: %s\n", url);
	printf("[FT] POST body: %s\n", pBuf);

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
	//curl_easy_setopt(curl, CURLOPT_PROXY, "socks5://192.168.3.101:12334");

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
		printf("[FT] response: %s\n", ctx.m_buf);
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

void test()
{
	char *resp = NULL;
	uint32_t respsz = 0;
	// NO 403 Gives high res HLS for VODs but it doesn't work on Vita 
	//const char *ctx = "{\"videoId\": \"1zpCNefHbC0\", \"context\": {\"client\": {\"hl\": \"en\",\"gl\": \"US\",\"clientName\": \"IOS\",\"clientVersion\": \"20.10.4\",\"deviceMake\": \"Apple\",\"deviceModel\": \"iPhone16, 2\",\"osName\": \"iPhone\",\"userAgent\": \"com.google.ios.youtube/20.10.4 (iPhone16,2; U; CPU iOS 18_3_2 like Mac OS X;)\", \"osVersion\": \"18.3.2.22D82\", \"visitorData\": \"CgtXa1hoT0xQa3JyRSiqvOrEBjIKCgJSVRIEGgAgYjoKIKuf8PmnxafNaFi975qhvNPSurcB\"}}, \"playbackContext\": {\"contentPlaybackContext\": {\"signatureTimestamp\": \"0\"}}}";
	// NO 403 Gives good HLS for livestreams (confirmed OK on Vita), but only SD for VODs
	//const char *ctx = "{\"videoId\": \"viYCSbidzDg\", \"context\": {\"client\": {\"hl\": \"en\",\"gl\": \"US\",\"clientName\": \"MWEB\",\"clientVersion\": \"2.20250311.03.00\",\"userAgent\": \"Mozilla/5.0 (iPad; CPU OS 16_7_10 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1,gzip(gfe)\", \"visitorData\": \"CgtXa1hoT0xQa3JyRSiqvOrEBjIKCgJSVRIEGgAgYjoKIKuf8PmnxafNaFi975qhvNPSurcB\"}}, \"playbackContext\": {\"contentPlaybackContext\": {\"signatureTimestamp\": \"0\"}}}";

	//const char *ctx = "{\"videoId\": \"QGQQNfdy8Y0\", \"context\": {\"client\": {\"hl\": \"en\",\"gl\": \"US\",\"clientName\": \"ANDROID\",\"clientVersion\": \"19.35.36\",\"userAgent\": \"Mozilla/5.0 (iPad; CPU OS 16_7_10 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1,gzip(gfe)\", \"visitorData\": \"CgtXa1hoT0xQa3JyRSiqvOrEBjIKCgJSVRIEGgAgYjoKIKuf8PmnxafNaFi975qhvNPSurcB\"}}, \"playbackContext\": {\"contentPlaybackContext\": {\"signatureTimestamp\": \"0\"}}}";

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

	/*
	ret = ftParseSearch(&ctx, "Vtuber", NULL, FT_ITEM_TYPE_VIDEO, FT_SORT_RELEVANCE, FT_DATE_ALL, FT_FEATURE_LIVE, FT_DURATION_ALL, &pres, &cont);
	printf("ftParseSearch: 0x%08X\n", ret);

	if (cont)
	std::string ccont = cont;

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
	*/

	/*
	ret = ftParseSearch(&ctx, NULL, ccont.c_str(), FT_ITEM_TYPE_VIDEO, FT_SORT_RELEVANCE, FT_DATE_ALL, FT_FEATURE_NONE, FT_DURATION_ALL, &pres, NULL);
	printf("ftParseSearch: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
	*/

	/*
	ret = ftParseVideo(&ctx, "02hnaZL_OCw", FT_VIDEO_VOD_QUALITY_720P, FT_AUDIO_VOD_QUALITY_MEDIUM, &pres);
	printf("ftParseVideo: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
	*/
	
	ret = ftParseComments(&ctx, "jeFZSnA7lLc", NULL, FT_COMMENT_SORT_TOP, &pres, &cont);
	printf("ftParseComments: 0x%08X\n", ret);

	/*contStor = pres[0].commentItem->replyContinuation;

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);

	ret = ftParseComments(&ctx, "jeFZSnA7lLc", contStor.c_str(), FT_COMMENT_SORT_TOP, &pres, &cont);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
	*/

	/*
	ret = ftParseLiveComments(&ctx, "6RIxtZsvyIY", &pres);
	printf("ftParseLiveComments: 0x%08X\n", ret);

	ret = ftCleanup(ctx);
	printf("ftCleanup: 0x%08X\n", ret);
	*/

	//test();

	return 0;
}