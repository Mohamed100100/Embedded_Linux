################################################################################
#
# calculator
#
################################################################################
#
# Buildroot package recipe for the Calculator application.
#
# This file tells Buildroot:
#   - where the package source code is located
#   - how the source should be obtained
#   - which packages must be built before this package
#   - which Buildroot package infrastructure should be used
#
# In this setup, the real source code is provided through
# CALCULATOR_OVERRIDE_SRCDIR in local.mk.
#
################################################################################

# -----------------------------------------------------------------------------
# CALCULATOR_SITE
# -----------------------------------------------------------------------------
# This variable defines the package source location.
#
# Because we are using a local source override in local.mk,
# Buildroot will actually build from the directory specified by:
#
#   CALCULATOR_OVERRIDE_SRCDIR = /path/to/your/project
#
# Still, CALCULATOR_SITE must be defined as part of a valid package recipe.
# -----------------------------------------------------------------------------
CALCULATOR_SITE = $(TOPDIR)/package/calculator

# -----------------------------------------------------------------------------
# CALCULATOR_SITE_METHOD
# -----------------------------------------------------------------------------
# "local" means the package source is local on disk and should not be
# downloaded from the internet.
# -----------------------------------------------------------------------------
CALCULATOR_SITE_METHOD = local

# -----------------------------------------------------------------------------
# CALCULATOR_DEPENDENCIES
# -----------------------------------------------------------------------------
# These are the Buildroot packages that must be built before calculator.
#
# qt6base:
#   Provides the main Qt6 runtime, core libraries, and GUI support.
#
# qt6declarative:
#   Provides QML and Qt Quick support used by the application.
#
# qt6shadertools:
#   Provides shader tools needed by Qt Quick rendering.
# -----------------------------------------------------------------------------
CALCULATOR_DEPENDENCIES = qt6base qt6declarative qt6shadertools

# -----------------------------------------------------------------------------
# Build infrastructure
# -----------------------------------------------------------------------------
# Use Buildroot's CMake package infrastructure.
#
# This means Buildroot will:
#   1. configure the package with CMake
#   2. compile it
#   3. install it into the target root filesystem
#
# This requires your calculator project to have a valid CMakeLists.txt
# with proper install() rules.
# -----------------------------------------------------------------------------
$(eval $(cmake-package))
