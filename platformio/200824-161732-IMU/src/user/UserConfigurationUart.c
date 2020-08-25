/** ***************************************************************************
 * @file   UserConfiguration.c
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 ******************************************************************************/
/*******************************************************************************
Copyright 2018 ACEINNA, INC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#include "string.h"
#include "magAPI.h"
#include "platformAPI.h"
#include "UserConfiguration.h"
#include "UserMessagingUART.h"
#include "Indices.h"
#include "eepromAPI.h"

// Default user configuration structure
// So Far Not Saved into EEPROM
// Do Not remove - just add extra parameters if needed
// Change default settings  if desired
UserConfigurationUartStruct gUserUartConfig = {
    .dataCRC             =  0,
    .dataSize            =  sizeof(UserConfigurationUartStruct),
    .userUartBaudRate    =  115200,  
    .userPacketType      =  "z1",  
    .userPacketRate      =  0,  
    .lpfAccelFilterFreq  =  25,
    .lpfRateFilterFreq   =  25,
    .orientation         =  "+X+Y+Z",
    // add default parameter values here, if desired
};

uint8_t UserDataBuffer[4096];
static  volatile char   *info;



/** ***************************************************************************
 * @name UpdateSystemParameter - updating of system configuration parameter based of user preferences 
 * @brief
 *
 * @param [in]  number - parameter number in user configuration structure
 * @param [in]  data   - value of the parameter in little endian format
 * @retval error (1), no error (0)
 ******************************************************************************/
// NEEDS TO BE CHECKED
BOOL  UpdateSystemParameter(uint32_t number, uint64_t data, BOOL fApply)
{
     BOOL result = TRUE;
     uint64_t *ptr = (uint64_t *)&gUserUartConfig;

     if(number < USER_UART_CRC || number >= USER_UART_MAX_PARAM ){
         return FALSE;
     }

     switch (number) {
            case USER_UART_BAUD_RATE:
                result = platformSetBaudRate((int)data, fApply);
                break;
            case USER_UART_PACKET_TYPE:
                result = setUserPacketType((uint8_t*)&data, fApply);
                break;
            case USER_UART_PACKET_RATE:
                result = platformSetPacketRate((int)data, fApply);
                break;
            case  USER_UART_CRC:
            case  USER_UART_CONFIG_SIZE:
                return TRUE;
        
        // case USER_XXX:  add function calls here if parameter XXXX
        //                        required be updated on the fly
        //             break;
        default:
            // by default result should be FALSE for system parameter
            result = FALSE;
            break;
    }
    
    if(result == TRUE){
        ptr[number] = data;
    }

    return result;
}


void userInitConfigureUart()
{

     uint64_t *ptr  = (uint64_t*)&gUserUartConfig;
   
    // apply parameters to the platform
    for(int i = USER_UART_BAUD_RATE; i < USER_UART_MAX_PARAM; i++){
        UpdateSystemParameter(i, ptr[i], TRUE);
    }

    info = getBuildInfo();
} 



