#!/bin/bash
#set -x

REFERENCE=./reference
PGM=./assignment-3

# To add a test you can add an element the REQUIRED arrays 
# or BONUS Arrays
# Each test can have a descripttion, input and optionally a num
# num is passed as the argument on the command line to set
# the default max schedule size

# Required tests
REQUIRED_DESC[0]="Required 1: Simple List schedules and Add Schedule test"
REQUIRED_INPUT[0]="L
A Toronto
L"

REQUIRED_DESC[1]="Required 2: Basic test of add Flight"
REQUIRED_INPUT[1]="L
A Toronto
l Toronto
a Toronto
1000 5
l Toronto"

REQUIRED_NUM[2]="2"
REQUIRED_DESC[2]="Required 3: Corner Test max+1 adding schedules"
REQUIRED_INPUT[2]="L
A City1
A City2
L
A City3
L"

REQUIRED_DESC[3]="Required 4: Basic test of removing a flight schedule"
REQUIRED_INPUT[3]="A Naples
A Addis Ababa
L
R Naples
L"

REQUIRED_DESC[4]="Required 5: Basic test of removing a flight for a particular city"
REQUIRED_INPUT[4]="A San Jose
a San Jose
360 100
l San Jose
r San Jose
360
l San Jose"

REQUIRED_DESC[5]="Required 6: Corner test of removing a nonexistent flight for a particular city"
REQUIRED_INPUT[5]="A Hanoi
r Hanoi
100"

REQUIRED_DESC[6]="Required 7: Basic test of scheduling a flight for a particular city"
REQUIRED_INPUT[6]="A Cartagena
a Cartagena
720 100
a Cartagena
840 120
s Cartagena
480
l Cartagena"

REQUIRED_DESC[7]="Required 8: Basic test of unscheduling a flight for a particular city"
REQUIRED_INPUT[7]="L
A Abu Dhabi
a Abu Dhabi
360 50
a Abu Dhabi
840 100
s Abu Dhabi
700
l Abu Dhabi
u Abu Dhabi
840
l Abu Dhabi"

REQUIRED_DESC[8]="Required 9: Corner test of scheduling a full flight for a particular city"
REQUIRED_INPUT[8]="L
A Belgrade
a Belgrade
400 1
s Belgrade
400
l Belgrade
s Belgrade
400
l Belgrade"

REQUIRED_DESC[9]="Required 10: Corner test of unscheduling an empty flight"
REQUIRED_INPUT[9]="L
A Kyoto
a Kyoto
300 5
l Kyoto
u Kyoto
300
l Kyoto"

REQUIRED_DESC[10]="Required 11: Corner test of unscheduling a nonexistent flight"
REQUIRED_INPUT[10]="L
A Mumbai
a Mumbai
900 3
l Mumbai
u Mumbai
300
l Mumbai"

REQUIRED_DESC[11]="Required 12: Corner test for scheduling a flight after all possible times"
REQUIRED_INPUT[11]="L
A Prague
a Prague
360 5
a Prague
720 100
l Prague
s Prague
900
l Prague"

REQUIRED_NUM[12]="2"
REQUIRED_DESC[12]="Required 13: Exhaustive Test for adding schedules to maximuin amount and removing them"
REQUIRED_INPUT[12]="L
A City1
L
A City2
L
A City3
L
R City1
L
R City2
L
R City3
L
A City1
L"

REQUIRED_DESC[13]="Required 14: Exhaustive Test for maxinum flights on a city"
REQUIRED_INPUT[13]="L
A City1
a City1
60 5
l City1
a City1
120 5
l City1
a City1
180 5
l City1
a City1
240 5
l City1
a City1
300 5
l City1
a City1
360 5
l City1
r City1
360
l City1
r City1
300
l City1
r City1
240
l City1
r City1
180
l City1
r City1
120
l City1
r City1
60
l City1
a City1
60 5
l City1"

