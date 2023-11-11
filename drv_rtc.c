/* *****************************************************************************
 * File:   drv_rtc.c
 * Author: Dimitar Lilov
 *
 * Created on 2022 06 18
 * 
 * Description: ...
 * 
 **************************************************************************** */

/* *****************************************************************************
 * Header Includes
 **************************************************************************** */
#include "drv_rtc.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#include "esp_log.h"

#include "drv_time.h"

#if CONFIG_DRV_HT1302_USE
#include "drv_ht1302.h"
#endif

#if CONFIG_DRV_MCP7941X_USE
#include "drv_mcp7941x.h"
#endif

/* *****************************************************************************
 * Configuration Definitions
 **************************************************************************** */
#define TAG "drv_rtc"

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
 * Function-Like Macros
 **************************************************************************** */

/* *****************************************************************************
 * Variables Definitions
 **************************************************************************** */
drv_time_stamp_t rtc_time_stamp_rd = {0};
drv_time_stamp_t rtc_time_stamp_wr = {0};
struct tm rtc_time_rd;
struct tm rtc_time_wr;

/* *****************************************************************************
 * Prototype of functions definitions
 **************************************************************************** */

/* *****************************************************************************
 * Functions
 **************************************************************************** */


bool drv_rtc_time_stamp_valid(drv_time_stamp_t* psTimestamp)
{
    bool bResult = true;
    uint8_t u8DigitBDCLo;
    uint8_t u8DigitBDCHi;
   
    u8DigitBDCLo = psTimestamp->u8YearBCD  & 0x0F;
    u8DigitBDCHi = psTimestamp->u8YearBCD  >> 4;    
    if (u8DigitBDCLo > 9) bResult = false;
    if (u8DigitBDCHi > 9) bResult = false;
    
    u8DigitBDCLo = psTimestamp->u8MonthBCD & 0x0F;
    u8DigitBDCHi = psTimestamp->u8MonthBCD >> 4;    
    if (u8DigitBDCLo > 9) bResult = false;
    if (u8DigitBDCHi > 1) bResult = false;
    if (psTimestamp->u8MonthBCD == 0) bResult = false;
    if (psTimestamp->u8MonthBCD > 0x12) bResult = false;

    u8DigitBDCLo = psTimestamp->u8DayBCD & 0x0F;
    u8DigitBDCHi = psTimestamp->u8DayBCD >> 4;    
    if (u8DigitBDCLo > 9) bResult = false;
    if (u8DigitBDCHi > 3) bResult = false;
    if (psTimestamp->u8DayBCD == 0) bResult = false;
    if (psTimestamp->u8DayBCD > 0x31) bResult = false;
    
    if (psTimestamp->u8MonthBCD == 0x02)
    {
        uint8_t u8YearBCD = (psTimestamp->u8YearBCD >> 4)*10 + (psTimestamp->u8YearBCD    & 0x0F);
        /* to do : check non - leap year %100 not %400 */
        if ((u8YearBCD % 4) == 0)
        {
            if (psTimestamp->u8DayBCD > 0x29) bResult = false;
        }
        else
        {
            if (psTimestamp->u8DayBCD > 0x28) bResult = false;
        }
    }
    else 
    if ((psTimestamp->u8MonthBCD == 0x01) 
     || (psTimestamp->u8MonthBCD == 0x03) 
     || (psTimestamp->u8MonthBCD == 0x05) 
     || (psTimestamp->u8MonthBCD == 0x07) 
     || (psTimestamp->u8MonthBCD == 0x08) 
     || (psTimestamp->u8MonthBCD == 0x10) 
     || (psTimestamp->u8MonthBCD == 0x12))
    {
        if (psTimestamp->u8DayBCD > 0x31) bResult = false;
    }
    else
    {
        if (psTimestamp->u8DayBCD > 0x30) bResult = false;
    }

    u8DigitBDCLo = psTimestamp->u8HoursBCD & 0x0F;
    u8DigitBDCHi = psTimestamp->u8HoursBCD >> 4;    
    if (u8DigitBDCLo > 9) bResult = false;
    if (u8DigitBDCHi > 2) bResult = false;
    if (psTimestamp->u8HoursBCD > 0x23) bResult = false;
    
    u8DigitBDCLo = psTimestamp->u8MinutesBCD & 0x0F;
    u8DigitBDCHi = psTimestamp->u8MinutesBCD >> 4;    
    if (u8DigitBDCLo > 9) bResult = false;
    if (u8DigitBDCHi > 5) bResult = false;
    if (psTimestamp->u8MinutesBCD > 0x59) bResult = false;
    
    u8DigitBDCLo = psTimestamp->u8SecondsBCD & 0x0F;
    u8DigitBDCHi = psTimestamp->u8SecondsBCD >> 4;    
    if (u8DigitBDCLo > 9) bResult = false;
    if (u8DigitBDCHi > 5) bResult = false;
    if (psTimestamp->u8SecondsBCD > 0x59) bResult = false;
    
    return bResult;
}

