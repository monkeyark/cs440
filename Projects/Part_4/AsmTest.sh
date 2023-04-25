#!/bin/bash

TIMEOUT="30"
SCRIPT=$0
ERRLINES=10
CONTEXT=2
MODE="-4"
EXT="j"
ERR="error"
FILES="files"
RMFILES=""
QUIETPASS="y"

bailout()
{
    echo
    echo "$1"
    echo "Terminating script"
    echo
    rm -f $RMFILES
    exit 1
}

ctrlc()
{
    bailout "Caught CTRL-C"
}

debug()
{
#    echo "$1" > /dev/stderr
    echo "$1" > /dev/null
}

usage()
{
    echo "Usage: $SCRIPT \"command to run\" [options] infile infile ..."
    echo "Run compiler executable, in code generation mode, on specified"
    echo "input files and examine generated output."
    echo "Input files should be in a subdirectory; if so they will be"
    echo "copied into the working directory to run.  Specifications for"
    echo "how to examine generated output should be in files in directory SPECS."
    echo "Any error messages should be in files in directory OUTPUTS_B."
    echo "Input files without corresponding specification files will be skipped."
    echo "Options: "
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

#   debug "$acond"

    printf "%20s  Input file relevant lines\n" " "
    printf "%20s  ----------------------------------------------------\n" " "
    awk "$acond {printf(\"%15d  | %s\n\", NR, \$0);}" $inf
    printf "%20s  ----------------------------------------------------\n" " "
}

# Arg1: whose (quoted string)
# Arg2: error file
showFirstError()
{
    if [ ! -f "$2" ]; then
        return 0
    fi
    printf "%20s  $1:\n" " "
    printf "%20s  ----------------------------------------------------\n" " "
    firstError $2 | awk "(NR<=$ERRLINES){print \"                      | \" \$0}"
    printf "%20s  ----------------------------------------------------\n" " "
}

