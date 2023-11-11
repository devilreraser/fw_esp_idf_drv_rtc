idf_build_get_property(project_dir PROJECT_DIR)

# Initialize an empty list to hold conditional requirements
set(conditionally_required_components "")

if(EXISTS "${project_dir}/components/drv_mcp7941x")
    list(APPEND conditionally_required_components "drv_mcp7941x")
endif()

if(EXISTS "${project_dir}/components/drv_ht1302")
    list(APPEND conditionally_required_components "drv_ht1302")
endif()

idf_component_register(SRCS "drv_rtc.c" 
                    INCLUDE_DIRS "." 
                    REQUIRES    
                        "drv_time" 
                        ${conditionally_required_components}
                                      )
                 

