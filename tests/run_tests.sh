#!/usr/bin/env bash

# running tests with correct input data
DataRange=(001 002 003 004 005 006 007)
DataPath=input/
ExpPath=expect/
OutPath=output/

echo > test_log

ERR_COUNTER=0

for i in ${DataRange[@]}
do
    echo "running $i test"
    ANSW=$(valgrind --log-file=va_logs/va_logData${i}.txt ./../bin/transformator < ${DataPath}${i}.in )
    EXPECT=$(cat ${ExpPath}${i}.out)
    if [[ "$EXPECT" != "$ANSW" ]]; then
       ERR_COUNTER+=1
       STATUS="FAILED"
    else
       STATUS="OK"
    fi
    echo "TEST ${i} $STATUS : answ = $ANSW  expected = $EXPECT" >> test_log
done

if [[ $ERR_COUNTER == 0 ]]; then
  echo "ALL TESTS PASSED"
else
  echo "TESTS FAILED"
  echo "CHECK test_log"
fi