# Arg1: error file
showRawError()
{
    if [ ! -s "$1" ]; then
        return 0
    fi
    printf "%20s  Student error stream (first $ERRLINES lines):\n" " "
    printf "%20s  ----------------------------------------------------\n" " "
    awk "(NR<=$ERRLINES){print \"                      | \" \$0}" $1
    printf "%20s  ----------------------------------------------------\n" " "
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
#       ^ avoid messages if we kill the process
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


# Arg1: input base file
# Arg2: oracle directory
#
compareInvalid()
{
    local inf="$1.c"
    local out="$1.$EXT"
    local err="$1.$ERR"
    local dir="$2"

    if [ ! -f $dir/$err ]; then
        return 0
    fi

    if [ ! -s $err ]; then
        red "Empty error stream" "\n"
        return 0
    fi

    firstoracle=`firstError $dir/$err`
    firststudent=`firstError $err`

    if [ ! "$firststudent" ]; then
        red "Unexpected error" "\n"
        showRawError $err
        return 0
    fi

    if [ "$firstoracle" == "$firststudent" ]; then
        green "1st Error matches" "\n"
        return 0
    fi

    linetextoracle=`head -n 1 <<< "$firstoracle"`
    linetextstudent=`head -n 1 <<< "$firststudent"`
    lineoracle=`sed 's/.*\([Ll]ine *[0-9]*\).*/\1/' <<< "$linetextoracle"`
    linestudent=`sed 's/.*\([Ll]ine *[0-9]*\).*/\1/' <<< "$linetextstudent"`

    if [ "$linetextoracle" == "$linetextstudent" ]; then
        cyan  "1st Error line matches" "\n"
    elif [ "$lineoracle" == "$linestudent" ]; then
        yellow "1st Error line number matches" "\n"
    else
        red "1st Error is different" "\n"
    fi

    showContext $inf `firstErrorLine $err`
    showFirstError "Instructor error message" $dir/$err
    showFirstError "Student error message (first $ERRLINES lines)" $err
}


#############################################################

#
# Global arrays and variables
#
#   methline: array of method lines
#   methlength: length of methline array
#
#   defname: array of definition names
#   defbody: array of definition bodies
#   numdefs: length of defname, defbody arrays
#
numdefs=0

#
# Arg1: original sequence
# Arg2: next item
seq_append()
{
    if [ ! "$1" ]; then
        echo "$2"
    else
        echo "$1,,$2"
    fi
}

#
# Arg1: disjuncts (a OR b OR c ...)
# Arg2: index
#
get_disjunct()
{
    awk '-F OR ' -v "j=$2" '{print $j}' <<< "$1" | sed 's/^ *//' | sed 's/ *$//'
}

#
# Arg1: definition name to look for
# echo the body that matches the given name, or empty string
findDef()
{
    # debug "searching for definition $1"
    local d="0"
    while [ "$d" -lt "$numdefs" ]; do
        if [ "$1" == "${defname[$d]}" ]; then
            # debug "Found in slot $d: ${defbody[$d]}"
            echo "${defbody[$d]}"
            return 0
        fi
        d=$[d+1]
    done
    return 1
}

#
# Arg1: header or empty
#
showMethod()
{
    if [ "$1" ]; then
        printf "\t\t  $1\n"
    fi
    printf "%20s  ----------------------------------------------------\n" " "
    indent=""
    local i=0
    while [ "$i" -lt "$methlength" ]; do
        local line="${methline[$i]}"
        i=$[i+1]
        local fw=`awk '{print $1}' <<< $line`
        if [ ".end" == "$fw" ]; then
            indent="${indent:4:200}"
            printf "%20s  | $indent$line\n" " "
            continue
        fi
        if [ ".code" == "${fw:0:5}" -o ".meth" == "${fw:0:5}" ]; then
            printf "%20s  | $indent$line\n" " "
            indent="$indent    "
            continue
        fi
        printf "%20s  | $indent" " "
        yellow "$line" "\n"
    done
    printf "%20s  ----------------------------------------------------\n" " "
}

#
# Arg1: MATCH or OMIT
# Arg2: 1 on matched sequence, 0 otherwise
# Arg3: sequence we wanted to match
#
showMethodMatch()
{
    if [ 0 -eq "$methlength" ]; then
        red   "No method generated" " (or couldn't extract it)\n"
        return
    fi

    local matchcode=""

    if [ "MATCH" == "$1" ]; then
        matchcode="1"
    else
        matchcode="0"
    fi

    if [ "$2" == "$matchcode" ]; then
        green "$1" "\n"
        if [ "$QUIETPASS" ]; then
            return
        fi
    else
        red   "Did not $1" "\n"
    fi

    debug "args: $1 $2 $3"

    local seqlen=`awk -F,, '{print NF}' <<< "$3"`
    local s=0

    printf "%20s  ----------------------------------------------------\n" " "
    printf "%20s  Closest sequence to $1\n" " "
    printf "%20s  ----------------------------------------------------\n" " "
    local i=0
    while [ "$s" -lt "$seqlen" ]; do
        local line="${methline[$i]}"
        i=$[i+1]
        s=$[s+1]
        local item=`awk -F,, -v "i=$s" '{print $i}' <<< "$3"`
        if [ ":" == "$item" ]; then
            continue
        fi
        printf "%20s  |         " " "
        egrep "$item" <<< "$line" > /dev/null
        local code="$?"
        if [ "$matchcode" != "$code" ]; then
            cyan "$item" "\n"
        else
            red "$item" "\n"
        fi
    done
    printf "%20s  ----------------------------------------------------\n" " "
    printf "%20s  Student's method\n" " "
    printf "%20s  ----------------------------------------------------\n" " "
    s=0
    i=0
    indent=""
    while [ "$i" -lt "$methlength" ]; do
        local line="${methline[$i]}"
        i=$[i+1]
        s=$[s+1]
        local item=`awk -F,, -v "i=$s" '{print $i}' <<< "$3"`
        if [ ":" == "$item" ]; then
            item=""
        fi

        if [ ".end" == "${line:0:4}" ]; then
            indent="${indent:4:200}"
        fi
        printf "%20s  | $indent" " "
        if [ "$item" ]; then
            egrep "$item" <<< "$line" > /dev/null
            local code="$?"
            if [ "$matchcode" != "$code" ]; then
                cyan "$line" "\n"
            else
                red "$line" "\n"
            fi
        else
            printf "$line\n"
        fi
        if [ ".code" == "${line:0:5}" -o ".meth" == "${line:0:5}" ]; then
            indent="$indent    "
        fi
    done
    printf "%20s  ----------------------------------------------------\n" " "
}


#
# Extract and clean up method into the array.
#
# Arg1: file containing method
# Arg2: method name
#
grab_method()
{
  #
  # Extract and clean up method
  #
    sed -n "/[.]method.* $2/,/[.]end.*method/p" $1 | sed 's/;.*//' | sed 's/ *[.]line.*//' | sed 's/L.*:/&\n/' | grep -v "^ *$" | tr '\t' ' ' | sed 's/ * / /g' > .methfile

    methlength=0
    while read line; do
        if [ ! "$line" ]; then
            continue
        fi
        methline[$methlength]="$line"
        methlength=$[ methlength + 1 ]
    done < .methfile
    rm .methfile
}


#
# Append the 'first' sequence to the matched sequence list.
#
# Arg1: sequence
# Arg2: current item in sequence
# Arg3: matched
#
append_first()
{
    local seq="$1"
    local s="$2"
    local matched="$3"
    local seqlen=`awk -F,, '{print NF}' <<< "$seq"`

    debug "        starting call append_first \"$1\" $2 \"$3\""

    while [ "$s" -le "$seqlen" ]; do
        # Get next item in sequence
        local item=`awk -F,, -v "i=$s" '{print $i}' <<< "$seq"`

        # Pull off the first disjunct
        local disj=`get_disjunct "$item" 1`
        #local disj=`awk '-F OR ' '{print $1}' <<< "$item" | sed 's/ *$//'`

        # Check if this is a defined sequence
        local rec=`findDef $disj`
        if [ "$rec" ]; then
            # Yes.  Build recursively
            matched=`append_first "$rec" 1 "$matched"`
        else
            # No.  Add this to the sequence
            matched=`seq_append "$matched" "$disj"`
        fi

        s=$[ s + 1 ]
    done
    echo "$matched"

    debug "        finished call append_first \"$1\" $2 \"$3\""
    debug "        result is \"$matched\""
}

#
# Attempt to match a sequence from a starting point.
# Arg1: method line to start at
# Arg2: sequence to attempt
#
#   Echos an integer, followed by the best matched sequence.
#   The absolute value of the integer is the index of the "next" instruction.
#   On a perfect match, the index will be positive.
#   On a fail, the index will be negative, indicating how much we were able to match.
#
match_seq()
{
    local i="$1"
    local seqlen=`awk -F,, '{print NF}' <<< "$2"`
    local s=1
    local theseq=""
    debug "attempting sequence $2 of length $seqlen"
    local besti="0"
    local bestseq="" # best sequence when we didn't match
    while [ "$s" -le "$seqlen" ]; do

        local item=`awk -F,, -v "i=$s" '{print $i}' <<< "$2"`
        local line="${methline[$i]}"
        debug "  item is $item"
        debug "  line is $line"

        # Skip . "instructions"
        if [ "." == "${line:0:1}" ]; then
            debug "skipping dot line"
            i=$[i+1]
            theseq=`seq_append "$theseq" ":"`
            continue
        fi

        # TBD Deal with extra labels

        #
        # current item might be the OR of several possible matches;
        # loop through those
        #
        local matched=""
        local partial=""
        local numors=`awk '-F OR ' '{print NF}' <<< "$item"`
        local j=1
        while [ "$j" -le "$numors" ]; do
            local disj=`get_disjunct "$item" $j`
            # local disj=`awk '-F OR ' -v "j=$j" '{print $j}' <<< "$item" | sed 's/ *$//'`
            debug "    disj is \"$disj\""

            # Check if this is a defined sequence
            local rec=`findDef $disj`
            if [ "$rec" ]; then
                # Yes.  Check the defined sequence, recursively
                debug "    Checking defined sequence $disj"
                # local pair=`match_seq $i "$rec"`
                read ii mseq <<< `match_seq $i "$rec"`
                if [ "$ii" -gt 0 ]; then
                    matched="y"
                    i="$ii"
                    theseq=`seq_append "$theseq" "$mseq"`
                    debug "    defined match; seq: $theseq"
                    break
                else
                    debug "    defined fail"
                    if [ "$ii" -lt "$besti" ]; then
                        besti="$ii"
                        bestseq=`seq_append "$theseq" "$mseq"`
                        debug "        New best: $besti $bestseq"
                        partial="y"
                    fi
                fi
            else
                ii=0
                # No.  Check as a regex
                if egrep "$disj" <<< "$line" > /dev/null; then
                    matched="y"
                    i=$[i+1]
                    theseq=`seq_append "$theseq" "$disj"`
                    debug "      regex match; seq: $theseq"
                    break
                else
                    debug "      regex fail"
                fi
            fi

            # No matches; check if we need to extend best sequence

            j=$[j+1]
        done
        if [ "$matched" ]; then
            s=$[s+1]
            besti="-$i"
            continue
        fi

        debug "    failed to match $item with $line"
        if [ "$partial" ]; then
            # Partial match
            s=$[s+1]
            bestseq=`append_first "$2" "$s" "$bestseq"`
        else
            # No match
            bestseq=`append_first "$2" "$s" "$theseq"`
        fi

        echo "$besti $bestseq"
        return
    done
    # Success!
    echo "$i $theseq"
}

#
# Attempt to match a sequence starting anywhere in the method.
# Arg1: sequence to attempt
#
#
match_anywhere()
{
    SEQUENCE=""
    local v=0
    local dots=""
    local pair=""
    local bestseq=""
    local beststp="1"
    while [ "$v" -lt "$methlength" ]; do
        local line="${methline[$v]}"
        debug "MATCHING STARTING AT LINE $v: $line"
        if [ "." != "${line:0:1}" ]; then
            read stp seq <<< `match_seq $v "$1"`
            if [ 0 -lt $stp ]; then
                echo 1 `seq_append "$dots" "$seq"`
                return
            fi
            debug "Candidate sequence $stp $seq"
            if [ "$stp" -lt "$beststp" ]; then
                beststp="$stp"
                bestseq=`seq_append "$dots" "$seq"`
                debug "   ... is new best"
            fi
        fi
        v=$[v+1]
        dots=`seq_append "$dots" ":"`
    done
    echo "0 $bestseq"
}


#
# Arg1: base name
# Arg2: spec directory
parse_spec()
{
    numdefs=0
    local lineno=0
    local asmfile="$1.$EXT"
    local specfile="$2/$1.spec"
    local nextname=""
    echo

    while read word ident extra; do
        lineno=$[ lineno + 1 ]
        if [ ! "$word" ]; then
            continue
        fi
        if [ "#" == "$word" ]; then
            continue
        fi

        #
        # Sequence definitions
        #
        if [ "$word" == "define" ]; then
            if findDef "$ident" > /dev/null; then
                bailout "Error line $lineno: $ident already defined"
            fi
            defname[$numdefs]="$ident";
            # Read until }
            local body=""
            while read line; do
                lineno=$[ lineno + 1 ]
                line=`sed 's/#.*$//' <<< "$line"`
                if [ ! "$line" ]; then
                    continue
                fi
                if [ "}" == "$line" ]; then
                    break
                fi
                if [ "$body" ]; then
                    body="$body,,$line"
                else
                    body="$line"
                fi
            done
            defbody[$numdefs]="$body"
            numdefs=$[ numdefs + 1 ]

            # echo "Parsed definition $ident = $body"
            continue
        fi

        #
        # Method tests
        #
        if [ "$word" == "within" ]; then
            echo "    Extracting method $ident"
            grab_method $asmfile $ident
            # showMethod "Extracted method $ident"
            local testno=0

            #
            # Read items in the method test
            #
            while read cmd seq; do
                lineno=$[ lineno + 1 ]

                if [ ! "$cmd" ]; then
                    continue
                fi
                if [ "#" == "$cmd" ]; then
                    continue
                fi

                if [ "done" == "$cmd" ]; then
                    break
                fi

                testno=$[ testno + 1 ]

                if [ "NAME" == "$cmd" ]; then
                    nextname="$seq"
                    continue
                fi

                if [ "MISSING" == "$cmd" ]; then
                    printf "%20s: " "Method missing?"
                    if [ "$methlength" -gt 0 ]; then
                        red   "PRESENT" "\n"
                    else
                        green "MISSING" "\n"
                    fi
                    nextname=""
                    continue
                fi

                if [ "PRESENT" == "$cmd" ]; then
                    printf "%20s: " "Method present?"
                    if [ "$methlength" -gt 0 ]; then
                        green "PRESENT" "\n"
                    else
                        red   "MISSING" "\n"
                    fi
                    nextname=""
                    continue
                fi

                if [ "MATCH" == "$cmd" -o "OMIT" == "$cmd" ]; then
                    read code thematch <<< `match_anywhere "$seq"`
                    debug "code: $code thematch: $thematch"

                    if [ "$nextname" ]; then
                        printf "%20s: " "$nextname"
                    else
                        printf "             Test %02d: " "$testno"
                    fi
                    showMethodMatch "$cmd" "$code" "$thematch"
                    nextname=""
                    continue
                fi

                bailout "Syntax error line $lineno: $cmd $seq"
            done
            continue
        fi

        bailout "Syntax error line $lineno: $word $ident $extra"
    done < $specfile
    echo
}

#############################################################

trap ctrlc INT

if [ $# -eq 0 ]; then
    usage 0
fi

EXE="$1"
shift

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

    if [ "x-t" == "x-$arg" ]; then
        TOnext="y"
        continue
    fi

    argdir=`dirname $arg`
    argbase=`basename -s .c $arg`
    argout="$argbase.$EXT"
    argerr="$argbase.$ERR"

#
#
# Check for basic output/error files
#

    skip="y"
    if [ -f OUTPUTS_B/$argerr ]; then
        skip=""
    fi
    if [ -f SPECS/$argbase.spec ]; then
        skip=""
    fi
    if [ "$skip" ]; then
        continue
    fi

#
# Run student compiler
#

    printf "%-20s  " "$argbase.c"

    timeoutCompile $argdir $argbase $TIMEOUT

    if [ -f $argbase.timeout ]; then
        red "Timeout exceeded ($TIMEOUT seconds)" "\n"
    elif [ -f OUTPUTS_B/$argerr ]; then
        compareInvalid $argbase OUTPUTS_B
    elif [ -s $argerr ]; then
        red "Compile error" "\n"
        showRawError $argerr
    else
        parse_spec $argbase SPECS
    fi

#
# Cleanup
#

    rm -f $RMFILES
    RMFILES=""
done


