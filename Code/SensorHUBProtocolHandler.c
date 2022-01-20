#include <string.h>

#include "SensorHUBProtocolHandler.h"

#pragma pack(push, 1)	

struct SCANFilterID
{
	// Source ID [0 - SensorHUB, 1 - Host]
	uint32_t nSrcDeviceID : 3;
	// Destination ID [0 - SensorHUB, 1 - Host]
	uint32_t nDestDeviceID : 3;
	// Message ID [0x00 - 0x03]
	uint32_t nMessageID : 5;
	// Not used all 0
	uint32_t nReserved : 18;
	uint32_t nUnused : 3;
};

struct SSensorHUBStatusMsg // DLC: 8bytes
{
	uint32_t dwTimeFromStartUpMS : 24; 	// [0..16,777,215] ms
	uint32_t bLoopBackMode : 1;  		// [true - CAN in loopback mode, false - CAN not in loopback mode]
	uint32_t byCPUTemperature : 7;    	// [-128..127] C
	uint16_t wBoardVoltageMV; 			// [0..4,000] mV
	uint16_t wExternalADCMV; 			// [0..16,000] mV
};

struct SSensorHUBCtrlMsg // DLC: 1 byte
{
	uint8_t bReset : 1; 		// [true - reset the board, false - do not reset the board]
	uint8_t bCANLoopBack : 1;  	// [true - do CAN loopback, false - do not CAN loopback]
	uint8_t nResv : 6;			// Reserved - 0
};

struct SSensorHUBVL6180Msg
{
	uint16_t wDistanceMM;     		// [0 - 100] mm
	uint32_t dwLightLux : 30;  		// [0 - 100000] Lux
	uint32_t bSensorOverRange : 1;	// true - sensor over range, false - sensor range is fine
	uint32_t bSensorIsOK : 1;  		// true - sensor is OK, false - sensor failure
};

	
#pragma pack(pop)


//==============================================================
// Private constants
//==============================================================

static const enum ESensorHUBCANProtocolHandlerMessageID m_ceMessgeID[] =
{
	eSensorHUBCANProtocolHandlerStatusMsgID,
	eSensorHUBCANProtocolHandlerCtrlMsgID,
	eSensorHUBCANProtocolHandlerVL6180MsgID
};

//==============================================================
// Public methods
//==============================================================

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerInit(void)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	return eRc;
}

// Builders

// HOST -> SensorHUB

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerBuildCtrlMsg(struct SSensorHUBCANProtocolHandlerCtrlMsg *pstCtrlMsg, struct SSensorHUBCANProtocolHandlerMessage *const cpstMessage)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	
	// Set CAN filter ID
	struct SCANFilterID *pstCANFilterID = (struct SCANFilterID *)&cpstMessage->dwFilterID;
	pstCANFilterID->nSrcDeviceID = eSensorHUBCANProtocolHandlerSourceHost;
	pstCANFilterID->nDestDeviceID = eSensorHUBCANProtocolHandlerDestinationSensorHUB;
	pstCANFilterID->nMessageID = eSensorHUBCANProtocolHandlerCtrlMsgID;
	pstCANFilterID->nReserved = 0;
	pstCANFilterID->nUnused = 0;
	
	// Set CAN data
	struct SSensorHUBCtrlMsg *pstSensorHUBCtrlMsg = (struct SSensorHUBCtrlMsg *)cpstMessage->byBuffer;
	pstSensorHUBCtrlMsg->bReset = pstCtrlMsg->bReset;
	pstSensorHUBCtrlMsg->bCANLoopBack = pstCtrlMsg->bCANLoopBack;
	
	// Set CAN DLC
	cpstMessage->byMessageLength = sizeof(struct SSensorHUBCtrlMsg);
	
	return eRc;
}

