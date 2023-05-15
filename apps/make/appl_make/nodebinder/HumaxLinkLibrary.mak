#####################################################
# Module dependent link and library
#####################################################

APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -ldlib -ldapi -lsitextlib -lsqlite3
