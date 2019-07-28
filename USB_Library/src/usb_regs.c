#include "usb_regs.h"
/* USB include */
#include "usb_type.h"



void SetCNTR(u16 wRegValue)
{
    _SetCNTR(wRegValue);
}

u16 GetCNTR(void)
{
    return _GetCNTR();
}

void SetISTR(u16 wRegValue)
{
    _SetISTR(wRegValue);
}

u16 GetISTR(void)
{
    return _GetISTR();
}

u16 GetFNR(void)
{
    return _GetFNR();
}

void SetDADDR(u16 wRegValue)
{
    _SetDADDR(wRegValue);
}

u16 GetDADDR(void)
{
    return _GetDADDR();
}

void SetBTABLE(u16 wRegValue)
{
    _SetBTABLE(wRegValue);
}

u16 GetBTABLE(void)
{
    return _GetBTABLE();
}

void SetENDPOINT(u8 bEpNum, u16 wRegValue)
{
    _SetENDPOINT(bEpNum, wRegValue);
}

u16 GetENDPOINT(u8 bEpNum)
{
    return _GetENDPOINT(bEpNum);
}

void SetEPType(u8 bEpNum, u16 wType)
{
    _SetEPType(bEpNum, wType);
}

u16 GetEPType(u8 bEpNum)
{
    return _GetEPType(bEpNum);
}

void SetEPTxStatus(u8 bEpNum, u16 wState)
{
    _SetEPTxStatus(bEpNum, wState);
}

void SetEPRxStatus(u8 bEpNum, u16 wState)
{
    _SetEPRxStatus(bEpNum, wState);
}

void SetDouBleBuffEPStall(u8 bEpNum, u8 bDir)
{
    u16 Endpoint_DTOG_Status;

    Endpoint_DTOG_Status = GetENDPOINT(bEpNum);

    /* OUT double buffered endpoint */
    if (bDir == EP_DBUF_OUT) {
        _SetENDPOINT(bEpNum, Endpoint_DTOG_Status & ~EPRX_DTOG1);
    /* IN double buffered endpoint */
    } else if (bDir == EP_DBUF_IN) {
        _SetENDPOINT(bEpNum, Endpoint_DTOG_Status & ~EPTX_DTOG1);
    }
}

u16 GetEPTxStatus(u8 bEpNum)
{
    return _GetEPTxStatus(bEpNum);
}

u16 GetEPRxStatus(u8 bEpNum)
{
    return _GetEPRxStatus(bEpNum);
}

void SetEPTxValid(u8 bEpNum)
{
    _SetEPTxStatus(bEpNum, EP_TX_VALID);
}

void SetEPRxValid(u8 bEpNum)
{
    _SetEPRxStatus(bEpNum, EP_RX_VALID);
}

void SetEP_KIND(u8 bEpNum)
{
    _SetEP_KIND(bEpNum);
}

void ClearEP_KIND(u8 bEpNum)
{
    _ClearEP_KIND(bEpNum);
}

void Clear_Status_Out(u8 bEpNum)
{
    _ClearEP_KIND(bEpNum);
}

void Set_Status_Out(u8 bEpNum)
{
    _SetEP_KIND(bEpNum);
}

void SetEPDoubleBuff(u8 bEpNum)
{
    _SetEP_KIND(bEpNum);
}

void ClearEPDoubleBuff(u8 bEpNum)
{
    _ClearEP_KIND(bEpNum);
}

u16 GetTxStallStatus(u8 bEpNum)
{
    return _GetTxStallStatus(bEpNum);
}

u16 GetRxStallStatus(u8 bEpNum)
{
    return _GetRxStallStatus(bEpNum);
}

void ClearEP_CTR_RX(u8 bEpNum)
{
    _ClearEP_CTR_RX(bEpNum);
}

void ClearEP_CTR_TX(u8 bEpNum)
{
    _ClearEP_CTR_TX(bEpNum);
}

void ToggleDTOG_RX(u8 bEpNum)
{
    _ToggleDTOG_RX(bEpNum);
}

void ToggleDTOG_TX(u8 bEpNum)
{
    _ToggleDTOG_TX(bEpNum);
}

void ClearDTOG_RX(u8 bEpNum)
{
    _ClearDTOG_RX(bEpNum);
}

