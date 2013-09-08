# Makefile

# Name of text file containing build number.
BUILD_NUMBER_FILE=build-number.txt

OBJECTS=bnum.o

a.out: 	$(OBJECTS) $(BUILD_NUMBER_FILE)
	$(CC) $(LDFLAGS) $(BUILD_NUMBER_LDFLAGS) -o $@ $(OBJECTS)

# Include build number rules.
include buildnumber.mak
