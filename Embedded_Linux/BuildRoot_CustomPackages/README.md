# README: Buildroot for Raspberry Pi 3 Model B+ with Calculator Qt6 App

This guide explains how to:

- clone Buildroot
- switch to a specific Buildroot version
- create a symbolic link to Buildroot inside this repository
- configure Buildroot for **Raspberry Pi 3 Model B+**
- add a custom package menu named **Gemy_Packages**
- add a custom **calculator** package
- build the image
- flash the generated image to an SD card using `dd`

This setup uses:

- **Qt6**
- **QML / Qt Quick**
- **linuxfb platform plugin**


The calculator application is intended to run using:

```bash
appcalculator -platform linuxfb
```

---

# Repository structure

```text
BuildRoot_CustomPackages/
├── README.md
├── buildroot_Slink -> <path_to_buildroot>
├── package/
│   ├── gemy/
│   │   └── Config.in
│   └── calculator/
│       ├── Config.in
│       └── calculator.mk
└── local.mk
```

- `buildroot_Slink` is a symbolic link pointing to your Buildroot directory
- `package/` contains custom package files to copy into Buildroot
- `local.mk` contains the source override for the calculator application
- Your actual calculator application source code is stored outside Buildroot

---

# 1. Prerequisites

Install required packages on your Linux host.

For Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    git \
    wget \
    rsync \
    unzip \
    bc \
    python3 \
    cpio \
    file \
    cmake \
    ninja-build \
    pkg-config
```

---

# 2. Clone Buildroot

Clone the Buildroot repository to any location on your system:

```bash
git clone https://github.com/buildroot/buildroot.git <path_to_buildroot>
```

Example:

```bash
git clone https://github.com/buildroot/buildroot.git ~/tools/buildroot

```

> Choose any location that suits your setup.

---

# 3. Switch to a specific Buildroot version

Move into the Buildroot directory:

```bash
cd <path_to_buildroot>
```

Example:

```bash
cd ~/tools/buildroot
```

List available tags to find your desired version:

```bash
git tag
```

Switch to your desired version.

Example for version `2024.02`:

```bash
git checkout 2024.02
```

Or for version `2024.02.1`:

```bash
git checkout 2024.02.1
```

> Replace `2024.02` with the actual version you want to use.

---

# 4. Clone or navigate to this repository

Clone this repository or navigate to it:

```bash
cd BuildRoot_CustomPackages
```

---

# 5. Create a symbolic link to Buildroot

Inside this repository, create a symbolic link pointing to your Buildroot directory:

```bash
ln -s <path_to_buildroot> buildroot_Slink
```

Example:

```bash
ln -s ~/tools/buildroot buildroot_Slink
```


Now `buildroot_Slink` points to your Buildroot directory.

All following commands will use `buildroot_Slink`.

---

# 6. Repository files

This repository already contains the following files:

```text
BuildRoot_CustomPackages/
├── README.md
├── package/
│   ├── gemy/
│   │   └── Config.in
│   └── calculator/
│       ├── Config.in
│       └── calculator.mk
└── local.mk
```

---

## 6.1 `package/gemy/Config.in`

File path:

```text
package/gemy/Config.in
```

Content:

```make
# -----------------------------------------------------------------------------
# package/gemy/Config.in
# -----------------------------------------------------------------------------
#
# This file creates a custom top-level menu inside Buildroot menuconfig.
#
# The purpose of this file is only to group our custom packages under
# one menu named "Gemy_Packages".
#
# It does not define the calculator package itself.
# Instead, it loads the calculator package Config.in file.
#
# -----------------------------------------------------------------------------

# Create a new menu in menuconfig named "Gemy_Packages".
menu "Gemy_Packages"

    # Load the calculator package configuration file.
    # This makes the calculator package appear inside this menu.
    source "package/calculator/Config.in"

# End of custom menu.
endmenu
```

---

## 6.2 `package/calculator/Config.in`

File path:

```text
package/calculator/Config.in
```

Content:

```make
# -----------------------------------------------------------------------------
# package/calculator/Config.in
# -----------------------------------------------------------------------------
#
# This file defines the calculator package entry in Buildroot menuconfig.
#
# This application:
#   - uses Qt6
#   - uses QML / Qt Quick
#   - runs using the linuxfb platform plugin
#   - does not use X11
#   - does not use xcb
#
# The calculator package depends on qt6base.
# So the user must first enable qt6base in menuconfig.
# After that, the calculator package becomes selectable.
#
# A note is shown in menuconfig if qt6base is not enabled yet.
#
# -----------------------------------------------------------------------------

# Define a Buildroot package option named BR2_PACKAGE_CALCULATOR.
config BR2_PACKAGE_CALCULATOR

    # Create a yes/no option in menuconfig with label "calculator".
    bool "calculator"

    # This package depends on qt6base being enabled first.
    # If qt6base is not enabled, this package will not be selectable.
    depends on BR2_PACKAGE_QT6BASE

    # Automatically select Qt Declarative module.
    # This is required because the application uses QML.
    select BR2_PACKAGE_QT6DECLARATIVE

    # Automatically select Qt Quick support.
    # This is required for graphical interfaces using Qt Quick/QML.
    select BR2_PACKAGE_QT6DECLARATIVE_QUICK

    # Automatically select Qt shader tools.
    # Qt Quick rendering requires shader support.
    select BR2_PACKAGE_QT6SHADERTOOLS

    # Automatically select fontconfig.
    # This helps the system find and configure fonts at runtime.
    select BR2_PACKAGE_FONTCONFIG

    # Automatically select FreeType.
    # This provides font rendering support.
    select BR2_PACKAGE_FREETYPE

    # Automatically select HarfBuzz.
    # This is used for text shaping and proper text rendering.
    select BR2_PACKAGE_HARFBUZZ

    # Automatically select DejaVu fonts.
    # This ensures default fonts are available for numbers and text.
    select BR2_PACKAGE_DEJAVU

    # Help text shown in menuconfig when the user highlights this package.
    help
      Calculator is a Qt6/QML calculator application.

      This application is configured to run using the Qt linuxfb
      platform plugin.

      It does not require X11 or xcb.

      Before enabling this package, enable qt6base first from:
        Target packages
          -> Graphic libraries and applications
            -> qt6
              -> qt6base

# Show a visible note in menuconfig when qt6base is not enabled.
# This helps the user understand why the calculator package is not available.
comment "NOTE: Enable qt6base first, then return here to enable calculator"
    depends on !BR2_PACKAGE_QT6BASE
```

---

## 6.3 `package/calculator/calculator.mk`

File path:

```text
package/calculator/calculator.mk
```

Content:

```make
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
```

---

## 6.4 `local.mk`

File path:

```text
local.mk
```

Content:

```make
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
```

> **Important:** Update the path in `local.mk` to match your actual calculator project location.

---

# 7. Copy files into Buildroot

After creating the symbolic link, copy the custom package files into Buildroot.

## Copy package directories

```bash
cp -r package/gemy buildroot_Slink/package/
cp -r package/calculator buildroot_Slink/package/
```

## Copy local.mk

```bash
cp local.mk buildroot_Slink/
```

---

# 8. Add custom package menu to Buildroot top-level Config.in

You must add one line to the main Buildroot `Config.in` file.

Edit:

```bash
nano buildroot_Slink/Config.in
```

Add this line somewhere with the other package `source` entries:

```make
source "package/gemy/Config.in"
```

Save and exit.

---

# 9. Configure Buildroot for Raspberry Pi 3 Model B+

Move into Buildroot using the symbolic link:

```bash
cd buildroot_Slink
```

Load the Raspberry Pi 3 64-bit default configuration:

```bash
make raspberrypi3_64_defconfig
```

---

# 10. Open menuconfig

Run:

```bash
make menuconfig
```

---

# 11. Enable qt6base first

Before enabling the calculator package, you must first enable `qt6base`.

Go to:

```text
Target packages
  -> Graphic libraries and applications
    -> qt6
      -> qt6base
```

Enable:

```text
[*] qt6base
```

This is required because the calculator package depends on `qt6base`.

If `qt6base` is not enabled, the `Gemy_Packages` menu will show this message:

```text
NOTE: Enable qt6base first, then return here to enable calculator
```

---

# 12. Enable the calculator package

After enabling `qt6base`, go to:

```text
Gemy_Packages
```

Enable:

```text
[*] calculator
```

When selected, it will automatically enable:

- `qt6declarative`
- `qt6declarative quick`
- `qt6shadertools`
- `fontconfig`
- `freetype`
- `harfbuzz`
- `dejavu`

---

# 13. Increase root filesystem size

Qt6 and its dependencies require a larger root filesystem.

In menuconfig, go to:

```text
Filesystem images
  -> ext2/3/4 root filesystem
