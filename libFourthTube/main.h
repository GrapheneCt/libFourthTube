#pragma once

#include <stdint.h>

#include "ftube.h"

//#define FT_DUMP_PRETTY_RESPONSE
//#define FT_DUMP_BUFFER_USAGE

// ----- Platform-specific definitions start -----
#ifdef FT_BUILD
#define FT_EXPORT __declspec(dllexport)
#if defined(__SNC__)
#include <kernel.h>
#define ft_snprintf sceClibSnprintf
#define ft_strnlen sceClibStrnlen
#define ft_memset sceClibMemset
#define ft_memcpy sceClibMemcpy
#define ft_strcmp sceClibStrcmp
#define ft_strstr sceClibStrstr
#define ft_strchr sceClibStrchr
#define strcmp sceClibStrcmp
#ifdef _DEBUG
#define ft_printf sceClibPrintf
#else
#define ft_printf
#endif
#elif defined(WIN32)
#include <string.h>
#define ft_snprintf snprintf
#define ft_strnlen strnlen
#define ft_memset memset
#define ft_memcpy memcpy
#define ft_strcmp strcmp
#define ft_strstr strstr
#define ft_strchr strchr
#ifdef _DEBUG
#define ft_printf printf
#else
#define ft_printf
#endif
#endif
#endif
// ----- Platform-specific definitions end -----

#define ARDUINOJSON_DEFAULT_NESTING_LIMIT	50
#include "ArduinoJson.hpp"
#include "base64.h"

#define FT_VIDEO_BUFFER_SIZE_COEFF			0.4f
#define FT_COMMENTS_BUFFER_SIZE_COEFF		0.7f
#define FT_LIVE_COMMENTS_BUFFER_SIZE_COEFF	0.45f

#define FT_CONTEXT_MAX_BUFFERS				2

int32_t _ftSetRegion(FTRegion region);

int32_t _ftInit(FTAllocator allocator, FTDeallocator deallocator, FTReallocator reallocator, FTGetter getter, FTPoster poster);

int32_t _ftTerm();

int32_t _ftParseSearch(void **ppContext, const char *request, const char *continuation, FTItemType type, FTSort sort, FTDate date, FTFeature feature, FTDuration dur, FTItem **firstItem, const char **continuationToken);

int32_t _ftParseVideo(void **ppContext, const char *videoId, uint32_t videoQuality, uint32_t audioQuality, FTItem **item);

int32_t _ftParseComments(void **ppContext, const char *videoId, const char *continuation, FTCommentSort sort, FTItem **item, const char **continuationToken);

int32_t _ftParseLiveComments(void **ppContext, const char *videoId, uint32_t maxNum, FTItem **firstItem);

int32_t _ftCleanup(void *pContext);

class FTMemoryHandler
{
public:

	void *operator new(size_t sz)
	{
		if (!s_allocate)
		{
			return NULL;
		}

		return s_allocate(sz);
	}

	void operator delete(void *pMem, size_t sz)
	{
		if (!s_free)
		{
			return;
		}

		s_free(pMem);
	}

	void operator delete(void *pMem)
	{
		if (!s_free)
		{
			return;
		}

		s_free(pMem);
	}

	void *operator new[](size_t sz)
	{
		if (!s_allocate)
		{
			return NULL;
		}

		return s_allocate(sz);
	}

	void operator delete[](void *pMem, size_t sz)
	{
		if (!s_free)
		{
			return;
		}

		s_free(pMem);
	}

	void operator delete[](void *pMem)
	{
		if (!s_free)
		{
			return;
		}

		s_free(pMem);
	}

	static FTAllocator s_allocate;
	static FTDeallocator s_free;
	static FTReallocator s_reallocate;
};

class FTString : public FTMemoryHandler
{
public:

	FTString() : m_ptr(NULL), m_length(0)
	{

	}

	~FTString()
	{
		if (m_ptr)
		{
			operator delete(m_ptr);
		}
	}

	FTString& operator=(const char *c)
	{
		Clear();
		Copy(c, ft_strnlen(c, 0xFFFFFFFF));

		return *this;
	}

	FTString& operator+=(const char *c)
	{
		uint32_t len1 = m_length;
		uint32_t len2 = ft_strnlen(c, 0xFFFFFFFF);

		char *newptr = NULL;

		newptr = static_cast<char *>(operator new(sizeof(char) * (len1 + len2 + 1)));

		if (m_ptr)
		{
			ft_memcpy(newptr, m_ptr, len1);
		}
		ft_memcpy(&newptr[len1], c, len2 + 1);

		Clear();
		Initialize(newptr, static_cast<uint32_t>(len1 + len2), true);

		return *this;
	}

	void Append(const char *s, uint32_t length)
	{
		if (!s || length == 0)
		{
			return;
		}

		uint32_t len1 = m_length;
		uint32_t len2 = length;

		char *newptr = static_cast<char *>(operator new(sizeof(char) * (len1 + len2 + 1)));

		if (m_ptr)
		{
			ft_memcpy(newptr, m_ptr, len1);
		}
		ft_memcpy(&newptr[len1], s, len2);

		newptr[len1 + len2] = 0;

		Clear();
		Initialize(newptr, static_cast<uint32_t>(len1 + len2), true);
	}

