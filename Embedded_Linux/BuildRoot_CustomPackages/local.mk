################################################################################
# local.mk
################################################################################
#
# This file is used to override the package source directory during development.
#
# Buildroot will build the calculator package directly from this external source
# directory instead of using package/calculator as the actual source tree.
#
# This is very useful while developing and testing changes quickly.
#
################################################################################

# -----------------------------------------------------------------------------
# CALCULATOR_OVERRIDE_SRCDIR
# -----------------------------------------------------------------------------
# Replace this path with the real path of your calculator application source.
#
# Buildroot will use this directory as the source tree for the calculator
# package instead of using CALCULATOR_SITE.
#
# Example:
#   /home/user/projects/Calculator
# -----------------------------------------------------------------------------
CALCULATOR_OVERRIDE_SRCDIR = /media/gemy/Linux_Workspace/ITI/ITI_QT_WS/Tasks/Calculator
