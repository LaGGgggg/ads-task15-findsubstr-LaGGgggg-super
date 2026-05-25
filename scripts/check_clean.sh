#!/bin/sh -e

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

EXECUTABLE_FILES=$(find "$ROOTDIR" -executable -type f -o -type d \( -path "${ROOTDIR}/scripts" -o -path "${ROOTDIR}/.git" \) -prune -false)
if [ -n "$EXECUTABLE_FILES" ]; then
	echo "Executable files were found"
	echo "$EXECUTABLE_FILES"
	exit 1
fi
EXE_FILES=$(find "$ROOTDIR" -name "*.exe" -type f)
if [ -n "$EXE_FILES" ]; then
	echo ".exe files were found"
	echo "$EXE_FILES"
	exit 1
fi
DLL_FILES=$(find "$ROOTDIR" -name "*.dll" -type f)
if [ -n "$DLL_FILES" ]; then
	echo ".dll files were found"
	echo "$DLL_FILES"
	exit 1
fi
ARCHIVE_LIBRARIES=$(find "$ROOTDIR" -name "*.a" -type f)
if [ -n "$ARCHIVE_LIBRARIES" ]; then
	echo "Archive libraries were found"
	echo "$ARCHIVE_LIBRARIES"
	exit 1
fi
OBJECTIVE_FILES=$(find "$ROOTDIR" -name "*.o" -type f)
if [ -n "$OBJECTIVE_FILES" ]; then
	echo "Objective files were found"
	echo "$OBJECTIVE_FILES"
	exit 1
fi
echo "Repository is clean"
