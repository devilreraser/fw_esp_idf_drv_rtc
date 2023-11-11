/* *****************************************************************************
 * File:   drv_rtc.h
 * Author: Dimitar Lilov
 *
 * Created on 2022 06 18
 * 
 * Description: ...
 * 
 **************************************************************************** */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sdkconfig.h>
#if CONFIG_DRV_RTC_USE

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
    
/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Constants and Macros Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Enumeration Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Type Definitions
 **************************************************************************** */

/* *****************************************************************************
 * Function-Like Macro
 **************************************************************************** */

/* *****************************************************************************
 * Variables External Usage
 **************************************************************************** */ 

/* *****************************************************************************
 * Function Prototypes
 **************************************************************************** */
time_t drv_rtc_time_rd(void);
bool drv_rtc_time_wr(time_t new_time);
void drv_rtc_time_print(void);

#endif //#if CONFIG_DRV_RTC_USE

#ifdef __cplusplus
}
#endif /* __cplusplus */

