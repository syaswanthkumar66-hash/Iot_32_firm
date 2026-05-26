idf_component_register(SRCS "udp_server.c" "session_manager.c" "packet_validator.c"
                       INCLUDE_DIRS "."
                       REQUIRES lwip security opcode_protocol relay_control freertos)