```

Set:

```text
exact size = 512M
```

Recommended values:

- `512M` minimum
- `1G` if needed

This avoids errors like:

```text
Could not allocate block in ext2 filesystem
Maybe you need to increase BR2_TARGET_ROOTFS_EXT2_SIZE
```

---

# 14. Save and exit menuconfig

After making all changes:

- save the configuration
- exit menuconfig

---

# 15. Build the image

Run:

```bash
make
```

This builds:

- toolchain
- Qt6 packages
- calculator package
- root filesystem
- Raspberry Pi bootable image

Build time depends on your system.  
First build may take **several hours**.

---

# 16. Output image

After the build finishes, output files are placed in:

```bash
buildroot_Slink/output/images/
```

Check:

```bash
ls buildroot_Slink/output/images/
```

Typical file to flash:

```text
sdcard.img
```

---

# 17. Flash image to SD card

## 17.1 Insert SD card

Insert the SD card and find its device name:

```bash
lsblk
```

Example:

```text
/dev/mmcblk0
```

Be very careful to choose the correct device.

---

## 17.2 Unmount mounted partitions

If auto-mounted, unmount them first:

```bash
sudo umount /dev/mmcblk0p1
sudo umount /dev/mmcblk0p2
```

Replace `/dev/mmcblk0` with your real SD card device.

---

## 17.3 Write the image

```bash
sudo dd if=buildroot_Slink/output/images/sdcard.img of=/dev/mmcblk0 bs=4M status=progress conv=fsync
```

Then run:

```bash
sync
```

Now remove the SD card safely.

---

# 18. Boot the Raspberry Pi

- Insert the SD card into Raspberry Pi 3 Model B+
- connect display and power
- boot the board

---

# 19. Run the application

The application is intended to run using `linuxfb`.

Example:

```bash
appcalculator -platform linuxfb
```



because this setup does not include X11/xcb support.

---


# 20. Package explanations

### qt6base

Qt6 Base is the foundation module of the Qt6 framework. It provides core non-graphical classes used by other modules, as well as the GUI and widget functionality needed to create graphical applications.

### qt6declarative

Qt6 Declarative provides the QML engine and language infrastructure. QML is a declarative language that allows designing user interfaces with a JSON-like syntax, making it easier to create fluid and dynamic UIs.

### qt6shadertools

Qt6 Shader Tools provides tools for graphics shader processing. It is required by Qt Quick for processing and compiling shaders used in rendering graphical elements and effects.

### fontconfig

Fontconfig is a library for configuring and customizing font access. It helps applications discover and select fonts based on patterns, ensuring the correct fonts are used for text rendering.

### freetype

FreeType is a software library used to render fonts. It converts font files into bitmap images, enabling applications to display text on screen with various sizes and styles.

### harfbuzz

HarfBuzz is a text shaping library. It handles the complex process of converting Unicode text into properly arranged glyphs, which is essential for correct rendering of many languages and scripts.

### dejavu

DejaVu is a font family based on the Bitstream Vera fonts. It provides a wide range of characters and styles, ensuring that text and numbers display correctly across different languages.

---


# 21. Final notes

This setup is designed for:

- Raspberry Pi 3 Model B+
- Buildroot
- Qt6/QML application
- linux framebuffer backend

It does not use:

- X11
- xcb
- desktop window system

---

# 22. Troubleshooting

## Calculator not visible in menuconfig

Make sure you:

1. added `source "package/gemy/Config.in"` to top-level `Config.in`
2. enabled `qt6base` first

## Filesystem size error

Increase root filesystem size to `512M` or `1G` in menuconfig.

## Application does not start

Make sure to run using:

```bash
appcalculator -platform linuxfb
```
---

# 23.Calculator Application Repository Link

The calculator application source code is available at:

**GitHub Repository:** [https://github.com/Calculator](https://github.com/Mohamed100100/ITI_QT_WS/tree/main/Tasks/Calculator)

> Update this link to match your actual calculator repository URL.

### Clone the calculator application

```bash
git clone https://github.com/Mohamed100100/ITI_QT_WS/tree/main/Tasks/Calculator
```

### Calculator application requirements

The calculator project must contain:

- `CMakeLists.txt` with proper build and install rules
- C++ source files
- QML files for the user interface
- Any required resources such as images or fonts

---
