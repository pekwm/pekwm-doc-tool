set(pekwm_doc_tool_SOURCES
    pekwm-doc-tool.cc
    md.cc
    util.cc)

add_executable(pekwm-doc-tool ${pekwm_doc_tool_SOURCES})
target_include_directories(pekwm-doc-tool PUBLIC ${MD4C_INCLUDE_DIRS})
target_link_libraries(pekwm-doc-tool ${MD4C_LINK_LIBRARIES})
set_target_properties(pekwm-doc-tool PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON)
