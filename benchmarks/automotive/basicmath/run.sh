PATH2LIB=~/Final583/build/gather_data/LLVMPJT.so      # Specify your build directory in the project
PASS=-heuristics

# Canonicalize natural loops
opt -loop-simplify ${1}.bc -o ${1}.ls.bc
# Instrument profiler
opt -pgo-instr-gen -instrprof ${1}.ls.bc -o ${1}.ls.prof.bc
# Generate binary executable with profiler embedded
clang -fprofile-instr-generate ${1}.ls.prof.bc -o ${1}_prof

# Generate profiled data
./${1}_prof > correct_output
llvm-profdata merge -o ${1}.profdata default.profraw

# Apply Heuristics
opt -o ${1}.heuristics.bc -pgo-instr-use -pgo-test-profile-file=${1}.profdata -load ${PATH2LIB} ${PASS} < ${1}.ls.bc > /dev/null

