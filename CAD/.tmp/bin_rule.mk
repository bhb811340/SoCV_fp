$(BIN_TOP)/$(MODE)/CAD_EC : $(PKG_DIR)/core/$(BIN_DIR)/$(MODE)/CAD_EC
	@if [ ! -d $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(AT)rm -f $@
	@echo "    CP $(notdir $@) $(PRT_MODE)"
	$(AT)cp -f $< $@