void ClearDTOG_TX(u8 bEpNum)
{
    _ClearDTOG_TX(bEpNum);
}

void SetEPAddress(u8 bEpNum, u8 bAddr)
{
    _SetEPAddress(bEpNum, bAddr);
}

u8 GetEPAddress(u8 bEpNum)
{
    return _GetEPAddress(bEpNum);
}

void SetEPTxAddr(u8 bEpNum, u16 wAddr)
{
    _SetEPTxAddr(bEpNum, wAddr);
}

void SetEPRxAddr(u8 bEpNum, u16 wAddr)
{
    _SetEPRxAddr(bEpNum, wAddr);
}

u16 GetEPTxAddr(u8 bEpNum)
{
    return _GetEPTxAddr(bEpNum);
}

u16 GetEPRxAddr(u8 bEpNum)
{
    return _GetEPRxAddr(bEpNum);
}

void SetEPTxCount(u8 bEpNum, u16 wCount)
{
    _SetEPTxCount(bEpNum, wCount);
}

void SetEPCountRxReg(u32 *pdwReg, u16 wCount)
{
    _SetEPCountRxReg(pdwReg, wCount);
}

void SetEPRxCount(u8 bEpNum, u16 wCount)
{
    _SetEPRxCount(bEpNum, wCount);
}

u16 GetEPTxCount(u8 bEpNum)
{
    return _GetEPTxCount(bEpNum);
}

u16 GetEPRxCount(u8 bEpNum)
{
    return _GetEPRxCount(bEpNum);
}

void SetEPDblBuffAddr(u8 bEpNum, u16 wBuf0Addr, u16 wBuf1Addr)
{
    _SetEPDblBuffAddr(bEpNum, wBuf0Addr, wBuf1Addr);
}

void SetEPDblBuf0Addr(u8 bEpNum, u16 wBuf0Addr)
{
    _SetEPDblBuf0Addr(bEpNum, wBuf0Addr);
}

void SetEPDblBuf1Addr(u8 bEpNum, u16 wBuf1Addr)
{
    _SetEPDblBuf1Addr(bEpNum, wBuf1Addr);
}

u16 GetEPDblBuf0Addr(u8 bEpNum)
{
    return _GetEPDblBuf0Addr(bEpNum);
}

u16 GetEPDblBuf1Addr(u8 bEpNum)
{
    return _GetEPDblBuf1Addr(bEpNum);
}

void SetEPDblBuffCount(u8 bEpNum, u8 bDir, u16 wCount)
{
    _SetEPDblBuffCount(bEpNum, bDir, wCount);
}

void SetEPDblBuf0Count(u8 bEpNum, u8 bDir, u16 wCount)
{
    _SetEPDblBuf0Count(bEpNum, bDir, wCount);
}

void SetEPDblBuf1Count(u8 bEpNum, u8 bDir, u16 wCount)
{
    _SetEPDblBuf1Count(bEpNum, bDir, wCount);
}

u16 GetEPDblBuf0Count(u8 bEpNum)
{
    return _GetEPDblBuf0Count(bEpNum);
}

u16 GetEPDblBuf1Count(u8 bEpNum)
{
    return _GetEPDblBuf1Count(bEpNum);
}

EP_DBUF_DIR GetEPDblBufDir(u8 bEpNum)
{
    if ((*_pEPRxCount(bEpNum)) & 0xFC00)
        return EP_DBUF_OUT;
    else if ((*_pEPTxCount(bEpNum)) & 0x03FF)
        return EP_DBUF_IN;
    else
        return EP_DBUF_ERR;
}

void FreeUserBuffer(u8 bEpNum, u8 bDir)
{
    /* OUT double buffered endpoint */
    if (bDir == EP_DBUF_OUT) {
        _ToggleDTOG_TX(bEpNum);
    /* IN double buffered endpoint */
    } else if (bDir == EP_DBUF_IN) {
        _ToggleDTOG_RX(bEpNum);
    }
}

u16 ToWord(u8 bh, u8 bl)
{
    u16 wRet;

    wRet = (u16)bl | ((u16)bh << 8);

    return wRet;
}

u16 ByteSwap(u16 wSwW)
{
    u8 bTemp;
    u16 wRet;

    bTemp = (u8)(wSwW & 0xFF);
    wRet = (wSwW >> 8) | ((u16)bTemp << 8);

    return wRet;
}