// SensorHUB -> HOST

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerBuildStatusMsg(struct SSensorHUBCANProtocolHandlerStatusMsg *pstStatusMsg, struct SSensorHUBCANProtocolHandlerMessage *const cpstMessage)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	
	if (pstStatusMsg->dwTimeFromStartUpMS > 16777215)
	{
		eRc = eProtocolHandlerSensorHUBCANInvalidStatus;
	}
	
	if (pstStatusMsg->wBoardVoltageMV > 4000)
	{
		eRc = eProtocolHandlerSensorHUBCANInvalidStatus;
	}
	
	if (pstStatusMsg->wExternalADCMV > 16000)
	{
		eRc = eProtocolHandlerSensorHUBCANInvalidStatus;
	}
	
	// Set CAN filter ID
	struct SCANFilterID *pstCANFilterID = (struct SCANFilterID *)&cpstMessage->dwFilterID;
	pstCANFilterID->nSrcDeviceID = eSensorHUBCANProtocolHandlerSourceSensorHUB;
	pstCANFilterID->nDestDeviceID = eSensorHUBCANProtocolHandlerDestinationHost;
	pstCANFilterID->nMessageID = eSensorHUBCANProtocolHandlerStatusMsgID;
	pstCANFilterID->nReserved = 0;
	pstCANFilterID->nUnused = 0;
	
	// Set CAN data
	struct SSensorHUBStatusMsg *pstSensorHUBStatusMsg = (struct SSensorHUBStatusMsg *)cpstMessage->byBuffer;
	pstSensorHUBStatusMsg->dwTimeFromStartUpMS = pstStatusMsg->dwTimeFromStartUpMS;
	pstSensorHUBStatusMsg->wBoardVoltageMV = pstStatusMsg->wBoardVoltageMV;
	pstSensorHUBStatusMsg->wExternalADCMV = pstStatusMsg->wExternalADCMV;
	pstSensorHUBStatusMsg->bLoopBackMode = pstStatusMsg->bLoopBackMode;
	pstSensorHUBStatusMsg->byCPUTemperature = pstStatusMsg->byCPUTemperature;
	
	// Set CAN DLC
	cpstMessage->byMessageLength = sizeof(struct SSensorHUBStatusMsg);
	
	return eRc;
}

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerBuildVL6180Msg(struct SSensorHUBCANProtocolHandlerVL6180Msg *pstVL6180Msg, struct SSensorHUBCANProtocolHandlerMessage *const cpstMessage)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	
	if (pstVL6180Msg->wDistanceMM > 300)
	{
		eRc = eProtocolHandlerSensorHUBCANInvalidVL6180;
	}
	
	if (pstVL6180Msg->dwLightLux > 100000)
	{
		eRc = eProtocolHandlerSensorHUBCANInvalidVL6180;
	}
	
	// Set CAN filter ID
	struct SCANFilterID *pstCANFilterID = (struct SCANFilterID *)&cpstMessage->dwFilterID;
	pstCANFilterID->nSrcDeviceID = eSensorHUBCANProtocolHandlerSourceSensorHUB;
	pstCANFilterID->nDestDeviceID = eSensorHUBCANProtocolHandlerDestinationHost;
	pstCANFilterID->nMessageID = eSensorHUBCANProtocolHandlerVL6180MsgID;
	pstCANFilterID->nReserved = 0;
	pstCANFilterID->nUnused = 0;
	
	// Set CAN data
	struct SSensorHUBVL6180Msg *pstSensorHUBVL6180Msg = (struct SSensorHUBVL6180Msg *)cpstMessage->byBuffer;
	pstSensorHUBVL6180Msg->wDistanceMM = pstVL6180Msg->wDistanceMM;
	pstSensorHUBVL6180Msg->dwLightLux = pstVL6180Msg->dwLightLux;
	pstSensorHUBVL6180Msg->bSensorOverRange = pstVL6180Msg->bSensorOverRange;
	pstSensorHUBVL6180Msg->bSensorIsOK = pstVL6180Msg->bSensorIsOK;
	
	// Set CAN DLC
	cpstMessage->byMessageLength = sizeof(struct SSensorHUBVL6180Msg);
	
	return eRc;
}

