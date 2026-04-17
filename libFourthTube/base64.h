#pragma once

#include <stdint.h>

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#define FT_BASE64_ENC_BUF_SIZE(_size) (((_size) + 2) / 3 * 4 + 1)
#define FT_BASE64_DEC_BUF_SIZE(_size) ((_size) / 4 * 3 + 1)

int32_t __ftBase64Encoder(char *pDst, const void *pSrc, uint32_t uiSiz );

int32_t __ftBase64Decoder(void *pDst, const char *pSrc, uint32_t uiSize);

void __ftBase64StringDeformat(char *str, uint32_t len);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif