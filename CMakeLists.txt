idf_component_register(SRCS "drv_rtc.c" 
                    INCLUDE_DIRS "." 
                    REQUIRES    "drv_time" "drv_mcp7941x" "drv_ht1302"
                                      )
                 

