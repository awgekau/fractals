#!/bin/bash

# *-------------------- `check.sh` Exit Codes --------------------* #
#  0 : Success              :   Student gets at least 50%
#  1 : Student error        :   `diff` fail, timeouts, etc
#  2 : Development error    :   Missing testcase dirs, missing output files, etc


# Required files, compilation flags and time limits for each
compilation_flags="-Wall -Werror -std=c99"
required_source_files=("ex3q1.c")
time_limits=(5)
header_path="./src/ex3q1.h"

# Required directories
source_dir="./src"
testcases_dir="./Testcases"

# Postfixes for files found or generated in the testcases directory
input_postfix="_input.txt"
output_postfix="_output.ppm"
student_postfix="_student.ppm"

# Logging functions and starting message
width=$(tput cols);
log()     { printf "\033[00m[CHECK] %s\033[0m\n" "$1"; }
info()    { printf "\033[33m[CHECK] %s\033[0m\n" "$1"; }
success() { printf "\033[32m[CHECK] %s\033[0m\n" "$1"; }
warning() { printf "\033[35m[CHECK] Warning: %s\033[0m\n" "$1"; }
br()      { break=$(printf "%*s" "$((width-8))" "" | tr " " "-"); log "$break"; }
error() {
    printf "\033[31m[CHECK] Error: %s\033[0m\n" "$1";
    br; info "END: Check failed."; br; echo;
    exit "$2";
}
echo; br; success "START";

# Check if required directory exists
if [ ! -d "$testcases_dir" ]; then
    error "Expected test cases directory '$testcases_dir' does not exist." 2
fi
if [ ! -d "$source_dir" ]; then
    error "Expected source directory '$source_dir' does not exist." 1
fi

# Check that all required files exist
for file in "${required_source_files[@]}"; do
    if [ ! -e "$source_dir/$file" ]; then
        error "Required file '$source_dir/$file' not found." 1
    fi
done

# Iterate through source files matching 'ex[0-9]q[0-9].c' in the source directory
for i in "${!required_source_files[@]}"; do

    br

    source_path="$source_dir/${required_source_files[$i]}"
    exe_path=${source_path/.c/}
    program_name=$(basename "$exe_path")
    time_limit="${time_limits[$i]}"
    
    log "Checking '$program_name'..."

    # Check for extra includes
    extra_includes=$(cat "$source_path" | grep '^#include' | grep -v -E 'ex3q1.h' | wc -l)
    if [[ extra_includes -ne '0' ]]; then
        error "You're not allowed to include libraries other than the provided header file." 1
    fi
    extra_includes=$(cat "$header_path" | grep '^#include' | grep -v -E 'stdlib.h|stdio.h' | wc -l)
    if [[ extra_includes -ne '0' ]]; then
        error "You're not allowed to include libraries other than the provided header file." 1
    fi

    # Get exercise number from source file name by regex
    [[ $source_path =~ ex([0-9])q([0-9]).c ]]
    ex_num=${BASH_REMATCH[2]}

    # Compile source file
    if gcc ${compilation_flags} "$source_path" -o "$exe_path"; then
        success "'$source_path' compiled successfully."
    else
        error "Compilation of '$source_path' failed." 1
    fi

    # Iterate over every input file corresponding to the exercise number
    for input_path in "$testcases_dir"/"$ex_num"[0-9]"$input_postfix"; do

        log ""

        # Check if the corresponding output file exists. If it doesn't then skip this test case
        output_path=${input_path/"$input_postfix"/"$output_postfix"}
        if [[ ! -e $output_path ]]; then
            info "Missing output file: '$output_path'. Skipping test case."
            continue
        fi

        # Set timeout and generate student output file
        student_output_path=${input_path/"$input_postfix"/"$student_postfix"}
        log "Testing using '$input_path'..."
        timeout "$time_limit" "$exe_path" < "$input_path" > "$student_output_path"
        
        # Check if program took longer than the time limit, or the program exited with non-zero code
        exit_code=$?
        if [ $exit_code -eq 124 ]; then
            error "'$program_name' timed out." 1
        elif [ $exit_code -ne 0 ]; then
            error "'$program_name' exited with non-zero code." 1
        else
            success "'$program_name' terminated successfully under the time limit."        
        fi

        # Run `ppmcmp` then check if the output is less than or equal to 5
        ppmcmp=$(python3 ppmcmp.py "$output_path" "$student_output_path" 2> /dev/null)
        if awk "BEGIN {exit !($ppmcmp <= 5)}"; then
            success "'$student_output_path' is close enough to the output: ppmcmp = $ppmcmp"
        else
            error "'$program_name' does not give expected output: ppmcmp = $ppmcmp" 1
        fi
        
    done
    
    # Cleanup
    rm -f "$exe_path"
    
done

# Ending message
br; success "END: Passed! You will get at least 50% on this lab."; br; echo;
