# This is needed for the rlottie build to work properly.
set_overlay_var(ARCH "arm")
append_to_overlay_var(WN_RLOTTIE_ADDITIONAL_OPTIONS "-fno-integrated-as")