REQUIRED_DESC[14]="Required 15: Exhaustive Test for schedule up to maxinum seats on a flight"
REQUIRED_INPUT[14]="L
A City1
a City1
60 3
l City1
s City1
60
l City1
s City1
60
l City1
s City1
60
l City1
s City1
60
l City1
u City1
60
l City1
u City1
60
l City1
u City1
60
l City1
u City1
60
l City1"

REQUIRED_DESC[15]="Required 16: A bigger test "
REQUIRED_INPUT[15]="$(cat testSmall.txt)"
REQUIRED_VALUE[15]=5


# Bonus tests
BONUS_DESC[0]="Bonus 1: Medium sized test"
BONUS_INPUT[0]="$(cat testMedium.txt)"
BONUS_VALUE[0]=3

BONUS_DESC[1]="Bonus 2: Large sized test"
BONUS_INPUT[1]="$(cat testBig.txt)"
BONUS_VALUE[1]=5


function run_test()
{
   description="$1"
   input="$2"
   num="$3"

   echo -n "$description : "
   [[ -n $VERBOSE ]] &&  echo "Input:"
   [[ -n $VERBOSE ]] &&  echo "--------------------------"
   [[ -n $VERBOSE ]] &&  echo "$input"
   [[ -n $VERBOSE ]] &&  echo "--------------------------"

   solution=$($REFERENCE $num <<EOF
$input
EOF
)

   answer=$($PGM $num<<EOF
$input
EOF
)

  if [[ -n $DEBUG ]]; then
      echo "$solution"
  else
    if [[ "$answer" == "$solution" ]]; then
       echo "PASS"
       return 0
    fi
    #echo "FAIL: your program produced: '$answer' output should have been: '$solution'"
    echo "FAIL"
    [[ -n $VERBOSE ]] && echo "Your program produced:"
    [[ -n $VERBOSE ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && echo "$answers"    
    [[ -n $VERBOSE ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && echo "Output should have been:"
    [[ -n $VERBOSE ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && echo "$solution"
    [[ -n $VERBOSE ]] && echo "--------------------------"
    [[ -n $VERBOSE ]] && echo ""
  fi
  return 1
}

testnum=$1
type=$2

total=0
correct=0

if [[ -z $testnum ]]; then
    for ((i=0; i<${#REQUIRED_INPUT[@]}; i++)); do
	value="${REQUIRED_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${REQUIRED_DESC[$i]}" "${REQUIRED_INPUT[$i]}" "${REQUIRED_NUM[$i]}"; then
	    (( correct += $value ))
	fi
	(( total += value ))
    done
    
    
    for ((i=0; i<${#BONUS_INPUT[@]}; i++)); do
	value="${BONUS_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${BONUS_DESC[$i]}" "${BONUS_INPUT[$i]}" "${BONUS_NUM[$i]}"; then
	    (( correct += $value ))
	fi
    done
else
    VERBOSE=1
    (( i=$testnum - 1 ))
    if [[ -z "$type" ]]; then
	if (( $i < 0 || $i >= ${#REQUIRED_INPUT[@]} )); then
	    echo -n "There are ${#REQUIRED_INPUT[@]} required tests.  Please specify a test number"
	    echo " between 1 and ${#REQUIRED_INPUT[@]} inclusively"
	    exit -1
	fi
	value="${REQUIRED_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${REQUIRED_DESC[$i]}" "${REQUIRED_INPUT[$i]}" "${REQUIRED_NUM[$i]}"; then
	    (( correct += $value ))
	fi
	(( total += value ))
    else
	if (( $i < 0 || $i >= ${#BONUS_INPUT[@]} )); then
	    echo -n "There are ${#BONUS_INPUT[@]} bonus tests.  Please specify a test number"
	    echo " between 1 and ${#BONUS_INPUT[@]} inclusively"
	    exit -1
	fi	
	value="${BONUS_VALUE[$i]}"
	[[ -z $value ]] && value=1
	if run_test "${BONUS_DESC[$i]}" "${BONUS_INPUT[$i]}" "${BONUS_NUM[$i]}"; then
         (( correct += $value ))
	fi
    fi
fi
echo "score: ${correct}/${total}"
