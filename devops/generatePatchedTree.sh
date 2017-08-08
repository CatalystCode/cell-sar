#!/bin/bash
echo "Generating patched source code trees..."
echo "  Where am I?"

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd`
popd > /dev/null
SCRIPTPATH=$(readlink -m "${SCRIPTPATH}")

SANDBOXPATH=$(readlink -m "${SCRIPTPATH}/../.sandbox")
LIBPATH=$(readlink -m "${SCRIPTPATH}/../lib")
PATCHPATH=$(readlink -m "${SCRIPTPATH}/../bts")

echo "      I'm at ${SCRIPTPATH}"
if [ ! -d "${LIBPATH}" ]; then
    echo "      Error: Libraries not in ${LIBPATH}"
    exit 1;
else
    echo "      Original libraries are at ${LIBPATH}"
fi

if [ ! -d "${PATCHPATH}" ]; then
    echo "      Error: Patches not in ${PATCHPATH}"
    exit 1;
else
    echo "      Patches are at ${PATCHPATH}"
fi

# check if sanbox exsists with changes (we don't want to accidentally overwrite them)
if [ -d "${SANDBOXPATH}/.git" ]; then
   diffcount=`git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" diff | wc -c`
   if [ $diffcount -ne 0 ]; then 
      echo ""
      echo "ABORT: uncommited changed detected in ${SANDBOXPATH}."
      echo "  either commit those changes with generatePatches.sh,"
      echo "  or delete the sandbox directory listed above and try again."
      exit 1;
   fi 
fi

echo "  Creating sandbox at ${SANDBOXPATH}"

rm -rf "${SANDBOXPATH}"
git init "${SANDBOXPATH}"
cp ${SCRIPTPATH}/sandbox.gitignore ${SANDBOXPATH}/.gitignore

cp -R "${LIBPATH}"/* "${SANDBOXPATH}"

echo "  Stripping repository links from submodules..."
for libDirectory in `ls -d ${LIBPATH}/* `; do
    moduleName=`basename ${libDirectory}`
    rm -rf ${SANDBOXPATH}/${moduleName}/.git
done

echo "	Creating initial snapshot of sandbox..."
git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" add -A && git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" commit -m "Initial snapshot" > /dev/null

echo "Applying patches..."
# Iterate over libraries
for libDirectory in `ls -d ${LIBPATH}/* `; do
    moduleName=`basename ${libDirectory}`
    echo "  Applying patches to ${SANDBOXPATH}/${moduleName}..."
    cd ${SANDBOXPATH}
    for patchFile in `ls ${PATCHPATH}/${moduleName}/*.patch 2>/dev/null`; do
        if [ -e "$patchFile" ]; then
            echo "    Processing ${patchFile}...";
            git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" apply "${patchFile}"
            git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" add -A
            git --git-dir="${SANDBOXPATH}/.git" --work-tree="${SANDBOXPATH}" commit -m "apply ${patchFile}"
        fi
    done
done

echo "Done!"
