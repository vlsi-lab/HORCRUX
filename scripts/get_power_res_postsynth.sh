#!/usr/bin/env bash
# get_power_res_postsynth
# Compile all apps listed in ./PQC-tests-DAC_poweranalysis.md, then for each (ver,test):
#   - make run-tests-postsynth-<ver>-<test> (in .)
#   - make power-analysis (in .)
#   - mv implementation/power_analysis/reports -> implementation/power_analysis/reports_<test>-<ver>
#
# Fixed paths:
#   - Makefile directory: "."
#   - Commands file: "./PQC-tests-DAC_poweranalysis.md"
#   - Reports dir: "implementation/power_analysis/reports"
#
# Optional filters:
#   --only-ver {simple|custom|both} (default: both)
#   --only-tests keccak,cbd_eta1,... (comma-separated list)
#   --dry-run (print actions only)

set -euo pipefail

# ---------- Fixed configuration ----------
MAKE_CWD="."
COMMANDS_FILE="./PQC-tests-DAC_poweranalysis.md"
REPORTS_DIR="implementation/power_analysis/reports"

# ---------- Defaults for optional filters ----------
WANT_SIMPLE=1
WANT_CUSTOM=1
ONLY_TESTS=""
DRY_RUN=0

say() { printf "[%s] %s\n" "$(date +'%F %T')" "$*"; }
die() { echo "Error: $*" >&2; exit 1; }
run() { if [[ $DRY_RUN -eq 1 ]]; then echo "DRYRUN: $*"; else eval "$@"; fi; }

usage() {
  cat <<EOF
Usage: $0 [--only-ver simple|custom|both] [--only-tests LIST] [--dry-run]

Fixed paths:
  Makefile dir : ${MAKE_CWD}
  Commands file: ${COMMANDS_FILE}
  Reports dir  : ${REPORTS_DIR}

Flow:
  1) Parse (ver,test) pairs from lines like: 'make app-tests-<ver>-<test>'
  2) Compile: make app-tests-<ver>-<test> VER=<ver> TESTS=<test>
  3) Run    : make run-tests-postsynth-<ver>-<test> VER=<ver> TESTS=<test>
  4) Power  : make power-analysis
  5) Rename : ${REPORTS_DIR} -> ${REPORTS_DIR}_<test>-<ver>
EOF
}

# ---------- Parse args ----------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --only-ver)
      case "$2" in
        simple) WANT_SIMPLE=1; WANT_CUSTOM=0 ;;
        custom) WANT_SIMPLE=0; WANT_CUSTOM=1 ;;
        both)   WANT_SIMPLE=1; WANT_CUSTOM=1 ;;
        *) die "Invalid --only-ver '$2' (use simple|custom|both)";;
      esac
      shift 2
      ;;
    --only-tests)
      ONLY_TESTS="$2"
      shift 2
      ;;
    --dry-run)
      DRY_RUN=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      die "Unknown arg: $1"
      ;;
  esac
done

[[ -d "$MAKE_CWD" ]] || die "Make directory not found: $MAKE_CWD"
[[ -f "$COMMANDS_FILE" ]] || die "Commands file not found: $COMMANDS_FILE"

# ---------- Build filters ----------
declare -A ONLY_MAP=()
if [[ -n "$ONLY_TESTS" ]]; then
  IFS=',' read -r -a _only_arr <<< "$ONLY_TESTS"
  for t in "${_only_arr[@]}"; do
    t_trim="${t//[[:space:]]/}"
    [[ -n "$t_trim" ]] && ONLY_MAP["$t_trim"]=1
  done
fi

declare -A WANT_VER=()
(( WANT_SIMPLE )) && WANT_VER["simple"]=1
(( WANT_CUSTOM )) && WANT_VER["custom"]=1

# ---------- Parse (ver,test) pairs from the commands file ----------
declare -A PAIRS=()  # key "ver|test" -> 1
while IFS= read -r line; do
  l="$(echo "$line" | sed -E 's/^[[:space:]]+//;s/[[:space:]]+$//')"
  # Match lines like: make app-tests-simple-<test> ... or app-tests-custom-<test> ...
  if [[ "$l" =~ ^make[[:space:]]+app-tests-(simple|custom)-([A-Za-z0-9_]+) ]]; then
    ver="${BASH_REMATCH[1]}"
    test="${BASH_REMATCH[2]}"
  else
    continue
  fi
  [[ -n "${WANT_VER[$ver]:-}" ]] || continue
  if [[ ${#ONLY_MAP[@]} -gt 0 && -z "${ONLY_MAP[$test]:-}" ]]; then
    continue
  fi
  PAIRS["$ver|$test"]=1
done < "$COMMANDS_FILE"

(( ${#PAIRS[@]} > 0 )) || die "No (ver,test) pairs found in ${COMMANDS_FILE} after filters."

# ---------- Order pairs (stable: by ver then test) ----------
declare -a VER_ARR=()
declare -a TEST_ARR=()
for key in "${!PAIRS[@]}"; do
  VER_ARR+=( "${key%%|*}" )
  TEST_ARR+=( "${key##*|}" )
done

if command -v paste >/dev/null; then
  mapfile -t SORTED < <(paste <(printf "%s\n" "${VER_ARR[@]}") <(printf "%s\n" "${TEST_ARR[@]}") | sort)
  VER_ARR=(); TEST_ARR=()
  for row in "${SORTED[@]}"; do
    ver="${row%%$'\t'*}"
    test="${row#*$'\t'}"
    VER_ARR+=( "$ver" )
    TEST_ARR+=( "$test" )
  done
fi

say "Discovered ${#VER_ARR[@]} (ver,test) pair(s) from ${COMMANDS_FILE}."

# ---------- Phase 1: Compile all ----------
say "=== Compile phase ==="
for i in "${!VER_ARR[@]}"; do
  ver="${VER_ARR[$i]}"; test="${TEST_ARR[$i]}"
  app_target="app-tests-${ver}-${test}"
  say "Compile: ${app_target}"
  run "make -C \"$MAKE_CWD\" ${app_target} VER=${ver} TESTS=${test}"
done

# ---------- Phase 2: Run + power-analysis + rename ----------
say "=== Run + power-analysis phase ==="
for i in "${!VER_ARR[@]}"; do
  ver="${VER_ARR[$i]}"; test="${TEST_ARR[$i]}"

  run_target="run-tests-postsynth-${ver}-${test}"
  say "Run: ${run_target}"
  run "make -C \"$MAKE_CWD\" ${run_target} VER=${ver} TESTS=${test}"

  say "Power analysis"
  run "make -C \"$MAKE_CWD\" power-analysis"

  if [[ -d "$REPORTS_DIR" ]]; then
    dest="${REPORTS_DIR}_${test}-${ver}"
    say "Renaming \"$REPORTS_DIR\" -> \"$dest\" (overwriting if exists)"
    # Remove previous destination (careful: this is destructive)
    run "rm -rf \"$dest\""
    run "mv \"$REPORTS_DIR\" \"$dest\""
  else
    say "WARNING: Reports dir not found: $REPORTS_DIR (skipping rename)"
  fi
done

say "All done."
