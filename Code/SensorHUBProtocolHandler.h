#pragma once

#include <stdint.h>
#include <stdbool.h>

/* @brief SensorHUB CAN Protocol handler is a layer above CAN BUS transaction.
 * It is responsible for making and parsing raw bytes (up to 8) to informative data used by
 * SensorHUB and Host using it, its filter ID used for addressing SensorHUB and host addresses.
 */ 

//==============================================================
// Public defines
//==============================================================

//==============================================================
// Public declarations
//==============================================================

struct SSensorHUBCANProtocolHandlerMessage
{
	uint32_t dwFilterID;
	uint8_t byBuffer[8];
	uint8_t byMessageLength;
};

enum ESensorHUBCANProtocolHandlerMessageID
{
	eSensorHUBCANProtocolHandlerStatusMsgID = 0x00,
	eSensorHUBCANProtocolHandlerCtrlMsgID = 0x01,
	eSensorHUBCANProtocolHandlerVL6180MsgID = 0x02,
	eSensorHUBProtocolHandlerUnknownMsgID	= 0xFE
};

enum ESensorHUBCANProtocolHandlerSourceID
{
	eSensorHUBCANProtocolHandlerSourceSensorHUB = 0,
	eSensorHUBCANProtocolHandlerSourceHost = 1
};

enum ESensorHUBCANProtocolHandlerDestinationID
{
	eSensorHUBCANProtocolHandlerDestinationSensorHUB = 0,
	eSensorHUBCANProtocolHandlerDestinationHost = 1
};

struct SSensorHUBCANProtocolHandlerParsedMessage
{
	enum ESensorHUBCANProtocolHandlerMessageID eMessageID;
	enum ESensorHUBCANProtocolHandlerSourceID eSourceID;
	enum ESensorHUBCANProtocolHandlerDestinationID eDestinationID;
};

struct SSensorHUBCANProtocolHandlerStatusMsg
{
	uint32_t dwTimeFromStartUpMS;	//  [0..16,777,215] ms
	uint16_t wBoardVoltageMV;		// [0..4,000] mV
	uint16_t wExternalADCMV;		// [0..16,000] mV
	bool bLoopBackMode;				// [true - CAN in loopback mode, false - CAN not in loopback mode]
	uint8_t byCPUTemperature; 		// [-128..127] C
};

struct SSensorHUBCANProtocolHandlerVL6180Msg
{
	uint16_t wDistanceMM;    		// [0 - 300] mm
	uint32_t dwLightLux; 			// [0 - 100000] Lux
	bool bSensorOverRange; 			// true - sensor over range, false - sensor range is fine
	bool bSensorIsOK;				// true - sensor is OK, false - sensor failure
};

struct SSensorHUBCANProtocolHandlerCtrlMsg
{
	bool bReset;		// [true - reset the board, false - do not reset the board]
	bool bCANLoopBack;	// [true - do CAN loopback, false - do not CAN loopback]
};

typedef enum
{
	eProtocolHandlerSensorHUBCANNoError,
	eProtocolHandlerSensorHUBCANInvalidMessage,
	eProtocolHandlerSensorHUBCANInvalidDestDeviceID,
	eProtocolHandlerSensorHUBCANInvalidSrcDeviceID,
	eProtocolHandlerSensorHUBCANInvalidStatus,
	eProtocolHandlerSensorHUBCANInvalidVL6180,
	eProtocolHandlerSensorHUBCANInvalidCtrl,
	eInvalidBuffer,
} ESensorHUBCANProtocolHandlerErrorCode;

//==============================================================
// Public methods
//==============================================================

/**
*	@brief SensorHUB CAN Protocol handler component initialization
	@return EProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerInit(void);

// Builders

// HOST -> SensorHUB

/**
*	@brief SensorHUB CAN Protocol handler build sensorHUB control message
*	@param pstCtrlMsg Sensor HUB control message structure
*	@param cpstMessage Builded Sensor HUB control CAN message
*	@param byBufferSize size of buffer to be parsed
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerBuildCtrlMsg(struct SSensorHUBCANProtocolHandlerCtrlMsg *pstCtrlMsg, struct SSensorHUBCANProtocolHandlerMessage *const cpstMessage);


// Builders

// SensorHUB -> HOST

/**
*	@brief SensorHUB CAN Protocol handler build status message
*	@param pstStatusMsg status message structure
*	@param cpstMessage Builded Sensor HUB status CAN message
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerBuildStatusMsg(struct SSensorHUBCANProtocolHandlerStatusMsg *pstStatusMsg, struct SSensorHUBCANProtocolHandlerMessage *const cpstMessage);

/**
*	@brief SensorHUB CAN Protocol handler build VL6180 voltage message
*	@param pstStatusMsg status message structure
*	@param cpstMessage Builded Sensor HUB VL6180 CAN message
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerBuildVL6180Msg(struct SSensorHUBCANProtocolHandlerVL6180Msg *pstVL6180Msg, struct SSensorHUBCANProtocolHandlerMessage *const cpstMessage);


// Parsers

/**
*	@brief SensorHUB CAN Protocol handler message parser
*	@param dwFrameFormatID CAN bus ID
*	@param cpstParsedMessage parsed message
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseCANMessage(uint32_t dwFrameFormatID, struct SSensorHUBCANProtocolHandlerParsedMessage *const cpstParsedMessage);

// HOST -> SensorHUB

/**
*	@brief SensorHUB CAN Protocol handler control message parser
*	@param pbyBuffer buffer to be parsed
*	@param byBufferSize size of buffer to be parsed
*	@param cpstCtrlMessage parsed message
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseCtrlMsg(uint8_t *pbyBuffer, uint8_t byBufferSize, struct SSensorHUBCANProtocolHandlerCtrlMsg *const cpstCtrlMessage);

// SensorHUB -> HOST

/**
*	@brief SensorHUB CAN Protocol handler status message parser
*	@param pbyBuffer buffer to be parsed
*	@param byBufferSize size of buffer to be parsed
*	@param cpstStatusMsg sensor HUB status message structure
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseStatusMsg(uint8_t *pbyBuffer, uint8_t byBufferSize, struct SSensorHUBCANProtocolHandlerStatusMsg *const cpstStatusMsg);

/**
*	@brief SensorHUB CAN Protocol handler VL6180 message parser
*	@param pbyBuffer buffer to be parsed
*	@param byBufferSize size of buffer to be parsed
*	@param cpstVL6180Msg sensor HUB VL6180 message structure
	@return ESensorHUBCANProtocolHandlerErrorCode
*/
extern ESensorHUBCANProtocolHandlerErrorCode SensorHUBCANProtocolHandlerParseVL6180Msg(uint8_t *pbyBuffer, uint8_t byBufferSize, struct SSensorHUBCANProtocolHandlerVL6180Msg *const cpstVL6180Msg);

