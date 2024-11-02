
# Exception Entry and Exit Sequence

## Exception entry sequence

1. Pending bit set in nvic
1. Processor does stacking and vector fetch
    * Stacking -> pushing the content of register to stack
    * vector fetch -> fetch the address of exception handler
1. Entry into the handler and active bit set in nvic register
1. Pending status clears
1. Processor mode changed to handler mode
1. Handler code is executing
1. MSGP will be used for any stack operations inside the handler

## Exit sequence

* In cortex m3/m4 processors the exception return mechanism is triggered using a special return address called EXC_RETURN
* EXC_RETURN is generated during exception entry and is stored in LR
* When EXC_RETURN is written to PC it triggers the exception return
* Then it triggers unstacking

## When EXC_RETURN generated?

During an exception handler entry, the value of the return address (PC) is not stored in the LR as it is done during calling of a normal C function.
Instead the exception mechanism stores the special value called EXC_RETURN in LR.

EXC_RETURN stores information about:
* Return mode (whether it is thread mode or handler mode)
* Return stack (whether it is PSP or MSP)