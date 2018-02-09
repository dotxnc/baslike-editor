(Define function "TEST"
which just does a bunch
of text printing and
then exits)
FNC TEST
    PRN TEST
    (This is a simple loop)
    DEF A
    MDS 0
    IFL 3
        ADD 1
        PRN FUCK
        JMP A
    ENF
    PRN DONE
END

(Call function "TEST"
and return back to
the starting point)
CAL TEST
PRN PROGRAM_EXIT

(Does a lot of cool stuff)
(Function parameters to come)

