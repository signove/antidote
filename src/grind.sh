#!/bin/sh

export G_SLICE=always-malloc
export G_DEBUG=gc-friendly
VALGRIND_CMD="valgrind --suppressions=../tests/resources/memcheck.supp --error-exitcode=1 --tool=memcheck --leak-check=full"

$VALGRIND_CMD .libs/lt-healthd $*