/** ****************************************************************************
 * @name UpdateUserConfig
 * @brief writes user data into user configuration structure, validates data if
 *        required, updates system parameters  
 *
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
BOOL UpdateUserConfig(userConfigPayload*  pld, uint8_t *payloadLen)
{
    uint32_t offset, i;
    BOOL offsetValid = TRUE;
    BOOL lenValid = TRUE;
    BOOL numValid = TRUE;
    BOOL ret = TRUE;
    int32_t result = 0;

    // Validate parameters numbers and quantity 
    if(pld->numParams  >= USER_UART_MAX_PARAM){
        lenValid = FALSE;
        result   = INVALID_PAYLOAD_SIZE;
    }


    if((pld->numParams + pld->paramOffset) >= USER_UART_MAX_PARAM){
        numValid = FALSE;        
        result   = INVALID_PARAM;
    }
    
    if(offsetValid && numValid && lenValid){
        // Validate parameters values first
        offset = pld->paramOffset;
        for (i = 0; i < pld->numParams && ret; i++){
            ret = UpdateSystemParameter(offset, pld->parameters[i], FALSE);
            offset++;
        }
        if(ret == TRUE){
            // Apply parameters values here
            offset = pld->paramOffset;
            for (i = 0; i < pld->numParams; i++){
                ret = UpdateSystemParameter(offset, pld->parameters[i], TRUE);
                offset++;
            }
        }
    }

    if(ret == FALSE){
        result = INVALID_VALUE;
    }

    pld->numParams  = result;
    *payloadLen     = 4;     

    return TRUE;
}


/** ****************************************************************************
 * @name UpdateUserParam
 * @brief writes user data into user configuration structure, validates data if
 *        required, updates system parameters  
 *
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
BOOL UpdateUserParam(userParamPayload*  pld, uint8_t *payloadLen)
{
    BOOL offsetValid;
    BOOL ret = TRUE;
    int32_t result = 0;

    offsetValid = pld->paramNum <=  USER_UART_MAX_PARAM;        
    
    if(offsetValid){
        // Validate parameter first
        ret = UpdateSystemParameter(pld->paramNum, pld->parameter, FALSE);
        if(ret == TRUE){
            // Apply parameter if valid
            ret = UpdateSystemParameter(pld->paramNum, pld->parameter, TRUE);
        }else{
            result = INVALID_VALUE;
        }    
    } else  {
        result = INVALID_PARAM;
    }
    
    pld->paramNum = result;
    *payloadLen   = 4;                  

    return TRUE;
}


/** ****************************************************************************
 * @name UpdateAllUserParams
 * @brief writes user data into user configuration structure, validates data if
 *        required, updates system parameters  
 *
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
/** ****************************************************************************
 * @name UpdateUserConfig
 * @brief writes user data into user configuration structure, validates data if
 *        required, updates system parameters  
 *
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
BOOL UpdateAllUserParams(allUserParamsPayload*  pld, uint8_t *payloadLen)
{
    uint32_t offset, i;
    BOOL numValid = TRUE;
    BOOL ret      = TRUE;
    int32_t    result = 0; 

    int    numParams = (*payloadLen)/8;

    if(numParams  > USER_UART_MAX_PARAM){
        numValid = FALSE;
        result   = INVALID_PAYLOAD_SIZE;
    }

    
    if(numValid){
        // Validate parameters here
        offset = 0;
        for (i = 0; i < numParams && ret; i++){
            ret = UpdateSystemParameter(offset, pld->parameters[i], FALSE);
            offset++;
        }
        if(ret == TRUE){
            // Apply parameters here
            offset = 0;
            for (i = 0; i < numParams; i++){
                UpdateSystemParameter(offset, pld->parameters[i], TRUE);
                offset++;
            }
        }
    }

    if(ret == FALSE){
        result = INVALID_VALUE;
    }

    pld->parameters[0] = result;
    *payloadLen        = 4;            // return error code

    return TRUE;

}


/** ****************************************************************************
 * @name  GetUserConfig
 * @brief Retrieves specified number of user configuration parameters data for 
 *        sending to the external host starting from specified offset in user 
 *        configuration structure (refer to UserConfigParamOffset structure for
 *        specific value of offsets)
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
BOOL GetUserConfig(userConfigPayload*  pld, uint8_t *payloadLen)
{
    uint32_t offset, i;
    BOOL lenValid = TRUE;
    uint64_t *ptr = (uint64_t *)&gUserUartConfig;

    lenValid    = (pld->numParams + pld->paramOffset) <= USER_UART_MAX_PARAM;
    
    if(lenValid){
        offset = pld->paramOffset;
        for (i = 0; i < pld->numParams; i++, offset++)
        {
            pld->parameters[i] = ptr[offset];
        }
        *payloadLen     = (pld->numParams + 1) * 8;  
    } else  {
        *payloadLen    = 4;
        pld->numParams = INVALID_PARAM;
    }

    return TRUE;    

}


/** ****************************************************************************
 * @name  GetUserParam
 * @brief Retrieves specified number of user configuration parameters data for 
 *        sending to the external host starting from specified offset in user 
 *        configuration structure (refer to UserConfigParamOffset structure for
 *        specific value of offsets)
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
BOOL GetUserParam(userParamPayload*  pld, uint8_t *payloadLen)
{
    uint32_t offset;
    BOOL offsetValid;
    uint64_t *ptr = (uint64_t *)&gUserUartConfig;

    offsetValid = pld->paramNum < USER_UART_MAX_PARAM;        
    
    if(offsetValid){
        offset = pld->paramNum;
        pld->parameter = ptr[offset];
        *payloadLen     = 8 + 4;           // parameter + number
    } else  {
       *payloadLen     = 4;                // number
        pld->paramNum  = INVALID_PARAM;    // invalid
    }

    return TRUE;

}


/** ****************************************************************************
 * @name  GetAllUserParams
 * @brief Retrieves specified number of user configuration parameters data for 
 *        sending to the external host starting from specified offset in user 
 *        configuration structure (refer to UserConfigParamOffset structure for
 *        specific value of offsets)
 * @param [in] pointer to userData payload in the packet
 * @retval N/A
 ******************************************************************************/
BOOL GetAllUserParams(allUserParamsPayload*  pld, uint8_t *payloadLen)
{
    uint32_t offset, i, numParams;
    uint64_t *ptr = (uint64_t *)&gUserUartConfig;

    numParams   = USER_UART_MAX_PARAM;
    
    offset = 0;

    for (i = 0; i < numParams; i++, offset++){
            pld->parameters[i] = ptr[offset];
    }

    *payloadLen     = numParams* 8;  

    return TRUE;
}


