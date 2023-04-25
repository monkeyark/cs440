#!/bin/bash

TIMEOUT="30"
SCRIPT=$0
ERRLINES=10
CONTEXT=2
MODE="-3"
EXT="types"
ERR="error"
FILES="files"
RMFILES=""

bailout()
{
  echo
  echo "Caught CTRL-C, terminating script"
  echo
  rm -f $RMFILES
  exit 1
}


usage()
{
  echo "Usage: $SCRIPT \"command to run\" [options] infile infile ..."
	echo "Run compiler executable, in type checking mode, on specified"
  echo "input files and compare with expected output files."
  echo "Input files should be in a subdirectory; if so they will be"
  echo "copied into the working directory to run.  Expected outputs"
  echo "should be in directories OUTPUT_B for basic and OUTPUT_X for"
  echo "extra credit (if different).  Input files without corresponding"
  echo "output files will be skipped."
  echo "Options: "
  echo "  -gb:  generate non-extra credit output files"
  echo "  -ge:  generate extra credit output files"
  echo "  -t N: set timeout to N seconds (default: $TIMEOUT)"
  exit $1
}

green()
{
  printf "[1;32m%s[0;39m$2" "$1"
}

cyan()
{
  printf "[1;36m%s[0;39m$2" "$1"
}

yellow()
{
  printf "[1;33m%s[0;39m$2" "$1"
}

red()
{
  printf "[1;31m%s[0;39m$2" "$1"
}

# Arg1: error file
firstError()
{
  awk '/^.*[Ee]rror in .*[Ll]ine *[0-9]*/ { errors++; if (errors>1) exit }\
                              { if (errors) print }' $1
}

# Arg1: error file
firstErrorLine()
{
  if [ -f $1 ]; then
    firstError $1 | head -n 1 | sed 's/.*[Ll]ine *\([0-9][0-9]*\).*/\1/'
  fi
}

# Arg1: line number or empty string
getAWKcontext()
{
  if [ "$1" ]; then
    local start=$[ $1 - $CONTEXT ]
    local stop=$[ $1 + $CONTEXT ]
    echo "(NR>=$start && NR<=$stop)"
  fi
}

# Arg1: input file
# Arg2: error line1 (or missing)
# Arg3: error line2 (or missing)
# Arg4: error line3 (or missing)
showContext()
{
  if [ ! "$2" ]; then
    return
  fi

  local inf="$1"

  local c1=`getAWKcontext $2`
  local c2=`getAWKcontext $3`
  local c3=`getAWKcontext $4`

  local acond=""

  if [ "$c1" ]; then
    acond="$acond $c1 ||"
  fi
  if [ "$c2" ]; then
    acond="$acond $c2 ||"
  fi
  if [ "$c3" ]; then
    acond="$acond $c3 ||"
  fi
  acond="($acond false)"

#   echo "$acond"

  printf "%15s  Input file relevant lines\n" " "
  printf "%15s  ------------------------------------------------------------\n" " "
  awk "$acond {printf(\"%15d  | %s\n\", NR, \$0);}" $inf
  printf "%15s  ------------------------------------------------------------\n" " "
}

# Arg1: whose (quoted string)
# Arg2: error file
showFirstError()
{
  if [ ! -f "$2" ]; then
    return 0
  fi
  printf "%15s  $1:\n" " "
  printf "%15s  ------------------------------------------------------------\n" " "
  firstError $2 | awk "(NR<=$ERRLINES){print \"                 | \" \$0}"
  printf "%15s  ------------------------------------------------------------\n" " "

}


# Arg1: oracle line
# Arg2: student line
diffline()
{
  printf "%15s  " " "

  local strip1=`echo $1`
  local strip2=`echo $2`
  if [ "x$strip1" == "x$strip2" ]; then
    cyan "| $2" "\n"
    return
  fi
  red "|" " "

  local oracle="$1"
  local student="$2"

  while [ "$student" ]; do
    read sfront srest <<< "$student"
    student="$srest"
    read ofront orest <<< "$oracle"
    oracle="$orest"

    if [ "x$sfront" == "x$ofront" ]; then
      cyan "$sfront" " "
    else
      red "$sfront" " "
    fi
  done

  if [ "$oracle" ]; then
    red "((eol))" " "
  fi
  printf "\n"
}

# Arg1: oracle file
# Arg2: student file
mydiff()
{
  # Read oracle lines
  oracle=()
  i=0
  printf "%15s  ------------------------------------------------------------\n" " "
  while read line; do
    oracle+=("$line")
    printf "%15s  | " " "
    echo $line
    i=$[i+1]
  done < $1
  local numlines="$i"
  printf "%15s  ------------------------------------------------------------\n" " "
  printf "%15s  Student output:\n" " "
  printf "%15s  ------------------------------------------------------------\n" " "

  # Read and compare student lines
  i=0
  while read line; do
    diffline  "${oracle[$i]}" "$line"
    i=$[i+1]
  done < $2
  local missing=$[ numlines - i ]
  if [ "$missing" -gt 0 ]; then
    diffline " " "((Missing $missing lines))"
  fi
  printf "%15s  ------------------------------------------------------------\n" " "
}



