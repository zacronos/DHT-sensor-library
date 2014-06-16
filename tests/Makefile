##############################################################################
# This is a horrible makefile.  It's been a long time since I've coded in C or
# C++, and my make-fu has all but disappeared.  This is just something I threw
# together quickly to make it easier to test my library.
##############################################################################

.PHONY=clean
COMMAND=g++ -I./mocks -I..

test.out: MockedSensorTester.o WProgram.o DHT.o DHT_TempHumidUtils.o
	${COMMAND} -o test.out MockedSensorTester.o WProgram.o DHT.o DHT_TempHumidUtils.o

clean:
	rm -f ./*.o

MockedSensorTester.o: MockedSensorTester.cpp
	${COMMAND} -c $^ -o $@

WProgram.o: mocks/WProgram.c
	${COMMAND} -c $^ -o $@

DHT.o: ../DHT.cpp
	${COMMAND} -c $^ -o $@

DHT_TempHumidUtils.o: ../DHT_TempHumidUtils.cpp
	${COMMAND} -c $^ -o $@
