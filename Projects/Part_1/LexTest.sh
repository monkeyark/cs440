#!/bin/bash

TIMEOUT="30"
SCRIPT=$0
ERRLINES=10
EXT="lexer"
ERR="error1"
FILES="files"

bailout()
{
  echo
  echo "Caught CTRL-C, terminating script"
  echo
  exit 1
}


usage()
{
  echo "Usage: $SCRIPT \"command to run\" [options] infile infile ..."
	echo "Run compiler executable, in mode 1, on specified input files"
	echo "and compare with expected output files."
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
  awk '/^Lexer error in .*[Ll]ine *[0-9]*/ { errors++; if (errors>1) exit }\
                              { if (errors) print }' $1
}

# Arg1: whose (quoted string)
# Arg2: file
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


highlight()
{
  if [ "x$1" == "x$2" ]; then
    cyan "$2" " "
  else
    red "$2" " "
  fi
}

# Arg1: oracle file
# Arg2: student file
mydiff()
{
  # Read oracle lines
  oracle=()
  olexeme=()
  i=0
  printf "%15s  ------------------------------------------------------------\n" " "
  while read file fname line lnum token tnum text lexeme; do
    olexeme+=("$lexeme")
    oracle+=("$file $fname $line $lnum $token $tnum $text")
    printf "%15s  | %s\n" " " "${oracle[$i]}"
    i=$[i+1]
  done < $1
  printf "%15s  ------------------------------------------------------------\n" " "
  printf "%15s  Student output:\n" " "
  printf "%15s  ------------------------------------------------------------\n" " "

  # Read and compare student lines
  i=0
  while read sfile sfn sline sln stoken stid stext slexeme; do
    read ofile ofn oline oln otoken otid otext <<< "${oracle[$i]}"

    printf "%15s  | " " "
    highlight "$ofile" "$sfile"
    highlight "$ofn" "$sfn"
    highlight "$oline" "$sline"
    highlight "$oln" "$sln"
    highlight "$otoken" "$stoken"
    highlight "$otid" "$stid"
    highlight "$otext" "$stext"
    highlight "${olexeme[$i]}" "$slexeme"
    printf "\n"

    i=$[i+1]
  done < $2
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
    if [ -f $indir/$inbase.files ]; then
      while read src; do
        cp $indir/$src .
      done < $indir/$inbase.files
    fi
  fi

  nice $EXE -1 $inbase.c 1> /dev/null 2> $inbase.$ERR &
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
          red "    Timeout exceeded ($TIMEOUT seconds), terminating"
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

  # Cleanup
  if [ "$indir" != "." ]; then
    rm $inbase.[ch]
    if [ -f $indir/$inbase.files ]; then
      while read src; do
        rm $src
      done < $indir/$inbase.files
    fi
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
    echo "N"
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
#   m no error detected       (red)
compareInvalid()
{
  local out="$1.$EXT"
  local err="$1.$ERR"
  local dir="$2"

  if [ ! -f $dir/$err ]; then
    echo ":"
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

# are we generating outputs?

  argdir=`dirname $arg`
  argbase=`basename -s .c $arg`
  argout="$argbase.$EXT"
  argerr="$argbase.$ERR"

  if [ $GENBASIC ]; then
    echo generating regular outputs for $argbase.c in mode 1
    timeoutCompile $argdir $argbase

    rm -f OUTPUTS_B/$argerr OUTPUTS_B/$argout

    if [ -s $argerr ]; then
      cp $argerr OUTPUTS_B
      removeDuplicate $argerr
    else
      cp $argout OUTPUTS_B
      removeDuplicate $argout
    fi
    rm -f $argout $argerr

    continue
  fi

  if [ $GENEXTRA ]; then
    echo generating extra outputs for $argbase.c in mode 1
    timeoutCompile $argdir $argbase

    rm -f OUTPUTS_X/$argerr OUTPUTS_X/$argout

    if [ -s $argerr ]; then
      cp $argerr OUTPUTS_X
      removeDuplicate $argerr
    else
      cp $argout OUTPUTS_X
      removeDuplicate $argout
    fi
    rm -f $argout $argerr

    continue
  fi


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
          showFirstError "Instructor error message" OUTPUTS_X/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    c?) cyan  "1st Error line matches basic" "\n"
          showFirstError "Instructor error message" OUTPUTS_B/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    ?y) yellow "1st Error line number matches extra" "\n"
          showFirstError "Instructor error message" OUTPUTS_X/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    y?) yellow "1st Error line number matches basic" "\n"
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
          showFirstError "Instructor basic message" OUTPUTS_B/$argerr
          showFirstError "Instructor extra message" OUTPUTS_X/$argerr
          showFirstError "Student error message (first $ERRLINES lines)" $argerr
          ;;

    ?R | R?)
          red "Different output" "\n"
          showDifferences OUTPUTS_B $argout
          showDifferences OUTPUTS_X $argout
          ;;

    N?) red "No output" "\n"
          ;;

    m?) red "No error detected" "\n"
          ;;

    *)    red "Unexpected state: $status_basic$status_extra" "\n"

  esac

  rm -f $argout $argerr
done