# Arg1: oracle directory
# Arg2: output file
showDifferences()
{
  if [ ! -f "$1/$2" ]; then
    return 0
  fi
  local which=""
  if [ "$1" == "OUTPUTS_X" ]; then
    which="Instructor extra credit output"
  else
    which="Instructor basic output"
  fi
  printf "%15s  $which:\n" " "
  mydiff $1/$2 $2
}




# Arg1: input directory
# Arg2: base filename
# Arg3: timeout
timeoutCompile()
{
  local indir="$1"
  local inbase="$2"
  local timeout="$3"

  if [ "$indir" != "." ]; then
    cp $indir/$inbase.[ch] .
    RMFILES="$RMFILES $inbase.[ch]"
    if [ -f $indir/$inbase.files ]; then
      while read src; do
        cp $indir/$src .
        RMFILES="$RMFILES $src"
      done < $indir/$inbase.files
    fi
  fi

  RMFILES="$RMFILES $inbase.$ERR $inbase.$EXT $inbase.timeout"

  nice $EXE $MODE $inbase.c 1> /dev/null 2> $inbase.$ERR &
  local pid=$!

  if [ ! "$timeout" ]; then
    wait
  else
    disown -r
# ^ avoid messages if we kill the process
    local killsignal=""
    SECONDS=0
    while true; do
      sleep 1
      if ps $pid > /dev/null; then
        # still running
        if [ $SECONDS -gt $TIMEOUT ]; then
          touch $inbase.timeout
          if [ $killsignal ]; then
            kill $killsignal $pid
          else
            kill $pid
            killsignal="-9"
          fi
        fi
      else
        # completed
        break
      fi
    done
  fi
}

#
# Arg1: output or error filename
removeDuplicate()
{
  local outf="$1"
  if [ -f OUTPUTS_B/$outf -a -f OUTPUTS_X/$outf ]; then
    if diff -w -q OUTPUTS_B/$outf OUTPUTS_X/$outf > /dev/null; then
      rm OUTPUTS_X/$outf
    fi
  fi
}

# Arg1: input base file
# Arg2: oracle directory
#
# Writes:
#   : missing oracle
#   G output matches and empty error (Green)
#   Y output matches but nonempty error (Yellow)
#   R output different (Red)
#   N no output (Red)
#   m unexpected error (red)
#
compareValid()
{
  local out="$1.$EXT"
  local err="$1.$ERR"
  local dir="$2"

  if [ ! -f $dir/$out ]; then
    echo ":"
    return 0
  fi
  if [ ! -s $out ]; then
    if [ -s $err ]; then
      echo "m"
    else
      echo "N"
    fi
    return 0
  fi
  if diff -w -q $dir/$out $out > /dev/null; then
    if [ -s $err ]; then
      echo "Y"
    else
      echo "G"
    fi
  else
    echo "R"
  fi
}

# Arg1: input base file
# Arg2: oracle directory
#
# Writes:
#   : missing oracle
#   g first error matches perfectly (green)
#   c first line of error matches (cyan)
#   y first error line numbers match  (yellow)
#   r first errors different  (red)
#   e empty error stream      (red)
#   m unexpected error        (red)
compareInvalid()
{
  local out="$1.$EXT"
  local err="$1.$ERR"
  local dir="$2"

  if [ ! -f $dir/$err ]; then
    echo ":"
    return 0
  fi

  if [ ! -s $err ]; then
    echo "e"
    return 0
  fi

  firstoracle=`firstError $dir/$err`
  firststudent=`firstError $err`

  if [ ! "$firststudent" ]; then
    echo "m"
    return 0
  fi

  if [ "$firstoracle" == "$firststudent" ]; then
    echo "g"
    return 0
  fi

  linetextoracle=`head -n 1 <<< "$firstoracle"`
  linetextstudent=`head -n 1 <<< "$firststudent"`
  if [ "$linetextoracle" == "$linetextstudent" ]; then
    echo "c"
    return 0
  fi

  lineoracle=`sed 's/.*\([Ll]ine *[0-9]*\).*/\1/' <<< "$linetextoracle"`
  linestudent=`sed 's/.*\([Ll]ine *[0-9]*\).*/\1/' <<< "$linetextstudent"`

  if [ "$lineoracle" == "$linestudent" ]; then
    echo "y"
    return 0
  fi

  echo "r"
}



#############################################################

# Arg1: 'B'asic or e'X'tra
# Arg2: directory holding input file
# Arg3: base name of input file (without .c)
generate_oracle()
{
  local which="$1"
  local argdir="$2"
  local argbase="$3"
  local argout="$argbase.$EXT"
  local argerr="$argbase.$ERR"

  local human_which=""
  case "$which" in
    B)  human_which="regular"
        ;;
    X)  human_which="extra"
        ;;
    *)  echo "Unexpected oracle type: $which"
        return 1
  esac

  echo Generating $human_which outputs for $argbase.c in mode $MODE
  timeoutCompile $argdir $argbase

  rm -f OUTPUTS_$which/$argerr OUTPUTS_$which/$argout

  if [ -s $argerr ]; then
    cp $argerr OUTPUTS_$which
    removeDuplicate $argerr
  else
    if [ ! -s $argout ]; then
      echo "Warning: empty output file $argout"
    fi
    cp $argout OUTPUTS_$which
    removeDuplicate $argout
  fi

  rm -f $RMFILES
  RMFILES=""
}