	uint32_t GetLength() const
	{
		return m_length;
	}

	const char *GetRawString() const
	{
		return m_ptr;
	}

private:

	void Copy(const char *c, uint32_t length)
	{
		if (!length)
		{
			return;
		}

		m_length = length;

		m_ptr = static_cast<char *>(operator new(sizeof(char) * (length + 1)));

		ft_memcpy(m_ptr, c, length);
		m_ptr[length] = 0;
	}

	void Clear()
	{
		if (IsEmpty())
		{
			return;
		}

		operator delete(m_ptr);

		SetEmpty();
	}

	void Initialize(const char *c, uint32_t length, bool usePtr)
	{
		if (!length)
		{
			SetEmpty();
			return;
		}

		if (!usePtr)
		{
			Copy(c, length);
		}
		else
		{
			m_ptr = const_cast<char *>(c);
			m_length = length;
			m_ptr[length] = 0;
		}
	}

	void SetEmpty()
	{
		m_ptr = NULL;
		m_length = 0;
	}

	bool IsEmpty() const
	{
		return (m_ptr == NULL);
	}

	char *m_ptr;
	uint32_t m_length;
};

class FTJsonDocument : public FTMemoryHandler, public ArduinoJson::JsonDocument
{
public:

	FTJsonDocument(char *buf, size_t caps) : ArduinoJson::JsonDocument(buf, caps)
	{

	}
};

#define k_header "<?xml version=\"1.0\" encoding=\"utf-8\"?><MPD xmlns=\"urn:mpeg:dash:schema:mpd:2011\" profiles=\"urn:mpeg:dash:profile:full:2011\" minBufferTime=\"PT%gS\" type=\"static\" mediaPresentationDuration=\"PT%gS\"><Period>"
#define k_tail "</Period></MPD>"

#define k_asetHeader "<AdaptationSet id=\"%u\" mimeType=\"%s\" startWithSAP=\"1\" subsegmentAlignment=\"true\""
#define k_asetTail "</AdaptationSet>"

#define k_repVideoHeader "<Representation id=\"%s\" codecs=\"%s\" bandwidth=\"%u\" width=\"%u\" height=\"%u\" maxPlayoutRate=\"1\" frameRate=\"%u\">"
#define k_repAudioHeader "<Representation id=\"%s\" codecs=\"%s\" bandwidth=\"%u\"><AudioChannelConfiguration schemeIdUri=\"urn:mpeg:dash:23003:3:audio_channel_configuration:2011\" value=\"2\"/>"
#define k_repTail "<BaseURL>%s</BaseURL><SegmentBase indexRange=\"%s-%s\"><Initialization range=\"%s-%s\"/></SegmentBase></Representation>"

class FTVODDashManifest : public FTMemoryHandler
{
public:

	FTVODDashManifest()
	{
		asetId = 0;
	}

	~FTVODDashManifest()
	{

	}

	void Init(float minBufferTimeSec, double durationSec)
	{
		char tmp[256];
		ft_snprintf(tmp, sizeof(tmp), k_header, minBufferTimeSec, durationSec);
		buffer = tmp;
	}

	void BeginAdaptationSet(const char *mimeType, bool video)
	{
		char tmp[128];
		ft_snprintf(tmp, sizeof(tmp), k_asetHeader, asetId, mimeType);
		asetId++;
		buffer += tmp;
		if (video)
		{
			buffer += " scanType=\"progressive\">";
		}
		else
		{
			buffer += ">";
		}
	}

	void EndAdaptationSet()
	{
		buffer += k_asetTail;
	}

	void AddRepresentation(const char *id, const char *codec, uint32_t bandwidth,
		const char *url, const char *indexBegin, const char *indexEnd, const char *initBegin, const char *initEnd,
		uint32_t width = 0, uint32_t height = 0, uint32_t frameRate = 0)
	{
		if (!id || !url || !codec || !bandwidth || !indexEnd || !indexEnd || !initBegin || !initEnd)
		{
			return;
		}

		uint32_t tmpsz = ft_strnlen(url, 0xFFFFFFFF) + 256;
		char *tmp = (char *)operator new(tmpsz);
		if (width && height && frameRate)
		{
			ft_snprintf(tmp, tmpsz, k_repVideoHeader, id, codec, bandwidth, width, height, frameRate);
		}
		else
		{
			ft_snprintf(tmp, tmpsz, k_repAudioHeader, id, codec, bandwidth);
		}
		buffer += tmp;

		ft_snprintf(tmp, tmpsz, k_repTail, url, indexBegin, indexEnd, initBegin, initEnd);
		buffer += tmp;

		operator delete(tmp);
	}

	void Finish()
	{
		buffer += k_tail;
	}

	const char *GetBuffer()
	{
		return buffer.GetRawString();
	}

private:

	FTString buffer;
	uint32_t asetId;
};

class FTContext : public FTMemoryHandler
{
public:

	class Buffer : public FTMemoryHandler
	{
	public:

		Buffer(uint32_t docSz, uint32_t respSz) : m_docBuffer((char *)operator new(docSz)), m_respDoc(m_docBuffer, docSz)
		{
			m_docBufferSz = docSz;
			m_respBuffer = (char *)operator new(respSz);
			m_respBufferSz = respSz;
		}

		Buffer(void *buf, uint32_t bufSz) : m_docBuffer((char *)operator new(1)), m_respDoc(m_docBuffer, 1)
		{
			m_docBufferSz = 1;
			m_respBuffer = (char *)buf;
			m_respBufferSz = bufSz;
		}

		Buffer(uint32_t docSz, void *respBuf, uint32_t respSz) : m_docBuffer((char *)operator new(docSz)), m_respDoc(m_docBuffer, docSz)
		{
			m_docBufferSz = docSz;
			m_respBuffer = (char *)respBuf;
			m_respBufferSz = respSz;
		}

		~Buffer()
		{
			operator delete(m_docBuffer);
			operator delete(m_respBuffer);
		}

	private:

		char *m_docBuffer;

	public:

		FTJsonDocument m_respDoc;
		char *m_respBuffer;
		uint32_t m_respBufferSz;

	private:

		uint32_t m_docBufferSz;
	};

	FTContext()
	{
		for (int i = 0; i < FT_CONTEXT_MAX_BUFFERS; i++)
		{
			m_buffers[i] = NULL;
		}
		m_bufferCount = 0;
		m_vodManifest = NULL;
		m_firstItem = NULL;
	}

	~FTContext()
	{
		for (int i = 0; i < m_bufferCount; i++)
		{
			if (m_buffers[i])
			{
				delete m_buffers[i];
			}
		}

		if (m_vodManifest)
		{
			delete m_vodManifest;
		}

		FTItem *item = m_firstItem;
		while (item)
		{
			FTItem *itemToDel = item;
			item = item->next;

			if (itemToDel->videoItem)
			{
				if (itemToDel->type == FT_ITEM_TYPE_COMMENT)
				{
					((FTString *)itemToDel->commentItem->reserved)->~FTString();
				}
				operator delete(itemToDel->videoItem);
			}
			operator delete(itemToDel);
		}
	}

	Buffer *CreateBuffer(uint32_t docSz, uint32_t respSz)
	{
		Buffer *ret = NULL;

		if (m_bufferCount >= FT_CONTEXT_MAX_BUFFERS)
		{
			return NULL;
		}

		ret = new Buffer(docSz, respSz);
		if (!ret)
		{
			return NULL;
		}

		m_buffers[m_bufferCount] = ret;
		m_bufferCount++;

		return ret;
	}

	Buffer *CreateBufferEx(uint32_t docSz, void *buf, uint32_t bufSz)
	{
		Buffer *ret = NULL;

		if (m_bufferCount >= FT_CONTEXT_MAX_BUFFERS)
		{
			return NULL;
		}

		if (docSz)
		{
			ret = new Buffer(docSz, buf, bufSz);
		}
		else
		{
			ret = new Buffer(buf, bufSz);
		}
		if (!ret)
		{
			return NULL;
		}

		m_buffers[m_bufferCount] = ret;
		m_bufferCount++;

		return ret;
	}

	FTVODDashManifest *CreateVODDashManifest()
	{
		m_vodManifest = new FTVODDashManifest();
		return m_vodManifest;
	}

	FTItem *CreateItem(FTItemType type, bool first = false)
	{
		FTItem *ret = NULL;

		ret = (FTItem *)operator new(sizeof(FTItem));
		if (!ret)
		{
			return NULL;
		}

		ret->type = type;
		ret->next = NULL;

		switch (type)
		{
		case FT_ITEM_TYPE_VIDEO:
			ret->videoItem = (FTItemVideo *)operator new(sizeof(FTItemVideo));
			ft_memset(ret->videoItem, 0, sizeof(FTItemVideo));
			break;
		case FT_ITEM_TYPE_PLAYLIST:
			ret->playlistItem = (FTItemPlaylist *)operator new(sizeof(FTItemPlaylist));
			ft_memset(ret->playlistItem, 0, sizeof(FTItemPlaylist));
			break;
		case FT_ITEM_TYPE_CHANNEL:
			ret->channelItem = (FTItemChannel *)operator new(sizeof(FTItemChannel));
			ft_memset(ret->channelItem, 0, sizeof(FTItemChannel));
			break;
		case FT_ITEM_TYPE_COMMENT:
			ret->commentItem = (FTItemComment *)operator new(sizeof(FTItemComment));
			ft_memset(ret->commentItem, 0, sizeof(FTItemComment));
			break;
		default:
			operator delete(ret);
			ret = NULL;
			break;
		}

		if (first)
		{
			m_firstItem = ret;
		}

		return ret;
	}

	Buffer *GetBuffer(uint32_t idx)
	{
		if (idx >= m_bufferCount)
		{
			return NULL;
		}

		return m_buffers[idx];
	}

private:

	Buffer *m_buffers[FT_CONTEXT_MAX_BUFFERS];
	uint8_t m_bufferCount;
	FTVODDashManifest *m_vodManifest;
	FTItem *m_firstItem;
};