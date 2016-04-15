#ifndef _HV_API_BASE_H
#define _HV_API_BASE_H

#include <stdint.h>
#include "swbasetype.h"
#include "hv_opt_base.h"

#define HV_API extern "C"


typedef void* HV_HANDLE;
typedef int HRESULT;
typedef char* PSTR;

typedef unsigned short WORD;
#define E_HV_INVALID_HANDLE NULL


HV_API HV_HANDLE OpenHv(const PSTR pcIP);

HV_API HRESULT CloseHv(HV_HANDLE hHandle);

HV_API HRESULT HvIsConnected(HV_HANDLE hHandle, DWORD32 *piStatus);

HV_API HRESULT SetHvCallBack(HV_HANDLE hHandle, void *pFunction,
                            void *pFirstParameter, WORD16 wVideoID, WORD16 wStream);

HV_API HRESULT GetHistoryVideo(HV_HANDLE hHandle, QWORD64 dw64TimeMs, DWORD32 dwTimeSliceS);
HV_API HRESULT HV_GetPlateInfo(HV_HANDLE hHandle, char** ppszPlateInfo);

HV_API HRESULT HV_ForceSend(HV_HANDLE hHandle);
HV_API HRESULT HV_ForceSendEx(HV_HANDLE hHandle, int iVIdeoID=-1);

HV_API HRESULT ResetHv(HV_HANDLE hHandle, DWORD32 dwResetMode);

HV_API HRESULT GetHvTime(HV_HANDLE hHandle, QWORD64 *pdw64TimeMs);
HV_API HRESULT SetHvTime(HV_HANDLE hHandle, QWORD64 dwTimeMs );
HV_API HRESULT SetHvIP(HV_HANDLE hHandle, DWORD32 dwHvIP, DWORD32 dwMask,  DWORD32 dwGatewayIP);

//HV_API HRESULT SetFiltCondition(HV_HANDLE hHandle, void* pFunction,
   //                             WORD16 wVideoID, WORD16 wStream);

typedef int (*PLATE_NO_CALLBACK)(void *pFirstParameter, DWORD32 dwCarID,
PSTR pcPlateNo, QWORD64 dw64TimeMs);


typedef int (*BIG_IMAGE_CALLBACK)(void* pFirstParameter, DWORD32 dwCarID, WORD16 wImageWidth, WORD16 wImageHigh,
                                BYTE8 bType, WORD16 wSize, PBYTE8 pbImage, WORD16 wImageID, WORD16 wHighImgFlag,
                                WORD16 wPlateLeftTop, WORD16 wPlateRightBottom, QWORD64 dwTimeMs);


typedef int (*SMALL_IMAGE_CALLBACK)(void* pFirstParameter, DWORD32 dwCarID, WORD16 wImageWidth, WORD16 wImageHigh,
                                    BYTE8 bType, WORD16 wSize, BYTE8 *pbImage, QWORD64 dwTimes);

typedef int (*BINARY_IMAGE_CALLBACK)(void* pFirstParameter, DWORD32 dwCarID, WORD16 wImageWidth, WORD16 wImageHigh,
                                    BYTE8 bType, WORD16 wSize, BYTE8 *pbImage, QWORD64 dwTimeMs);

typedef int (*VIDEO_CALLBACK)(void* pFirstParameter, WORD16 wVideoID, DWORD32 dwSize, BYTE8 *pbImage);

typedef int (*AVI_CALLBACK)(void * pFirstParameter, WORD16 wVideoID, char *pszAviFile);

typedef int (*VIDEO_HISTORY_CALLBACK)(void *pFirstParameter, WORD16 wVideoID, DWORD32 dwSize, BYTE8 *pbImage, QWORD64 dwTimeMs);

typedef int (*CARINFO_BEGIN_CALLBACK)(void *pFirstParameter, DWORD32 dwCarID);

typedef int (*CARINFO_END_CALLBACK)(void *pFirstParameter, DWORD32 dwCarID);

typedef int (*STREAM_CALLBACK)(void *pFirstParameter, DWORD32 iLen, PSTR pString);

typedef int (*TRIGGERINFO_CALLBACK)(void *pFirstParameter, DWORD32 dwTrackNum, DWORD32 dwTriggerNum, DWORD32 dwCarType,
            DWORD32  dwCarSpeed, QWORD64 dwTimeMs);

typedef int (*CAR_IN_LEFT_CALLBACK)(void *pFirstParameter, DWORD32 dwCarID, WORD16 wFlag, QWORD64 dwTimeMs);

typedef int (*DEBUG_STR_CALLBACK)(void *pFirstParameter, PSTR pString);


HV_API void HV_WriteLog(const char *pszLog);

HV_API HRESULT Yuv2BMP(BYTE8 *pbDest, int iDestBufLen, int *piDestLen, BYTE8 *pbSrc, int iSrcWidth, int iSrcHeight);

HV_API HRESULT Yuv2JPG(BYTE8 *pbDest, int iDestBufLen, int *piDestLen, BYTE8 *pbSrc, int iSrcWidth, int iSrcHeight);

HV_API HRESULT SearchHVDeviceCount(DWORD32 *pdwCount);
HV_API HRESULT GetHVDeviceAddr(int iIndex, QWORD64 *pdw64MacAddr, DWORD32 *pdwIP, DWORD32 *pdwMask, DWORD32 *pdwGateway);
HV_API HRESULT SetHvIPFromMac(QWORD64 dw64MacAddr, DWORD32 dwIP, DWORD32 dwMask,DWORD32 dwGateway);

//active Link
HV_API HRESULT OpenHvServer(WORD wPort, int iCount);
HV_API HRESULT CloseHvServer();

typedef int (*CONNECT_EVENT)(void *pFirstParameter, HV_HANDLE hHandle, char *pszIp, WORD wPort);
typedef int (*DISCONNECT_EVENT)(void *pFirstParameter, HV_HANDLE hHandle, char *pszIp, WORD wPort );
HV_API HRESULT SetHvServerEvent(void *pFirstParameter, void *pEvent, int iEventType);


#endif
