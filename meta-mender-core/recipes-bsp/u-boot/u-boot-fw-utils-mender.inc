require u-boot-mender-common.inc

inherit deploy

DEPENDS_append = " u-boot"

do_compile_append() {
    alignment_bytes=$(expr ${MENDER_PARTITION_ALIGNMENT_MB} \* 1024 \* 1024)
    if [ $(expr ${MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET} % $alignment_bytes) -ne 0 ]; then
        bberror "MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET must be aligned to" \
                "MENDER_PARTITION_ALIGNMENT_MB"
    fi

    # fw-utils seem to only be able to handle hex values.
    HEX_BOOTENV_SIZE="$(printf 0x%x "${BOOTENV_SIZE}")"

    # create fw_env.config file
    cat > ${WORKDIR}/fw_env.config <<EOF
${MENDER_STORAGE_DEVICE} ${MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET_1} $HEX_BOOTENV_SIZE
${MENDER_STORAGE_DEVICE} ${MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET_2} $HEX_BOOTENV_SIZE
EOF
}

do_install_append() {
    install -d -m 755 ${D}${sysconfdir}
    ln -sf /data/u-boot/fw_env.config ${D}${sysconfdir}/fw_env.config
}

do_deploy() {
    install -d ${WORKDIR}/data/u-boot
    install -m 0644 ${WORKDIR}/fw_env.config ${WORKDIR}/data/u-boot/fw_env.config
    # We need to keep the permissions in a tar file, because they are not
    # remembered by bitbake's fakeroot mechanism if we put it directly in the
    # deploy directory.
    ( cd ${WORKDIR} && tar cf ${WORKDIR}/data.tar --owner=root --group=root data )

    cp ${WORKDIR}/data.tar ${DEPLOYDIR}
}
addtask deploy after do_compile
