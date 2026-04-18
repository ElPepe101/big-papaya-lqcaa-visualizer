# OpenFrameworks app Makefile — requires a full openFrameworks tree (with libs/, addons/, etc.).
#
# This project lives beside the OF release folder (not inside it), e.g.:
#   OpenFrameworks/<this folder>/Makefile
#   OpenFrameworks/of_v0.12.1_osx_release/
#
# Override if needed:
#   export OF_ROOT=$HOME/path/to/of_v0.12.1_osx_release
#   make
#
ifndef OF_ROOT
	OF_ROOT := ../of_v0.12.1_osx_release
endif

include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk
