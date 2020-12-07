#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

year=$1; shift
day=$1; shift

dayDir=${DIR}/${year}/day${day}

if [ -e ${dayDir} ]; then
	echo "Directory $dayDir already exists! Exiting"
	exit 1
fi

mkdir -p ${dayDir}

file=${dayDir}/Makefile
echo "include ../../Makefile.conf"   > ${file}
echo ""                             >> ${file}
echo "SRCS_A += main.cpp"           >> ${file}
echo ""                             >> ${file}
echo "include ../../Makefile.rules" >> ${file}


file=${dayDir}/main.cpp
echo "#include \"common/types.h\"" > ${file}
echo "" >> ${file}
echo "#include \"utils/file.h\""   >> ${file}
echo "#include \"utils/utils.h\""  >> ${file}
echo "" >> ${file}
echo "#define DEBUG_LEVEL 5" >> ${file}
echo "#include \"common/debug.h\"" >> ${file}
echo "" >> ${file}
echo "int main(int argc, char *argv[]) {" >> ${file}
echo "	auto lines = File::readAllLines(argv[1]);" >> ${file}
echo "" >> ${file}
echo "	return 0;" >> ${file}
echo "}" >> ${file}

curl --silent --fail https://adventofcode.com/${year}/day/${day}/input -o ${dayDir}/data.txt
if [ $? -ne 0 ]; then
	echo "Cannot download input data for day ${day} of year ${year}"
	rm -f ${dayDir}/data.txt
fi

cd ${dayDir}