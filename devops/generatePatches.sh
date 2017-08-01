#!/bin/bash
echo "Generating patches for source code tree..."
echo "  Where am I?"

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd`
popd > /dev/null
SCRIPTPATH=$(readlink -m "${SCRIPTPATH}")

SANDBOXPATH=$(readlink -m "${SCRIPTPATH}/../.sandbox")
LIBPATH=$(readlink -m "${SCRIPTPATH}/../lib")
PATCHPATH=$(readlink -m "${SCRIPTPATH}/../bts")

echo "      I'm at ${SCRIPTPATH}"
echo "      Original libraries are at ${LIBPATH}"
echo "      Patches are at ${PATCHPATH}"
echo "  Validating sandbox at ${SANDBOXPATH}"
if [ ! -d "${SANDBOXPATH}" ]; then
    echo "      Error: No sandbox present!"
fi

echo "Creating patches..."
PATCHSET=`date +%s`
echo "  *** Patch set will be named ${PATCHSET}"

# Iterate over libraries
for libDirectory in `ls -d ${LIBPATH}/* `; do
    moduleName=`basename ${libDirectory}`
    mkdir -p ${PATCHPATH}/${moduleName}
    echo "  Creating patches for ${moduleName}..."
    git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" add -A "${SANDBOXPATH}/${moduleName}"
    git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" diff HEAD "${SANDBOXPATH}/${moduleName}/" > ${PATCHPATH}/${moduleName}/${PATCHSET}.patch
    if [ ! -s "${PATCHPATH}/${moduleName}/${PATCHSET}.patch" ]; then
        echo "      No patch required."
        rm "${PATCHPATH}/${moduleName}/${PATCHSET}.patch"
    else
        git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" commit -m "applied ${PATCHPATH}/${moduleName}/${PATCHSET}.patch"
    fi
done

echo "Done!"
echo " "
echo "*** Don't forget to clean up your sandbox! ***"
echo " "