void drv_rtc_time_stamp_to_time(struct tm* s_time, drv_time_stamp_t* time_stamp)
{
    int month;
    int year;

    s_time->tm_sec  = (time_stamp->u8SecondsBCD >> 4)*10 + (time_stamp->u8SecondsBCD    & 0x0F);
    s_time->tm_min  = (time_stamp->u8MinutesBCD >> 4)*10 + (time_stamp->u8MinutesBCD    & 0x0F);
    s_time->tm_hour = (time_stamp->u8HoursBCD   >> 4)*10 + (time_stamp->u8HoursBCD      & 0x0F);
    s_time->tm_mday = (time_stamp->u8DayBCD     >> 4)*10 + (time_stamp->u8DayBCD        & 0x0F);
    month           = (time_stamp->u8MonthBCD   >> 4)*10 + (time_stamp->u8MonthBCD      & 0x0F);
    year            = (time_stamp->u8YearBCD    >> 4)*10 + (time_stamp->u8YearBCD       & 0x0F);
    s_time->tm_mon  = month - 1;
    s_time->tm_year = year  + (2000 - 1900);
}

void drv_rtc_get_timestamp_from_time(struct tm* s_time, drv_time_stamp_t* time_stamp)
{
    int month = s_time->tm_mon + 1;
    int year  = s_time->tm_year - (2000 - 1900);
    int wday  = s_time->tm_wday;
    if (wday == 0) wday = 7; /* sunday in RTC is 7 */
    
    time_stamp->u8SecondsBCD = (s_time->tm_sec   % 10) + (s_time->tm_sec   / 10) * 16;
    time_stamp->u8MinutesBCD = (s_time->tm_min   % 10) + (s_time->tm_min   / 10) * 16;
    time_stamp->u8HoursBCD   = (s_time->tm_hour  % 10) + (s_time->tm_hour  / 10) * 16;
    time_stamp->u8DayBCD     = (s_time->tm_mday  % 10) + (s_time->tm_mday  / 10) * 16;
    time_stamp->u8MonthBCD   = (month            % 10) + (month            / 10) * 16;
    time_stamp->u8YearBCD    = (year             % 10) + (year             / 10) * 16;
    time_stamp->u8WeekdayBCD = (wday             % 10) + (wday             / 10) * 16;
}

bool drv_rtc_write(struct tm* p_tm)
{
    drv_rtc_get_timestamp_from_time(p_tm, &rtc_time_stamp_wr);
    #if CONFIG_DRV_MCP7941X_USE
    return drv_mcp7941x_write_time(&rtc_time_stamp_wr);
    #elif CONFIG_DRV_HT1302_USE
    return drv_ht1302_write_time(&rtc_time_stamp_wr);
    #else
    return false;
    #endif
}


bool drv_rtc_time_wr(time_t new_time)
{
    localtime_r(&new_time, &rtc_time_wr);
    return drv_rtc_write(&rtc_time_wr);
}

struct tm* drv_rtc_read(void)
{
    memset(&rtc_time_stamp_rd, 0, sizeof(rtc_time_stamp_rd));

    #if CONFIG_DRV_MCP7941X_USE
    drv_mcp7941x_read_time(&rtc_time_stamp_rd);
    #elif CONFIG_DRV_HT1302_USE
    drv_ht1302_read_time(&rtc_time_stamp_rd);
    #else
    /* use default NULL data */
    #endif
    if(drv_rtc_time_stamp_valid(&rtc_time_stamp_rd))
    {
        drv_rtc_time_stamp_to_time(&rtc_time_rd, &rtc_time_stamp_rd);
    }
    else
    {
       rtc_time_rd.tm_sec = 0;
       rtc_time_rd.tm_min = 0;
       rtc_time_rd.tm_hour = 0;
       rtc_time_rd.tm_mday = 1;
       rtc_time_rd.tm_mon = 0;
       rtc_time_rd.tm_year = 1970 - 1900;
    }
    
    return &rtc_time_rd;
}

time_t drv_rtc_time_rd(void)
{
    return mktime(drv_rtc_read());
}

void drv_rtc_time_print(void)
{
    time_t now_time = mktime(drv_rtc_read());
    struct tm rtc_time_info;

    localtime_r(&now_time, &rtc_time_info);  //strftime - exception if not used localtime_r
    ESP_LOGI(TAG, "Read Time from RTC: %s", asctime(&rtc_time_info)); //asctime - exception if not used localtime_r
}
