#include <stdint.h>
#include <stddef.h>
#include "base64.h"

static const signed char s_base64decTbl[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
static const char s_base64encTbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int32_t __ftBase64Encoder(char *pDst, const void *pSrc, uint32_t uiSize)
{
	int32_t ret=0;
	const unsigned char *pcSrc = (const unsigned char *)pSrc;
	uint32_t uiData;

	if (!pDst || !pSrc)
	{
		return -1;
	}

	while(uiSize > 0)
	{
		uiData = (uint32_t)(pcSrc[0] << 16);
		if (uiSize > 1)
		{
			uiData |= (uint32_t)(pcSrc[1] << 8);
		}
		if (uiSize > 2)
		{
			uiData |= pcSrc[2];
		}
		pDst[0] = s_base64encTbl[(uiData >> 18) & 0x3f];
		pDst[1] = s_base64encTbl[(uiData >> 12) & 0x3f];
		pDst[2] = (char)((uiSize > 1) ? s_base64encTbl[(uiData >> 6) & 0x3f] : '=');
		pDst[3] = (char)((uiSize > 2) ? s_base64encTbl[(uiData >> 0) & 0x3f] : '=');
		pDst+=4;
		ret+=4;
		pcSrc+=3;
		if (uiSize > 3)
		{
			uiSize -= 3;
		} 
		else
		{
			uiSize = 0;
		}
	}

	pDst[0] = '\0';	/* ASCIZ*/

	return ret;
}

int32_t __ftBase64Decoder(void *pDst, const char *pSrc, uint32_t uiSize)
{
	int32_t ret = 0;
	uint32_t uiData;
	unsigned char *pcDst = (unsigned char *)pDst;
	char scData;

	if ((pDst == NULL) || (pSrc == NULL))
	{
		ret = -1;
		goto error;
	}

	while (uiSize >= 4)
	{
		if (0 > (scData = s_base64decTbl[(int32_t)pSrc[0]]))
		{
			ret = -1;
			goto error;
		}
		uiData = ((uint32_t)scData << 18);
		if (0 > (scData = s_base64decTbl[(int32_t)pSrc[1]]))
		{
			ret = -1;
			goto error;
		}
		uiData |= ((uint32_t)scData << 12);
		ret++;
		*pcDst++ = (unsigned char)(uiData >> 16);

		if (pSrc[2] == '=')
		{
			break;
		}
		if (0 > (scData = s_base64decTbl[(int32_t)pSrc[2]]))
		{
			ret = -1;
			goto error;
		}
		uiData |= ((uint32_t)scData << 6);
		ret++;
		*pcDst++ = (unsigned char)(uiData >> 8);

		if (pSrc[3] == '=')
		{
			break;
		}
		if (0 > (scData = s_base64decTbl[(int32_t)pSrc[3]]))
		{
			ret = -1;
			goto error;
		}
		uiData |= (scData & 0xFF);
		ret++;
		*pcDst++ = (unsigned char)(uiData >> 0);

		pSrc += 4;
		uiSize -= 4;
	}
	pcDst[0] = '\0';
	return ret;

error:

	return ret;
}

void __ftBase64StringDeformat(char *str, uint32_t len)
{
	char *src = str;
	char *dst = str;

	while (*src && (src - str) < (ptrdiff_t)len)
	{
		if (src[0] == '%' &&
			src[1] && src[2] &&
			(src[1] == '3') &&
			(src[2] == 'D' || src[2] == 'd')) {

			*dst++ = '=';
			src += 3;
		}
		else {
			*dst++ = *src++;
		}
	}

	*dst = '\0';
}
