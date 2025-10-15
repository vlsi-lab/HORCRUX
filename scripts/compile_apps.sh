#!/usr/bin/env bash
set -euo pipefail

# Default config
V="v4"                       # version selector: v1|v2|v3
SCHEME=""                    # e.g., KEM or DS
ALG=""                       # e.g., ML-KEM, HQC, ML-DSA, SLH-DSA
VER=""                       # e.g., ml-kem-512, HQC-3, ML-DSA-5, SPHINCS+-128f-robust
DO_APP=1                     # build app target
DO_RUN=1                     # run target
DRY_RUN=0
MAIN_C_PATH="main.c"         # path to your main.c (change if needed)

# TEST_* defines (empty = do not change)
TEST_KEY_SET="1"
TEST_ENC_SET="1"
TEST_DEC_SET="1"

# --- New: "all mode" controls ---
ALL_MODE=1                   # run all app/run for a version from a file
APPLIST_FILE="PQC-Apps.md"   # file to scan for make lines
LIST_ONLY=0                  # list commands but don't run
KEEP_GOING=1                 # continue on errors in all mode

usage() {
  cat <<EOF
Usage:
  # Normal mode (single app)
  $0 -V {v1|v2|v3} -S {KEM|DS} -A ALG -R VER [options]

  # All mode (run every app/run for one version found in a file)
  $0 -V {v1|v2|v3} --all [--from-file <PQC-Apps.md>] [--list-only] [--keep-going]

Required (normal mode):
  -V, --version     v1 | v2 | v3 
  -S, --scheme      KEM | DS
  -A, --alg         Algorithm name (e.g., ML-KEM, HQC, ML-DSA, SLH-DSA)
  -R, --ver         Variant string (e.g., ml-kem-512, HQC-3, ML-DSA-2, SPHINCS+-256f-simple)

Optional (normal mode):
  --no-app          Skip "app-..." make
  --no-run          Skip "run-..." make
  --dry-run         Print what would be done, don't execute
  --main PATH       Path to main.c (default: ./main.c)

Change TEST_* in main.c (if provided):
  --key {0|1}       Set #define TEST_KEY <val>
  --enc {0|1}       Set #define TEST_ENC <val>
  --dec {0|1}       Set #define TEST_DEC <val>

All mode options:
  --all             Run ALL app/run targets for the chosen -V by scanning a file
  --from-file PATH  File to scan (default: PQC-Apps.md)
  --list-only       Only list the commands that would run
  --keep-going      Don't stop at first failure during all mode

Examples:
  # Normal:
  $0 -V v2 -S KEM -A ML-KEM -R ml-kem-512
  $0 -V v3 -S DS  -A SLH-DSA -R SPHINCS+-128f-robust --key 1 --enc 0 --dec 0

  # All mode for v1, preview:
  $0 -V v1 --all --list-only

  # All mode for v2, execute from custom file and keep going on errors:
  $0 -V v2 --all --from-file ./PQC-Apps.md --keep-going
EOF
}

# --- Parse args ---
while [[ $# -gt 0 ]]; do
  case "$1" in
    -V|--version) V="$2"; shift 2;;
    -S|--scheme)  SCHEME="$2"; shift 2;;
    -A|--alg)     ALG="$2"; shift 2;;
    -R|--ver)     VER="$2"; shift 2;;
    --no-app)     DO_APP=0; shift;;
    --no-run)     DO_RUN=0; shift;;
    --dry-run)    DRY_RUN=1; shift;;
    --main)       MAIN_C_PATH="$2"; shift 2;;
    --key)        TEST_KEY_SET="$2"; shift 2;;
    --enc)        TEST_ENC_SET="$2"; shift 2;;
    --dec)        TEST_DEC_SET="$2"; shift 2;;
    --all)        ALL_MODE=1; shift;;
    --from-file)  APPLIST_FILE="$2"; shift 2;;
    --list-only)  LIST_ONLY=1; shift;;
    --keep-going) KEEP_GOING=1; shift;;
    -h|--help)    usage; exit 0;;
    *) echo "Unknown arg: $1"; usage; exit 1;;
  esac
done

pause_if_tty() {
  # Only prompt if stdout is a terminal (not in CI/pipes)
  if [[ -t 1 ]]; then
    read -rp "Done. Press Enter to close…"
  fi
}


# --- Validate version ---
[[ "$V" =~ ^v[1-4]$ ]] || { echo "Error: -V must be v1|v2|v3"; exit 1; }

