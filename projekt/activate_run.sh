#!/usr/bin/env sh
export CONAN_OLD_PATH="$PATH"

while read -r line; do
    LINE="$(eval echo $line)";
    export "$LINE";
done < "/home/nixen/Projects/JA/ja_stegano/projekt/environment_run.sh.env"

export CONAN_OLD_PS1="$PS1"
export PS1="(conanrunenv) $PS1"