// Parsers

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseCANMessage(uint32_t dwFrameFormatID, struct SSensorHUBCANProtocolHandlerParsedMessage *const cpstParsedMessage)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	struct SCANFilterID *pstCANFilterID = (struct SCANFilterID *)&dwFrameFormatID;
	uint8_t byMsgID = 0;
	bool bFound = false;
	
	// Check if message is valid
	while ((byMsgID < sizeof(m_ceMessgeID) / sizeof(m_ceMessgeID[0])) && (!bFound))
	{
		if (m_ceMessgeID[byMsgID] == pstCANFilterID->nMessageID)
		{
			bFound = true;
		}
		else
		{
			byMsgID++;
		}
	}
	
	if (!bFound)
	{
		eRc = eProtocolHandlerSensorHUBCANInvalidMessage;
		cpstParsedMessage->eMessageID = eSensorHUBProtocolHandlerUnknownMsgID;
	}
	else
	{
		if (pstCANFilterID->nSrcDeviceID > 1)
		{
			eRc = eProtocolHandlerSensorHUBCANInvalidSrcDeviceID;
		}
		
		if (pstCANFilterID->nDestDeviceID > 1)
		{
			eRc = eProtocolHandlerSensorHUBCANInvalidDestDeviceID;
		}
		
		if (pstCANFilterID->nSrcDeviceID == pstCANFilterID->nDestDeviceID)
		{
			eRc = eProtocolHandlerSensorHUBCANInvalidSrcDeviceID;
		}
	}
	
	if (eRc == eProtocolHandlerSensorHUBCANNoError)
	{
		cpstParsedMessage->eMessageID = (enum ESensorHUBCANProtocolHandlerMessageID)pstCANFilterID->nMessageID;
		cpstParsedMessage->eSourceID = (enum ESensorHUBCANProtocolHandlerSourceID)pstCANFilterID->nSrcDeviceID;
		cpstParsedMessage->eDestinationID = (enum ESensorHUBCANProtocolHandlerDestinationID)pstCANFilterID->nDestDeviceID;
	}
	
	return eRc;
}

// HOST -> SensorHUB

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseCtrlMsg(uint8_t *pbyBuffer, uint8_t byBufferSize, struct SSensorHUBCANProtocolHandlerCtrlMsg *const cpstCtrlMessage)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	struct SSensorHUBCtrlMsg *pstSensorHUBCtrlMsg = (struct SSensorHUBCtrlMsg *)pbyBuffer;
	cpstCtrlMessage->bReset = (float)pstSensorHUBCtrlMsg->bReset;
	cpstCtrlMessage->bCANLoopBack = (float)pstSensorHUBCtrlMsg->bCANLoopBack;
	return eRc;
}

// SensorHUB -> HOST 

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseStatusMsg(uint8_t *pbyBuffer, uint8_t byBufferSize, struct SSensorHUBCANProtocolHandlerStatusMsg *const cpstStatusMsg)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	struct SSensorHUBStatusMsg *pstSensorHUBStatusMsg = (struct SSensorHUBStatusMsg *)pbyBuffer;
	cpstStatusMsg->dwTimeFromStartUpMS = pstSensorHUBStatusMsg->dwTimeFromStartUpMS;
	cpstStatusMsg->wBoardVoltageMV = pstSensorHUBStatusMsg->wBoardVoltageMV;
	cpstStatusMsg->wExternalADCMV = pstSensorHUBStatusMsg->wExternalADCMV;
	cpstStatusMsg->bLoopBackMode = pstSensorHUBStatusMsg->bLoopBackMode;
	cpstStatusMsg->byCPUTemperature = pstSensorHUBStatusMsg->byCPUTemperature;
	return eRc;
}

ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseVL6180Msg(uint8_t *pbyBuffer, uint8_t byBufferSize, struct SSensorHUBCANProtocolHandlerVL6180Msg *const cpstVL6180Msg)
{
	ESensorHUBCANProtocolHandlerErrorCode eRc = eProtocolHandlerSensorHUBCANNoError;
	struct SSensorHUBVL6180Msg *pstSensorHUBVL6180Msg = (struct SSensorHUBVL6180Msg *)pbyBuffer;
	cpstVL6180Msg->wDistanceMM = pstSensorHUBVL6180Msg->wDistanceMM;
	cpstVL6180Msg->dwLightLux = pstSensorHUBVL6180Msg->dwLightLux;
	cpstVL6180Msg->bSensorOverRange = pstSensorHUBVL6180Msg->bSensorOverRange;
	cpstVL6180Msg->bSensorIsOK = pstSensorHUBVL6180Msg->bSensorIsOK;
	return eRc;
}