# --- Optional main.c edits ---
do_sed_define () {
  local name="$1" val="$2" file="$3"
  [[ -n "$val" ]] || return 0
  [[ -f "$file" ]] || { echo "Warning: $file not found; skipping $name"; return 0; }
  if [[ "$DRY_RUN" -eq 1 || "$LIST_ONLY" -eq 1 ]]; then
    echo "[dry-run] sed -Ei 's/^(#define[[:space:]]+$name[[:space:]]+)[0-9]+/\\1$val/' '$file'"
  else
    sed -Ei "s/^(#define[[:space:]]+$name[[:space:]]+)[0-9]+/\\1$val/" "$file"
  fi
}

# Collect unique 'make app-$V-...' lines (apps only), preserving order.
collect_make_lines() {
  local pattern="^[[:space:]]*make[[:space:]]+app-$V-[^[:space:]]+"
  # Produce the stream (don’t let grep's exit=1 kill us under 'set -e')
  local stream; stream="$(grep -E "$pattern" "$APPLIST_FILE" 2>/dev/null || true | sed -E 's/^[[:space:]]+//' | uniq)"
  # Fill MAKE_LINES array portably (works even on Bash 3.2)
  MAKE_LINES=()
  if command -v mapfile >/dev/null 2>&1; then
    mapfile -t MAKE_LINES <<<"$stream"
  else
    while IFS= read -r line; do
      [[ -n "$line" ]] && MAKE_LINES+=("$line")
    done <<<"$stream"
  fi
}

do_sed_define "TEST_KEY" "$TEST_KEY_SET" "$MAIN_C_PATH"
do_sed_define "TEST_ENC" "$TEST_ENC_SET" "$MAIN_C_PATH"
do_sed_define "TEST_DEC" "$TEST_DEC_SET" "$MAIN_C_PATH"

# --- All mode ---
# --- All mode (apps only) ---
if [[ "$ALL_MODE" -eq 1 ]]; then
  [[ -f "$APPLIST_FILE" ]] || { echo "Error: file not found: $APPLIST_FILE"; exit 1; }
  echo "All-mode (apps only): scanning '$APPLIST_FILE' for version $V ..."
  collect_make_lines
  if [[ ${#MAKE_LINES[@]} -eq 0 ]]; then
    echo "No app targets for $V found in $APPLIST_FILE."
    exit 0
  fi
  printf '%s\n' "${MAKE_LINES[@]}"
  if [[ "$LIST_ONLY" -eq 1 || "$DRY_RUN" -eq 1 ]]; then
    echo "[list-only/dry-run] Not executing."
    pause_if_tty
    exit 0
  fi
 # Execute each make command line exactly as listed
  if [[ "$KEEP_GOING" -eq 1 ]]; then
    set +e
    for cmd in "${MAKE_LINES[@]}"; do
      echo "==> $cmd"
      bash -lc "$cmd"
      rc=$?
      if (( rc != 0 )); then
        echo "[warn] command failed with exit $rc, continuing (--keep-going)."
      fi
    done
    set -e
    pause_if_tty
    exit 0
  else
    for cmd in "${MAKE_LINES[@]}"; do
      echo "==> $cmd"
      bash -lc "$cmd"
      pause_if_tty
      exit 0
    done
  fi
fi



# --- Normal (single app) mode validation ---
[[ -n "$SCHEME" && -n "$ALG" && -n "$VER" ]] || { echo "Error: -S, -A, -R are required (or use --all)"; usage; exit 1; }

# --- Build target names (normal mode) ---
APP_TARGET="app-$V-$SCHEME-$ALG-$VER"
RUN_TARGET="run-$V-$SCHEME-$ALG-$VER"

echo "Version: $V"
echo "Scheme : $SCHEME"
echo "Alg    : $ALG"
echo "Variant: $VER"
echo "main.c : $MAIN_C_PATH"
[[ -n "$TEST_KEY_SET" ]] && echo "Set TEST_KEY=$TEST_KEY_SET"
[[ -n "$TEST_ENC_SET" ]] && echo "Set TEST_ENC=$TEST_ENC_SET"
[[ -n "$TEST_DEC_SET" ]] && echo "Set TEST_DEC=$TEST_DEC_SET"
echo "Targets: ${DO_APP:+$APP_TARGET }${DO_RUN:+$RUN_TARGET}"
echo

# --- Run make commands (normal mode) ---
MAKE_ENV=(SCHEME="$SCHEME" ALG="$ALG" VERSION="$VER" V="$V")

if [[ "$DO_APP" -eq 1 ]]; then
  if [[ "$DRY_RUN" -eq 1 ]]; then
    echo "[dry-run] make ${MAKE_ENV[*]} $APP_TARGET"
  else
    make "${MAKE_ENV[@]}" "$APP_TARGET"
  fi
fi

if [[ "$DO_RUN" -eq 1 ]]; then
  if [[ "$DRY_RUN" -eq 1 ]]; then
    echo "[dry-run] make ${MAKE_ENV[*]} $RUN_TARGET"
  else
    make "${MAKE_ENV[@]}" "$RUN_TARGET"
  fi
fi
