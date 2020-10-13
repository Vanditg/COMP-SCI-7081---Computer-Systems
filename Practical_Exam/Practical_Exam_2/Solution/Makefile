# runs updates/installer.bash to install latest updates .zip
# then runs make using the latest Makefile

# make all make args PHONY 
.PHONY: $(MAKECMDGOALS)

default:=all
# make first arg depend on all
ifneq (,$(MAKECMDGOALS))

    # make first arg default and all other args after the first do nothing
    unique:=$(sort $(MAKECMDGOALS))
    default:=$(firstword $(unique))
    $(eval $(wordlist 2, $(words $(unique)), $(unique)):;@:)

endif


ifeq (install,$(MAKECMDGOALS))

# install - just run updates/installer.bash
install:
	@bash updates/installer.bash $(MAKECMDGOALS)

else

# default - just run Make from latest zip
$(default):
	@bash updates/installer.bash $(MAKECMDGOALS)
	@make --no-print-directory -f updates/Makefile $(MAKECMDGOALS)
	-@touch updates/log
endif