#############################################################

trap bailout INT

if [ $# -eq 0 ]; then
  usage 0
fi

EXE="$1"
shift

GENBASIC=""
GENEXTRA=""

echo Running tests using compiler:
$EXE -0 | awk '{print "  | " $0}'

echo " "

TOnext=""
for arg; do

# check for options/switches

  if [ $TOnext ]; then
    TIMEOUT=$arg
    TOnext=""
    continue
  fi

  case "$arg" in
    -gb)
        GENBASIC="y"
        GENEXTRA=""
        mkdir -p OUTPUTS_B
        continue
        ;;

    -ge)
        GENBASIC=""
        GENEXTRA="y"
        mkdir -p OUTPUTS_X
        continue
        ;;

    -t)
        TOnext="y"
        continue
        ;;

  esac

  argdir=`dirname $arg`
  argbase=`basename -s .c $arg`
  argout="$argbase.$EXT"
  argerr="$argbase.$ERR"

#
# are we generating outputs?
#

  if [ $GENBASIC ]; then
    generate_oracle "B" $argdir $argbase
    continue
  fi

  if [ $GENEXTRA ]; then
    generate_oracle "X" $argdir $argbase
    continue
  fi


#
#
# Check for basic output/error files
#

  skip="y"
  if [ -f OUTPUTS_B/$argout ]; then
    skip=""
  fi
  if [ -f OUTPUTS_B/$argerr ]; then
    skip=""
  fi
  if [ "$skip" ]; then
    continue
  fi

#
# Run student compiler
#

  timeoutCompile $argdir $argbase $TIMEOUT
  if [ -f $argbase.timeout ]; then
    status_basic="T"
    status_extra="T"
  else
    #
    # Compare with basic
    #
    if [ -f OUTPUTS_B/$argerr ]; then
      status_basic=`compareInvalid $argbase OUTPUTS_B`
    else
      status_basic=`compareValid $argbase OUTPUTS_B`
    fi

    #
    # Compare with extra credit
    #
    if [ -f OUTPUTS_X/$argerr ]; then
      status_extra=`compareInvalid $argbase OUTPUTS_X`
    else
      status_extra=`compareValid $argbase OUTPUTS_X`
    fi
  fi

#
# Display best result
#

  # printf "%-15s$status_basic$status_extra" "$argbase.c"
  printf "%-15s  " "$argbase.c"

  case "$status_basic$status_extra" in

    ?g) green "1st Error matches extra" "\n"
          ;;

    g?) green "1st Error matches basic" "\n"
          ;;

    ?c) cyan  "1st Error line matches extra" "\n"
          showContext $arg `firstErrorLine $argerr`
          showFirstError "Instructor error message" OUTPUTS_X/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr

          ;;

    c?) cyan  "1st Error line matches basic" "\n"
          showContext $arg `firstErrorLine $argerr`
          showFirstError "Instructor error message" OUTPUTS_B/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    ?y) yellow "1st Error line number matches extra" "\n"
          showContext $arg `firstErrorLine $argerr`
          showFirstError "Instructor error message" OUTPUTS_X/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    y?) yellow "1st Error line number matches basic" "\n"
          showContext $arg `firstErrorLine $argerr`
          showFirstError "Instructor error message" OUTPUTS_B/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;


    G?) green "Output    matches basic" "\n"
          ;;

    ?G) green "Output    matches extra" "\n"
          ;;

    Y?) green "Output    matches basic "
          yellow "but non-empty error stream" "\n"
          ;;

    ?Y) green "Output    matches extra "
          yellow "but non-empty error stream" "\n"
          ;;

    r? | ?r)
          red "1st Error is different" "\n"
          showContext $arg `firstErrorLine OUTPUTS_B/$argerr` `firstErrorLine OUTPUTS_X/$argerr` `firstErrorLine $argerr`
          showFirstError "Instructor basic message" OUTPUTS_B/$argerr
          showFirstError "Instructor extra message" OUTPUTS_X/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    ?R | R?)
          red "Different output" "\n"
          showDifferences OUTPUTS_B $argout
          showDifferences OUTPUTS_X $argout
          ;;

    m? | ?m)
          red "Unexpected error" "\n"
          showRawError $argerr
          ;;

    ?e)   red "Empty error stream" "\n"
          ;;

    ?N)   red "No output" "\n"
          ;;

    e?)   red "Empty error stream" "\n"
          ;;

    N?)   red "No output" "\n"
          ;;

    TT)   red "Timeout exceeded ($TIMEOUT seconds)" "\n"
          ;;

    *)    red "Unexpected state: $status_basic$status_extra" "\n"

  esac

  rm -f $RMFILES
  RMFILES=""
done

