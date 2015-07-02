$(LIB_TOP)/$(MODE)/liblibs.a : $(call get_ar_objs,$(call get_dep_pkgs,$(LIB_libs_PKGS)))
	@if [ ! -d $(dir $@) ]; then \
		mkdir -p $(dir $@); \
	fi
	$(AT)rm -f $@
	@echo "    AR $(notdir $@) $(PRT_MODE)"
	$(AT)$(AR) $(AR_FLAGS) $@ $^
	$(AT)$(RANLIB) $@

