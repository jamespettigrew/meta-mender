FILESEXTRAPATHS_prepend := "${THISDIR}/patches:"

SRC_URI_append = " file://0001-NanoPi-NEO-Enable-Mender-requirements.patch"

BOOTENV_SIZE ?= "0x20000"
