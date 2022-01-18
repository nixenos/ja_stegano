#!/usr/bin/env sh
export PS1="$CONAN_OLD_PS1"
unset CONAN_OLD_PS1


export PATH="$CONAN_OLD_PATH"
unset CONAN_OLD_PATH
unset DYLD_LIBRARY_PATH
unset LD_LIBRARY_PATH