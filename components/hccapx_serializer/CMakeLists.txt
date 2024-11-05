idf_component_register(
	SRCS "hccapx_serializer.c"
	INCLUDE_DIRS "interface"
	REQUIRES lwip
	PRIV_REQUIRES frame_analyzer
)